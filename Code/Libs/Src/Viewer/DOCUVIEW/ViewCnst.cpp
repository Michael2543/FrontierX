//--------------------------------------------------------------------------//
//                                                                          //
//                               ViewCnst.cpp                                 //
//                                                                          //
//                  COPYRIGHT (C) 2002 Fever Pitch Studios, INC.               //
//                                                                          //
//--------------------------------------------------------------------------//
/*

    $Header: /Libs/Src/Viewer/DataViewer/ViewCnst.cpp 2     11/15/02 10:42a Tmauer $

					View Contructor, parses C/C++ header, 
*/			    
//--------------------------------------------------------------------------//

// 4514: unused inline function
// 4201: nonstandard no-name structure use
// 4100: formal parameter was not used
// 4512: assignment operator could not be generated
// 4245: conversion from signed to unsigned
// 4127: constant condition expression
// 4355: 'this' used in member initializer
// 4244: conversion from int to unsigned char, possible loss of data
// 4200: zero sized struct member
// 4710: inline function not expanded
// 4702: unreachable code
// 4786: truncating function name (255 chars) in browser info
#pragma warning (disable : 4514 4201 4100 4512 4245 4127 4355 4244 4710 4702 4786)
#define WIN32_LEAN_AND_MEAN
#ifdef _WIN32
#include <windows.h>
#endif
#pragma warning (disable : 4355 4201)

#include "ViewCnst.h"
#include "Symtable.h"
#include "ViewSymbolOperations.h"
#ifdef _WIN32
#include "DataView.h"
#include "Document.h"
#include "IConnection.h"
#include "TSmartPointer.h"
#else
struct VIEWDESC;
#endif
#include "TComponent.h"
#include "HeapObj.h"
#include "DataParser.h"
#ifndef _WIN32
#include "Ferror.h"
#endif

#include <algorithm>
#include <cstring>

#ifdef _WIN32
extern HINSTANCE hInstance;
#endif
extern ICOManager * DACOM;

static char interface_name[] = "IViewConstructor";

extern SymbolManager SymbolTable;

static BOOL32 CorrelateSymbol (SYMBOL pOldSymbol, char *pOldData, SYMBOL pNewSymbol, char *pNewData
#if !defined(_WIN32)
	, SYMBOL optionalLegacyMetadata = nullptr, SYMBOL optionalLegacySlotChallenge = nullptr,
	  SYMBOL optionalLegacyCommandLimit = nullptr
#endif
);
void COMAPI CreateDataParser (SYMBOL symbol, IDataParser ** pParser);
#ifndef _WIN32
BOOL32 CopyViewSymbolValue (SYMBOL source, U32 sourceModifiers, const char * sourceData,
	SYMBOL destination, U32 destinationModifiers, char * destinationData);
#endif

//--------------------------------------------------------------------------
//  
struct ViewConstructor : public IViewConstructor, IViewConstructor2, IAggregateComponent
{
	U32 dwRefs;
	SYMBOL table;
	IDAComponent * outer;

	BEGIN_DACOM_MAP_INBOUND(ViewConstructor)
	DACOM_INTERFACE_ENTRY(IViewConstructor)
	DACOM_INTERFACE_ENTRY(IViewConstructor2)
	DACOM_INTERFACE_ENTRY2(IID_IViewConstructor,IViewConstructor)
	DACOM_INTERFACE_ENTRY2(IID_IViewConstructor2,IViewConstructor2)
	END_DACOM_MAP()

	
	ViewConstructor (void)
	{
		dwRefs=1;
	}

	void * operator new (size_t size)
	{
		return HEAP->ClearAllocateMemory(size, "ViewConstructor");
	}
#ifndef _WIN32
	void operator delete (void * memory) noexcept
	{
		if (memory) HEAP->FreeMemory(memory);
	}
#endif

	/* IDAComponent members */

	DEFMETHOD(QueryInterface) (const C8 *interface_name, void **instance);
	DEFMETHOD_(U32,AddRef)           (void);
	DEFMETHOD_(U32,Release)          (void);

	/* IComponentFactory members */

	DEFMETHOD(CreateInstance) (DACOMDESC *descriptor, void **instance);

	/* IAggregateComponent members */

	DEFMETHOD(Initialize) (void);

	/* IViewConstructor members */

	DEFMETHOD(ParseFile) (const C8 *filename);

	DEFMETHOD(ParseMemory) (const C8 *memory);

	DEFMETHOD_(BOOL32,EnumerateTypes) (IVIEWCNST_ENUM_PROC proc, void *context=0);

	DEFMETHOD_(U32,GetTypeSize) (const C8 *typeName);

	/* IViewConstructor2 members */

	DEFMETHOD_(BOOL32,EnumerateTypes) (IVIEWCNST_ENUM_PROC2 proc, void *context=0);

	DEFMETHOD_(U32,GetTypeSize) (SYMBOL symbol);
	
	DEFMETHOD_(HANDLE,ParseNewMemory) (const C8 *memory);	// create a secondary symbol table

	DEFMETHOD_(SYMBOL,GetSymbol) (HANDLE hSymbolList, const C8 *pszSymbolName); // get handle to specific type
	
	DEFMETHOD(DestroySymbols) (HANDLE hSymbolList);

	DEFMETHOD_(void,CorrelateSymbol) (SYMBOL oldSymbol, void *pOldData, SYMBOL newSymbol, void *pNewData);

	DEFMETHOD_(BOOL32,IsEqual) (SYMBOL oldSymbol, SYMBOL newSymbol);

	DEFMETHOD_(BOOL32,HasVariableSize) (SYMBOL symbol);

	/* ViewConstructor members */

	GENRESULT CreateViewer (VIEWDESC *lpDesc, void **instance);

	IDAComponent * getBase (void)
	{
		return (IViewConstructor *)this;
	}
};
//--------------------------------------------------------------------------//
//
GENRESULT ViewConstructor::Initialize (void)
{
	return GR_OK;
}
//--------------------------------------------------------------------------//
//
GENRESULT ViewConstructor::CreateInstance (DACOMDESC *lpDesc, void **instance)
{
	GENRESULT		result        = GR_OK;
	ViewConstructor *pNewInstance = NULL;
	AGGDESC * aggDesc = (AGGDESC *) lpDesc;

	if (lpDesc==NULL || (lpDesc->interface_name==NULL))
	{
		result = GR_INTERFACE_UNSUPPORTED;
		goto Done;
	}

   //
   // If unsupported interface requested, fail call
   //

	if (strcmp(lpDesc->interface_name, "IDataParser") == 0)
	{
		if (lpDesc->size != sizeof(DPARSERDESC))
		{
			result = GR_INVALID_PARMS;
			goto Done;
		}

		CreateDataParser(((DPARSERDESC *)lpDesc)->symbol, (IDataParser **) instance);
		if (*instance==0)
			return GR_GENERIC;
		return GR_OK;
	}
	else
	if (strcmp(lpDesc->interface_name, interface_name))
	{
		return CreateViewer((VIEWDESC *) lpDesc, instance);
	}

	if (lpDesc->size == sizeof(AGGDESC))
	{
		outer = aggDesc->outer;
		*aggDesc->inner = getBase();
	}
	else
	if (lpDesc->size != sizeof(DACOMDESC))
	{
		result = GR_INTERFACE_UNSUPPORTED;
		goto Done;
	}

	if (table == 0 && (table = SymbolTable.set_reserved()) != 0)
		table->AddRef();

	AddRef();
	pNewInstance = this;

Done:
	*instance = pNewInstance;
	return result;
}
//--------------------------------------------------------------------------//
//
GENRESULT ViewConstructor::QueryInterface (const C8 *interface_name, void **instance)
{
	int i;
	const _DACOM_INTMAP_ENTRY * interfaces = _GetEntriesIn();

	for (i = 0; interfaces[i].interface_name; i++)
	{
		if (strcmp(interfaces[i].interface_name, interface_name) == 0)
		{
			IDAComponent *result = (IDAComponent *) (((char *) this) + interfaces[i].offset);
			result->AddRef();
			*instance = result;
			return GR_OK;
		}
	}

	*instance = 0;
	return GR_INTERFACE_UNSUPPORTED;
}
//--------------------------------------------------------------------------//
//
U32 ViewConstructor::AddRef (void)
{
	dwRefs++;
	return dwRefs;
}
//--------------------------------------------------------------------------//
//
U32 ViewConstructor::Release (void)
{
	if (dwRefs > 0)
		dwRefs--;
	if (dwRefs <= 1)
	{
		SYMBOL tmp;

		outer = 0;

		while (table)
		{
			tmp = table->link;
			table->Release();
			table = tmp;
		}

		SymbolTable.free();
			
		if (dwRefs == 0)
		{
 			dwRefs++;	// prevent infinite looping
			delete this;
			return 0;
		}
	}

	return dwRefs;
}

SYMBOL parse_file   (const char *name);
SYMBOL parse_memory (const char *memory);

//--------------------------------------------------------------------------//
//
GENRESULT ViewConstructor::ParseFile (const C8 *value)
{
	SYMBOL sym = parse_file(value), tmp;

	if (sym)
	{
		if ((tmp = table) != 0)
		{
			while (tmp->link)
				tmp = tmp->link;

			tmp->link = sym;	
		}
		else
			table = sym;
	}

	return GR_OK;
}
//--------------------------------------------------------------------------//
//
GENRESULT ViewConstructor::ParseMemory (const C8 *memory)
{
#ifndef _WIN32
	num_errors = 0;
#endif
	SYMBOL sym = parse_memory(memory), tmp;

#ifndef _WIN32
	if (num_errors || !sym)
	{
		if (sym) DestroySymbols(sym);
		return GR_GENERIC;
	}
#endif

	if (sym)
	{
		if ((tmp = table) != 0)
		{
			while (tmp->link)
				tmp = tmp->link;

			tmp->link = sym;	
		}
		else
			table = sym;
	}

	return GR_OK;
}
//--------------------------------------------------------------------------//
//
BOOL32 ViewConstructor::EnumerateTypes (IVIEWCNST_ENUM_PROC proc, void *context)
{
	BOOL32 result = 1;
	SYMBOL list = table;

	while (list && result)
	{
		result = proc(this, list->name,	context);
		list = list->link;
	}

	return result;
}
//--------------------------------------------------------------------------//
//
U32 ViewConstructor::GetTypeSize (const C8 *typeName)
{
	SYMBOL list = table;

	while (list)
	{
		if (strcmp(typeName, list->name) == 0)
			return list->size;
		list = list->link;
	}

	return 0;
}
//--------------------------------------------------------------------------//
//
BOOL32 ViewConstructor::EnumerateTypes (IVIEWCNST_ENUM_PROC2 proc, void *context)
{
	BOOL32 result = 1;
	SYMBOL list = table;

	while (list && result)
	{
		result = proc(this, list->name,	list, context);
		list = list->link;
	}

	return result;
}
//--------------------------------------------------------------------------//
//
U32 ViewConstructor::GetTypeSize (SYMBOL symbol)
{
	return symbol->size;
}
//--------------------------------------------------------------------------//
//
HANDLE ViewConstructor::ParseNewMemory (const C8 *memory)
{
	SYMBOL oldTable = table;

	SymbolTable.free();
	if ((table = SymbolTable.set_reserved()) != 0)
		table->AddRef();
	
#ifndef _WIN32
	num_errors = 0;
#endif
	SYMBOL sym = parse_memory(memory), tmp;

	if (sym)
	{
		if ((tmp = table) != 0)
		{
			while (tmp->link)
				tmp = tmp->link;

			tmp->link = sym;	
		}
		else
			table = sym;
	}

	tmp = table;
	table = oldTable;	// restore old state
	SymbolTable.free();

#ifndef _WIN32
	if (num_errors || !sym)
	{
		if (tmp) DestroySymbols(tmp);
		return 0;
	}
#endif

	return tmp;			// return new parsed list
}
//--------------------------------------------------------------------------//
//
SYMBOL ViewConstructor::GetSymbol (HANDLE hSymbolList, const C8 *pszSymbolName)
{
	SYMBOL list = (SYMBOL) hSymbolList;

	if (list == 0)
		list = table;

	while (list)
	{
		if (strcmp(pszSymbolName, list->name) == 0)
			break;
		list = list->link;
	}

	return list;
}
//--------------------------------------------------------------------------//
//
GENRESULT ViewConstructor::DestroySymbols (HANDLE hSymbolList)
{
	SYMBOL tmp, list = (SYMBOL) hSymbolList;

	while (list)
	{
		tmp = list->link;
		list->Release();
		list = tmp;
	}

	return GR_OK;
}
//--------------------------------------------------------------------------//
//
void ViewConstructor::CorrelateSymbol (SYMBOL oldSymbol, void *pOldData, SYMBOL newSymbol, void *pNewData)
{
	memset(pNewData, 0, ResolveViewSymbolType(newSymbol)->size);		// initialize output to 0

	::CorrelateSymbol(oldSymbol, (char *)pOldData, newSymbol, (char *)pNewData);
}
#if !defined(_WIN32)
bool CorrelateNativeViewSymbols(IViewConstructor2 *provider, SYMBOL oldSymbol,
	const void *oldData, SYMBOL newSymbol, void *newData)
{
	if (!dynamic_cast<ViewConstructor *>(provider) || !oldSymbol || !newSymbol ||
		!oldData || !newData)
		return false;
	SYMBOL oldType = ResolveViewSymbolType(oldSymbol);
	SYMBOL newType = ResolveViewSymbolType(newSymbol);
	if (!oldType || !newType || oldType->size < 0 || newType->size < 0 ||
		(!oldType->size && (oldType->kind != RECORDSYM || oldType->datatype)) ||
		(!newType->size && (newType->kind != RECORDSYM || newType->datatype)))
		return false;
	std::memset(newData, 0, newType->size);
	return ::CorrelateSymbol(oldSymbol, const_cast<char *>(static_cast<const char *>(oldData)),
		newSymbol, static_cast<char *>(newData)) != 0;
}

bool CorrelateNativeMissionGlobals(IViewConstructor2 *provider, SYMBOL oldSymbol,
	const void *oldData, SYMBOL newSymbol, void *newData)
{
	// Retail initial maps omit later lobby metadata and use older OPTIONS.
	// The Windows provider clears the destination and leaves absent fields
	// zero. Pin those schema variants; retain every other required field.
	if (!dynamic_cast<ViewConstructor *>(provider) || !oldSymbol || !newSymbol ||
		!oldData || !newData) return false;
	SYMBOL oldType = ResolveViewSymbolType(oldSymbol);
	SYMBOL newType = ResolveViewSymbolType(newSymbol);
	if (!oldType || !newType || oldType->kind != RECORDSYM || newType->kind != RECORDSYM ||
		!oldType->name || !newType->name || std::strcmp(oldType->name, "MT_GlobalData") ||
		std::strcmp(newType->name, "MT_GlobalData") || oldType->size != 3236 || newType->size != 3236)
		return false;
	// MT_GlobalData is an ordinary struct. Reject an invented union-like
	// replacement that would take the original provider's raw-copy shortcut.
	if (!oldType->datatype || !newType->datatype ||
		(oldType->datatype->link && oldType->datatype->link->offset == 0 &&
		 oldType->datatype->bit_count == 0) ||
		(newType->datatype->link && newType->datatype->link->offset == 0 &&
		 newType->datatype->bit_count == 0)) return false;
	const auto field = [](SYMBOL record, const char *name) {
		for (SYMBOL value = record->datatype; value; value = value->link)
		{
			const char *fieldName = value->name ? value->name : ResolveViewSymbolType(value)->name;
			if (fieldName && !std::strcmp(fieldName, name)) return value;
		}
		return SYMBOL(nullptr);
	};
	SYMBOL metadata = field(newType, "maxPlayers");
	SYMBOL player = field(newType, "currentPlayer");
	SYMBOL oldPlayer = field(oldType, "currentPlayer");
	if (!metadata || !player || !oldPlayer || metadata->offset != 624 ||
		metadata->bit_offset != 4 || metadata->bit_count != 4 ||
		player->offset != 624 || player->bit_offset != 0 || player->bit_count != 4 ||
		oldPlayer->offset != 624 || oldPlayer->bit_offset != 0 || oldPlayer->bit_count != 4 ||
		ResolveViewSymbolType(metadata)->kind != BASICTYPE ||
		ResolveViewSymbolType(metadata)->size != 1 ||
		ResolveViewSymbolType(player)->size != 1 || ResolveViewSymbolType(oldPlayer)->size != 1)
		return false;
	SYMBOL oldMetadata = field(oldType, "maxPlayers");
	if (oldMetadata && (oldMetadata->offset != 624 || oldMetadata->bit_offset != 4 ||
		oldMetadata->bit_count != 4 || ResolveViewSymbolType(oldMetadata)->size != 1)) return false;
	SYMBOL optionalMetadata = oldMetadata ? nullptr : metadata;
	const auto slot = [&](SYMBOL globals) {
		SYMBOL game = field(globals, "gameSettings");
		if (!game) return SYMBOL(nullptr);
		game = ResolveViewSymbolType(game);
		if (game->kind != RECORDSYM || game->size != 76) return SYMBOL(nullptr);
		SYMBOL base = field(game, "_CQGAME");
		if (!base) return SYMBOL(nullptr);
		base = ResolveViewSymbolType(base);
		if (base->kind != RECORDSYM || base->size != 68) return SYMBOL(nullptr);
		SYMBOL slots = field(base, "slot");
		if (!slots || slots->offset != 4) return SYMBOL(nullptr);
		slots = ResolveViewSymbolType(slots);
		if (slots->kind != ARRAYSYM || slots->arrayval != 8 || slots->size != 64)
			return SYMBOL(nullptr);
		SYMBOL type = ResolveViewSymbolType(slots->datatype);
		return type && type->kind == RECORDSYM && type->name &&
			!std::strcmp(type->name, "SLOT") && type->size == 8 ? type : SYMBOL(nullptr);
	};
	SYMBOL oldSlot = slot(oldType), newSlot = slot(newType);
	if (!oldSlot || !newSlot) return false;
	SYMBOL challenge = field(newSlot, "compChalange");
	if (!challenge || challenge->offset != 0 || challenge->bit_offset != 3 ||
		challenge->bit_count != 4 || ResolveViewSymbolType(challenge)->kind != BASICTYPE ||
		ResolveViewSymbolType(challenge)->size != 4) return false;
	SYMBOL oldChallenge = field(oldSlot, "compChalange");
	if (oldChallenge && (oldChallenge->offset != 0 || oldChallenge->bit_offset != 3 ||
		oldChallenge->bit_count != 4 || ResolveViewSymbolType(oldChallenge)->size != 4)) return false;
	const bool missingChallenge = oldChallenge == nullptr;
	struct SlotField { const char *name; unsigned bit; unsigned count; };
	const SlotField slots[] = {{"type",0,3}, {"state",7,3}, {"race",10,4},
		{"color",14,5}, {"team",19,4}, {"zoneSeat",23,3}};
	for (const auto &expected : slots)
	{
		SYMBOL oldField = field(oldSlot, expected.name), newField = field(newSlot, expected.name);
		const unsigned oldBit = missingChallenge && expected.bit ? expected.bit-4 : expected.bit;
		if (!oldField || !newField || oldField->offset != 0 || newField->offset != 0 ||
			oldField->bit_offset != oldBit || newField->bit_offset != expected.bit ||
			oldField->bit_count != expected.count || newField->bit_count != expected.count ||
			ResolveViewSymbolType(oldField)->size != 4 || ResolveViewSymbolType(newField)->size != 4)
			return false;
	}
	SYMBOL oldID = field(oldSlot, "dpid"), newID = field(newSlot, "dpid");
	if (!oldID || !newID || oldID->offset != 4 || newID->offset != 4 ||
		oldID->bit_count || newID->bit_count || ResolveViewSymbolType(oldID)->size != 4 ||
		ResolveViewSymbolType(newID)->size != 4) return false;
	// The added four-bit lobby/standard-AI difficulty slot is absent from retail
	// maps. Preserve the original Windows zero default; do not invent AI rules.
	SYMBOL optionalChallenge = missingChallenge ? challenge : nullptr;
	const auto options = [&](SYMBOL globals) {
		SYMBOL game = field(globals, "gameSettings");
		if (!game || game->offset != 3160) return SYMBOL(nullptr);
		SYMBOL value = field(ResolveViewSymbolType(game), "OPTIONS");
		if (!value || value->offset != 68) return SYMBOL(nullptr);
		value = ResolveViewSymbolType(value);
		return value && value->kind == RECORDSYM && value->size == 8 ? value : SYMBOL(nullptr);
	};
	SYMBOL oldOptions = options(oldType), newOptions = options(newType);
	if (!oldOptions || !newOptions) return false;
	SYMBOL command = field(newOptions, "commandLimit");
	SYMBOL oldCommand = field(oldOptions, "commandLimit");
	const auto integerBits = [](SYMBOL value, unsigned bit, unsigned count, const char *legacyEnum = nullptr) {
		if (!value || value->offset != 4 || value->bit_offset != bit || value->bit_count != count)
			return false;
		SYMBOL type = ResolveViewSymbolType(value);
		return type->size == 4 && (type->kind == BASICTYPE ||
			(type->kind == ENUMSYM && legacyEnum && type->name && !std::strcmp(type->name, legacyEnum)));
	};
	if (!integerBits(command, 30, 2) || (oldCommand && !integerBits(oldCommand, 30, 2, "COMMANDLIMIT"))) return false;
	const bool legacyOptions = oldCommand == nullptr;
	// The older OPTIONS record spends six bits on gameSpeed and has no
	// commandLimit. Correlate named values through the original provider and
	// retain its zero default only for that absent field, never raw-copy bits.
	struct OptionField { const char *name; unsigned bit; unsigned count; const char *legacyEnum; };
	const OptionField optionFields[] = {{"gameType",0,3,"GAMETYPE"}, {"gameSpeed",3,5,nullptr},
		{"regenOn",8,1,nullptr}, {"spectatorsOn",9,1,nullptr}, {"lockDiplomacyOn",10,1,nullptr},
		{"numSystems",11,5,nullptr}, {"money",16,2,"MONEY"}, {"mapType",18,2,"MAPTYPE"},
		{"templateType",20,2,"RANDOM_TEMPLATE"}, {"mapSize",22,2,"MAPSIZE"},
		{"terrain",24,2,"TERRAIN"}, {"units",26,2,"STARTING_UNITS"}, {"visibility",28,2,"VISIBILITYMODE"}};
	for (const auto &expected : optionFields)
	{
		const unsigned oldBit = expected.bit + (legacyOptions && expected.bit > 3 ? 1U : 0U);
		const unsigned oldCount = expected.count +
			(legacyOptions && !std::strcmp(expected.name, "gameSpeed") ? 1U : 0U);
		if (!integerBits(field(newOptions, expected.name), expected.bit, expected.count) ||
			!integerBits(field(oldOptions, expected.name), oldBit, oldCount, expected.legacyEnum)) return false;
	}
	SYMBOL oldVersion = field(oldOptions, "version"), newVersion = field(newOptions, "version");
	if (!oldVersion || !newVersion || oldVersion->offset || newVersion->offset ||
		oldVersion->bit_count || newVersion->bit_count ||
		ResolveViewSymbolType(oldVersion)->size != 4 || ResolveViewSymbolType(newVersion)->size != 4) return false;
	std::memset(newData, 0, newType->size);
	return ::CorrelateSymbol(oldSymbol, const_cast<char *>(static_cast<const char *>(oldData)),
		newSymbol, static_cast<char *>(newData), optionalMetadata, optionalChallenge,
        legacyOptions ? command : nullptr) != 0;
}
#endif

//--------------------------------------------------------------------------//
//
BOOL32 ViewConstructor::IsEqual (SYMBOL oldSymbol, SYMBOL newSymbol)
{
	return EqualViewSymbols(oldSymbol, newSymbol);
}
//--------------------------------------------------------------------------//
//
BOOL32 ViewConstructor::HasVariableSize (SYMBOL symbol)
{
	return HasVariableViewSymbolSize(symbol);
}
//--------------------------------------------------------------------------//
//
#ifdef _WIN32
GENRESULT ViewConstructor::CreateViewer (VIEWDESC *lpDesc, void **instance)
{
	GENRESULT result = GR_GENERIC;
	DAComponent<DataViewer> *pNewInstance = NULL;
	SYMBOL list = 0;
	COMPTR<IDAConnectionPoint> connection;
	HWND hOwnerWindow=0;

	//
	// If unsupported interface requested, fail call
	//

	if (strcmp(lpDesc->interface_name, "IViewer"))
	{
		result = GR_INTERFACE_UNSUPPORTED;
		goto Done;
	}

	if (lpDesc->size != sizeof(*lpDesc))
	{
		// create viewer from SYMBOL directly
		if (lpDesc->size == sizeof(*lpDesc) + 4 && strcmp(lpDesc->className, "SYMBOL")==0)
		{
			list = ((SYMBOL *)(lpDesc+1))[0];
			hOwnerWindow = (HWND) lpDesc->hOwnerWindow;
		}
		else
		if (lpDesc->size != sizeof(*lpDesc) - 8)		// old style
		{
			result = GR_INTERFACE_UNSUPPORTED;
			goto Done;
		}
	}
	else
	{
		hOwnerWindow = (HWND) lpDesc->hOwnerWindow;
	}

	if (lpDesc->doc->QueryOutgoingInterface("IDocumentClient", connection) != GR_OK)
	{
		result = GR_GENERIC;
		goto Done;
	}

	if (list==0)
	{
		list = table;
		while (list)
		{
			if (strcmp(list->name, lpDesc->className) == 0)
				break;
			list = list->link;
		}
	}
	
	if (list)
	{
		if (lpDesc->doc->GetFileSize() == (DWORD) list->size || DataViewer::HasVariableSize(list))
		{
			if ((pNewInstance = new DAComponent<DataViewer>) == 0)
			{
				result = GR_OUT_OF_MEMORY;
				goto Done;
			}

			pNewInstance->doc = lpDesc->doc;
			pNewInstance->symbol = list;
			pNewInstance->hParentWindow = hOwnerWindow;
			pNewInstance->set_spelling_list(lpDesc->spellingSet);
			list->AddRef();

			strncpy(pNewInstance->szClassName, list->name, sizeof(pNewInstance->szClassName)-1);
			strcpy_s(pNewInstance->szInstanceName, "\1(Instance name goes here)\1");
			if (connection->Advise((IDocumentClient *)pNewInstance, &pNewInstance->connHandle) != GR_OK ||
				pNewInstance->init() == 0)
			{
				connection->Unadvise(pNewInstance->connHandle);
				list->Release();
				delete pNewInstance;
				pNewInstance = 0;
			}
			else
			{
				pNewInstance->OnUpdate(lpDesc->doc);
				pNewInstance->CreateView();
				result = GR_OK;
			}
		}
	}
	
Done:
	*instance = pNewInstance;
	return result;
}
#else
GENRESULT ViewConstructor::CreateViewer (VIEWDESC *, void **instance)
{
	*instance = 0;
	return GR_INTERFACE_UNSUPPORTED;
}
#endif
//--------------------------------------------------------------------------//
//
static SYMBOL GetTypeByName (SYMBOL list, SYMBOL match)
{
	const char * matchName;
	const char * oldName;

	if ((matchName = match->name) == 0)
	{
		if ((matchName = ResolveViewSymbolType(match)->name) == 0)
		{
#if !defined(_WIN32)
			// Source TOBJMOVE persists an unnamed overrideSpeed/overrideYaw
			// union. Two separately parsed identical schemas cannot match it
			// by a name. Require one exact storage/member match; never default
			// an absent or ambiguous record and never alter serialized names.
			SYMBOL target = ResolveViewSymbolType(match);
			if (target->kind != RECORDSYM || target->size <= 0 ||
				!target->datatype || !target->datatype->link ||
				target->datatype->offset != 0 || target->datatype->link->offset != 0 ||
				target->datatype->bit_count) return 0;
			SYMBOL found = nullptr;
			for (SYMBOL candidate = list; candidate; candidate = candidate->link)
			{
				if (candidate->name || candidate->size != match->size || candidate->offset != match->offset ||
					candidate->bit_count != match->bit_count || candidate->bit_offset != match->bit_offset)
					continue;
				SYMBOL type = ResolveViewSymbolType(candidate);
				if (type->name || type->kind != RECORDSYM || type->size != target->size) continue;
				SYMBOL oldMember = type->datatype, newMember = target->datatype;
				bool equal = true;
				while (oldMember && newMember)
				{
					SYMBOL oldType = ResolveViewSymbolType(oldMember->datatype);
					SYMBOL newType = ResolveViewSymbolType(newMember->datatype);
					if (!oldMember->name || !newMember->name ||
						std::strcmp(oldMember->name, newMember->name) ||
						oldMember->offset || newMember->offset ||
						oldMember->bit_count || newMember->bit_count ||
						oldMember->size != newMember->size ||
						oldMember->modifiers != newMember->modifiers ||
						oldType->modifiers != newType->modifiers ||
						(oldType->kind == BASICTYPE && newType->kind == BASICTYPE &&
						 oldType->basicdt != newType->basicdt) ||
						!EqualViewSymbols(oldMember->datatype, newMember->datatype))
					{
						equal = false;
						break;
					}
					oldMember = oldMember->link;
					newMember = newMember->link;
				}
				if (!equal || oldMember || newMember) continue;
				if (found) return 0; // No unique original storage member.
				found = candidate;
			}
			return found;
#else
			return 0;
#endif
		}
	}

	while (list)
	{
		if ((oldName = list->name) == 0)
			oldName = ResolveViewSymbolType(list)->name;
		
		//if (oldName && strcmp(oldName, matchName) == 0)
		if (oldName == matchName)	// all names come from the same table
			break;
		list = list->link;
	}

	return list;
}
//--------------------------------------------------------------------------//
// return TRUE if symbols are equal
//
BOOL32 CorrelateSymbol (SYMBOL pOldSymbol, char *pOldData, SYMBOL pNewSymbol, char *pNewData
#if !defined(_WIN32)
	, SYMBOL optionalLegacyMetadata, SYMBOL optionalLegacySlotChallenge, SYMBOL optionalLegacyCommandLimit
#endif
)
{
	SYMBOL type = ResolveViewSymbolType(pNewSymbol);
	BOOL32 result = 1;

	switch (type->kind)
	{
	case BASICTYPE:
	case ENUMSYM:
#ifdef _WIN32
		result &= DataViewer::CopySymValue(pOldSymbol, 0, (const char *) pOldData, pNewSymbol, 0, (char *)pNewData);
#else
		result &= CopyViewSymbolValue(pOldSymbol, 0, (const char *) pOldData, pNewSymbol, 0, (char *)pNewData);
#endif
		break;

	case RECORDSYM:
	{
		SYMBOL pOldSubType;

		pOldSymbol = ResolveViewSymbolType(pOldSymbol);
		pOldSubType = pOldSymbol->datatype;	// this should be a VARSYM, can be NULL if empty struct!
		
		//
		// do not correlate unions. that causes too much chaos
		//
		if (pOldSubType==0 || (pOldSubType->link && pOldSubType->link->offset == 0 && pOldSubType->bit_count==0))	// union if next var is also at offset 0
		{	
			memcpy(pNewData, pOldData, std::min(pOldSymbol->size, type->size));
		}
		else	// we are a struct
		{
			pOldSymbol = pOldSubType;
			type = type->datatype;									// this should be a VARSYM

			while (type)
			{
				if ((pOldSubType = GetTypeByName(pOldSymbol, type)) != 0)
					result &= CorrelateSymbol(pOldSubType, pOldData+pOldSubType->offset, type, pNewData+type->offset
#if !defined(_WIN32)
						, optionalLegacyMetadata, optionalLegacySlotChallenge, optionalLegacyCommandLimit
#endif
					);
				else
#if !defined(_WIN32)
				if (type != optionalLegacyMetadata && type != optionalLegacySlotChallenge &&
                    type != optionalLegacyCommandLimit)
#endif
					result = 0;

				type = type->link;
			}
		}
	}
		break;

	case ARRAYSYM:
	{
		int i = type->arrayval;		// i = num entries in the array

		pOldSymbol = ResolveViewSymbolType(pOldSymbol);
		if (i != pOldSymbol->arrayval)
			result = 0;
		i = std::min(i, pOldSymbol->arrayval);	// i min of old and new

		pOldSymbol = pOldSymbol->datatype;		// this should be some kind of type
		type = type->datatype;					// this should be some kind of type
		if (type->kind != pOldSymbol->kind)
			result = 0;
		else
		{
			if (type->kind == BASICTYPE && type->size == pOldSymbol->size)
			{
				memcpy(pNewData, pOldData, type->size * i);
				pOldData+=type->size*i;
				pNewData+=type->size*i;
			}
			else
			while (i-- > 0)
			{
				result &= CorrelateSymbol(pOldSymbol, pOldData, type, pNewData
#if !defined(_WIN32)
					, optionalLegacyMetadata, optionalLegacySlotChallenge, optionalLegacyCommandLimit
#endif
				);
				pOldData+=pOldSymbol->size;
				pNewData+=type->size;
			}
		}
	}
	break;
	
	case POINTERSYM:
		break;
	}


	return result;
}
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
//
void RegisterViewConstructor (ICOManager * DACOM)
{
	ViewConstructor *view;

	if ((view = new ViewConstructor) != 0)
	{
		DACOM->RegisterComponent((IViewConstructor *)view, interface_name);
		view->Release();
	}
}

//--------------------------------------------------------------------------//
//----------------------------End ViewCnst.cpp------------------------------//
//--------------------------------------------------------------------------//
