#include <cstdarg>
#include <cstring>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include "StringHelper.h"

using namespace std;

std::string format_arg_list(const char *fmt, va_list args)
{
    if (!fmt) return "";
    int   result = -1, length = BUFF_SIZE;
    char *buffer = 0;
    while (result == -1)
    {
        if (buffer) delete [] buffer;
        buffer = new char [length + 1];
        memset(buffer, 0, length + 1);
		#if defined(WIN32)
	        result = _vsnprintf(buffer, length, fmt, args);
		#else
			result = vsnprintf(buffer, length, fmt, args);
		#endif
        length *= 2;
    }
    std::string s(buffer);
    delete [] buffer;
    return s;
}

std::string format(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string s = format_arg_list(fmt, args);
    va_end(args);
    return s;
}
