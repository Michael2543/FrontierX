//--------------------------------------------------------------------------//
//                                                                          //
//                               GenData.cpp                                //
//                                                                          //
//                  COPYRIGHT (C) 1998 BY DIGITAL ANVIL, INC.               //
//                                                                          //
//--------------------------------------------------------------------------//
/*
    $Header: /Conquest/App/Src/GenData.cpp 24    8/31/00 6:42p Jasony $
*/			    
//---------------------------------------------------------------------------
#if defined(_WIN32)
#include "pch.h"
#include <globals.h>
#else
#include "Include/NativeAppGlobals.h"
#endif

#include "GenData.h"
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
#include <Document.h>
#include <EventSys.h>
#ifndef _WIN32
#include <DGlobalData.h>
#include "Include/NativeGenData.h"
#endif

#include <stdlib.h>
#include <stdio.h>

#pragma warning (disable : 4200)	// zero sized array

//-----------------------------------------------------------------------------------
//--------------------------------------------------------------------------//
//
struct ARCHDATATYPE
{
	C8 name[32];
	GENBASE_DATA * objData;
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
//
struct GENNODE 
{
	struct GENNODE * prev, * next;
	U32 usage;
	ARCHDATATYPE * archDataType;
	COMPTR<ICQFactory> factory;
	HANDLE hArchetype;

	GENNODE (void)
	{
		memset(this, 0, sizeof(*this));
	}

private:
	// not allowed
	GENNODE & operator = (const GENNODE & obj)
	{
		return *this;
	}
};
//--------------------------------------------------------------------------//
//---------------------------------GenData Class----------------------------//
//--------------------------------------------------------------------------//
//
struct DACOM_NO_VTABLE GenData : public IGeneralData, ConnectionPointContainer<GenData>, IEventCallback
{
	GENNODE *archList;							// pointer to first GENNODE instance in archetype list
	ARCHDATA *archData;							// pointer to loaded archetype database info

	//
	// Interface mapping
	//

	BEGIN_DACOM_MAP_INBOUND(GenData)
	DACOM_INTERFACE_ENTRY(IGeneralData)
	DACOM_INTERFACE_ENTRY(IEventCallback)
	DACOM_INTERFACE_ENTRY(IDAConnectionPointContainer)
	DACOM_INTERFACE_ENTRY2(IID_IDAConnectionPointContainer, IDAConnectionPointContainer)
	END_DACOM_MAP()

	BEGIN_DACOM_MAP_OUTBOUND(GenData)
	DACOM_INTERFACE_ENTRY_AGGREGATE("ICQFactory", point)
	END_DACOM_MAP()

	ConnectionPoint<GenData,ICQFactory> point;
	COMPTR<IFileSystem> pFile;
	COMPTR<IDocument> pDoc;

	U32 eventHandle;

	GenData (void);

	~GenData (void);

    void * operator new (size_t size)
	{
		return calloc(size, 1);
	}

	void   operator delete (void *ptr)
	{
		::free(ptr);
	}

	/* IGeneralData methods  */

	virtual PGENTYPE GetArchetype (const C8 *name);

	virtual PGENTYPE LoadArchetype (const C8 *name);

	virtual BOOL32 UnloadArchetype (const C8 *name);

	virtual GENRESULT CreateInstance (PGENTYPE pArchetype, IDAComponent **pInstance);

	virtual GENRESULT CreateInstance (const char *name, IDAComponent **pInstance);

	virtual const char * GetArchName (PGENTYPE pArchetype);

	virtual void * GetArchetypeData (PGENTYPE pArchetype);

	virtual void * GetArchetypeData (const C8 * name);

	virtual void * GetArchetypeData (const C8 * name, U32 & dataSize);		// also returns data size

	virtual void AddRef (PGENTYPE pArchetype);		// add to the usage count

	virtual void Release (PGENTYPE pArchetype);		// decrement the usage count

	virtual GENRESULT GetDataFile (struct IDocument ** ppDocument);

	virtual void FlushUnusedArchetypes (void);

	/* IEventCallback methods */

	DEFMETHOD(Notify) (U32 message, void *param);

	/* GenData methods */

	void unloadArchetype (GENNODE * node);

	BOOL32 loadTypesData (IFileSystem * nativeSource = 0);

	ARCHDATATYPE * getArchDataType (const C8 * name);

	static GENRESULT copyOpenFile (IFileSystem *file);

	IDAComponent * getBase (void)
	{
		return static_cast<IGeneralData *>(this);
	}
};
//--------------------------------------------------------------------------//
//
GenData::GenData (void) : point(0)
{
}
//--------------------------------------------------------------------------//
//
GenData::~GenData (void)
{
	COMPTR<IDAConnectionPoint> connection;

	FlushUnusedArchetypes();

	free(archData);
	archData = 0;

#ifdef _WIN32
Retry:
	if (pDoc != 0 && pDoc->IsModified() && MessageBox(hMainWindow, "Save GenData?", "Data has changed", MB_YESNO) == IDYES)
	{
		if (copyOpenFile(pFile) != GR_OK)
		{
			CQERROR0("Failed write to GenData.db.");
			goto Retry;
		}
	}
#endif

	if (GS && GS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
		connection->Unadvise(eventHandle);
}
//-------------------------------------------------------------------
//
PGENTYPE GenData::GetArchetype (const C8 *name)
{
	GENNODE *list = archList;

	while (list)
	{
		if (strcmp(name, list->archDataType->name) == 0)
			break;
		list = list->next;
	}
	
	return list;
}
//-------------------------------------------------------------------
//
PGENTYPE GenData::LoadArchetype (const C8 *name)
{
	PGENTYPE result;

	if ((result = GetArchetype(name)) == 0)
	{
		ARCHDATATYPE * dataType;
		
		if ((dataType = getArchDataType(name)) != 0)
		{
			CONNECTION_NODE<ICQFactory> *node= point.pClientList;
			HANDLE handle=0;

			result = new GENNODE;
			result->prev=0;
			if ((result->next=archList) != 0)
				archList->prev = result;
			result->archDataType = dataType;
			archList = result;

			if (FILE *trace = fopen("debug/gendata_diag.txt", "a"))
			{
				fprintf(trace, "begin name=%s result=%p factory=%p\n",
					name, (void *)result, (void *)result->factory.ptr);
				fclose(trace);
			}

#ifdef _WIN32
			CURSOR->SetBusy(1);
#endif
			
			while (node)
			{
				if ((handle = node->client->CreateArchetype(result, dataType->objData->type, dataType->objData)) != 0)
					break;
				node = node->pNext;
			}

			if (node)
			{
				if (FILE *trace = fopen("debug/gendata_diag.txt", "a"))
				{
					fprintf(trace, "created name=%s result=%p factory=%p client=%p handle=%p\n",
						name, (void *)result, (void *)result->factory.ptr,
						(void *)node->client, handle);
					fclose(trace);
				}
				result->factory = node->client;
				result->hArchetype = handle;
			}
			else
			{
				unloadArchetype(result);
				result = 0;
				CQERROR1("No provider found for '%s'", dataType->name);
			}

#ifdef _WIN32
			CURSOR->SetBusy(0);
#endif
		}
		else
		{
			if (name[0])
				CQERROR1("Invalid archetype name: '%s'", name);
		}
	}

	return result;
}
//-------------------------------------------------------------------
//
BOOL32 GenData::UnloadArchetype (const C8 *name)
{
	BOOL32 result=0;
	GENNODE *list = archList;

	while (list)
	{
		if (strcmp(name, list->archDataType->name) == 0)
		{
			unloadArchetype(list);
			result = 1;
			break;
		}
		list = list->next;
	}
	
	return result;
}
//-------------------------------------------------------------------
//
GENRESULT GenData::CreateInstance (PGENTYPE pArchetype, IDAComponent **pInstance)
{
	GENRESULT result;
	GENNODE * node = pArchetype;

	CQASSERT(pArchetype);
	if (!node)
		return GR_GENERIC;

	if (!node->factory)
		return GR_GENERIC;

	result = node->factory->CreateInstance(node->hArchetype, pInstance);

	if (result == GR_OK)
	{
		node->usage++;
	}

	return result;
}
//-------------------------------------------------------------------
//
GENRESULT GenData::CreateInstance (const char *name, IDAComponent **pInstance)
{
	GENNODE * node = LoadArchetype(name);
	if (!node) { if (pInstance) *pInstance = 0; return GR_GENERIC; }
	return CreateInstance(node, pInstance);
}
//-------------------------------------------------------------------
//
const char * GenData::GetArchName (PGENTYPE pArchetype)
{
	if (pArchetype)
		return pArchetype->archDataType->name;
	else
		return 0;
}
//-------------------------------------------------------------------
//
void * GenData::GetArchetypeData (PGENTYPE pArchetype)
{
	if (pArchetype)
		return pArchetype->archDataType->objData;
	else
		return 0;
}
//-------------------------------------------------------------------
//
void * GenData::GetArchetypeData (const C8 * name)
{
	ARCHDATATYPE * dataType;
		
	if ((dataType = getArchDataType(name)) != 0)
		return dataType->objData;
	else
		return 0;
}
//-------------------------------------------------------------------
//
void * GenData::GetArchetypeData (const C8 * name, U32 & dataSize)
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
void GenData::AddRef (PGENTYPE pArchetype)
{
	pArchetype->usage++;
}
//-------------------------------------------------------------------
//
void GenData::Release (PGENTYPE pArchetype)
{
	pArchetype->usage--;
	CQASSERT(S32(pArchetype->usage) >= 0);
}
//-------------------------------------------------------------------
//
GENRESULT GenData::GetDataFile (IDocument ** ppDocument)
{
	if ((*ppDocument = pDoc) != 0)
	{	
		pDoc->AddRef();
		return GR_OK;
	}
	return GR_GENERIC;
}
//-------------------------------------------------------------------
//
void GenData::FlushUnusedArchetypes (void)
{
	GENNODE * node = archList;

	while (node)
	{
		if (node->usage == 0)
		{
			unloadArchetype(node);
			node = archList;
		}
		else
			node = node->next;
	}
}
//-------------------------------------------------------------------
//
GENRESULT GenData::Notify (U32 message, void *param)
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
//-------------------------------------------------------------------
//
void GenData::unloadArchetype (GENNODE * node)
{
	if (node->factory)
		node->factory->DestroyArchetype(node->hArchetype);

	if (node == archList)
		archList = node->next;
	if (node->next)
		node->next->prev = node->prev;
	if (node->prev)
		node->prev->next = node->next;

	delete node;
}
//--------------------------------------------------------------------------//
// return total numbers of files, and cumulative size of all files
//
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
				#ifndef _WIN32
					if (dataSize > 64U * 1024U * 1024U ||
						data.nFileSizeLow > 64U * 1024U * 1024U - dataSize)
						dataSize = ~U32(0);
					else
				#endif
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
static U32 calcCheckSum (const U8 * buffer, U32 bufferSize, U32 checkSum)
{
	while (bufferSize-- > 0)
	{
		checkSum += *buffer++;
	}
	return checkSum;
}
//--------------------------------------------------------------------------//
//
static void load_bytes (IFileSystem * file, ARCHDATA * archData, U32 & checkSum)
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
							load_bytes(file, archData, checkSum);
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
							archData->type[i].objData = (GENBASE_DATA *) (((U8 *)archData->type[i-1].objData) + archData->type[i-1].dataSize);

						strncpy(archData->type[i].name, data.cFileName, sizeof(archData->type[i].name)-1);
						archData->type[i].dataSize = file->GetFileSize(hFile);
						file->ReadFile(hFile, archData->type[i].objData, archData->type[i].dataSize, &dwRead, 0);
						file->CloseHandle(hFile);
						checkSum = calcCheckSum((U8 *)archData->type[i].objData, dwRead, checkSum);
					}
				}
			}

		} while (file->FindNextFile(handle, &data));

		file->FindClose(handle);
	}
}
//--------------------------------------------------------------------------//
//
#ifndef _WIN32
// Validate the production archive traversal before exposing its archetypes.
static bool load_bytes_native (IFileSystem * file, ARCHDATA * archData,
	U32 maxFiles, U32 maxBytes, U32 & bytesUsed, U32 & checkSum)
{
	WIN32_FIND_DATA data;
	HANDLE search = file->FindFirstFile("*.*", &data);
	if (search == INVALID_HANDLE_VALUE)
		return file->GetLastError() == ERROR_FILE_NOT_FOUND;
	bool valid = true;
	do
	{
		if (data.cFileName[0] == '.' && strchr(data.cFileName, '\\') == 0)
			continue;
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(data.cFileName, "Parsed Files") == 0)
				continue;
			valid = file->SetCurrentDirectory(data.cFileName) &&
				load_bytes_native(file, archData, maxFiles, maxBytes, bytesUsed, checkSum);
			valid = file->SetCurrentDirectory("..") && valid;
			continue;
		}
		if (archData->numArchetypes >= maxFiles || bytesUsed > maxBytes ||
			data.nFileSizeLow > maxBytes - bytesUsed)
		{
			valid = false;
			break;
		}
		DAFILEDESC descriptor = data.cFileName;
		HANDLE child = file->OpenChild(&descriptor);
		if (child == INVALID_HANDLE_VALUE)
		{
			valid = false;
			break;
		}
		const U32 size = file->GetFileSize(child);
		U32 read = 0;
		U8 * target = (U8 *)archData->type[0].objData + bytesUsed;
		valid = size == data.nFileSizeLow &&
			file->ReadFile(child, target, size, &read, 0) && read == size;
		valid = file->CloseHandle(child) && valid;
		if (!valid) break;
		ARCHDATATYPE & entry = archData->type[archData->numArchetypes++];
		strncpy(entry.name, data.cFileName, sizeof(entry.name)-1);
		entry.objData = (GENBASE_DATA *)target;
		entry.dataSize = size;
		bytesUsed += size;
		checkSum = calcCheckSum(target, size, checkSum);
	} while (valid && file->FindNextFile(search, &data));
	valid = valid && file->GetLastError() == ERROR_NO_MORE_FILES;
	return file->FindClose(search) && valid;
}
#endif
BOOL32 GenData::loadTypesData (IFileSystem * nativeSource)
{
#ifndef _WIN32
	if (!nativeSource || !nativeSource->SetCurrentDirectory("\\"))
		return 0;
	U32 dataSize = 0, numFiles = 0;
	get_total_bytes(nativeSource, dataSize, numFiles);
	if (numFiles == 0 || numFiles > 16384 || dataSize == 0 ||
		dataSize > 64U * 1024U * 1024U)
		return 0;
	const size_t allocationSize = sizeof(ARCHDATA) +
		size_t(sizeof(ARCHDATATYPE)) * numFiles + dataSize;
	archData = (ARCHDATA *) calloc(allocationSize, 1);
	if (!archData) return 0;
	archData->type[0].objData = (GENBASE_DATA *)
		(((U8 *)archData) + sizeof(ARCHDATA) + sizeof(ARCHDATATYPE) * numFiles);
	U32 checkSum = 0;
	U32 bytesUsed = 0;
	const bool loaded = load_bytes_native(nativeSource, archData,
		numFiles, dataSize, bytesUsed, checkSum);
	U32 globalSize = 0;
	const bool complete = loaded && bytesUsed == dataSize &&
		archData->numArchetypes == numFiles &&
		GetArchetypeData("GlobalValues", globalSize) != 0 &&
		globalSize == sizeof(GT_GLOBAL_VALUES);
	if (!complete)
	{
		free(archData);
		archData = 0;
		return 0;
	}
	return 1;
#else
	(void)nativeSource;
	DAFILEDESC fdesc = "GenData.db";
	U32 dataSize=0, numFiles=0, checkSum=0;
	BOOL32 result = 0;
	COMPTR<IFileSystem> file, pMemFile;
	HANDLE hMapping = 0;
	void * pImage = 0;
	U32 size = 0;

	fdesc.lpImplementation = "DOS";
	if (DACOM->CreateInstance(&fdesc, file) != GR_OK)
	{
		fdesc.lpFileName = "..\\DB\\GenData.db";
		if (DACOM->CreateInstance(&fdesc, file) != GR_OK)
		{
			CQBOMB1("Could not access '%s'", fdesc.lpFileName);
			goto Done;
		}
	}

	hMapping = file->CreateFileMapping();
	pImage = file->MapViewOfFile(hMapping);
	size = file->GetFileSize();

	if (CQFLAGS.bNoGDI==0)
	{
		//
		// create a memory file
		//
		MEMFILEDESC mdesc = fdesc.lpFileName;
		mdesc.lpBuffer = pImage;
		mdesc.dwBufferSize = size;
		mdesc.dwFlags = 0;
		mdesc.dwDesiredAccess |= GENERIC_WRITE;

		if (CreateUTFMemoryFile(mdesc, pFile) != GR_OK)
			CQBOMB0("Could not create memory file");

		//
		// create a document
		//
		DOCDESC ddesc = fdesc.lpFileName;

		ddesc.lpImplementation = "DOS";
		ddesc.dwShareMode = 0;		 // no sharing
		ddesc.dwCreationDistribution = CREATE_ALWAYS;

		ddesc.lpParent = pFile;
		pFile->AddRef();

		if (DACOM->CreateInstance(&ddesc, pDoc) != GR_OK)
			CQBOMB0("Could not create document");

		pMemFile = pFile;
	}
	else
	{
		//
		// create a memory file
		//
		MEMFILEDESC mdesc = fdesc.lpFileName;
		mdesc.lpBuffer = pImage;
		mdesc.dwBufferSize = size;
		mdesc.dwFlags = CMF_DONT_COPY_MEMORY;

		if (CreateUTFMemoryFile(mdesc, pMemFile) != GR_OK)
			CQBOMB0("Could not create memory file");
	}
	
	get_total_bytes(pMemFile, dataSize, numFiles);
	
	archData = (ARCHDATA *) calloc(sizeof(ARCHDATA)+(sizeof(ARCHDATATYPE)*numFiles)+dataSize, 1);

	archData->type[0].objData = (GENBASE_DATA *) (((U8 *)archData) + sizeof(ARCHDATA)+(sizeof(ARCHDATATYPE)*numFiles)); // mark beginning of data
	load_bytes(pMemFile, archData, checkSum);
#ifdef _WIN32
	SendMessage(hMainWindow, CQE_DATA_CHECKSUM, 1, checkSum);
#endif

	pMemFile.free();
	file->UnmapViewOfFile(pImage);
	file->CloseHandle(hMapping);
	
	result = 1;
Done:
	return result;
#endif
}
//--------------------------------------------------------------------------//
//
ARCHDATATYPE * GenData::getArchDataType (const C8 * name)
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
GENRESULT GenData::copyOpenFile (IFileSystem *file)
{
#ifndef _WIN32
	(void)file;
	return GR_GENERIC;
#else
	GENRESULT result;
	COMPTR<IFileSystem> outFile, temp;
	DAFILEDESC fdesc = "GenData.db";
	U32 dwSize = file->GetFileSize();
	U32 dwRead,dwWritten;
	void *buffer = malloc(dwSize);
	MEMFILEDESC mdesc = fdesc.lpFileName;

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
	fdesc.dwCreationDistribution = TRUNCATE_EXISTING;

	if ((result = DACOM->CreateInstance(&fdesc,outFile)) == GR_OK)
	{
		outFile->WriteFile(0,buffer,dwRead,&dwWritten,0);
	}
	else
	{
		fdesc.lpFileName = "..\\DB\\GenData.db";
		if ((result = DACOM->CreateInstance(&fdesc,outFile)) == GR_OK)
			outFile->WriteFile(0,buffer,dwRead,&dwWritten,0);
	}


	free(buffer);

	return result;
#endif
}

#ifndef _WIN32
namespace FrontierX
{
bool CreateNativeGeneralData(IFileSystem * source, IGeneralData ** output,
                             unsigned int * globalValueBytes)
{
	if (!output || !globalValueBytes) return false;
	*output = 0;
	*globalValueBytes = 0;
	if (!source) return false;
	GenData * data = new DAComponent<GenData>;
	if (!data) return false;
	if (!data->loadTypesData(source))
	{
		data->getBase()->Release();
		return false;
	}
	U32 bytes = 0;
	if (!data->GetArchetypeData("GlobalValues", bytes) ||
		bytes != sizeof(GT_GLOBAL_VALUES))
	{
		data->getBase()->Release();
		return false;
	}
	*output = static_cast<IGeneralData *>(data->getBase());
	*globalValueBytes = bytes;
	return true;
}

void ReleaseNativeGeneralData(IGeneralData * data) noexcept
{
	if (data) static_cast<IDAComponent *>(data)->Release();
}
}
#endif
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//
#ifdef _WIN32
struct _genlist : GlobalComponent
{
	GenData * list;

	virtual void Startup (void)
	{
		GENDATA = list = new DAComponent<GenData>;
		AddToGlobalCleanupList((IDAComponent **) &GENDATA);
	}

	virtual void Initialize (void)
	{
		COMPTR<IDAConnectionPoint> connection;

		if (list->loadTypesData() == 0)
			CQBOMB0("Load failed on general database.");

		if (GS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
			connection->Advise(list->getBase(), &list->eventHandle);
	}
};

static _genlist genlist;
#endif

//-------------------------------------------------------------------
//-------------------------END GenData.cpp---------------------------
//-------------------------------------------------------------------
