#include <libelf.h>
#include <gelf.h>
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <malloc.h>
#include <command/command.h>

static struct readelf {
	int node;
	Elf * e;
	char file[50];
} elfs[10] = {};

static int elfs_next = 0;

int log_readelf_open(int node, char * file) {

	int fd = open(file, 0, 0);
	if (fd < 0) {
		printf("%s\r\n", strerror(errno));
		return -1;
	}

	if (elf_version(EV_CURRENT) == EV_NONE) {
		printf("%s\r\n", elf_errmsg(elf_errno()));
		return -1;
	}

	int idx = elfs_next;

	elfs[idx].e = elf_begin(fd, ELF_C_READ, NULL);
	if (elfs[idx].e == NULL) {
		printf("%s\r\n", elf_errmsg(elf_errno()));
		return -1;
	}

	elfs[idx].node = node;
	strncpy(elfs[idx].file, file, 50);
	elfs_next++;

	return 0;

}

static struct readelf * log_readelf_findelf(int node) {
	for (int i = 0; i < elfs_next; i++) {
		if (elfs[i].node == node)
			return &elfs[i];
	}
	return NULL;
}

char * log_readelf_findstr(int node, void * ptr) {

	/* Find elf */
	struct readelf * re = log_readelf_findelf(node);
	if (re == NULL)
		return NULL;

	/* Loop sections */
	Elf_Scn * scn = NULL;
	while ((scn = elf_nextscn(re->e , scn)) != NULL ) {

		GElf_Shdr shdr;
		if (gelf_getshdr(scn, &shdr) != &shdr) {
			printf("gelf_getshdr: %s\r\n", elf_errmsg(elf_errno()));
			return NULL;
		}

		ptrdiff_t section_begin = shdr.sh_addr;
		ptrdiff_t section_end = shdr.sh_addr + shdr.sh_size;

		if ((ptrdiff_t) ptr > section_begin && (ptrdiff_t) ptr <= section_end) {
			Elf_Data * data = NULL;
			data = elf_getdata(scn, data);
			if (data == NULL) {
				printf("%s\r\n", elf_errmsg(elf_errno()));
				return NULL;
			}

			return (char *) data->d_buf + (ptrdiff_t) ptr - section_begin;

		}

	}

	return NULL;

}

int cmd_log_readelf_open(struct command_context *ctx) {

	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;

	int node = atoi(ctx->argv[1]);
	char * file = ctx->argv[2];

	if (log_readelf_open(node, file) < 0)
		return CMD_ERROR_FAIL;

	return CMD_ERROR_NONE;

}

int cmd_log_readelf_findstr(struct command_context *ctx) {

	if (ctx->argc != 3)
		return CMD_ERROR_SYNTAX;

	int node = atoi(ctx->argv[1]);
	void * ptr;
	sscanf(ctx->argv[2], "%p", &ptr);

	printf("Searching for %p at node %d...\r\n", ptr, node);

	char * str = log_readelf_findstr(node, ptr);

	if (str == NULL) {
		printf("Not found\r\n");
		return CMD_ERROR_NONE;
	}

	printf("Found: %s\r\n", str);

	return CMD_ERROR_NONE;

}

command_t __sub_command log_readelf_cmds[] = {
		{
				.name = "open",
				.help = "Parses .elf file for a given node",
				.usage = "<node> <file>",
				.handler = cmd_log_readelf_open,
		},{
				.name = "scan",
				.help = "Tests the scan function",
				.usage = "<node> <addr>",
				.handler = cmd_log_readelf_findstr,
		}
};

command_t __root_command log_readelf_cmd[] = {
		{
				.name = "readelf",
				.help = "Read strings from elf file",
				.chain = INIT_CHAIN(log_readelf_cmds),
		}
};
