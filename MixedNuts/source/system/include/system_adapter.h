#ifndef _SYSTEM_ADAPTER_H_
#define _SYSTEM_ADAPTER_H_

extern void SERVER_SLEEP(unsigned msec);

typedef FILE filetype;

typedef enum {
	mode_read	= 0x01,
	mode_write	= 0x02,
	mode_append	= 0x04
} filemode;

typedef enum {
	seek_set	= 0x01,
	seek_cur	= 0x02,
	seek_end	= 0x04
} seekpos;

int file_close(filetype *fp);
filetype* file_open(const char *filename, int mode);
size_t file_read(filetype *fp, void *buf, unsigned int size);
char* file_readline(filetype *fp, char *buf, int max);

#endif