#include "CQImage.h"

#if !defined(_WIN32)
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

bool ICQImage::Assert(const char * expression, void * file, unsigned line)
{
    std::fprintf(stderr, "Native render assertion: %s (%s:%u)\n",
        expression, static_cast<const char *>(file), line);
    std::abort();
}

bool DACOM_CDECL ICQImage::Bomb(const char * expression, ...)
{
    std::fputs("Native render fatal diagnostic: ", stderr);
    va_list arguments;
    va_start(arguments, expression);
    std::vfprintf(stderr, expression, arguments);
    va_end(arguments);
    std::fputc('\n', stderr);
    std::abort();
}

bool DACOM_CDECL ICQImage::Error(const char * expression, ...)
{
    std::fputs("Native App error: ", stderr);
    va_list arguments;
    va_start(arguments, expression);
    std::vfprintf(stderr, expression, arguments);
    va_end(arguments);
    std::fputc('\n', stderr);
    return false;
}
#endif
