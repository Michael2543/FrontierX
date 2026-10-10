#include "Include/NativeAppGlobals.h"
#include "Include/NativeAppProfileParser.h"

#include <Filesys.h>
#include <IProfileParser.h>
#include <TSmartPointer.h>

#include <new>
#include <vector>

GENRESULT __stdcall CreateProfileParser(const char *filename,
    IProfileParser2 **output, IFileSystem *parent)
{
    if (!output) return GR_INVALID_PARMS;
    *output = nullptr;
    if (!filename || !filename[0] || !DACOM) return GR_INVALID_PARMS;
    if (!parent) parent = PROFILEDIR;
    if (!parent) return GR_INVALID_PARMS;

    DAFILEDESC descriptor(filename);
    const HANDLE file = parent->OpenChild(&descriptor);
    if (file == INVALID_HANDLE_VALUE) return GR_GENERIC;
    const DWORD length = parent->GetFileSize(file, nullptr);
    if (!length || length == DWORD(-1) || length > 16U * 1024U * 1024U)
    {
        parent->CloseHandle(file);
        return GR_GENERIC;
    }
    std::vector<char> bytes;
    try { bytes.resize(length); }
    catch (const std::bad_alloc &)
    {
        parent->CloseHandle(file);
        return GR_OUT_OF_MEMORY;
    }
    DWORD read = 0;
    const bool loaded = parent->ReadFile(file, bytes.data(), length, &read, nullptr) &&
                        read == length;
    const bool closed = parent->CloseHandle(file) != FALSE;
    if (!loaded || !closed) return GR_GENERIC;

    PROFPARSEDESC2 profileDescriptor;
    COMPTR<IProfileParser2> parser;
    GENRESULT result = DACOM->CreateInstance(&profileDescriptor, parser);
    if (result != GR_OK) return result;
    result = parser->Initialize2(bytes.data(), length);
    if (result != GR_OK) return result;
    *output = parser;
    parser->AddRef();
    return GR_OK;
}
