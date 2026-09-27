#ifndef FRONTIERX_APP_STRING_TABLE_LOAD_H
#define FRONTIERX_APP_STRING_TABLE_LOAD_H

#include "Filesys.h"
#include "SaveLoad.h"
#include "StringTable.h"
#include "TSmartPointer.h"

namespace FrontierX
{
// The mission loader receives a DOS archive and loads its StringTable child.
inline bool LoadScriptStringTable(IFileSystem & archive, IStringTable & table)
{
    COMPTR<IFileSystem> root;
    DAFILEDESC descriptor = "\\";
    descriptor.lpImplementation = "DOS";
    descriptor.dwDesiredAccess = GENERIC_READ;
    descriptor.dwShareMode = 0;
    descriptor.dwCreationDistribution = OPEN_EXISTING;

    if (archive.CreateInstance(&descriptor, root) != GR_OK) return false;
    COMPTR<ISaverLoader> loader;
    if (table.QueryInterface("ISaverLoader", loader) != GR_OK) return false;
    root->SetCurrentDirectory("\\");
    return loader->Load(*root.ptr);
}
}

#endif
