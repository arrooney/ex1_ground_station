#include <sys/types.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <errno.h>
#include <malloc.h>

#include <util/crc32.h>
#include <util/log.h>

#include <lzo/lzoutils.h>

#include <ftp/ftp_server.h>

#include <dirent.h>

/* Chunk status markers */
static const char const *packet_missing = "-";
static const char const *packet_ok = "+";

struct ftp_newlib_state {
	/* Current bitmap and file */
	FILE *fd_file;
	FILE *fd_map;
	char *mem_map;

	/* Current file info */
	char file_name[FTP_PATH_LENGTH];
	uint32_t file_size;
	uint32_t file_chunk_size;
	uint32_t file_chunks;

	/* Directory pointer used in listing */
	DIR *dirp;
	char dirpath[FTP_PATH_LENGTH];
};

ftp_return_t ftp_newlib_abort(void *state);
ftp_return_t ftp_newlib_timeout(void *state);

/* Calculate CRC of current file */
int file_crc(void *state, uint32_t *crc_arg) {

	struct ftp_newlib_state *newlib_state = (struct ftp_newlib_state *)state;

	if (!newlib_state)
		return FTP_RET_INVAL;

	/* Flush file before calculating CRC */
	if (fflush(newlib_state->fd_file) != 0)
		printf("Flush error\r\n");
	if (fsync(fileno(newlib_state->fd_file)) != 0)
		printf("Fsync error\r\n");
	if (fseek(newlib_state->fd_file, 0, SEEK_SET) != 0)
		printf("Fseek error\r\n");


	//printf("File size to check %"PRIu32", state %p, fd %p\r\n", newlib_state->file_size, newlib_state, newlib_state->fd_file);
	uint8_t byte;
	uint32_t crc = 0xFFFFFFFF;
	while(fread(&byte, 1, 1, newlib_state->fd_file) == 1)
		crc = chksum_crc32_step(crc, byte);
	crc = (crc ^ 0xFFFFFFFF);

	*crc_arg = crc;

	//printf("Backend CRC 0x%"PRIx32" (%p), 0x%"PRIx32"\r\n", *crc_arg, crc_arg, crc);

	return 0;

}

ftp_return_t ftp_newlib_init(void **state) {

	if (state) {
#ifdef __linux__
		struct ftp_newlib_state *newlib_state = malloc(sizeof(struct ftp_newlib_state));
#else
		struct ftp_newlib_state *newlib_state = pvPortMalloc(sizeof(struct ftp_newlib_state));
#endif

		if (newlib_state) {
			newlib_state->fd_file = NULL;
			newlib_state->fd_map = NULL;
			newlib_state->mem_map = NULL;
			*state = newlib_state;
			return FTP_RET_OK;
		}
	}

	return FTP_RET_NOMEM;

}

ftp_return_t ftp_newlib_release(void *state) {

	if (state) {
#ifdef __linux__
		free(state);
#else
		vPortFree(state);
#endif
	}

	return FTP_RET_OK;
}

ftp_return_t ftp_newlib_upload(void *state, const char const *path, uint32_t memaddr, uint32_t size, uint32_t chunk_size) {

	/* Map file name */
	char map[100];

	struct ftp_newlib_state *newlib_state = (struct ftp_newlib_state*)state;

	if (!newlib_state)
		return FTP_RET_INVAL;

	newlib_state->file_size = size;
	newlib_state->file_chunk_size = chunk_size;
	newlib_state->file_chunks = (size + chunk_size - 1) / chunk_size;
	strncpy(newlib_state->file_name, path, FTP_PATH_LENGTH);
	newlib_state->file_name[FTP_PATH_LENGTH - 1] = '\0';

	/* Abort if a previous transfer was incomplete */
	if (newlib_state->fd_file || newlib_state->fd_map || newlib_state->mem_map) {
		printf("Aborting previous transfer %p %p %p\r\n", newlib_state->fd_file, newlib_state->fd_map, newlib_state->mem_map);
		ftp_newlib_timeout(state);
		printf("ftp_newlib_abort complete\r\n");
	}

	/* Try to open file */
	newlib_state->fd_file = fopen((const char *) path, "r+");
	if (newlib_state->fd_file == NULL) {
		/* Create new file */
		newlib_state->fd_file = fopen((const char *) path, "w+");
		if (newlib_state->fd_file == NULL) {
			printf("Server: Failed to create data file\r\n");
			return FTP_RET_IO;
		}
	}

	/* Allocate memory map */
	if (newlib_state->mem_map == NULL) {
#ifdef __linux__
		newlib_state->mem_map = malloc(newlib_state->file_chunks);
#else
		newlib_state->mem_map = pvPortMalloc(newlib_state->file_chunks);
#endif

		if (newlib_state->mem_map == NULL) {
			fclose(newlib_state->fd_file);
			newlib_state->fd_file = NULL;
			return FTP_RET_NOSPC;
		}
	}

	/* Try to create a new map file */
	strncpy(map, path, FTP_PATH_LENGTH);
	strcpy((char *) map + strlen(path) - 4, ".MAP");

	/* Check if file already exists */
	newlib_state->fd_map = fopen(map, "r+");
	if (newlib_state->fd_map == NULL || fread(newlib_state->mem_map, 1, newlib_state->file_chunks, newlib_state->fd_map) != newlib_state->file_chunks) {
		unsigned int i;

		/* Close file if already open but data could not be read */
		if (newlib_state->fd_map) {
			fclose(newlib_state->fd_map);
			newlib_state->fd_map = NULL;
		}

		/* Create new file */
		newlib_state->fd_map = fopen(map, "w+");
		if (newlib_state->fd_map == NULL) {
			printf("Failed to create bitmap %s\r\n", map);
			fclose(newlib_state->fd_file);
			newlib_state->fd_file = NULL;
			free(newlib_state->mem_map);
			newlib_state->mem_map = NULL;
			return FTP_RET_IO;
		}

		/* Clear contents */
		for (i = 0; i < newlib_state->file_chunks; i++) {
			if (fwrite(packet_missing, 1, 1, newlib_state->fd_map) < 1) {
				printf("Failed to clear bitmap\r\n");
				fclose(newlib_state->fd_map);
				newlib_state->fd_map = NULL;
				fclose(newlib_state->fd_file);
				newlib_state->fd_file = NULL;
				free(newlib_state->mem_map);
				newlib_state->mem_map = NULL;
				return FTP_RET_IO;
			}
			newlib_state->mem_map[i] = *packet_missing;
		}

		fflush(newlib_state->fd_map);
		fsync(fileno(newlib_state->fd_map));
	}

	return FTP_RET_OK;

}

ftp_return_t ftp_newlib_download(void *state, const char const *path, uint32_t memaddr, uint32_t memsize, uint32_t chunk_size, uint32_t *size, uint32_t *crc) {

	struct ftp_newlib_state *newlib_state = (struct ftp_newlib_state *)state;

	if (!newlib_state)
		return FTP_RET_INVAL;

	/* Try to open file */
	newlib_state->fd_file = fopen((const char *) path, "r+");
	if (newlib_state->fd_file == NULL)
		return FTP_RET_NOENT;

	/* Read size */
	struct stat statbuf;
	if (stat(path, &statbuf) != 0)
		return FTP_RET_IO;

    printf("size: %li\r\n", (long int)statbuf.st_size);

	*size = statbuf.st_size;
	newlib_state->file_size = *size;
	newlib_state->file_chunk_size = chunk_size;
	newlib_state->file_chunks = (newlib_state->file_size + chunk_size - 1) / chunk_size;
	strncpy(newlib_state->file_name, path, FTP_PATH_LENGTH);
	newlib_state->file_name[FTP_PATH_LENGTH - 1] = '\0';

	/* Calculate CRC */
	if (file_crc(state, crc) != 0)
		return FTP_RET_IO;
	else
		return FTP_RET_OK;

}

ftp_return_t ftp_newlib_write_chunk(void *state, uint32_t chunk, uint8_t *data, uint32_t size) {

	struct ftp_newlib_state *newlib_state = (struct ftp_newlib_state *)state;

	if (!newlib_state)
		return FTP_RET_INVAL;

	/* Write to file */
	if (ftell(newlib_state->fd_file) != chunk * newlib_state->file_chunk_size) {
		if (fseek(newlib_state->fd_file, chunk * newlib_state->file_chunk_size, SEEK_SET) != 0)
			goto chunk_write_error;
	}
	if (fwrite(data, 1, size, newlib_state->fd_file) != size)
		goto chunk_write_error;

#ifndef __linux__
	fflush(newlib_state->fd_map);
	fsync(fileno(newlib_state->fd_map));
#endif

	return FTP_RET_OK;

chunk_write_error:
	printf("Filesystem write error\r\n");
	fclose(newlib_state->fd_file);
	newlib_state->fd_file = NULL;
	return FTP_RET_IO;

}

ftp_return_t ftp_newlib_read_chunk(void *state, uint32_t chunk, uint8_t *data, uint32_t size) {

	struct ftp_newlib_state *newlib_state = (struct ftp_newlib_state *)state;

	if (!newlib_state)
		return FTP_RET_INVAL;

	/* Read from file */
	if (ftell(newlib_state->fd_file) != chunk * newlib_state->file_chunk_size) {
		if (fseek(newlib_state->fd_file, chunk * newlib_state->file_chunk_size, SEEK_SET) != 0)
			goto chunk_read_error;
	}
	if (fread(data, 1, size, newlib_state->fd_file) != size)
		goto chunk_read_error;

	return FTP_RET_OK;

chunk_read_error:
	printf("Filesystem read error (chunk %"PRIu32", size %"PRIu32", file chunk size %"PRIu32")\r\n",
			chunk, size, newlib_state->file_chunk_size);
	fclose(newlib_state->fd_file);
	newlib_state->fd_file = NULL;
	return FTP_RET_IO;

}

ftp_return_t ftp_newlib_get_status(void *state, uint32_t chunk, int *status) {

	struct ftp_newlib_state *newlib_state = (struct ftp_newlib_state *)state;

	if (!newlib_state || !status)
		return FTP_RET_INVAL;

	*status = (newlib_state->mem_map[chunk] == *packet_ok);
	return FTP_RET_OK;

}

ftp_return_t ftp_newlib_set_status(void *state, uint32_t chunk) {

	struct ftp_newlib_state *newlib_state = (struct ftp_newlib_state *)state;

	if (!newlib_state)
		return FTP_RET_INVAL;

	/* Write to map file */
	if (ftell(newlib_state->fd_map) != chunk) {
		if (fseek(newlib_state->fd_map, chunk, SEEK_SET) != 0)
			goto status_write_error;
	}
	if (fwrite(packet_ok, 1, 1, newlib_state->fd_map) != 1)
		goto status_write_error;

#ifndef __linux__
	fflush(newlib_state->fd_map);
	fsync(fileno(newlib_state->fd_map));
#endif

	/* Mark in RAM map */
	newlib_state->mem_map[chunk] = *packet_ok;

	return FTP_RET_OK;

status_write_error:
	printf("Failed to write status for chunk %"PRIu32" %p\r\n", chunk, newlib_state->fd_map);
	fclose(newlib_state->fd_map);
	newlib_state->fd_map = NULL;
	return FTP_RET_IO;

}

ftp_return_t ftp_newlib_get_crc(void *state, uint32_t *crc) {

	return (crc != NULL && file_crc(state, crc) == 0) ? FTP_RET_OK : FTP_RET_INVAL;

}

ftp_return_t ftp_newlib_done(void *state) {

	struct ftp_newlib_state *newlib_state = (struct ftp_newlib_state *)state;

	if (!newlib_state)
		return FTP_RET_INVAL;

	/* Close files */
	if (newlib_state->fd_file) {
		fclose(newlib_state->fd_file);
		newlib_state->fd_file = NULL;
	}
	if (newlib_state->fd_map) {
		fclose(newlib_state->fd_map);
		newlib_state->fd_map = NULL;
	}
	if (newlib_state->mem_map) {
		free(newlib_state->mem_map);
		newlib_state->mem_map = NULL;
	}

	/* Generate map path */
	char map[100];
	strncpy(map, newlib_state->file_name, FTP_PATH_LENGTH);
	strcpy((char *) map + strlen(newlib_state->file_name) - 4, ".MAP");

	/* Remove map */
	remove(map);

	return FTP_RET_OK;

}

ftp_return_t ftp_newlib_abort(void *state) {

	struct ftp_newlib_state *newlib_state = (struct ftp_newlib_state *)state;

	if (!newlib_state)
		return FTP_RET_INVAL;

	/* Close files */
	if (newlib_state->fd_file) {
		fclose(newlib_state->fd_file);
		newlib_state->fd_file = NULL;
	}
	if (newlib_state->fd_map) {
		fclose(newlib_state->fd_map);
		newlib_state->fd_map = NULL;
	}
	if (newlib_state->mem_map) {
		free(newlib_state->mem_map);
		newlib_state->mem_map = NULL;
	}

	/* Generate map path */
	char map[100];
	newlib_state->file_name[FTP_PATH_LENGTH-1] = '\0';
	snprintf(map, FTP_PATH_LENGTH, "%sm", newlib_state->file_name);

	/* Remove map */
	if (remove(map) != 0) {
		printf("Failed to remove %s\r\n", map);
		return FTP_RET_IO;
	}

	/* Remove file */
	if (remove(newlib_state->file_name) != 0) {
		printf("Failed to remove %s\r\n", newlib_state->file_name);
		return FTP_RET_IO;
	}

	return FTP_RET_OK;

}

ftp_return_t ftp_newlib_timeout(void *state) {

	struct ftp_newlib_state *newlib_state = (struct ftp_newlib_state *)state;

	if (!newlib_state)
		return FTP_RET_INVAL;

	/* Close files */
	if (newlib_state->fd_file) {
		fclose(newlib_state->fd_file);
		newlib_state->fd_file = NULL;
	}
	if (newlib_state->fd_map) {
		fclose(newlib_state->fd_map);
		newlib_state->fd_map = NULL;
	}
	if (newlib_state->mem_map) {
		free(newlib_state->mem_map);
		newlib_state->mem_map = NULL;
	}

	return FTP_RET_OK;

}

ftp_return_t ftp_newlib_remove(void *state, char *path) {

	if (remove(path) != 0) {
		/* We assume that file does not exist */
		printf("path: %s\r\n", path);
		return FTP_RET_NOENT;
	} else {
		return FTP_RET_OK;
	}

}

ftp_return_t ftp_newlib_move(void *state, char *from, char *to) {

	if (rename(from, to) != 0) {
		/* We assume that the file does not exist */
		printf("from: %s to: %s\r\n", from, to);
		return FTP_RET_NOENT;
	} else {
		return FTP_RET_OK;
	}

}

ftp_return_t ftp_newlib_list(void *state, char *path, uint16_t *entries) {

	struct ftp_newlib_state *newlib_state = (struct ftp_newlib_state *)state;

	if (!newlib_state || !path || !entries)
		return FTP_RET_INVAL;

	/* Copy entries */
	struct dirent *ent;
	*entries = 0;

	newlib_state->dirp = opendir((const char*)path);
	if (newlib_state->dirp == NULL)
		return FTP_RET_NOENT;

	strncpy(newlib_state->dirpath, path, FTP_PATH_LENGTH);

	/* Count entries */
	while ((ent = readdir(newlib_state->dirp)))
		(*entries)++;

	/* Rewind for entry function */
	rewinddir(newlib_state->dirp);

	return FTP_RET_OK;

}

ftp_return_t ftp_newlib_entry(void *state, ftp_list_entry_t *ent) {

	struct dirent *dent;
	struct stat stat_buf;
	char buf[FTP_PATH_LENGTH];
	char *sub;

	struct ftp_newlib_state *newlib_state = (struct ftp_newlib_state *)state;

	if (!newlib_state)
		return FTP_RET_INVAL;

	/* ent is allowed to be NULL, if an entry should be skipped */
	if (newlib_state->dirp == NULL)
		return FTP_RET_IO;

	/* Loop through directories */
	if ((dent = readdir(newlib_state->dirp))) {
		if (ent == NULL)
			return FTP_RET_INVAL;

		/* Name */
		strncpy(ent->path, dent->d_name, FTP_PATH_LENGTH);
		ent->path[FTP_PATH_LENGTH-1] = '\0';

		/* Size */
		strcpy(buf, newlib_state->dirpath);
		sub = buf;
		if (newlib_state->dirpath[strlen(newlib_state->dirpath)-1] != '/')
			sub = strcat(buf, "/");
		sub = strcat(sub, dent->d_name);
		stat(sub, &stat_buf);
		ent->size = (uint32_t)stat_buf.st_size;

		/* Type */
		ent->type = ((stat_buf.st_mode & S_IFMT) == S_IFDIR) ? FTP_LIST_DIR : FTP_LIST_FILE;
	} else {
		closedir(newlib_state->dirp);
		newlib_state->dirp = NULL;
		return FTP_RET_NOENT;
	}

	return FTP_RET_OK;
}

ftp_return_t ftp_newlib_zip(void *state, char *src, char *dest, uint8_t action, uint32_t * comp_sz, uint32_t * decomp_sz) {

	if ((action == FTP_ZIP) || (action == FTP_UNZIP)) {
		if (!strcmp(src, dest)) {
			return FTP_RET_EXISTS;
		}
		struct stat statbuf;
		if (stat(src, &statbuf) != 0) {
			return FTP_RET_NOENT;
		}
		if (action == FTP_ZIP) {
			/* Zip/compress */
			*decomp_sz = (uint32_t)statbuf.st_size;
			*comp_sz = *decomp_sz + 1024;
		} else {
			/* Unzip/decompress */
			uint32_t header_len;
			/* Read decompressed size */
			if (lzo_header(src, &header_len, decomp_sz, comp_sz) != 0) {
				printf("Failed to read header\r\n");
				return FTP_RET_INVAL;
			}
			/* Todo: figure out if this is needed or if it is bug in lzo_decompress_buffer */
			*decomp_sz += (*decomp_sz) / 16 + 256;
			*comp_sz = (uint32_t)statbuf.st_size;
		}

		ftp_return_t retval = FTP_RET_OK;
		/* Try to open source file */
		FILE *fd_in = fopen((const char *) src, "r");
		if (fd_in != NULL) {
			/* Create new file */
			FILE *fd_out = fopen((const char *) dest, "w+");
			if (fd_out != NULL) {
#ifdef __linux__
				uint8_t *data = malloc(*decomp_sz + *comp_sz);
#else
				uint8_t *data = pvPortMalloc(*decomp_sz + *comp_sz);
#endif
				if (data == NULL) {
					printf("Server: Failed to allocate memory\r\n");
					fclose(fd_out);
					fclose(fd_in);
					return FTP_RET_NOMEM;
				}
				uint8_t *comp_buf = data;
				uint8_t *decomp_buf = data + (*comp_sz);
				ftp_return_t retval = FTP_RET_OK;
				if (action == FTP_ZIP) {
					/* Zip / compress */
					size_t read = fread(decomp_buf, 1, *decomp_sz, fd_in);
					if (read != *decomp_sz) {
						printf("Error reading from uncompressed file\r\n");
						retval = FTP_RET_IO;
					} else {
						if (lzo_compress_buffer(decomp_buf, *decomp_sz, comp_buf, comp_sz, *comp_sz) != 0) {
							printf("Error compressing data\r\n");
							retval = FTP_RET_NOSPC;
						} else {
							size_t written = fwrite(comp_buf, 1, *comp_sz, fd_out);
							if (written != *comp_sz) {
								printf("Error writing to compressed file\r\n");
								retval = FTP_RET_NOSPC;
							}
						}
					}
				} else {
					/* Unzip / decompress */
					size_t read = fread(comp_buf, 1, *comp_sz, fd_in);
					if (read != *comp_sz) {
						printf("Error reading from compressed file\r\n");
						retval = FTP_RET_IO;
					} else {
						if (lzo_decompress_buffer(comp_buf, *comp_sz, decomp_buf, *decomp_sz, decomp_sz) != 0) {
							printf("Error decompressing data\r\n");
							retval = FTP_RET_NOSPC;
						} else {
							size_t written = fwrite(decomp_buf, 1, *decomp_sz, fd_out);
							if (written != *decomp_sz) {
								printf("Error writing to decompressed file\r\n");
								retval = FTP_RET_NOSPC;
							}
						}
					}
				}
#ifdef __linux__
				free(data);
#else
				vPortFree(data);
#endif
				fclose(fd_out);
				fclose(fd_in);
				if (retval != FTP_RET_OK) {
					/* Remove parital output file */
					remove(dest);
				}
			} else {
				printf("Server: Failed to create data file\r\n");
				fclose(fd_in);
				return FTP_RET_IO;
			}
		} else {
			return FTP_RET_NOENT;
		}
		return retval;
	}

	return FTP_RET_INVAL;
}

ftp_backend_t backend_newlib = {
	.init			= ftp_newlib_init,
	.release		= ftp_newlib_release,
	.upload 		= ftp_newlib_upload,
	.download 		= ftp_newlib_download,
	.chunk_write	= ftp_newlib_write_chunk,
	.chunk_read 	= ftp_newlib_read_chunk,
	.status_get 	= ftp_newlib_get_status,
	.status_set 	= ftp_newlib_set_status,
	.list 			= ftp_newlib_list,
	.entry 			= ftp_newlib_entry,
	.remove			= ftp_newlib_remove,
	.move			= ftp_newlib_move,
	.mkfs			= NULL,
	.crc 			= ftp_newlib_get_crc,
	.zip 			= ftp_newlib_zip,
	.abort 			= ftp_newlib_abort,
	.done 			= ftp_newlib_done,
	.timeout		= ftp_newlib_timeout,
};
