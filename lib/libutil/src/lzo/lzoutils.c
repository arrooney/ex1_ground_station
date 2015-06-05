/* Utilities for LZO decompression */

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <sys/stat.h>

#include <lzo/minilzo.h>
#include <lzo/lzoutils.h>
#include <lzo/lzoconf.h>
#include <util/log.h>

#ifndef __linux__
#include <FreeRTOS.h>
#include <task.h>
#endif

#include <util/byteorder.h>

#define F_ADLER32_D     0x00000001L
#define F_ADLER32_C     0x00000002L
#define F_STDIN         0x00000004L
#define F_STDOUT        0x00000008L
#define F_NAME_DEFAULT  0x00000010L
#define F_DOSISH        0x00000020L
#define F_H_EXTRA_FIELD 0x00000040L
#define F_H_GMTDIFF     0x00000080L
#define F_CRC32_D       0x00000100L
#define F_CRC32_C       0x00000200L
#define F_MULTIPART     0x00000400L
#define F_H_FILTER      0x00000800L
#define F_H_CRC32       0x00001000L
#define F_H_PATH        0x00002000L
#define F_MASK          0x00003FFFL

#define CRC32_INIT_VALUE    0

#define HEADER_FLAGS_OFFSET  17
#define HEADER_FNAME_OFFSET  33
/*
 * All fields are big endian byte order.

 * Magic: 9 bytes (0x89, 0x4c, 0x5a, 0x4f, 0x00, 0x0d, 0x0a, 0x1a, 0x0a)
 * Version: 2 bytes
 * Lib version: 2 bytes
 * Version needed: 2 bytes
 * Method: 1 byte
 * Level: 1 byte
 * Flags: 4 byte
 * Filter: 4 bytes (only if flags & F_H_FILTER)
 * Mode: 4 bytes
 * Mtime: 4 bytes
 * GMTdiff: 4 bytes
 * File name length: 1 byte
 * Name: 0-255 bytes
 * Checksum, original data: 4 bytes (CRC32 if flags & F_H_CRC32 else Adler32)
 * Uncompressed size: 4 bytes
 * Compressed size: 4 bytes
 * Checksum, uncompressed data: 4 bytes
 * Checksum, compressed data: 4 bytes (only if flags & F_ADLER32_C or flags & F_CRC32_C)
 * Compressed data: 0-x bytes
 */

static const char const magic[] = {0x89, 0x4c, 0x5a, 0x4f, 0x00, 0x0d, 0x0a, 0x1a, 0x0a};

int lzo_match_magic(void * add) {
	return !memcmp(add, magic, 9);
}

/* Test if path is a valid LZO image from lzop */
int lzo_is_lzop_image(char * path) {

	int ret = 0;
	char rmagic[9];

	FILE *fp = fopen(path, "r");
	if (!fp) {
		/* Failed to open file */
		goto out_open;
	} else if (fread(rmagic, 1, 9, fp) != 9) {
		/* Failed to read magic sequence */
		goto out;
	} else if (!lzo_match_magic(rmagic)) {
		/* Magic sequence did not match */
		goto out;
	}

	/* Magic found */
	ret = 1;
out:
	fclose(fp);
out_open:

	return ret;

}

int lzo_header(char * path, uint32_t * header_length, uint32_t * decomp_len, uint32_t * comp_len) {

	int ret = -1;
	uint32_t dlen, clen;
	uint8_t fname_len;

	if (!header_length || !decomp_len || !comp_len) {
		log_error("length pointer was NULL");
		goto err_open;
	}

	/* Reset size counters */
	*decomp_len = 0;
	*comp_len = 0;

	/* Open file */
	FILE *fp = fopen(path, "r");
	if (!fp) {
		log_error("lzo: cannot open %s", path);
		goto err_open;
	}

	/* Read flags */
	uint32_t header_flags = 0;
	if (fseek(fp, HEADER_FLAGS_OFFSET, SEEK_SET) != 0) {
		log_error("Failed to seek file pos %d", HEADER_FLAGS_OFFSET);
		goto err_read;
	} else {
		if (fread(&header_flags, 1, 4, fp) != 4) {
			log_error("Failed to header flags");
			goto err_read;
		}
	}
	header_flags = util_ntohl(header_flags);

	/* Determine CRC offset */
	uint32_t crc_size = 0;
	if ((header_flags & F_ADLER32_D) || (header_flags & F_CRC32_D))
		crc_size += 4;
	if ((header_flags & F_ADLER32_C) || (header_flags & F_CRC32_C))
		crc_size += 4;

	/* Read file name length */
	if (fseek(fp, HEADER_FNAME_OFFSET, SEEK_SET) != 0) {
		log_error("Failed to seek file pos %d", HEADER_FNAME_OFFSET);
		goto err_read;
	} else {
		if (fread(&fname_len, 1, 1, fp) != 1) {
			log_error("Failed to read file name length");
			goto err_read;
		}
	}

	/* Skip file name and checksum */
	if (fseek(fp, fname_len + 4, SEEK_CUR) != 0) {
		log_error("Failed to skip file name");
		goto err_read;
	}

	/* Store header length */
	*header_length = HEADER_FNAME_OFFSET + 1 + fname_len + 16;

	/* Read block sizes */
	while (1) {
		if (fread(&dlen, 1, sizeof(dlen), fp) != sizeof(dlen)) {
			log_error("Failed to read uncompressed file length");
			goto err_read;
		} else {
			dlen = util_ntohl(dlen);
			*decomp_len += dlen;
		}

		if (dlen == 0) {
			ret = 0;
			break;
		}

		if (fread(&clen, 1, sizeof(clen), fp) != sizeof(clen)) {
			log_error("Failed to read compressed file length");
			goto err_read;
		} else {
			clen = util_ntohl(clen);
			*comp_len += clen;
		}

		/* Skip checksum and seek forward to next block */
		fseek(fp, crc_size + clen, SEEK_CUR);
	}

err_read:
	fclose(fp);
err_open:

	return ret;

}

int lzo_decompress_buffer(uint8_t * src, uint32_t srclen, uint8_t * dst, uint32_t dstlen, uint32_t * decomp_len_p) {

	uint32_t comp_len, decomp_len, decomp = 0, overhead;
	int ret = -1, lzoret;

	uint8_t fname_len;
	uint32_t skip = 0;

	/* Validate buffer */
	if (!src || !dst || srclen < 1 || dstlen < 1) {
		log_error("Buffer error");
		goto out;
	}

	/* Read file name length */
	fname_len = src[HEADER_FNAME_OFFSET];

	/* Skip file name and checksum */
	skip = HEADER_FNAME_OFFSET + 1 + fname_len + sizeof(uint32_t);

	uint32_t header_flags = 0;
	memcpy(&header_flags, &src[HEADER_FLAGS_OFFSET], 4);
	header_flags = util_ntohl(header_flags);

	/* Determine CRC offset */
	uint32_t crc_size = 0;
	if ((header_flags & F_ADLER32_D) || (header_flags & F_CRC32_D))
		crc_size += 4;
	if ((header_flags & F_ADLER32_C) || (header_flags & F_CRC32_C))
		crc_size += 4;


	/* Decompress blocks */
	while (1) {
		/* Read decompressed block size */
		memcpy(&decomp_len, &src[skip], sizeof(decomp_len));
		decomp_len = util_ntohl(decomp_len);

		/* End of file is indicated when decompressed length equals zero */
		if (decomp_len == 0) {
			ret = 0;
			break;
		}

		skip += sizeof(uint32_t);

		/* Read compressed block size */
		memcpy(&comp_len, &src[skip], sizeof(comp_len));
		comp_len = util_ntohl(comp_len);
		skip += sizeof(uint32_t);

		/* Skip checksum */
		skip += crc_size;

		/* Validate buffer size */
		overhead = decomp_len / 16 + 64 + 3;
		if (dstlen < (decomp + decomp_len + overhead)) {
			log_error("Buffer not large enough for decompressed data %"PRIu32" %"PRIu32" %"PRIu32" %"PRIu32, dstlen, decomp, decomp_len, overhead);
			goto out;
		}

		/* Perform in-place decompression */
		lzoret = lzo1x_decompress_safe(&src[skip], comp_len, dst + decomp, (unsigned long int *) &decomp_len, NULL);
		if (lzoret != LZO_E_OK) {
			log_error("Decompression failed (ret %d)", lzoret);
			goto out;
		} else {
			decomp += decomp_len;
			skip += comp_len;
		}
		log_debug("Decompressed %"PRIu32" to %"PRIu32, comp_len, decomp_len);
	}

	if (decomp_len_p != NULL)
		*decomp_len_p = decomp;

out:
	return ret;

}

int lzo_decompress_image(char * path, void * dst, uint32_t dstlen) {

	int ret = -1;
	uint32_t header_len, comp_len, decomp_len;

	/* Validate buffer */
	if (dst == NULL || dstlen < 1) {
		log_error("Buffer error");
		goto err_open;
	}

	/* Read decompressed size */
	if (lzo_header(path, &header_len, &decomp_len, &comp_len) != 0) {
		log_error("Failed to read header");
		goto err_open;
	}

	/* Open file */
	FILE * fp = fopen(path, "r");
	if (!fp) {
		log_error("Cannot open %s", path);
		goto err_open;
	}

	struct stat statbuf;
	stat(path, &statbuf);

	/* Allocate buffer */
	if (dstlen < (decomp_len + 0x40000 / 16 + 64 + 3)) {
		log_error("Buffer error");
		goto err_read;
	}

	/* Read file into buffer */
	if (fread(dst + dstlen - statbuf.st_size, 1, statbuf.st_size, fp) != (size_t) statbuf.st_size) {
		log_error("Failed to read complete file");
		goto err_read;
	}

	/* Decompress */
	if (lzo_decompress_buffer(dst + dstlen - statbuf.st_size, statbuf.st_size, dst, dstlen, NULL) != 0) {
		log_error("Failed to decompress buffer");
		goto err_read;
	}

	ret = decomp_len;

err_read:
	fclose(fp);
err_open:
	return ret;

}

int lzo_write_header(lzo_header_t * head) {

	/* Init header */
	memcpy(head->magic, magic, 9);
	head->version = util_htons(0x1030);
	head->libversion = util_htons(0x2030);
	head->versionneeded = util_htons(0x0940);
	head->method = 1;
	head->level = 5;
	head->flags = util_htonl(F_H_CRC32);
	head->mode = 0;
	head->mtime_low = 0;
	head->mtime_high = 0;
	head->fnamelen = 0;

	/* Calculate checksum */
	lzo_uint32 lzo_crc32(lzo_uint32 c, const lzo_bytep buf, lzo_uint len);
	lzo_uint32 crc = CRC32_INIT_VALUE;
	char * in = (char *) &head->version;
	char * out = (char *) &head->checksum_header;
	while(in != out) {
		crc = lzo_crc32((lzo_uint32) crc, (const lzo_bytep) in, 1);
		in++;
	}
	head->checksum_header = util_htonl(crc);

	return 0;
}

int lzo_compress_buffer(void * src, uint32_t src_len, void *out, uint32_t *out_len, uint32_t out_size) {

	void write32(char * buf, uint32_t *offset, uint32_t data) {
	    uint32_t data_be = util_hton32(data);
	    memcpy(buf + *offset, &data_be, sizeof(uint32_t));
	    *offset += sizeof(uint32_t);
	}

	/* Init lzo */
	lzo_init();

	/* Allocate working memory */
	void * wrkmem = malloc(LZO1X_MEM_COMPRESS);
	if (wrkmem == NULL)
		return -1;

	/* Destination */
	unsigned long dst_len = 0;
	void * dst = malloc(BLOCK_SIZE);
	if (dst == NULL) {
		free(wrkmem);
		return -1;
	}

	*out_len = 0;

	/* Write header */
	lzo_write_header((void *) out);
	*out_len += sizeof(lzo_header_t);

	/* Compress blocks */
	unsigned int src_read = 0;
	unsigned int block_size;
	while(src_read < src_len) {

		/* Calculate block size */
		block_size = (src_len - src_read);
		if (block_size > BLOCK_SIZE)
			block_size = BLOCK_SIZE;

		/* Compress */
		int result = lzo1x_1_compress(src + src_read, block_size, dst, &dst_len, wrkmem);
		if (result < 0) {
			free(dst);
			free(wrkmem);
			return -1;
		}

		log_debug("Compressed %u to %lu", block_size, dst_len);

		/* Write lengths */
		write32(out, out_len, block_size);
		write32(out, out_len, dst_len);

		/* Write data */
		memcpy(out + *out_len, dst, dst_len);
		*out_len += dst_len;

		src_read += block_size;

	}

	/* Write EOF */
	write32(out, out_len, 0);

	/* Free mem */
	free(wrkmem);
	free(dst);

	return 0;

}
