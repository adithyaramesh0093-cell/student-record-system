#include "platform.h"

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

bool platform_replace_file(const char *from, const char *to)
{
    return MoveFileExA(from, to, MOVEFILE_REPLACE_EXISTING) != 0;
}

#else /* Linux, macOS, ... */

#include <stdio.h>

bool platform_replace_file(const char *from, const char *to)
{
    return rename(from, to) == 0;
}

#endif
