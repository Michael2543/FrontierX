#ifndef VIEW_SYMBOL_OPERATIONS_H
#define VIEW_SYMBOL_OPERATIONS_H

#include "FDump.h"
#include "Symtable.h"

// Keep schema comparisons shared by the Windows viewer and the native parser.
inline SYMBOL ResolveViewSymbolType(SYMBOL symbol)
{
	if (symbol->kind == TYPESYM || symbol->kind == VARSYM)
	{
		if (symbol->datatype)
			return ResolveViewSymbolType(symbol->datatype);
	}
	return symbol;
}

inline bool EqualViewSymbols(SYMBOL oldSymbol, SYMBOL newSymbol)
{
	newSymbol = ResolveViewSymbolType(newSymbol);
	oldSymbol = ResolveViewSymbolType(oldSymbol);

	bool result = (newSymbol->size == oldSymbol->size &&
		newSymbol->name == oldSymbol->name &&
		newSymbol->kind == oldSymbol->kind &&
		newSymbol->bit_count == oldSymbol->bit_count);
	if (!result)
		return false;

	switch (newSymbol->kind)
	{
	case ENUMSYM:
		oldSymbol = oldSymbol->datatype;
		newSymbol = newSymbol->datatype;
		while (newSymbol && oldSymbol)
		{
			if (newSymbol->name != oldSymbol->name ||
				newSymbol->constval.intnum != oldSymbol->constval.intnum)
				return false;
			newSymbol = newSymbol->link;
			oldSymbol = oldSymbol->link;
		}
		return newSymbol == nullptr && oldSymbol == nullptr;

	case RECORDSYM:
		oldSymbol = oldSymbol->datatype;
		newSymbol = newSymbol->datatype;
		while (newSymbol && oldSymbol)
		{
			if (newSymbol->name != oldSymbol->name ||
				!EqualViewSymbols(oldSymbol->datatype, newSymbol->datatype))
				return false;
			newSymbol = newSymbol->link;
			oldSymbol = oldSymbol->link;
		}
		return newSymbol == nullptr && oldSymbol == nullptr;

	case ARRAYSYM:
		return newSymbol->arrayval == oldSymbol->arrayval &&
			EqualViewSymbols(oldSymbol->datatype, newSymbol->datatype);

	default:
		return true;
	}
}

inline bool HasVariableViewSymbolSize(SYMBOL symbol)
{
	symbol = ResolveViewSymbolType(symbol);
	switch (symbol->kind)
	{
	case RECORDSYM:
		symbol = symbol->datatype;
		if (symbol) ASSERT(symbol->kind == VARSYM);
		for (; symbol; symbol = symbol->link)
		{
			if (!symbol->link)
				return HasVariableViewSymbolSize(symbol->datatype);
		}
		return false;

	case ARRAYSYM:
		return symbol->arrayval == 0;

	default:
		return false;
	}
}

#if !defined(_WIN32)
struct IViewConstructor2;
// Exposes the original provider's actual correlation result without changing
// its legacy void virtual interface or accepting a substitute implementation.
bool CorrelateNativeViewSymbols(IViewConstructor2 *provider, SYMBOL oldSymbol,
	const void *oldData, SYMBOL newSymbol, void *newData);
// Original fixed-width Globals only: permit the proven absent legacy lobby
// metadata nibble and the exact absent retail SLOT difficulty field, while
// preserving checked correlation of every other field.
bool CorrelateNativeMissionGlobals(IViewConstructor2 *provider, SYMBOL oldSymbol,
	const void *oldData, SYMBOL newSymbol, void *newData);
#endif

#endif
