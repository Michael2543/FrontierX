#ifndef FRONTIERX_NATIVE_APP_PROFILE_PARSER_H
#define FRONTIERX_NATIVE_APP_PROFILE_PARSER_H

#include <FrontierX/LegacyFileTypes.h>
#include <DACOM.H>

struct IFileSystem;
struct IProfileParser2;

GENRESULT __stdcall CreateProfileParser(const char *filename,
    IProfileParser2 **parser, IFileSystem *parent = nullptr);

#endif
