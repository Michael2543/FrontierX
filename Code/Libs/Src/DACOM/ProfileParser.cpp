 //--------------------------------------------------------------------------//
//                                                                          //
//                            ProfileParser.cpp                             //
//                                                                          //
//               COPYRIGHT (C) 1997 BY DIGITAL ANVIL, INC.                  //
//                                                                          //
//--------------------------------------------------------------------------//
/*
   $Author: Pbleisch $

   $Header: /Libs/dev/Src/DACOM/ProfileParser.cpp 15    3/21/00 4:30p Pbleisch $
*/
//--------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#ifdef _WIN32
#include <windows.h>
#endif

#include "IProfileParser.h"
#include <FrontierX/EncodedHandle.h>
#include <FrontierX/ProfileBuffer.h>
#include <FrontierX/ProfileParserFactory.h>
#include "TComponent.h"
#include <da_heap_utility.h>

#include <cstdlib>
#include <cstring>
#include <limits>
#ifndef _WIN32
#include <fstream>
#endif

namespace
{
bool IsProfileSymbol(char character)
{
    const auto value = static_cast<unsigned char>(character);
    return (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z') ||
           (value >= '0' && value <= '9') || value == '_';
}

#if !defined(_WIN32) && !defined(PROFILE_PARSER_IS_CASE_SENSITIVE)
unsigned char FoldAscii(unsigned char character)
{
    return character >= 'A' && character <= 'Z'
        ? static_cast<unsigned char>(character + ('a' - 'A')) : character;
}

int CompareAscii(const char * left, const char * right)
{
    while (*left && FoldAscii(static_cast<unsigned char>(*left)) ==
           FoldAscii(static_cast<unsigned char>(*right)))
    {
        ++left;
        ++right;
    }
    const auto lhs = FoldAscii(static_cast<unsigned char>(*left));
    const auto rhs = FoldAscii(static_cast<unsigned char>(*right));
    return lhs == rhs ? 0 : lhs < rhs ? -1 : 1;
}

int CompareAscii(const char * left, const char * right, std::size_t count)
{
    for (std::size_t index = 0; index < count; ++index)
    {
        const auto lhs = FoldAscii(static_cast<unsigned char>(left[index]));
        const auto rhs = FoldAscii(static_cast<unsigned char>(right[index]));
        if (lhs != rhs) return lhs < rhs ? -1 : 1;
        if (!lhs) return 0;
    }
    return 0;
}
#endif
}

#ifdef PROFILE_PARSER_IS_CASE_SENSITIVE
#define compare std::strcmp
#define compare_len std::strncmp
#elif defined(_WIN32)
#define compare _stricmp
#define compare_len _strnicmp
#else
#define compare CompareAscii
#define compare_len CompareAscii
#endif
#ifdef _MSC_VER
#pragma warning (disable : 4100)		// formal parameter unused
#define PROFILE_FASTCALL __fastcall
#else
#define PROFILE_FASTCALL
#endif

//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//

struct DACOM_NO_VTABLE ProfileParser : public IProfileParser
{
	//
	// interface mapping
	//
	BEGIN_DACOM_MAP_INBOUND(ProfileParser)
	DACOM_INTERFACE_ENTRY(IProfileParser)
	DACOM_INTERFACE_ENTRY2(IID_IProfileParser,IProfileParser)
	END_DACOM_MAP()

	//
	// data items
	//

	C8 * fileBuffer;
	U32 bufferSize;


	//
	// startup / shutdown code
	//

	GENRESULT init (PROFPARSEDESC *) { return GR_OK; }

	DA_HEAP_DEFINE_NEW_OPERATOR(ProfileParser);

	ProfileParser( void )
	{
		fileBuffer = NULL;
		bufferSize = 0;
	}

	~ProfileParser (void)
	{
		free();
	}


	/* IProfileParser methods */

	DEFMETHOD(Initialize) (const C8 *fileName, ACCESS access = READ_ACCESS );

	DEFMETHOD_(BOOL32,EnumerateSections) (ENUM_PROC proc = 0, void *context=0);
	
	DEFMETHOD_(HANDLE,CreateSection) (const C8 *sectionName, CREATE_MODE mode = PP_OPENEXISTING);

	DEFMETHOD_(BOOL32,CloseSection) (HANDLE hSection);

	DEFMETHOD_(U32,ReadProfileLine) (HANDLE hSection, U32 lineNumber, C8 * buffer, U32 bufferSize);

	DEFMETHOD_(U32,ReadKeyValue) (HANDLE hSection, const C8 * keyName, C8 * buffer, U32 bufferSize);

	/* ProfileParser methods */

	GENRESULT InitializeBuffer (const C8 *buffer, U32 size);

	void free (void);

	static const char * PROFILE_FASTCALL getLine (const char *buffer, int line);

	static const char * PROFILE_FASTCALL getSection (const char * buffer, int count);

	const char * getLine (HANDLE hSection, int line)
	{
		return getLine(fileBuffer + FrontierX::DecodeEncodedHandle(hSection), line);
	}
};
//--------------------------------------------------------------------------//
//
struct DACOM_NO_VTABLE ProfileParser2 : public IProfileParser2, ProfileParser
{
	using ProfileParser::EnumerateSections;

	//
	// interface mapping
	//
	BEGIN_DACOM_MAP_INBOUND(ProfileParser2)
	DACOM_INTERFACE_ENTRY(IProfileParser2)
	DACOM_INTERFACE_ENTRY(IProfileParser)
	DACOM_INTERFACE_ENTRY2(IID_IProfileParser2,IProfileParser2)
	DACOM_INTERFACE_ENTRY2(IID_IProfileParser,IProfileParser)
	END_DACOM_MAP()


	GENRESULT init (PROFPARSEDESC2 *) { return GR_OK; }

	/* IProfileParser methods */

	DEFMETHOD(Initialize) (const C8 *fileName, ACCESS access)
	{
		return ProfileParser::Initialize(fileName, access);
	}

	virtual HANDLE COMAPI CreateSection (const C8 *sectionName, IProfileParser::CREATE_MODE mode)
	{
		return ProfileParser::CreateSection(sectionName, mode);
	}

	virtual BOOL32 COMAPI CloseSection (HANDLE hSection)
	{
		return ProfileParser::CloseSection(hSection);
	}

	virtual U32 COMAPI ReadProfileLine (HANDLE hSection, U32 lineNumber, C8 * buffer, U32 bufferSize)
	{
		return ProfileParser::ReadProfileLine(hSection, lineNumber, buffer, bufferSize);
	}

	virtual U32 COMAPI ReadKeyValue (HANDLE hSection, const C8 * keyName, C8 * buffer, U32 bufferSize)
	{
		return ProfileParser::ReadKeyValue(hSection, keyName, buffer, bufferSize);
	}

	/* IProfileParser2 methods */

	virtual GENRESULT COMAPI Initialize2 (const C8 *buffer, U32 bufferSize );

	virtual BOOL32 COMAPI EnumerateSections (IProfileCallback * callback, void *context);

	virtual BOOL32 COMAPI EnumerateKeys (IProfileCallback * callback, HANDLE hSection, void *context);
};
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//
void ProfileParser::free (void)
{
	::free( fileBuffer );
	fileBuffer = 0;
}
//--------------------------------------------------------------------------//
//
GENRESULT ProfileParser::Initialize (const C8 *fileName, ACCESS access)
{
#ifdef _WIN32
	HANDLE hFile=INVALID_HANDLE_VALUE;
	GENRESULT result = GR_FILE_ERROR;
	U32 dwAccess = GENERIC_READ;
	U32 dwOpen = OPEN_EXISTING;
	bool bBufferType = (S32(access) < 0);

	if (access == WRITE_ACCESS)
	{
		dwAccess = GENERIC_READ | GENERIC_WRITE;
		dwOpen = OPEN_ALWAYS;
	}

	if (bBufferType || (hFile = CreateFile(fileName, dwAccess, FILE_SHARE_READ|FILE_SHARE_WRITE, 0, dwOpen, FILE_ATTRIBUTE_NORMAL, 0)) != INVALID_HANDLE_VALUE)
	{
		U32 fileSize;
		C8 * oldBuffer = fileBuffer;
		U32 bytesRead;

		if (bBufferType==false)
			fileSize = ::GetFileSize(hFile, 0);
		else
		{
			if (S32(access) == -1)
				fileSize = strlen(fileName);
			else
				fileSize = U32(access) & ~0x80000000;
		}

		if ((fileBuffer = (C8 *) calloc(fileSize+8,1)) != 0)
		{
			if (bBufferType)
				memcpy(fileBuffer+1, fileName, fileSize);
			else
				::ReadFile(hFile, fileBuffer+1, fileSize, &bytesRead, 0);
			::free(oldBuffer);
			bufferSize = fileSize+1;

			FrontierX::ProfileBuffer::MarkSections(fileBuffer);

			result = GR_OK;
		}
		else
		{
			fileBuffer = oldBuffer;
			result = GR_OUT_OF_MEMORY;
		}

		if (bBufferType==false)
			::CloseHandle(hFile);
	}

	return result;
#else
    if (!fileName) return GR_FILE_ERROR;
    if (access != READ_ACCESS && access != WRITE_ACCESS) return GR_INVALID_PARMS;
    std::size_t fileSize = 0;
    std::ifstream input;
    input.open(fileName, std::ios::binary | std::ios::ate);
    if (!input && access == WRITE_ACCESS)
    {
        std::ofstream create(fileName, std::ios::binary | std::ios::app);
        if (!create) return GR_FILE_ERROR;
        create.close();
        input.open(fileName, std::ios::binary | std::ios::ate);
    }
    if (!input) return GR_FILE_ERROR;
    const auto end = input.tellg();
    if (end < 0 || static_cast<std::uintmax_t>(end) >
        static_cast<std::uintmax_t>((std::numeric_limits<U32>::max)() - 8U))
        return GR_FILE_ERROR;
    fileSize = static_cast<std::size_t>(end);

    C8 * next = static_cast<C8 *>(std::calloc(fileSize + 8, 1));
    if (!next) return GR_OUT_OF_MEMORY;
    if (fileSize)
    {
        input.seekg(0, std::ios::beg);
        input.read(next + 1, static_cast<std::streamsize>(fileSize));
        if (!input || static_cast<std::size_t>(input.gcount()) != fileSize)
        {
            std::free(next);
            return GR_FILE_ERROR;
        }
    }

    std::free(fileBuffer);
    fileBuffer = next;
    bufferSize = static_cast<U32>(fileSize + 1);
    FrontierX::ProfileBuffer::MarkSections(fileBuffer);
    return GR_OK;
#endif
}
//--------------------------------------------------------------------------//
//
GENRESULT ProfileParser2::Initialize2 (const C8 *buffer, U32 bufferSize )
{
	return InitializeBuffer(buffer, bufferSize);
}
//--------------------------------------------------------------------------//
//
GENRESULT ProfileParser::InitializeBuffer (const C8 *buffer, U32 size)
{
	if (!buffer && size) return GR_INVALID_PARMS;
	if (size > (std::numeric_limits<U32>::max)() - 8U) return GR_OUT_OF_MEMORY;
	C8 * next = static_cast<C8 *>(std::calloc(static_cast<std::size_t>(size) + 8, 1));
	if (!next) return GR_OUT_OF_MEMORY;
	if (size) std::memcpy(next + 1, buffer, size);
	std::free(fileBuffer);
	fileBuffer = next;
	bufferSize = size + 1;
	FrontierX::ProfileBuffer::MarkSections(fileBuffer);
	return GR_OK;
}
//--------------------------------------------------------------------------//
// find 0D0A, then skip white space
//
const char * PROFILE_FASTCALL ProfileParser::getLine (const char * buffer, int line)
{
	return FrontierX::ProfileBuffer::GetLine(buffer, line);
}
//--------------------------------------------------------------------------//
//
const char * PROFILE_FASTCALL ProfileParser::getSection (const char * buffer, int count)
{
	return FrontierX::ProfileBuffer::GetSection(buffer, count);
}
//--------------------------------------------------------------------------//
//
BOOL32 ProfileParser::EnumerateSections (ENUM_PROC proc, void *context)
{
	if (proc == 0 || fileBuffer==0)
		return 0;

	return FrontierX::ProfileBuffer::EnumerateSections(fileBuffer,
		[this, proc, context](const char * section, const char *) {
			return proc(this, section, context) != 0;
		});
}
//--------------------------------------------------------------------------//
//
BOOL32 ProfileParser2::EnumerateSections (IProfileCallback * callback, void *context)
{
	if (callback == 0 || fileBuffer==0)
		return 0;

	return FrontierX::ProfileBuffer::EnumerateSections(fileBuffer,
		[this, callback, context](const char * section, const char * suffix) {
			return callback->ProfileCallback(this, section, suffix, context) != 0;
		});
}
//--------------------------------------------------------------------------//
//
BOOL32 ProfileParser2::EnumerateKeys (IProfileCallback * callback, HANDLE hSection, void *context)
{
	if (callback == 0 || fileBuffer == 0)
		return 0;

	const char * section = fileBuffer + FrontierX::DecodeEncodedHandle(hSection);
	return FrontierX::ProfileBuffer::EnumerateKeys(section,
		[](char character) { return IsProfileSymbol(character); },
		[this, callback, context](const char * name, const char * value) {
			return callback->ProfileCallback(this, name, value, context) != 0;
		});
}
//--------------------------------------------------------------------------//
//
HANDLE ProfileParser::CreateSection (const C8 *sectionName, CREATE_MODE mode)
{
	HANDLE result = 0;

	if (mode != PP_OPENEXISTING)
		goto Done;						// nothing else is supported yet
	if (fileBuffer == 0)
		goto Done;						// not initialized?

	//
	// is requested sectionName already found within buffer?
	//

	if (sectionName > fileBuffer && sectionName < fileBuffer+bufferSize)
	{
		result = FrontierX::EncodeHandleValue(
			static_cast<FrontierX::EncodedHandleValue>(sectionName - fileBuffer));
	}
	else
	{	  
		//
		// else we must search for the section name 
		//
		const char * buffer = fileBuffer;

		buffer = getSection(buffer, 1);
		while (buffer)
		{
			if (compare(buffer, sectionName) == 0)
			{
				result = FrontierX::EncodeHandleValue(
					static_cast<FrontierX::EncodedHandleValue>(buffer - fileBuffer));
				goto Done;
			}

			buffer = getSection(buffer, 1);
		}
	}

Done:
	return result;
}
//--------------------------------------------------------------------------//
//
BOOL32 ProfileParser::CloseSection (HANDLE)
{
	return 1;
}
//--------------------------------------------------------------------------//
//
U32 ProfileParser::ReadProfileLine (HANDLE hSection, U32 lineNumber, C8 * buffer, U32 bufferSize)
{
	const char * line = getLine(hSection, lineNumber+1);
	return line ? static_cast<U32>(FrontierX::ProfileBuffer::CopyLine(line, buffer, bufferSize)) : 0;
}
//--------------------------------------------------------------------------//
//
U32 ProfileParser::ReadKeyValue (HANDLE hSection, const C8 * keyName, C8 * buffer, U32 outBufferSize)
{
	const char * value = FrontierX::ProfileBuffer::FindValue(
		fileBuffer, bufferSize,
		fileBuffer + FrontierX::DecodeEncodedHandle(hSection), keyName,
		[](const char * left, const char * right, std::size_t count) {
			return compare_len(left, right, count);
		},
		[](char character) { return IsProfileSymbol(character); });
	return value ? static_cast<U32>(FrontierX::ProfileBuffer::CopyLine(value, buffer, outBufferSize)) : 0;
}
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//
IComponentFactory * CreateProfileParserFactory2 (void)
{
	return new DAComponentFactory2<DAComponentAggregate<ProfileParser2>, PROFPARSEDESC2> ("IProfileParser2");
}
IComponentFactory * CreateProfileParserFactory (void)
{
	return new DAComponentFactory2<DAComponentAggregate<ProfileParser>, PROFPARSEDESC> ("IProfileParser");
}

void RegisterProfileParsers(ICOManager * manager)
{
    if (!manager) return;
    IComponentFactory * factory = CreateProfileParserFactory();
    if (factory)
    {
        manager->RegisterComponent(factory, "IProfileParser");
        factory->Release();
    }
    factory = CreateProfileParserFactory2();
    if (factory)
    {
        manager->RegisterComponent(factory, "IProfileParser2");
        factory->Release();
    }
}

//--------------------------------------------------------------------------//
//----------------------------END ProfileParser.cpp-------------------------//
//--------------------------------------------------------------------------//
