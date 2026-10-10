//--------------------------------------------------------------------------//
//                                                                          //
//                               StringData.cpp                             //
//                                                                          //
//                  COPYRIGHT (C) 1999 BY DIGITAL ANVIL, INC.               //
//                                                                          //
//--------------------------------------------------------------------------//
/*
    $Header: /Conquest/App/Src/StringData.cpp 4     4/26/00 5:20p Jasony $
*/			    
//---------------------------------------------------------------------------
#if defined(_WIN32)
#include "pch.h"
#include <globals.h>
#else
#include "Include/NativeAppGlobals.h"
#include "Include/NativeAppMissionMedia.h"
#include <cstring>
#include <cstdio>
#include <vector>
#include <array>
#include <limits>
#endif

#include "StringData.h"
#include "DTypes.h"
#include "UserDefaults.h"
#include "Startup.h"
#include "Cursor.h"
#include "CQTrace.h"

#include <TSmartPointer.h>
#include <TComponent.h>
#include <IConnection.h>
#include <Filesys.h>
#include <TConnPoint.h>
#include <TConnContainer.h>
#include <HeapObj.h>
#include <MemFile.h>
#if defined(_WIN32)
#include <Document.h>
#endif
#include <EventSys.h>

#include <stdlib.h>
#include <stdio.h>

#pragma warning (disable : 4200)	// zero sized array

//-----------------------------------------------------------------------------------
//--------------------------------------------------------------------------//
//
struct ARCHDATATYPE
{
	C8 name[32];
	M_STRING * objData;
	U32 dataSize;		// size of data chunk in bytes
};

//--------------------------------------------------------------------------//
//
struct ARCHDATA
{
	U32 numArchetypes;
	ARCHDATATYPE type[];
};
//--------------------------------------------------------------------------//
//---------------------------------StringData Class-------------------------//
//--------------------------------------------------------------------------//
//
struct DACOM_NO_VTABLE StringData : public IStringData, IEventCallback
{
	ARCHDATA *archData;							// pointer to loaded archetype database info

	//
	// Interface mapping
	//

	BEGIN_DACOM_MAP_INBOUND(StringData)
	DACOM_INTERFACE_ENTRY(IStringData)
	DACOM_INTERFACE_ENTRY(IEventCallback)
	END_DACOM_MAP()

	U32 eventHandle;

	StringData (void);

	~StringData (void);

    void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void   operator delete (void *ptr)
	{
		::free(ptr);
	}

	/* IStringData methods  */

	virtual void * GetArchetypeData (const C8 * name);

	virtual void * GetArchetypeData (const C8 * name, U32 & dataSize);		// also returns data size

	/* IEventCallback methods */

	DEFMETHOD(Notify) (U32 message, void *param);

	/* StringData methods */

#if defined(_WIN32)
	BOOL32 loadTypesData (void);
#else
    BOOL32 loadTypesData(IFileSystem *source);
#endif

	ARCHDATATYPE * getArchDataType (const C8 * name);

#if defined(_WIN32)
	static GENRESULT copyOpenFile (IFileSystem *file, char *fileName);
#endif

	IDAComponent * getBase (void)
	{
		return static_cast<IStringData *>(this);
	}
};
//--------------------------------------------------------------------------//
//
StringData::StringData (void)
{
}
//--------------------------------------------------------------------------//
//
StringData::~StringData (void)
{
	COMPTR<IDAConnectionPoint> connection;

	free(archData);
	archData = 0;


#if defined(_WIN32)
	if (eventHandle && GS && GS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
		connection->Unadvise(eventHandle);
#endif
}
//-------------------------------------------------------------------
//
void * StringData::GetArchetypeData (const C8 * name)
{
	ARCHDATATYPE * dataType;
		
	if ((dataType = getArchDataType(name)) != 0)
		return dataType->objData;
	else
		return 0;
}
//-------------------------------------------------------------------
//
void * StringData::GetArchetypeData (const C8 * name, U32 & dataSize)
{
	ARCHDATATYPE * dataType;
		
	if ((dataType = getArchDataType(name)) != 0)
	{
		dataSize = dataType->dataSize;
		return dataType->objData;
	}
	else
	{
		dataSize = 0;
		return 0;
	}
}
//-------------------------------------------------------------------
//
GENRESULT StringData::Notify (U32 message, void *param)
{
	/*
	switch(message)
	{
	case CQE_ENABLE3DMODE:
		FlushUnusedArchetypes();
		break;  // end case CQE_ENABLE3DMODE
	}
	*/

	return GR_OK;
}
//--------------------------------------------------------------------------//
// return total numbers of files, and cumulative size of all files
//
#if defined(_WIN32)
static void get_total_bytes (IFileSystem * file, U32 & dataSize, U32 & numFiles)
{
	WIN32_FIND_DATA data;
	HANDLE handle;
	
	if ((handle = file->FindFirstFile("*.*", &data)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			// make sure this not a silly "." entry
			if (data.cFileName[0] != '.' || strchr(data.cFileName, '\\') != 0)
			{
				if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (strcmp(data.cFileName, "Parsed Files"))
					{
						// traverse subdirectory
						if (file->SetCurrentDirectory(data.cFileName))
						{
							get_total_bytes(file, dataSize, numFiles);
							file->SetCurrentDirectory("..");	// restore current directory
						}
					}
				}
				else 
				{	
				 	dataSize += data.nFileSizeLow;
					numFiles++;
				}
			}

		} while (file->FindNextFile(handle, &data));

		file->FindClose(handle);
	}
}
//--------------------------------------------------------------------------//
//
static void load_bytes (IFileSystem * file, ARCHDATA * archData)
{
	WIN32_FIND_DATA data;
	HANDLE handle;
	DAFILEDESC fdesc=data.cFileName;
	
	if ((handle = file->FindFirstFile("*.*", &data)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			// make sure this not a silly "." entry
			if (data.cFileName[0] != '.' || strchr(data.cFileName, '\\') != 0)
			{
				if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					// traverse subdirectory
					if (strcmp(data.cFileName, "Parsed Files"))
					{
						if (file->SetCurrentDirectory(data.cFileName))
						{
							load_bytes(file, archData);
							file->SetCurrentDirectory("..");	// restore current directory
						}
					}
				}
				else 
				{	
					HANDLE hFile;

					if ((hFile = file->OpenChild(&fdesc)) != INVALID_HANDLE_VALUE)
					{
						U32 i = archData->numArchetypes++;
						DWORD dwRead;

						if (i != 0)
							archData->type[i].objData = (M_STRING*) (((U8 *)archData->type[i-1].objData) + archData->type[i-1].dataSize);

						strncpy(archData->type[i].name, data.cFileName, sizeof(archData->type[i].name)-1);
						archData->type[i].dataSize = file->GetFileSize(hFile);
						file->ReadFile(hFile, archData->type[i].objData, archData->type[i].dataSize, &dwRead, 0);
						file->CloseHandle(hFile);
					}
				}
			}

		} while (file->FindNextFile(handle, &data));

		file->FindClose(handle);
	}
}
//--------------------------------------------------------------------------//
//
BOOL32 StringData::loadTypesData (void)
{
	DAFILEDESC fdesc = "StringPack.db";
	U32 dataSize=0, numFiles=0;
	BOOL32 result = 0;
	COMPTR<IFileSystem> pFile;

	if (DACOM->CreateInstance(&fdesc, pFile) != GR_OK)
	{
		fdesc.lpFileName = "..\\DB\\StringPack.db";
		if (DACOM->CreateInstance(&fdesc, pFile) != GR_OK)
		{
			CQBOMB1("Could not access '%s'", fdesc.lpFileName);
			goto Done;
		}
	}

	get_total_bytes(pFile, dataSize, numFiles);
	
	if (numFiles)
	{
		archData = (ARCHDATA *) calloc(sizeof(ARCHDATA)+(sizeof(ARCHDATATYPE)*numFiles)+dataSize, 1);

		archData->type[0].objData = (M_STRING *) (((U8 *)archData) + sizeof(ARCHDATA)+(sizeof(ARCHDATATYPE)*numFiles)); // mark beginning of data
		load_bytes(pFile, archData);
	}
	
	result = 1;
	
Done:
	return result;
}
//--------------------------------------------------------------------------//
//
ARCHDATATYPE * StringData::getArchDataType (const C8 * name)
{
	if (archData)
	{
		ARCHDATATYPE * result = archData->type;
		U32 i = archData->numArchetypes;

		while (i)
		{
			if (strcmp(result->name, name) == 0)
				return result;
			result++;
			i--;
		}
	}

	return 0;
}
//-------------------------------------------------------------------
//
GENRESULT StringData::copyOpenFile (IFileSystem *file,char *fileName)
{
	GENRESULT result;
	COMPTR<IFileSystem> outFile, temp;
	DAFILEDESC fdesc = fileName;
	U32 dwSize = file->GetFileSize();
	U32 dwRead,dwWritten;
	void *buffer = malloc(dwSize);
	MEMFILEDESC mdesc = fileName;

	mdesc.lpBuffer = buffer;
	mdesc.dwBufferSize = dwSize;
	mdesc.dwFlags = CMF_DONT_COPY_MEMORY;
	mdesc.dwDesiredAccess = GENERIC_READ |GENERIC_WRITE;
	mdesc.dwShareMode = 0;
	mdesc.dwCreationDistribution = OPEN_EXISTING;

	file->SetFilePointer(0,0);
	file->ReadFile(0,buffer,dwSize,&dwRead,0);

	if ((result = CreateUTFMemoryFile(mdesc, temp)) != GR_OK)
		return result;

	temp.free();		// clears all of the sharing flags

	fdesc.lpImplementation = "DOS";
	fdesc.dwDesiredAccess = GENERIC_READ |GENERIC_WRITE;
	fdesc.dwShareMode = 0;
	fdesc.dwCreationDistribution = CREATE_ALWAYS;

	if ((result = DACOM->CreateInstance(&fdesc,outFile)) == GR_OK)
	{
		outFile->WriteFile(0,buffer,dwRead,&dwWritten,0);
	}

	free(buffer);

	return result;
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//

struct _stringlist : GlobalComponent
{
	StringData * list;

	virtual void Startup (void)
	{
		STRINGDATA = list = new DAComponent<StringData>;
		AddToGlobalCleanupList((IDAComponent **) &STRINGDATA);
	}

	virtual void Initialize (void)
	{
		COMPTR<IDAConnectionPoint> connection;

		if (list->loadTypesData() == 0)
			CQBOMB0("Load failed on string database.");

		if (GS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
			connection->Advise(list->getBase(), &list->eventHandle);
	}
};

static _stringlist stringlist;

//-------------------------------------------------------------------
//-------------------------END StringData.cpp---------------------------
//-------------------------------------------------------------------
#else
namespace
{
struct NativeStringRecord
{
    std::array<char, 32> name{};
    std::vector<U8> bytes;
};

// StringData owns opaque payloads, including editor source files. UnitComm
// validates the fixed-width speech slots it consumes. Runtime pointers never
// come from UTF payloads; preserve source traversal and bare-name lookup.
bool ReadNativeStrings(IFileSystem *source, std::vector<NativeStringRecord> &records,
                       std::size_t &total, unsigned depth)
{
    if (depth > 32) return false;
    WIN32_FIND_DATA data{};
    const HANDLE search = source->FindFirstFile("*.*", &data);
    if (search == INVALID_HANDLE_VALUE) return true;
    bool valid = true;
    do
    {
        if (data.cFileName[0] == '.') continue;
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (std::strcmp(data.cFileName, "Parsed Files") == 0) continue;
            if (!source->SetCurrentDirectory(data.cFileName)) { valid = false; break; }
            valid = ReadNativeStrings(source, records, total, depth + 1);
            if (!source->SetCurrentDirectory("..")) valid = false;
            if (!valid) break;
            continue;
        }
        const std::size_t size = data.nFileSizeLow;
        if (data.nFileSizeHigh ||
            size > 64U * 1024U * 1024U - total || records.size() >= 65536 ||
            std::strlen(data.cFileName) >= 32)
        { std::fprintf(stderr, "Native StringPack invalid record: %s; bytes %zu\n", data.cFileName, size); valid = false; break; }
        for (const auto &record : records)
            if (std::strcmp(record.name.data(), data.cFileName) == 0)
            { std::fprintf(stderr, "Native StringPack duplicate name: %s\n", data.cFileName); valid = false; }
        if (!valid) break;
        NativeStringRecord record;
        std::strcpy(record.name.data(), data.cFileName);
        record.bytes.resize(size);
        DAFILEDESC desc(data.cFileName);
        const HANDLE file = source->OpenChild(&desc);
        if (file == INVALID_HANDLE_VALUE) { valid = false; break; }
        DWORD read = 0;
        valid = source->ReadFile(file, record.bytes.data(), static_cast<DWORD>(size), &read, nullptr) && read == size;
        if (!source->CloseHandle(file)) valid = false;
        if (!valid)
        { std::fprintf(stderr, "Native StringPack incomplete record: %s\n", data.cFileName); break; }
        total += size;
        records.push_back(std::move(record));
    } while (source->FindNextFile(search, &data));
    if (!source->FindClose(search)) valid = false;
    return valid;
}
}

BOOL32 StringData::loadTypesData(IFileSystem *source)
{
    if (!source || archData) return FALSE;
    std::vector<NativeStringRecord> records;
    std::size_t total = 0;
    if (!ReadNativeStrings(source, records, total, 0) || records.empty()) return FALSE;
    const std::size_t headerSize = sizeof(ARCHDATA) + sizeof(ARCHDATATYPE) * records.size();
    auto *loaded = static_cast<ARCHDATA *>(std::calloc(headerSize + total, 1));
    if (!loaded) return FALSE;
    U8 *payload = reinterpret_cast<U8 *>(loaded) + headerSize;
    for (const auto &record : records)
    {
        auto &entry = loaded->type[loaded->numArchetypes++];
        std::memcpy(entry.name, record.name.data(), record.name.size());
        entry.objData = reinterpret_cast<M_STRING *>(payload);
        entry.dataSize = static_cast<U32>(record.bytes.size());
        if (!record.bytes.empty()) std::memcpy(payload, record.bytes.data(), record.bytes.size());
        payload += record.bytes.size();
    }
    archData = loaded;
    return TRUE;
}

ARCHDATATYPE *StringData::getArchDataType(const C8 *name)
{
    if (archData && name)
        for (U32 index = 0; index < archData->numArchetypes; ++index)
            if (std::strcmp(archData->type[index].name, name) == 0)
                return &archData->type[index];
    return nullptr;
}

namespace FrontierX
{
IStringData *CreateNativeStringData(IFileSystem *source)
{
    auto *owner = new DAComponent<StringData>;
    if (!owner->loadTypesData(source)) { owner->Release(); return nullptr; }
    return owner;
}
}
#endif
