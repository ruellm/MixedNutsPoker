#include <stdio.h>
#include <string.h>
#include "system_adapter.h"
#ifdef WIN32
	#include <windows.h>
#endif
void SERVER_SLEEP(unsigned msec)
{
#ifdef WIN32
	Sleep(msec);
#else
	//TODO: add linux equivalent here
#endif
}
const char* build_mode_string(int mode)
{
	static char mode_str[4];
	char *ms = mode_str;
	
	if (mode & mode_read && mode & mode_write)
	{
		*ms++ = 'w';
		*ms++ = '+';
	}
	else if (mode & mode_read && mode & mode_append)
	{
		*ms++ = 'a';
		*ms++ = '+';
	}
	else
	{
		if (mode & mode_read)
			*ms++ = 'r';
		
		if (mode & mode_write)
			*ms++ = 'w';
		
		if (mode & mode_append)
			*ms++ = 'a';
	}
	
#if defined(WIN32)
	*ms++ = 'b';
#endif
	
	*ms = '\0';
	
	return mode_str;
}

filetype* file_open(const char *filename, int mode)
{
	return fopen(filename, build_mode_string(mode));
}

int file_close(filetype *fp)
{
	return fclose(fp);
}

size_t file_read(filetype *fp, void *buf, unsigned int size)
{
	return fread(buf, 1, size, fp);
}

size_t file_write(filetype *fp, const void *buf, unsigned int size)
{
	return fwrite(buf, 1, size, fp);
}

char* file_readline(filetype *fp, char *buf, int max)
{
	char *s;
	s = fgets(buf, max, fp);
	
	if (s)
	{
		// remove newline
		int length = strlen(buf);
		
		if (length >= 1 && (buf[length - 1] == '\n' || buf[length - 1] == '\r'))
			buf[length - 1] = '\0';
		if (length >= 2 && (buf[length - 2] == '\n' || buf[length - 2] == '\r'))
			buf[length - 2] = '\0';
		
	}
	
	return s;
}
