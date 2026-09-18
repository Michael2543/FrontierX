//--------------------------------------------------------------------------//
//                                                                          //
//                                IFF.CPP                                   //
//                                                                          //
//               COPYRIGHT (C) 1997 BY DIGITAL ANVIL, INC.                  //
//                                                                          //
//--------------------------------------------------------------------------//
/*

   $Author: Tbratton $
*/
//--------------------------------------------------------------------------//

#include <windows.h>

#include "DACOM.h"
#include "FileSys.h"
#include "FrontierX/EncodedHandle.h"
#include "TComponent.h"

//--------------------------------------------------------------------------//
//-----------------------GLOBAL DATA & MEMBERS OF FILE SYSTEM---------------//
//--------------------------------------------------------------------------//

#define MAX_CHILD_HANDLES     4
#define MAX_FINDFIRST_HANDLES 4
#define MAX_FORMS			  16

#define CHUNK_NAME(d0,d1,d2,d3) ((long(d3)<<24)+(long(d2)<<16)+(d1<<8)+d0)
#define IFF_FORM	CHUNK_NAME('F','O','R','M')		// [] = "FORM";


extern C8 interface_name[];
extern ICOManager *DACOM;

static C8 implementation_name[] = "IFF";

struct READ_STRUCT
{
	HANDLE			hFileHandle;
	LPVOID			lpBuffer;
	DWORD			nNumberOfBytesToRead;
	LPDWORD			lpNumberOfBytesRead;
	LPOVERLAPPED	lpOverlapped;
};

struct WRITE_STRUCT
{
	HANDLE			hFileHandle;
	LPCVOID			lpBuffer;
	DWORD			nNumberOfBytesToWrite;
	LPDWORD			lpNumberOfBytesWritten;
	LPOVERLAPPED	lpOverlapped;
};

struct FIND_FIRST_STRUCT
{
	LPCTSTR lpFileName;
	LPWIN32_FIND_DATA lpFindFileData;
};

struct FIND_NEXT_STRUCT
{
	HANDLE hFindFile;
	LPWIN32_FIND_DATA lpFindFileData;
};

static FrontierX::EncodedHandleValue HandleValue(HANDLE handle)
{
	return FrontierX::DecodeEncodedHandle(handle);
}

static size_t HandleIndex(HANDLE handle)
{
	return static_cast<size_t>(HandleValue(handle));
}

static HANDLE EncodeHandleIndex(size_t index)
{
	return FrontierX::EncodeHandleValue(index);
}

static HANDLE HandleFromSerialResult(LONG result)
{
	if (result == -1)
		return INVALID_HANDLE_VALUE;
	return EncodeHandleIndex(static_cast<DWORD>(result));
}

static LONG SerialResultFromHandle(HANDLE handle)
{
	if (handle == INVALID_HANDLE_VALUE)
		return -1;
	return static_cast<LONG>(HandleValue(handle));
}


struct FORM
{
	DWORD dwUsers;
	DWORD dwParent;		// index of parent form

	DWORD dwOffset;
	DWORD dwLength;
	DWORD dwNextOffset;
	DWORD dwName;
};

struct CHILD_FILE
{
	DWORD	dwChunkLengthOffset;		// usually a negative offset from child start
	DWORD	dwStartOffset;
	DWORD	dwFileSize;
	DWORD	dwFilePosition;

	DWORD	dwCurrentForm;
};

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
struct DACOM_NO_VTABLE IFF : public IFileSystem
{
   char				szFilename[MAX_PATH+4];
   DWORD			dwAccess;            // The mode for the file
   DWORD			dwLastError;
   LPFILESYSTEM		pParent;
   HANDLE			hMapping;			// handle to memory mapped file

   CHILD_FILE		child[MAX_CHILD_HANDLES];
   HANDLE			handles[MAX_CHILD_HANDLES];

   FORM				ffhandles[MAX_FINDFIRST_HANDLES];

   FORM				form[MAX_FORMS];		// pool of available form structs

   DWORD			dwChunkOffset;
   DWORD			dwChunkName;
   DWORD			dwChunkLength;	
   DWORD			dwFormName;

   BOOL				bBusy;

   BEGIN_DACOM_MAP_INBOUND(IFF)
   DACOM_INTERFACE_ENTRY(IFileSystem)
   DACOM_INTERFACE_ENTRY2(IID_IFileSystem,IFileSystem)
   END_DACOM_MAP()
   
   //---------------------------
   // public methods
   //---------------------------
   
   IFF (void)
   {
      memset(((char *) this)+sizeof(DWORD), 0, sizeof(*this)-sizeof(DWORD));
      memset(handles+1, 0xFF, sizeof(handles)-sizeof(HANDLE));
	  child[0].dwCurrentForm=(DWORD)-1;
   }

   ~IFF (void);

   // *** IComponentFactory methods ***

   DEFMETHOD(CreateInstance) (DACOMDESC *descriptor, void **instance);


   // *** IFileSystem methods ***

   DEFMETHOD_(BOOL,CloseHandle) (HANDLE handle=0);

   DEFMETHOD_(BOOL,ReadFile) (HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, 
               LPDWORD lpNumberOfBytesRead,
               LPOVERLAPPED lpOverlapped);

   DEFMETHOD_(BOOL,WriteFile) (HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite,
                LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);

   DEFMETHOD_(BOOL,GetOverlappedResult)   (HANDLE hFile,  
                                 LPOVERLAPPED lpOverlapped,
                                 LPDWORD lpNumberOfBytesTransferred,   
                                 BOOL bWait);   

   DEFMETHOD_(DWORD,SetFilePointer) (HANDLE hFile, LONG lDistanceToMove, 
                     PLONG lpDistanceToMoveHigh=0, DWORD dwMoveMethod=FILE_BEGIN);

   DEFMETHOD_(BOOL,SetEndOfFile) (HANDLE hFileHandle = 0);

   DEFMETHOD_(DWORD,GetFileSize) (HANDLE hFile=0, LPDWORD lpFileSizeHigh=0);   

   DEFMETHOD_(BOOL,LockFile) (HANDLE hFile,	
							  DWORD dwFileOffsetLow,
							  DWORD dwFileOffsetHigh,
							  DWORD nNumberOfBytesToLockLow,
							  DWORD nNumberOfBytesToLockHigh);

   DEFMETHOD_(BOOL,UnlockFile) (HANDLE hFile, 
								DWORD dwFileOffsetLow,
								DWORD dwFileOffsetHigh,
								DWORD nNumberOfBytesToUnlockLow,
								DWORD nNumberOfBytesToUnlockHigh);

   DEFMETHOD_(BOOL,GetFileTime) (HANDLE hFile,   LPFILETIME lpCreationTime,
                   LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime);

   DEFMETHOD_(BOOL,SetFileTime) (HANDLE hFile,   CONST FILETIME *lpCreationTime, 
                  CONST FILETIME *lpLastAccessTime,
                  CONST FILETIME *lpLastWriteTime);

   DEFMETHOD_(HANDLE,CreateFileMapping)   (HANDLE hFile,
                                 LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
                                 DWORD flProtect,
                                 DWORD dwMaximumSizeHigh,
                                 DWORD dwMaximumSizeLow,
                                 LPCTSTR lpName);

   DEFMETHOD_(LPVOID,MapViewOfFile)      (HANDLE hFileMappingObject,
                                 DWORD dwDesiredAccess,
                                 DWORD dwFileOffsetHigh,
                                 DWORD dwFileOffsetLow,
                                 DWORD dwNumberOfBytesToMap);
   
   DEFMETHOD_(BOOL,UnmapViewOfFile)      (LPCVOID lpBaseAddress);

   DEFMETHOD_(HANDLE,FindFirstFile) (LPCTSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData);
   
   DEFMETHOD_(BOOL,FindNextFile) (HANDLE hFindFile, LPWIN32_FIND_DATA lpFindFileData);   
   
   DEFMETHOD_(BOOL,FindClose) (HANDLE hFindFile);   

   DEFMETHOD_(BOOL,CreateDirectory) (LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes);

   DEFMETHOD_(BOOL,RemoveDirectory) (LPCTSTR lpPathName);
   
   DEFMETHOD_(DWORD,GetCurrentDirectory) (DWORD nBufferLength, LPTSTR lpBuffer);

   DEFMETHOD_(BOOL,SetCurrentDirectory) (LPCTSTR lpPathName);

   DEFMETHOD_(BOOL,DeleteFile)  (LPCTSTR lpFileName); 

   DEFMETHOD_(BOOL,CopyFile)    (LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists);
   
   DEFMETHOD_(BOOL,MoveFile) (LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName);

   DEFMETHOD_(DWORD,GetFileAttributes) (LPCTSTR lpFileName);

   DEFMETHOD_(BOOL,SetFileAttributes) (LPCTSTR lpFileName, DWORD dwFileAttributes);

   DEFMETHOD_(DWORD,GetLastError) (VOID);

   //---------------   
   // IFileSystem extensions to WIN32 system
   //---------------   
   
   DEFMETHOD_(HANDLE,OpenChild) (DAFILEDESC *lpDesc);

   DEFMETHOD_(DWORD,GetFilePosition) (HANDLE hFile = 0, PLONG pPositionHigh=0);
   
   DEFMETHOD_(LONG,GetFileName) (LPSTR lpBuffer, LONG lBufferSize);
   
   DEFMETHOD_(DWORD,GetAccessType) (VOID);

   DEFMETHOD(GetParentSystem) (LPFILESYSTEM *lplpFileSystem);

   DEFMETHOD(SetPreference)  (DWORD dwNumber, DWORD  dwValue);

   DEFMETHOD(GetPreference)  (DWORD dwNumber, PDWORD pdwValue);
   
   DEFMETHOD(ReadDirectoryExtension) (HANDLE hFile, LPVOID lpBuffer, 
										DWORD nNumberOfBytesToRead,
										LPDWORD lpNumberOfBytesRead=0, DWORD dwStartOffset=0);

   DEFMETHOD(WriteDirectoryExtension) (HANDLE hFile, LPCVOID lpBuffer, 
										DWORD nNumberOfBytesToWrite,
										LPDWORD lpNumberOfBytesWritten=0, DWORD dwStartOffset=0);

   DEFMETHOD_(LONG,SerialCall) (LPFILESYSTEM lpSystem, DAFILE_SERIAL_PROC lpProc, VOID *lpContext);

   DEFMETHOD_(BOOL,GetAbsolutePath) (char *lpOutput, LPCTSTR lpInput, LONG lSize);

   //---------------   
   // serialized methods
   //---------------   
   
   SERIALMETHOD(ReadFile_S);

   SERIALMETHOD(WriteFile_S);

   SERIALMETHOD(SetEndOfFile_S);

   SERIALMETHOD(CreateMapping_S);	

   SERIALMETHOD(CreateDirectory_S);

   SERIALMETHOD(RemoveDirectory_S);

   SERIALMETHOD(DeleteFile_S);

   SERIALMETHOD(SetCurrentDirectory_S);

   SERIALMETHOD(GetFileAttributes_S);

   SERIALMETHOD(ReleaseForm_S);
   
   SERIALMETHOD(OpenChild_S);

   SERIALMETHOD(FindFirstFile_S);
   
   SERIALMETHOD(FindNextFile_S);

   SERIALMETHOD(OpenExistingForm_S);

   BOOL AddToChild (HANDLE hFile, DWORD dwNumBytes);	// called from a serialized method

   BOOL InsertSpace (DWORD dwStartOffset, DWORD dwNumBytes, DWORD dwStartingForm);  // called from a serialized method

   LONG AddForm (FORM * lpForm);	// called from a serialized method

   LONG AddHandle (void);			// called from a serial method

   //---------------   
   // IFF methods
   //---------------   

   BOOL init (void);

   int RootChunk(void);

   void ExitForm(void);

   int SeekForm (DWORD dwSearchName);

   int SeekChunk (DWORD dwSearchName);

   int end_of_form (DWORD dwOffset)
   {
		if (child[0].dwCurrentForm >= MAX_FORMS)
			return eof(dwOffset);
		else
			return (dwOffset >= (DWORD)form[child[0].dwCurrentForm].dwNextOffset);
   }

	int eof (DWORD dwOffset)
	{
		return (dwOffset >= GetFileSize());
	}

	int chunk_is_form(void)
	{
		return (dwChunkName == IFF_FORM);
	}

	int skip_to_next_chunk (void)
	{ 	
		return ReadChunkInfo(dwChunkOffset + ((dwChunkLength+1)&~1) + 8);
	}

	int ReadChunkInfo (DWORD dwNewOffset);

	HANDLE CreateNewChunk (DWORD dwName, LPCVOID lpData, DWORD dwSize);

	HANDLE OpenExistingChunk (DWORD dwName);
	
	static long Reverse (long _long);
};

//--------------------------------------------------------------------------//
//-------------------------------IFF Methods--------------------------------//
//--------------------------------------------------------------------------//
//
IFF::~IFF (void)
{
	if (pParent)
	{
		if (hMapping)
			pParent->CloseHandle(hMapping);
		pParent->CloseHandle(handles[0]);
		pParent->Release();
	}
}
//--------------------------------------------------------------------------//
//
GENRESULT IFF::CreateInstance (DACOMDESC *descriptor,  //)
                                         void      **instance)
{
	DAFILEDESC		*lpInfo     = (DAFILEDESC *) descriptor;
	GENRESULT		result     = GR_OK;
	IFF	*pNewSystem = NULL;
	HANDLE			handle;

	bBusy++;
	if (bBusy > 1)
	{
		result = GR_GENERIC;
		goto Done;
	}

	if (lpInfo==NULL || (lpInfo->interface_name==NULL))
	{
		result = GR_GENERIC;
		goto Done;
	}

   //
   // If unsupported interface requested, fail call
   //

	if ((lpInfo->size != sizeof(*lpInfo)) || 
		strcmp(lpInfo->interface_name, interface_name))
	{
		result = GR_INTERFACE_UNSUPPORTED;
		goto Done;
	}


	//
	// Can't handle this request (we already have a parent)
	//

	if (lpInfo->lpParent && pParent)
	{
		result = GR_GENERIC;
		goto Done;
	}

	// 
	// if we are an open file, see if we can find child inside us
	// 	
	
	if (pParent)
	{
		BOOL bIsADirectory=0;
		
		if ((handle = OpenChild(lpInfo)) == INVALID_HANDLE_VALUE)
		{
		  //
		  // OpenChild() failed; system could not be created
		  // See if	file is really a directory
  		  //

			if (lpInfo->lpFileName)
			{
				if ((lpInfo->dwDesiredAccess & dwAccess) == lpInfo->dwDesiredAccess)
				{
					DWORD formName = *((DWORD *) lpInfo->lpFileName);
					handle = HandleFromSerialResult(
						pParent->SerialCall(this, (DAFILE_SERIAL_PROC) OpenExistingForm_S, &formName));
				}
			}
		
			if (handle == INVALID_HANDLE_VALUE)
			{
				result = GR_FILE_ERROR;
				goto Done;
			}
			bIsADirectory=1;
		}

//		if (lpInfo->lpImplementation == 0 || 
//			strcmp(lpInfo->lpImplementation, implementation_name))
		if (bIsADirectory==0)
		{
			// need some other implementation

			lpInfo->lpParent = this;
			lpInfo->hParent   = handle;
			AddRef();			// child file system will now reference us
			if ((result = DACOM->CreateInstance(lpInfo, (void **) &pNewSystem)) != GR_OK)
			{
				Release();
				CloseHandle(handle);
				lpInfo->lpParent = 0;
				lpInfo->hParent   = 0;
				goto Done;
			}
			lpInfo->lpParent = 0;
			lpInfo->hParent   = 0;

			goto Done;
		}

		// 
		// else create a new instance of IFF
		//

		if ((pNewSystem = new DAComponent<IFF>) == 0)
		{
			result = GR_GENERIC;
			goto Done;
		}

		strncpy(pNewSystem->szFilename, lpInfo->lpFileName, sizeof(szFilename));
		pNewSystem->dwAccess = lpInfo->dwDesiredAccess;
		pNewSystem->pParent = this;
		pNewSystem->handles[0] = handle;
		AddRef();			// child file system will now references us
		goto Done;
	}


	//
	// Implementation name must match
	//
	
	if (lpInfo->lpImplementation != 0 && 
		strcmp(lpInfo->lpImplementation, implementation_name))
	{
		result = GR_GENERIC;
		goto Done;
	}
	else	// dont create a IFF without being asked
	if (lpInfo->lpImplementation == NULL &&
		lpInfo->dwCreationDistribution != OPEN_EXISTING)
	{
		result = GR_GENERIC;
		goto Done;
	}



   //
   // Attempt to create new file system instance
   //

	if ((pNewSystem = new DAComponent<IFF>) == NULL)
	{
		result = GR_GENERIC;
		goto Done;
	}

	// call DACOM to create a parent (if none specified in lpInfo)

	if (lpInfo->lpParent)
	{
		strncpy(pNewSystem->szFilename, lpInfo->lpFileName, sizeof(szFilename));
		pNewSystem->dwAccess = lpInfo->dwDesiredAccess;
		pNewSystem->pParent = lpInfo->lpParent;
		pNewSystem->handles[0] = lpInfo->hParent;
	}
	else
	{
		LPCTSTR lpSaved = lpInfo->lpImplementation;

		lpInfo->lpImplementation = 0;
		if ((result = DACOM->CreateInstance(lpInfo, (void **) &pNewSystem->pParent)) != GR_OK)
		{
			delete pNewSystem;
			pNewSystem = 0;
			result = GR_FILE_ERROR;
			lpInfo->lpImplementation = lpSaved;
			goto Done;
		}
		lpInfo->lpImplementation = lpSaved;
		strncpy(pNewSystem->szFilename, lpInfo->lpFileName, sizeof(szFilename));
		pNewSystem->dwAccess = lpInfo->dwDesiredAccess;
	}

	// make sure file really is an IFF file
	// do any extra init things

	if (lpInfo->dwCreationDistribution == OPEN_EXISTING &&
		pNewSystem->init() == 0)
	{
		if (lpInfo->lpParent)
		{
			lpInfo->lpParent->AddRef();		// prevent releasing a parent we didn't use
			pNewSystem->handles[0] = INVALID_HANDLE_VALUE;
		}
		pNewSystem->Release();
		pNewSystem=0;
		result = GR_FILE_ERROR;
	}

Done:
   *instance = pNewSystem;
   bBusy--;

   return result;
}
//--------------------------------------------------------------------------//
//  File instance is open, do any init stuff as needed
// open any file
// if this isn't really an IFF file, no harm done
//
//
BOOL IFF::init (void)
{
	DWORD dwBytesRead;
	DWORD dwID, dwLength;
	BOOL result=0;

	if (ReadFile(0, &dwID, sizeof(dwID), &dwBytesRead, 0) == 0 ||
		dwBytesRead != sizeof(dwID) ||
		dwID != IFF_FORM)
	{
		goto Done;
	}

	if (ReadFile(0, &dwLength, sizeof(dwLength), &dwBytesRead, 0) == 0 ||
		dwBytesRead != sizeof(dwLength) ||
		(DWORD)Reverse(dwLength) > GetFileSize())
	{
		goto Done;
	}
	
	result=1;
	
Done:
	SetFilePointer(0, 0, 0, FILE_BEGIN);

	return result;
}
//--------------------------------------------------------------------------//
//
BOOL IFF::CloseHandle (HANDLE handle)
{
	const FrontierX::EncodedHandleValue handleValue = HandleValue(handle);
	if (handleValue == 0 || handleValue >= MAX_CHILD_HANDLES)
	{
		if (handleValue < MAX_CHILD_HANDLES*2)	// memory mapping handle
			return 1;

		dwLastError = ERROR_INVALID_HANDLE;
		return 0;
	}

	const size_t handleIndex = static_cast<size_t>(handleValue);
	if (handles[handleIndex] != INVALID_HANDLE_VALUE)
	{
		DWORD formIndex = child[handleIndex].dwCurrentForm;
		SerialCall(this, (DAFILE_SERIAL_PROC) ReleaseForm_S, &formIndex);
		handles[handleIndex] = INVALID_HANDLE_VALUE;
		return 1;
	}
	else
	{
		dwLastError = ERROR_INVALID_HANDLE;
		return 0;
	}
}
//--------------------------------------------------------------------------//
//
BOOL IFF::ReadFile (HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, 
               LPDWORD lpNumberOfBytesRead,
               LPOVERLAPPED lpOverlapped)
{
	READ_STRUCT context = { hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped };
	return pParent->SerialCall(this, (DAFILE_SERIAL_PROC) ReadFile_S, &context);
}
//--------------------------------------------------------------------------//
//
BOOL IFF::WriteFile (HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite,
                LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
	WRITE_STRUCT context = { hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped };
	return pParent->SerialCall(this, (DAFILE_SERIAL_PROC) WriteFile_S, &context);
}
//--------------------------------------------------------------------------//
//
LONG IFF::ReadFile_S (VOID *lpContext)
{
	BOOL result;
	READ_STRUCT *pRead = (READ_STRUCT *) lpContext;

	if (HandleValue(pRead->hFileHandle) >= MAX_CHILD_HANDLES)
	{
		dwLastError = ERROR_INVALID_HANDLE;
		return 0;
	}
	
	if (pRead->lpOverlapped)
	{
		// limit the number of bytes read to size of the file
		if (pRead->hFileHandle)
		{
			int iOver;
			DWORD dwFSize = GetFileSize(pRead->hFileHandle);

			pRead->nNumberOfBytesToRead = __min(pRead->nNumberOfBytesToRead, dwFSize);
			iOver = dwFSize - pRead->lpOverlapped->Offset - pRead->nNumberOfBytesToRead;
			if (iOver < 0)
				pRead->nNumberOfBytesToRead += iOver;
		}

		pRead->lpOverlapped->Offset += child[HandleIndex(pRead->hFileHandle)].dwStartOffset;
		result = pParent->ReadFile(handles[0], pRead->lpBuffer, pRead->nNumberOfBytesToRead, pRead->lpNumberOfBytesRead,
				   pRead->lpOverlapped);
		pRead->lpOverlapped->Offset -= child[HandleIndex(pRead->hFileHandle)].dwStartOffset;
		if (result==0)
			dwLastError = pParent->GetLastError();
	}
	else
	{
		DWORD dwOffset;
		
		dwOffset = GetFilePosition(pRead->hFileHandle);

		// limit the number of bytes read to size of the file
		if (pRead->hFileHandle)
		{
			int iOver;
			DWORD dwFSize = GetFileSize(pRead->hFileHandle);

			pRead->nNumberOfBytesToRead = __min(pRead->nNumberOfBytesToRead, dwFSize);
			iOver = dwFSize - dwOffset - pRead->nNumberOfBytesToRead;
			if (iOver < 0)
				pRead->nNumberOfBytesToRead += iOver;
		}

		dwOffset += child[HandleIndex(pRead->hFileHandle)].dwStartOffset;
		if (GetFilePosition() != dwOffset)
			SetFilePointer(0, dwOffset);
		result = pParent->ReadFile(handles[0], pRead->lpBuffer, pRead->nNumberOfBytesToRead, 
					pRead->lpNumberOfBytesRead, 0);

		if (result == 0)
			dwLastError = pParent->GetLastError();
		else
			child[HandleIndex(pRead->hFileHandle)].dwFilePosition += *(pRead->lpNumberOfBytesRead);
	}

	return result;
}
//--------------------------------------------------------------------------//
//
LONG IFF::WriteFile_S (VOID *lpContext)
{
	BOOL result;
	WRITE_STRUCT *pWrite = (WRITE_STRUCT *) lpContext;

	if (HandleValue(pWrite->hFileHandle) >= MAX_CHILD_HANDLES)
	{
		dwLastError = ERROR_INVALID_HANDLE;
		return 0;
	}

	if ((LONG)pWrite->nNumberOfBytesToWrite < 0)
	{
		dwLastError = ERROR_INVALID_PARAMETER;
		return 0;
	}

	if (pWrite->lpOverlapped)
	{
		if (pWrite->hFileHandle)
		{
			int iOver;

			// check for overrages
			iOver = pWrite->lpOverlapped->Offset + pWrite->nNumberOfBytesToWrite - GetFileSize(pWrite->hFileHandle);
			if (iOver > 0)
				if (AddToChild(pWrite->hFileHandle, iOver) == 0)
					return 0;
		}

		pWrite->lpOverlapped->Offset += child[HandleIndex(pWrite->hFileHandle)].dwStartOffset;
		result = pParent->WriteFile(handles[0], pWrite->lpBuffer, pWrite->nNumberOfBytesToWrite, pWrite->lpNumberOfBytesWritten,
				   pWrite->lpOverlapped);
		pWrite->lpOverlapped->Offset -= child[HandleIndex(pWrite->hFileHandle)].dwStartOffset;
		if (result==0)
			dwLastError = pParent->GetLastError();
	}
	else
	{
		DWORD dwOffset;
		
		dwOffset = GetFilePosition(pWrite->hFileHandle);

		if (pWrite->hFileHandle)
		{
			int iOver;
			// check for overrages
			iOver = dwOffset + pWrite->nNumberOfBytesToWrite - GetFileSize(pWrite->hFileHandle);
			if (iOver > 0)
				if (AddToChild(pWrite->hFileHandle, iOver) == 0)
					return 0;
		}

		dwOffset += child[HandleIndex(pWrite->hFileHandle)].dwStartOffset;
		if (GetFilePosition() != dwOffset)
			SetFilePointer(0, dwOffset);

		result = pParent->WriteFile(handles[0], pWrite->lpBuffer, pWrite->nNumberOfBytesToWrite, 
						pWrite->lpNumberOfBytesWritten, 0);


		if (result == 0)
			dwLastError = pParent->GetLastError();
		else
			child[HandleIndex(pWrite->hFileHandle)].dwFilePosition += *(pWrite->lpNumberOfBytesWritten);
	}

	return result;
}
//--------------------------------------------------------------------------//
//
BOOL IFF::GetOverlappedResult (HANDLE hFileHandle, LPOVERLAPPED lpOverlapped,
                                 LPDWORD lpNumberOfBytesTransferred,   BOOL bWait)
{
	BOOL result;

	result = pParent->GetOverlappedResult(handles[0], lpOverlapped, lpNumberOfBytesTransferred, bWait);

	if (result == 0)
		dwLastError = pParent->GetLastError();

	return result;
}
//--------------------------------------------------------------------------//
//
DWORD IFF::SetFilePointer (HANDLE hFileHandle, LONG lDistanceToMove, 
                     PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod)
{
	DWORD result;

	if (HandleValue(hFileHandle) >= MAX_CHILD_HANDLES)
	{
		dwLastError = ERROR_INVALID_HANDLE;
		return 0xFFFFFFFF;
	}
	else
	if (hFileHandle == 0)
	{
		result = child[0].dwFilePosition = 
			pParent->SetFilePointer(handles[0], lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod);
	}
	else
	{
		switch (dwMoveMethod)
		{
		case FILE_BEGIN:
		default:
			break;

		case FILE_CURRENT:
			lDistanceToMove += GetFilePosition(hFileHandle);
			break;

		case FILE_END:
			lDistanceToMove = GetFileSize(hFileHandle) - lDistanceToMove;
			break;
		}

		if ((dwAccess & GENERIC_WRITE) == 0 && (DWORD)lDistanceToMove > GetFileSize(hFileHandle))
			lDistanceToMove = GetFileSize(hFileHandle);

		result = child[HandleIndex(hFileHandle)].dwFilePosition = lDistanceToMove;
		if (lpDistanceToMoveHigh)
			*lpDistanceToMoveHigh = 0;
	}
	
	if (result == 0xFFFFFFFF)
		dwLastError = pParent->GetLastError();

	return result;
}
//--------------------------------------------------------------------------//
//
BOOL IFF::SetEndOfFile (HANDLE hFileHandle)
{
	BOOL result=0;

	if (HandleValue(hFileHandle) >= MAX_CHILD_HANDLES)
	{
		dwLastError = ERROR_INVALID_HANDLE;
	}
	else
	if (hFileHandle == 0)
	{
		result = pParent->SetEndOfFile(handles[0]);
		if (result == 0)
			dwLastError = pParent->GetLastError();
	}
	else
	{
		if ((dwAccess & GENERIC_WRITE) == 0)
			dwLastError = ERROR_ACCESS_DENIED;
		else
			result = pParent->SerialCall(this, (DAFILE_SERIAL_PROC) SetEndOfFile_S, hFileHandle);
	}

	return result;
}
//--------------------------------------------------------------------------//
//
DWORD IFF::GetFileSize (HANDLE hFileHandle, LPDWORD lpFileSizeHigh)
{
	DWORD result;

	if (HandleValue(hFileHandle) >= MAX_CHILD_HANDLES)
	{
		dwLastError = ERROR_INVALID_HANDLE;
		return 0xFFFFFFFF;
	}
	else
	if (hFileHandle == 0)
	{
		result = pParent->GetFileSize(handles[0], lpFileSizeHigh);
	}
	else
	{
		result = child[HandleIndex(hFileHandle)].dwFileSize;
		if (lpFileSizeHigh)
			*lpFileSizeHigh = 0;
	}	
	
	if (result == 0xFFFFFFFF)
		dwLastError = pParent->GetLastError();

	return result;
}
//--------------------------------------------------------------------------//
//
BOOL IFF::LockFile (HANDLE hFileHandle,	
	  				  DWORD dwFileOffsetLow,
					  DWORD dwFileOffsetHigh,
					  DWORD nNumberOfBytesToLockLow,
					  DWORD nNumberOfBytesToLockHigh)
{
	BOOL result;

	if (HandleValue(hFileHandle) >= MAX_CHILD_HANDLES)
	{
		dwLastError = ERROR_INVALID_HANDLE;
		result = 0;
	}
	else
	if (hFileHandle == 0)
	{
		result = pParent->LockFile(handles[0],
									dwFileOffsetLow,
									dwFileOffsetHigh,
									nNumberOfBytesToLockLow,
									nNumberOfBytesToLockHigh);
		if (result == 0)
			dwLastError = pParent->GetLastError();
	}
	else
		result = 1;

	return result;
}
//--------------------------------------------------------------------------//
//
BOOL IFF::UnlockFile (HANDLE hFileHandle, 
						DWORD dwFileOffsetLow,
						DWORD dwFileOffsetHigh,
						DWORD nNumberOfBytesToUnlockLow,
						DWORD nNumberOfBytesToUnlockHigh)
{
	BOOL result;

	if (HandleValue(hFileHandle) >= MAX_CHILD_HANDLES)
	{
		dwLastError = ERROR_INVALID_HANDLE;
		result = 0;
	}
	else
	if (hFileHandle == 0)
	{
		result = pParent->UnlockFile(handles[0],
									dwFileOffsetLow,
									dwFileOffsetHigh,
									nNumberOfBytesToUnlockLow,
									nNumberOfBytesToUnlockHigh);
		if (result == 0)
			dwLastError = pParent->GetLastError();
	}
	else
		result = 1;

	return result;
}
//--------------------------------------------------------------------------//
//
BOOL IFF::GetFileTime (HANDLE hFileHandle, LPFILETIME lpCreationTime,
                   LPFILETIME lpLastAccessTime, LPFILETIME lpLastWriteTime)
{
	dwLastError = ERROR_EAS_NOT_SUPPORTED;
	return 0;
}
//--------------------------------------------------------------------------//
//
BOOL IFF::SetFileTime (HANDLE hFileHandle, CONST FILETIME *lpCreationTime, 
                  CONST FILETIME *lpLastAccessTime,
                  CONST FILETIME *lpLastWriteTime)
{
	dwLastError = ERROR_EAS_NOT_SUPPORTED;
	return 0;
}
//--------------------------------------------------------------------------//
//
HANDLE IFF::CreateFileMapping (HANDLE hFileHandle, LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
                                 DWORD flProtect, DWORD dwMaximumSizeHigh, DWORD dwMaximumSizeLow, LPCTSTR lpName)
{
	if (HandleValue(hFileHandle) >= MAX_CHILD_HANDLES)
	{
		dwLastError = ERROR_INVALID_HANDLE;
		return 0;
	}

	if ((dwAccess & GENERIC_WRITE)==0 && (flProtect & PAGE_READWRITE))
	{
		dwLastError = ERROR_ACCESS_DENIED;
		return 0;
	}

	if (dwMaximumSizeHigh || dwMaximumSizeLow)
	{
		dwLastError = ERROR_INVALID_PARAMETER;
		return 0;		// don't support enlarging via mapping mechanism
	}

	SerialCall(this, (DAFILE_SERIAL_PROC) CreateMapping_S, 0);

	if (hMapping == 0)
		return 0;

	return EncodeHandleIndex(MAX_CHILD_HANDLES + HandleIndex(hFileHandle));
}
//--------------------------------------------------------------------------//
//
LPVOID IFF::MapViewOfFile (HANDLE hFileMappingObject,
                            DWORD dwDesiredAccess,
                            DWORD dwFileOffsetHigh,
                            DWORD dwFileOffsetLow,
                            DWORD dwNumberOfBytesToMap)
{
	LPVOID result;

	size_t childIndex = 0;
	if (hFileMappingObject)
	{
		if (!FrontierX::TryDecodeOffsetHandleIndex(
			hFileMappingObject, MAX_CHILD_HANDLES, MAX_CHILD_HANDLES, childIndex))
		{
			dwLastError = ERROR_INVALID_HANDLE;
			return 0;
		}
	}

	// if mapping a child file
	if (childIndex)
	{
		if (dwNumberOfBytesToMap == 0)
			dwNumberOfBytesToMap = GetFileSize(EncodeHandleIndex(childIndex));
		else
			dwNumberOfBytesToMap = __min(dwNumberOfBytesToMap, GetFileSize(EncodeHandleIndex(childIndex)));
		dwFileOffsetLow+=child[childIndex].dwStartOffset;
	}

	result = pParent->MapViewOfFile(hMapping,
			                        dwDesiredAccess,
								    0,
				                    dwFileOffsetLow,
				                    dwNumberOfBytesToMap);
	if (result == 0)
		dwLastError = pParent->GetLastError();

	return result;
}
//--------------------------------------------------------------------------//
//
BOOL IFF::UnmapViewOfFile (LPCVOID lpBaseAddress)
{
	BOOL result;

	result = pParent->UnmapViewOfFile(lpBaseAddress);

	if (result == 0)
		dwLastError = pParent->GetLastError();

	return result;
}
//--------------------------------------------------------------------------//
//
BOOL IFF::CreateDirectory (LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	return pParent->SerialCall(this, (DAFILE_SERIAL_PROC) CreateDirectory_S, (void *)lpPathName);
}
//--------------------------------------------------------------------------//
//
LONG IFF::CreateDirectory_S (LPVOID lpContext)
{
	LPCTSTR lpPathName = (LPCTSTR) lpContext;

	if (SeekForm(*((DWORD *)lpPathName)))
	{
		ExitForm();

		dwLastError = ERROR_FILE_EXISTS;
		return 0;
	}

	HANDLE handle;
	
	if ((handle = CreateNewChunk(IFF_FORM, lpPathName, 4)) == INVALID_HANDLE_VALUE)
		return 0;

	CloseHandle(handle);
	return 1;
}
//--------------------------------------------------------------------------//
//
BOOL IFF::RemoveDirectory (LPCTSTR lpPathName)
{
	return pParent->SerialCall(this, (DAFILE_SERIAL_PROC) RemoveDirectory_S, (void *)lpPathName);
}
//--------------------------------------------------------------------------//
//
DWORD IFF::GetCurrentDirectory (DWORD nBufferLength, LPTSTR lpBuffer)
{
	if (nBufferLength > 4)
	{
		if (child[0].dwCurrentForm >= MAX_FORMS)
		{
			*lpBuffer = '\\';
			lpBuffer[1] = 0;
			return 1;
		}
		strncpy(lpBuffer, (const char *)&form[child[0].dwCurrentForm].dwName, 4);
		lpBuffer[4] = 0;
	}
	return 4;
}
//--------------------------------------------------------------------------//
//
BOOL IFF::SetCurrentDirectory (LPCTSTR lpPathName)
{
	return pParent->SerialCall(this, (DAFILE_SERIAL_PROC) SetCurrentDirectory_S, (void *)lpPathName);
}
//--------------------------------------------------------------------------//
//
LONG IFF::SetCurrentDirectory_S (LPVOID lpContext)
{
	LONG result=1;
	LPCTSTR lpPathName = (LPCTSTR) lpContext;

	if (strcmp(lpPathName, "..") == 0)
		ExitForm();
	else
		result = SeekForm(*((DWORD *)lpPathName));
	
	return result;
}
//--------------------------------------------------------------------------//
//
BOOL IFF::DeleteFile (LPCTSTR lpFileName)
{
	return pParent->SerialCall(this, (DAFILE_SERIAL_PROC) DeleteFile_S, (void *)lpFileName);
}
//--------------------------------------------------------------------------//
//
BOOL IFF::CopyFile (LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists)
{
	LPVOID lpMemory=0;
	HANDLE hSrc, hDst=0;
	DAFILEDESC desc(lpExistingFileName, 0);
	DWORD dwLength, dwReadWrite;
	BOOL result=0;

	hSrc = OpenChild(&desc);
	if (hSrc == INVALID_HANDLE_VALUE)
		return 0;
	dwLength = GetFileSize(hSrc);

	if ((lpMemory = VirtualAlloc(0, dwLength, MEM_COMMIT, PAGE_READWRITE)) == 0)
	{
		dwLastError = ERROR_NOT_ENOUGH_MEMORY;
		goto Done;
	}

	desc.dwDesiredAccess = GENERIC_WRITE;
	desc.dwCreationDistribution = (bFailIfExists)? CREATE_NEW : CREATE_ALWAYS;
	desc.lpFileName = lpNewFileName;

	hDst = OpenChild(&desc);
	if (hDst == INVALID_HANDLE_VALUE)
		goto Done;
	
	if (ReadFile(hSrc,  lpMemory, dwLength, &dwReadWrite, 0) == 0)
		goto Done;
	if (WriteFile(hDst, lpMemory, dwLength, &dwReadWrite, 0) == 0)
		goto Done;
	
	result = 1;
		
Done:
	if (hDst)
		CloseHandle(hDst);
	if (hSrc)
		CloseHandle(hSrc);
	if (lpMemory)
		VirtualFree(lpMemory, 0, MEM_RELEASE);

	return result;
}   
//--------------------------------------------------------------------------//
//
BOOL IFF::MoveFile (LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName)
{
	dwLastError = ERROR_NOT_SUPPORTED;
	return 0;
}
//--------------------------------------------------------------------------//
//
DWORD IFF::GetFileAttributes (LPCTSTR lpFileName)
{
	return pParent->SerialCall(this, (DAFILE_SERIAL_PROC) GetFileAttributes_S, (void *)lpFileName);
}
//--------------------------------------------------------------------------//
//
LONG IFF::GetFileAttributes_S (LPVOID lpContext)
{
	LPCTSTR lpFileName = (LPCTSTR) lpContext;
	RootChunk();

	while (end_of_form(dwChunkOffset) == 0)
	{
		if (strncmp((const char *)&dwChunkName, lpFileName, 4) == 0)
		{
			return FILE_ATTRIBUTE_NORMAL;
		}
		else
		if (dwChunkName==IFF_FORM && strncmp((const char *)&dwFormName, lpFileName, 4) == 0)
		{
			return FILE_ATTRIBUTE_DIRECTORY;
		}
		
		if (skip_to_next_chunk() == 0)
			break;
	}

	dwLastError = ERROR_FILE_NOT_FOUND;
	return 0xFFFFFFFF;
}
//--------------------------------------------------------------------------//
//
BOOL IFF::SetFileAttributes (LPCTSTR lpFileName, DWORD dwFileAttributes)
{
	dwLastError = ERROR_EAS_NOT_SUPPORTED;
	return 0;
}
//--------------------------------------------------------------------------//
//
DWORD IFF::GetLastError (VOID)
{
   return dwLastError;
}
//--------------------------------------------------------------------------//
//
HANDLE IFF::OpenChild (DAFILEDESC *lpInfo)
{
	return HandleFromSerialResult(
		pParent->SerialCall(this, (DAFILE_SERIAL_PROC) OpenChild_S, lpInfo));
}
//--------------------------------------------------------------------------//
//
LONG IFF::OpenChild_S (LPVOID lpContext)
{
	DAFILEDESC *lpInfo = (DAFILEDESC *) lpContext;

	if (lpInfo->lpFileName == 0)
		goto Error;
	if ((lpInfo->dwDesiredAccess & dwAccess) != lpInfo->dwDesiredAccess)
	{
		dwLastError = ERROR_ACCESS_DENIED;
		return -1;
	}
	
	switch (lpInfo->dwCreationDistribution)
	{
	case CREATE_NEW:
		{
			if (SeekChunk(*((DWORD *)lpInfo->lpFileName)))
			{
				dwLastError = ERROR_FILE_EXISTS;
				return -1;
			}
			return SerialResultFromHandle(CreateNewChunk(*((DWORD *)lpInfo->lpFileName), 0, 0));
		}
		break;

	case CREATE_ALWAYS:
		{
			HANDLE handle;

			if ((handle = OpenExistingChunk(*((DWORD *)lpInfo->lpFileName))) != INVALID_HANDLE_VALUE)
			{
				SetEndOfFile(handle);	// truncate this chunk
				return SerialResultFromHandle(handle);
			}
			else	// create the file
				return SerialResultFromHandle(CreateNewChunk(*((DWORD *)lpInfo->lpFileName), 0, 0));
		}
		break;

	case OPEN_EXISTING:
		return SerialResultFromHandle(OpenExistingChunk(*((DWORD *)lpInfo->lpFileName)));
		break;

	case OPEN_ALWAYS:
		{
			HANDLE handle;

			if ((handle = OpenExistingChunk(*((DWORD *)lpInfo->lpFileName))) != INVALID_HANDLE_VALUE)
				return SerialResultFromHandle(handle);
			else
				return SerialResultFromHandle(CreateNewChunk(*((DWORD *)lpInfo->lpFileName), 0, 0));
		}
		break;

	case TRUNCATE_EXISTING:
		{
			HANDLE handle;

			if ((handle = OpenExistingChunk(*((DWORD *)lpInfo->lpFileName))) == INVALID_HANDLE_VALUE)
				return SerialResultFromHandle(handle);
			else
			{
				SetEndOfFile(handle);	// truncate this chunk
				return SerialResultFromHandle(handle);
			}
		}
		break;
	}
	
Error:
	dwLastError = ERROR_INVALID_PARAMETER;
	return -1;
}
//--------------------------------------------------------------------------//
//
LONG IFF::GetFileName (LPSTR lpBuffer, LONG lBufferSize)
{
   lBufferSize = __min(lBufferSize, (LONG)strlen(szFilename)+1);

   if (lBufferSize > 0 && lpBuffer)
      memcpy(lpBuffer, szFilename, lBufferSize);

   return lBufferSize;
}
//--------------------------------------------------------------------------//
//
DWORD IFF::GetAccessType (VOID)
{
   return dwAccess;
}
//--------------------------------------------------------------------------//
//
GENRESULT IFF::GetParentSystem (LPFILESYSTEM *lplpFileSystem)
{
   if ((*lplpFileSystem = pParent) != 0)
	   pParent->AddRef();
   return GR_OK;
}
//--------------------------------------------------------------------------//
//
GENRESULT IFF::SetPreference (DWORD dwNumber, DWORD  dwValue)
{
   return GR_GENERIC;
}
//--------------------------------------------------------------------------//
//
GENRESULT IFF::GetPreference (DWORD dwNumber, PDWORD pdwValue)
{
   return GR_GENERIC;
}
//--------------------------------------------------------------------------//
//
GENRESULT IFF::ReadDirectoryExtension (HANDLE hFile, LPVOID lpBuffer, 
										DWORD nNumberOfBytesToRead,
										LPDWORD lpNumberOfBytesRead, DWORD dwStartOffset)
{
	return GR_GENERIC;
}
//--------------------------------------------------------------------------//
//
GENRESULT IFF::WriteDirectoryExtension (HANDLE hFile, LPCVOID lpBuffer, 
										DWORD nNumberOfBytesToWrite,
										LPDWORD lpNumberOfBytesWritten, DWORD dwStartOffset)
{
	return GR_GENERIC;
}
//--------------------------------------------------------------------------//
//
LONG IFF::SerialCall (LPFILESYSTEM lpSystem, DAFILE_SERIAL_PROC lpProc, VOID *lpContext)
{
	return pParent->SerialCall(lpSystem, lpProc, lpContext);
}
//--------------------------------------------------------------------------//
//
BOOL IFF::GetAbsolutePath (char *lpOutput, LPCTSTR lpInput, LONG lSize)
{
	return 0;		// not implemented
}
//--------------------------------------------------------------------------//
//
LONG IFF::CreateMapping_S (VOID *lpContext)
{
	if (hMapping)
		return 1;

	// if this is for a child file, adjust his size to fit current mapping
	// if mapping size is larger than current child filesize AND child 
	// has write access

	DWORD flProtect = (dwAccess & GENERIC_WRITE)?PAGE_READWRITE:PAGE_READONLY;

	hMapping = pParent->CreateFileMapping(handles[0], 0, flProtect, 0,0,0);

	if (hMapping == 0)
		dwLastError = pParent->GetLastError();

	return hMapping ? 1 : 0;
}
//--------------------------------------------------------------------------//
//
LONG IFF::AddHandle (void)
{
	DWORD i;

	for (i = 1; i < MAX_CHILD_HANDLES; i++)
		if (handles[i] == INVALID_HANDLE_VALUE)
		{
			handles[i] = EncodeHandleIndex(i);
			return i;
		}

	dwLastError = ERROR_OUT_OF_STRUCTURES;
	return (DWORD) INVALID_HANDLE_VALUE;
}
//--------------------------------------------------------------------------//
// Called from a serialized method
//  _dwNumBytes might be a negative number
//
BOOL IFF::AddToChild (HANDLE hFile, DWORD _dwNumBytes)
{
	DWORD dwReadWrite, dwNumBytes, dwReversed;
	BOOL result = 0;
	size_t childIndex = 0;

	if (!FrontierX::TryDecodeHandleIndex(hFile, MAX_CHILD_HANDLES, childIndex) || childIndex == 0)
	{
		dwLastError = ERROR_INVALID_HANDLE;
		return 0;
	}

	// make sure file stays WORD aligned
	dwNumBytes = ((child[childIndex].dwFileSize + 1 + _dwNumBytes) & ~1) -
				  ((child[childIndex].dwFileSize + 1) & ~1);

	// move everything from end of child file to a new location

	{
		DWORD dwOffset;

		dwOffset = (child[childIndex].dwFileSize + child[childIndex].dwStartOffset + 1) & ~1;

		if ((LONG)dwNumBytes < 0)
			dwOffset += dwNumBytes;
		if (InsertSpace(dwOffset, dwNumBytes, child[childIndex].dwCurrentForm) == 0)
			goto Done;
	}

	child[childIndex].dwFileSize += _dwNumBytes;

	// write new chunk length

	if (child[childIndex].dwChunkLengthOffset == (DWORD)-4)	// if child is a CHUNK, not a FORM
	{
		dwReversed = Reverse(child[childIndex].dwFileSize);
		if (SetFilePointer(0, child[childIndex].dwStartOffset+child[childIndex].dwChunkLengthOffset) == 0xFFFFFFFF)
			goto Done;
		if (WriteFile(0, &dwReversed, sizeof(dwReversed), &dwReadWrite, 0) == 0)
			goto Done;
	}

	result = 1;

Done:
	return result;
}
//--------------------------------------------------------------------------//
// Warning: Assumes that file length fits in 2^31 bits
//
LONG IFF::SetEndOfFile_S (HANDLE hFile)
{
	DWORD dwNumBytes;
	size_t childIndex = 0;

	if (!FrontierX::TryDecodeHandleIndex(hFile, MAX_CHILD_HANDLES, childIndex) || childIndex == 0)
	{
		dwLastError = ERROR_INVALID_HANDLE;
		return 0;
	}

	dwNumBytes = child[childIndex].dwFilePosition - child[childIndex].dwFileSize;

	return AddToChild(hFile, dwNumBytes);
}
//--------------------------------------------------------------------------//
//
DWORD IFF::GetFilePosition (HANDLE hFileHandle, PLONG pPositionHigh)
{
	DWORD result;
	size_t handleIndex = 0;

	if (!FrontierX::TryDecodeHandleIndex(hFileHandle, MAX_CHILD_HANDLES, handleIndex))
	{
		dwLastError = ERROR_INVALID_HANDLE;
		return 0xFFFFFFFF;
	}
	else
	if (hFileHandle == 0)
	{
		result = pParent->GetFilePosition(handles[0], pPositionHigh);
	}
	else
	{
		result = child[handleIndex].dwFilePosition;
		if (pPositionHigh)
			*pPositionHigh = 0;
	}

	if (result == 0xFFFFFFFF)
		dwLastError = pParent->GetLastError();

	return result;
}
//---------------------------------------------------------------------
// position at beginning of a FORM
//
int IFF::RootChunk(void)
{
	if (child[0].dwCurrentForm < MAX_FORMS)
		return ReadChunkInfo(form[child[0].dwCurrentForm].dwOffset+12);
	else
		return ReadChunkInfo(0);
}
//---------------------------------------------------------------------
//	on entry: assumes that file pointer points to a header.
//	on exit: file pointer points just after the header
//
int IFF::ReadChunkInfo (DWORD dwNewOffset)
{
	if (!end_of_form(dwNewOffset))
	{
		long buf[3];
		DWORD result;
		OVERLAPPED overlapped;

		memset(&overlapped, 0, sizeof(overlapped));
		overlapped.Offset = dwChunkOffset = dwNewOffset;
		if (pParent->ReadFile(handles[0], (unsigned char *)buf, 12, &result, &overlapped) == 0)
			pParent->GetOverlappedResult(handles[0], &overlapped, &result, 1);

		if (result < 8)
			return 0;

		dwChunkName = buf[0];
		dwChunkLength = Reverse(buf[1]);
		if (chunk_is_form())	
			dwFormName = buf[2];
		return 1;
	}
	return 0;
}
//--------------------------------------------------------------------------//
//
long IFF::Reverse (long number)
{
	unsigned char *ptr;

	ptr = (unsigned char *) &number;

	return (long(ptr[3]) + 
		   (long(ptr[2]) << 8) +
		   (long(ptr[1]) << 16) +
		   (long(ptr[0]) << 24));
}
//---------------------------------------------------------------------
// exit from current form, read info on next chunk/form
//
void IFF::ExitForm(void)
{
	if (child[0].dwCurrentForm < MAX_FORMS)
	{	
		DWORD dwOldForm = child[0].dwCurrentForm;
		child[0].dwCurrentForm = form[dwOldForm].dwParent;
		ReleaseForm_S(&dwOldForm);

		if (child[0].dwCurrentForm < MAX_FORMS)
		{
			dwChunkName = IFF_FORM;
			dwChunkLength = form[child[0].dwCurrentForm].dwLength;
			dwChunkOffset = form[child[0].dwCurrentForm].dwOffset;
			dwFormName    = form[child[0].dwCurrentForm].dwName;
		}
		else
		{
			RootChunk();
		}
 	}
}
//---------------------------------------------------------------------
// IFF::seek_form()
//	 return TRUE if successful
//
int IFF::SeekForm (DWORD dwSearchName)
{
	int result = FALSE;

	// position ourselves at beginning of a FORM
	if (RootChunk() == 0)
		return 0;

	while ((result = (chunk_is_form() && dwSearchName == dwFormName)) == 0)
	{
		if (skip_to_next_chunk() == 0)
			break;
	}		

	if (result)
	{
		// enter form here
		// dont re-enter the same form
		if (child[0].dwCurrentForm >= MAX_FORMS || dwChunkOffset != form[child[0].dwCurrentForm].dwOffset)
		{
			FORM newForm;
			DWORD dwNewIndex;

			newForm.dwOffset = dwChunkOffset;
			newForm.dwLength = dwChunkLength;
			newForm.dwNextOffset = dwChunkOffset + ((dwChunkLength+9)&~1);
			newForm.dwName = dwFormName;
			newForm.dwParent = child[0].dwCurrentForm;
			newForm.dwUsers = 1;

			dwNewIndex = AddForm(&newForm);

		 	if (dwNewIndex >= MAX_FORMS)
				result = 0;
			else
			{
				child[0].dwCurrentForm = dwNewIndex;
				dwChunkLength = 4;
	 		}
		}
	}

	return result;		
}
//--------------------------------------------------------------------------//
//
int IFF::SeekChunk (DWORD dwSearchName)
{
	int result = FALSE;
	
	if (RootChunk() == 0)
		return 0;

	while ((result = (!chunk_is_form() && dwSearchName == dwChunkName)) == 0)
	{
		if (skip_to_next_chunk() == 0)
			break;
	}		

	return result;		
}
//--------------------------------------------------------------------------//
//
HANDLE IFF::FindFirstFile (LPCTSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData)
{
	FIND_FIRST_STRUCT context = { lpFileName, lpFindFileData };
	return HandleFromSerialResult(
		pParent->SerialCall(this, (DAFILE_SERIAL_PROC) FindFirstFile_S, &context));
}
//--------------------------------------------------------------------------//
//
LONG IFF::FindFirstFile_S (LPVOID lpContext)
{
	LONG result=0;
	FIND_FIRST_STRUCT *findFirst = (FIND_FIRST_STRUCT *) lpContext;
	LPCTSTR lpFileName = findFirst->lpFileName;
	LPWIN32_FIND_DATA lpFindFileData = findFirst->lpFindFileData;

	RootChunk();
	if (!end_of_form(dwChunkOffset))
	{
		int i;

		for (i = 1; i < MAX_FINDFIRST_HANDLES; i++)
		{
			if (ffhandles[i].dwUsers == 0)
			{
				ffhandles[i].dwLength = dwChunkLength;
				ffhandles[i].dwName   = dwChunkName;
				ffhandles[i].dwOffset = dwChunkOffset;
				ffhandles[i].dwNextOffset = dwFormName;
				if ((ffhandles[i].dwParent = child[0].dwCurrentForm) < MAX_FORMS)
					form[child[0].dwCurrentForm].dwUsers++;
				ffhandles[i].dwUsers = 2;
				result = i;
				HANDLE findHandle = EncodeHandleIndex(static_cast<size_t>(result));
				if (FindNextFile(findHandle, lpFindFileData) == 0)
				{
					FindClose(findHandle);
					return -1;
				}
				break;
			}
		}
	}

	if (result==0)
	{
		dwLastError = ERROR_OUT_OF_STRUCTURES;
		return -1;
	}

	return result;
}
//--------------------------------------------------------------------------//
//
BOOL IFF::FindNextFile (HANDLE hFindFile, LPWIN32_FIND_DATA lpData)
{
	FIND_NEXT_STRUCT context = { hFindFile, lpData };
	return pParent->SerialCall(this, (DAFILE_SERIAL_PROC) FindNextFile_S, &context);
}
//--------------------------------------------------------------------------//
//
LONG IFF::FindNextFile_S (LPVOID lpContext)
{
	FIND_NEXT_STRUCT *findNext = (FIND_NEXT_STRUCT *) lpContext;
	LPWIN32_FIND_DATA lpData = findNext->lpFindFileData;
	LONG result;
	size_t i = 0;

	if (!FrontierX::TryDecodeHandleIndex(
		findNext->hFindFile, MAX_FINDFIRST_HANDLES, i) || ffhandles[i].dwUsers == 0)
	{
		result = 0;
		dwLastError = ERROR_INVALID_HANDLE;
	}
	else
	if (ffhandles[i].dwUsers < 2)
	{
		result = 0;
		dwLastError = ERROR_FILE_NOT_FOUND;
	}
	else
	{
		result = 1;

		memset(lpData, 0, sizeof(*lpData));
		lpData->dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
		if ((dwAccess & GENERIC_WRITE) == 0)
			lpData->dwFileAttributes |= FILE_ATTRIBUTE_READONLY;
		lpData->nFileSizeLow = ffhandles[i].dwLength;
		if (ffhandles[i].dwName == IFF_FORM)
		{
			lpData->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
			strncpy(lpData->cFileName, (const char *) &ffhandles[i].dwNextOffset,4);
			strncpy(lpData->cAlternateFileName, (const char *) &ffhandles[i].dwNextOffset,4);
		}
		else
		{
			strncpy(lpData->cFileName, (const char *) &ffhandles[i].dwName,4);
			strncpy(lpData->cAlternateFileName, (const char *) &ffhandles[i].dwName,4);
		}
		lpData->cFileName[4] = 0;
		lpData->cAlternateFileName[4] = 0;
		dwChunkOffset = ffhandles[i].dwOffset;
		dwChunkLength = ffhandles[i].dwLength;
		DWORD dwSavedForm = child[0].dwCurrentForm;
		child[0].dwCurrentForm = ffhandles[i].dwParent;
		if (skip_to_next_chunk() == 0)
			ffhandles[i].dwUsers = 1;
		else
		{
			ffhandles[i].dwLength = dwChunkLength;
			ffhandles[i].dwName   = dwChunkName;
			ffhandles[i].dwOffset = dwChunkOffset;
			ffhandles[i].dwNextOffset = dwFormName;
		}
		// restore current form
		child[0].dwCurrentForm = dwSavedForm;
	}

	return result;
}
//--------------------------------------------------------------------------//
//
BOOL IFF::FindClose (HANDLE hFindFile)
{
	BOOL result;
	size_t i = 0;
   
	if (!FrontierX::TryDecodeHandleIndex(hFindFile, MAX_FINDFIRST_HANDLES, i))
	{
		dwLastError = ERROR_INVALID_HANDLE;
		return 0;
	}
	if (ffhandles[i].dwUsers)
	{
		result = 1;
		DWORD formIndex = ffhandles[i].dwParent;
		SerialCall(this, (DAFILE_SERIAL_PROC) ReleaseForm_S, &formIndex);
		ffhandles[i].dwUsers = 0;
	}
	else
	{
		result = 0;
		dwLastError = ERROR_INVALID_HANDLE;
	}

	return result;
}
//--------------------------------------------------------------------------//
//
HANDLE IFF::CreateNewChunk (DWORD dwName, LPCVOID lpData, DWORD dwSize)
{
	DWORD dwHandle, dwOffset, dwReadWrite, dwReversedSize;
	
	dwHandle = AddHandle();	// called directly since we are already serialized

	if (dwHandle >= MAX_CHILD_HANDLES)
		goto Error;

	// get offset of the end of the form

	if (child[0].dwCurrentForm < MAX_FORMS)
	{
		dwOffset = form[child[0].dwCurrentForm].dwNextOffset;

		// insert bytes into the file

		if (InsertSpace(dwOffset, (dwSize+9)&~1, child[0].dwCurrentForm) == 0)
			goto Error;
	}
	else
		dwOffset = GetFileSize();


	if (SetFilePointer(0, dwOffset) == 0xFFFFFFFF)
		goto Error;
	if (WriteFile(0, &dwName, sizeof(dwName), &dwReadWrite, 0) == 0)
		goto Error;
	dwReversedSize = Reverse(dwSize);
	if (WriteFile(0, &dwReversedSize, sizeof(dwSize), &dwReadWrite, 0) == 0)
		goto Error;
	if (dwSize)
		if (WriteFile(0, lpData, (dwSize + 1) & ~1, &dwReadWrite, 0) == 0)
			goto Error;

	// fill in the child data

	child[dwHandle].dwChunkLengthOffset = (DWORD) -4;
	child[dwHandle].dwStartOffset = dwOffset + 8;
	child[dwHandle].dwFileSize    = dwSize;
	child[dwHandle].dwFilePosition = 0;
	child[dwHandle].dwCurrentForm = child[0].dwCurrentForm;
	if (child[0].dwCurrentForm < MAX_FORMS)
		form[child[0].dwCurrentForm].dwUsers++;

	return EncodeHandleIndex(dwHandle);


Error:
	if (dwHandle < MAX_CHILD_HANDLES)
		handles[dwHandle] = INVALID_HANDLE_VALUE;
	return INVALID_HANDLE_VALUE;
}	
//--------------------------------------------------------------------------//
// called from a serialized method
//    dwNumBytes can be negative, this means shrink the file
//
BOOL IFF::InsertSpace (DWORD dwStartOffset, DWORD dwNumBytes, DWORD dwStartingForm)
{
	DWORD dwLength, dwReadWrite, dwReversed;
	LPVOID lpMemory=0;
	BOOL result=0;
	int i;

	if (dwNumBytes==0)
		return 1;
	if ((LONG)dwNumBytes < 0)
		dwStartOffset -= dwNumBytes;

	dwLength = GetFileSize() - dwStartOffset;	// amount of file to move

	if (dwLength)
	{
		if ((lpMemory = VirtualAlloc(0, dwLength, MEM_COMMIT, PAGE_READWRITE)) == 0)
		{
			dwLastError = ERROR_NOT_ENOUGH_MEMORY;
			goto Done;
		}
		if (SetFilePointer(0, dwStartOffset) == 0xFFFFFFFF)
			goto Done;
		if (ReadFile(0, lpMemory, dwLength, &dwReadWrite, 0) == 0)
			goto Done;
		if (SetFilePointer(0, dwStartOffset+dwNumBytes) == 0xFFFFFFFF)
			goto Done;
		if (WriteFile(0, lpMemory, dwLength, &dwReadWrite, 0) == 0)
			goto Done;
		if (dwReadWrite != dwLength)
		{
			if (pParent->SetFilePointer(handles[0], dwStartOffset) == 0xFFFFFFFF)
				goto Done;
			pParent->WriteFile(handles[0], lpMemory, dwLength, &dwReadWrite, 0);
			goto Done;
		}
		if (SetEndOfFile() == 0)
			goto Done;
	}
	else	// just concat-ing 
	{
		if (SetFilePointer(0, dwStartOffset+dwNumBytes) == 0xFFFFFFFF)
			goto Done;
		if (SetEndOfFile() == 0)
			goto Done;
	}

	// traverse back up the file chain, adding 'dwNumBytes' to every FORM above this chunk
	while (dwStartingForm < MAX_FORMS)
	{
		form[dwStartingForm].dwLength     += dwNumBytes;
		form[dwStartingForm].dwNextOffset += dwNumBytes;
		dwReversed = Reverse(form[dwStartingForm].dwLength);
		if (SetFilePointer(0, form[dwStartingForm].dwOffset+4) == 0xFFFFFFFF)
			goto Done;
		if (WriteFile(0, &dwReversed, sizeof(dwReversed), &dwReadWrite, 0) == 0)
			goto Done;
		dwStartingForm = form[dwStartingForm].dwParent;
	}
	
	// move the start position of every other child that starts after the position
	
	for (i = 1; i < MAX_CHILD_HANDLES; i++)
		if (child[i].dwStartOffset > dwStartOffset)
			child[i].dwStartOffset += dwNumBytes;

	if (dwChunkOffset > dwStartOffset)
		dwChunkOffset += dwNumBytes;
		
	result = 1;

Done:
	if (lpMemory)
		VirtualFree(lpMemory, 0, MEM_RELEASE);

	return result;
}
//--------------------------------------------------------------------------//
//
LONG IFF::RemoveDirectory_S (VOID *lpContext)
{
	char *lpPathName = (char *) lpContext;
	DWORD dwStartOffset, dwNumBytes;
	
	if (SeekForm(*((DWORD *)lpPathName)) == 0)
	{
		dwLastError = ERROR_FILE_NOT_FOUND;
		return 0;
	}

	dwStartOffset = form[child[0].dwCurrentForm].dwOffset;
	dwNumBytes    = form[child[0].dwCurrentForm].dwLength + 8;

	ExitForm();

	if (dwNumBytes > 12)
	{
		dwLastError = ERROR_DIR_NOT_EMPTY;
		return 0;
	}
		
	return InsertSpace(dwStartOffset, -((LONG)dwNumBytes), child[0].dwCurrentForm);
}
//--------------------------------------------------------------------------//
//
LONG IFF::DeleteFile_S (VOID *lpContext)
{
	char *lpFileName = (char *) lpContext;

	if (SeekChunk(*((DWORD *)lpFileName)) == 0)
	{
		dwLastError = ERROR_FILE_NOT_FOUND;
		return 0;
	}

	return InsertSpace(dwChunkOffset, -((LONG)(dwChunkLength+9)&~1), child[0].dwCurrentForm);
}	
//--------------------------------------------------------------------------//
//
HANDLE IFF::OpenExistingChunk (DWORD dwName)
{
	DWORD dwHandle;
	
	dwHandle = AddHandle();

	if (dwHandle >= MAX_CHILD_HANDLES)
		goto Done;

	if (SeekChunk(dwName) == 0)
	{
		handles[dwHandle] = INVALID_HANDLE_VALUE;
		dwHandle = (DWORD) INVALID_HANDLE_VALUE;
		dwLastError = ERROR_FILE_NOT_FOUND;
		goto Done;
	}

	// fill in the child data

	child[dwHandle].dwChunkLengthOffset = (DWORD) -4;
	child[dwHandle].dwStartOffset = dwChunkOffset + 8;
	child[dwHandle].dwFileSize = dwChunkLength;
	child[dwHandle].dwFilePosition = 0;
	child[dwHandle].dwCurrentForm = child[0].dwCurrentForm;
	if (child[0].dwCurrentForm < MAX_FORMS)
		form[child[0].dwCurrentForm].dwUsers++;

Done:
	if (dwHandle == (DWORD) INVALID_HANDLE_VALUE)
		return INVALID_HANDLE_VALUE;
	return EncodeHandleIndex(dwHandle);
}
//--------------------------------------------------------------------------//
//
LONG IFF::OpenExistingForm_S (LPVOID lpContext)
{
	DWORD dwHandle;
	DWORD dwName = *((DWORD *) lpContext);

	dwHandle = AddHandle();

	if (dwHandle >= MAX_CHILD_HANDLES)
		goto Done;

	if (SeekForm(dwName) == 0)
	{
		handles[dwHandle] = INVALID_HANDLE_VALUE;
		dwHandle = (DWORD) INVALID_HANDLE_VALUE;
		dwLastError = ERROR_FILE_NOT_FOUND;
		goto Done;
	}

	// fill in the child data

	child[dwHandle].dwChunkLengthOffset = (DWORD) -8;
	child[dwHandle].dwStartOffset = form[child[0].dwCurrentForm].dwOffset + 12;
	child[dwHandle].dwFileSize = form[child[0].dwCurrentForm].dwLength-4;
	child[dwHandle].dwFilePosition = 0;
	child[dwHandle].dwCurrentForm = child[0].dwCurrentForm;
	if (child[0].dwCurrentForm < MAX_FORMS)
		form[child[0].dwCurrentForm].dwUsers++;
	ExitForm();

Done:
	return dwHandle;
}
//--------------------------------------------------------------------------//
//  Returns index of form allocated, or -1 for failure
//  Copies members of *lpForm into global list if successful.
//    (Called from a serial procedure.)
//
LONG IFF::AddForm (FORM * lpForm)
{
	LONG i, j=-1;

	for (i = 0; i < MAX_FORMS; i++)
	{
		if (form[i].dwUsers == 0)
			j = i;
		else
		if (form[i].dwOffset == lpForm->dwOffset)
		{
			form[i].dwUsers++;
			return i;
		}
	}

	if (j >= 0)
	{
		form[j] = *lpForm;
		if (lpForm->dwParent < MAX_FORMS)
			form[lpForm->dwParent].dwUsers++;
	}

	return j;
}
//--------------------------------------------------------------------------//
//
LONG IFF::ReleaseForm_S (LPVOID lpContext)
{
	DWORD dwIndex = *((DWORD *) lpContext);

	if (dwIndex >= MAX_FORMS || form[dwIndex].dwUsers == 0)
		return 0;

	if (--form[dwIndex].dwUsers == 0)
	{
		dwIndex = form[dwIndex].dwParent;
		ReleaseForm_S(&dwIndex);
	}

	return 1;
}
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//
LPFILESYSTEM CreateAnIFF (void)
{
	return new DAComponent<IFF>;
}
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//--------------------------------END IFF.cpp-------------------------------//
//--------------------------------------------------------------------------//
