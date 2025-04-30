#include "log.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <stdio.h>

void PrintLog(const char format [], ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
}

void PrintError(const char format [], ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}
