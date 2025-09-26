#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "tz_boot_param.h"
#include "tz_boot_param_value.h"

static void dump_mem_region(char *out_file, const struct tz_boot_param_list *bp)
{
	const struct mem_region * mr;
	int mr_num;
	int i = 0;
	FILE *file;
	char *file_mr;

	if(bp->id != TZ_BOOT_PARAM_MEMREGION)
		return;

	file_mr = malloc(strlen(out_file) + 10);
	if (!file_mr)
		return;

	sprintf(file_mr, "%s.mr", out_file);

	file = fopen(file_mr, "w");
	if (file == NULL) {
		printf("fail to open %s", file_mr);
		free(file_mr);
		return;
	}
	mr = bp->data;
	mr_num = bp->size / sizeof(struct mem_region);

	for (i = 0; i < mr_num; i++){
		printf("%s: base = 0x%08x, size = 0x%08x\n", mr[i].name, mr[i].base, mr[i].size);
		fprintf(file, "%s 0x%x 0x%x\n", mr[i].name, mr[i].base, mr[i].size);
	}

	fclose(file);
	free(file_mr);
}

int main(int argc, char **argv)
{
	char				*out_file = 0;
	int				fd, ret = -1;
	struct tz_boot_param_header	param_header;
	struct tz_boot_param		param;
	uint32_t			total_size = 0;
	int i = 0;

	if (argc != 2)
	{
		fprintf(stderr,"usage: %s <filename>\n", argv[0]);
		return ret;
	}
	out_file = argv[1];

	fd = open(out_file, O_CREAT | O_TRUNC | O_WRONLY, 0644);
	if(fd < 0) {
		fprintf(stderr,"error: could not create '%s'\n", out_file);
		return ret;
	}

	if(lseek(fd, sizeof(param_header), SEEK_SET) != sizeof(param_header))
		goto END;

	for (i = 0; i < bp_num; i++) {
		param.id = bp_list[i].id;
		param.size = bp_list[i].size + sizeof(param);
		dump_mem_region(out_file, bp_list + i);
		if (write(fd, &param, sizeof(param)) != sizeof(param))
			goto END;
		if (write(fd, bp_list[i].data, bp_list[i].size) != bp_list[i].size)
			goto END;
		total_size += param.size;
	}

	param_header.magic_num = TZ_BOOT_PARAM_MAGIC_NUM;
	param_header.size = total_size + sizeof(param_header);
	param_header.version = TZ_BOOT_PARAM_VERSION;
	param_header.reserved = 0;

	if(lseek(fd, 0, SEEK_SET) != 0)
		goto END;
	if (write(fd, &param_header, sizeof(param_header)) != sizeof(param_header))
		goto END;

	printf("Generated boot parameter in %s successfully.\n", out_file);
	ret = 0;
END:
    close(fd);
    return ret;
}
