#include "ViewCnst.h"
#include "ViewSymbolOperations.h"
#include "Tokendef.h"

#include <algorithm>
#include <cstdint>
#include <cstring>

namespace
{
std::uint64_t ReadUnsigned(const char * data, unsigned size)
{
	std::uint64_t value = 0;
	for (unsigned index = 0; index < size; ++index)
		value |= std::uint64_t(static_cast<unsigned char>(data[index])) << (index * 8);
	return value;
}

void WriteUnsigned(char * data, unsigned size, std::uint64_t value)
{
	for (unsigned index = 0; index < size; ++index)
		data[index] = static_cast<char>(value >> (index * 8));
}

std::uint64_t ReadBits(const char * data, unsigned offset, unsigned count)
{
	std::uint64_t value = 0;
	for (unsigned bit = 0; bit < count; ++bit)
		value |= std::uint64_t((static_cast<unsigned char>(data[(offset + bit) / 8]) >>
			((offset + bit) % 8)) & 1U) << bit;
	return value;
}

void WriteBits(char * data, unsigned offset, unsigned count, std::uint64_t value)
{
	for (unsigned bit = 0; bit < count; ++bit)
	{
		const unsigned index = (offset + bit) / 8;
		const unsigned char mask = static_cast<unsigned char>(1U << ((offset + bit) % 8));
		unsigned char byte = static_cast<unsigned char>(data[index]);
		byte = static_cast<unsigned char>((byte & ~mask) |
			((value & (std::uint64_t(1) << bit)) ? mask : 0));
		data[index] = static_cast<char>(byte);
	}
}

std::int64_t SignExtend(std::uint64_t value, unsigned bits)
{
	if (bits == 64) return static_cast<std::int64_t>(value);
	const std::uint64_t sign = std::uint64_t(1) << (bits - 1);
	return static_cast<std::int64_t>((value ^ sign) - sign);
}

bool ValidBits(const SYMBOL symbol, unsigned storageBits)
{
	return symbol->bit_count > 0 && symbol->bit_count <= storageBits &&
		symbol->bit_offset + symbol->bit_count <= storageBits * 2;
}
}

// Native counterpart of DataViewer::CopySymValue, used by ViewConstructor.
BOOL32 CopyViewSymbolValue(SYMBOL source, U32 sourceModifiers,
	const char * sourceData, SYMBOL destination, U32 destinationModifiers,
	char * destinationData)
{
	(void)destinationModifiers;
	if (!source || !destination || !sourceData || !destinationData)
		return 0;
	const SYMBOL sourceType = ResolveViewSymbolType(source);
	const SYMBOL destinationType = ResolveViewSymbolType(destination);
	const unsigned sourceSize = sourceType->size;
	const unsigned destinationSize = destinationType->size;
	const unsigned copySize = std::min(sourceSize, destinationSize);
	const BOOL32 equalSize = sourceSize == destinationSize;
	sourceModifiers |= source->modifiers;

	if (sourceType->kind == BASICTYPE)
	{
		if (sourceType->basicdt == REAL || sourceType->basicdt == STRINGTYPE ||
			source->bit_count == 0)
		{
			std::memcpy(destinationData, sourceData, copySize);
			return equalSize;
		}
		const unsigned storageBits = sourceSize <= 4 ? 32U : 64U;
		if (!ValidBits(source, storageBits) ||
			!ValidBits(destination, storageBits)) return 0;
		std::uint64_t value = ReadBits(sourceData, source->bit_offset,
			source->bit_count);
		if ((sourceModifiers & MODIFIER_UNSIGNED) == 0)
			value = static_cast<std::uint64_t>(SignExtend(value, source->bit_count));
		WriteBits(destinationData, destination->bit_offset,
			destination->bit_count, value);
		return equalSize;
	}

	if (sourceType->kind == ENUMSYM)
	{
		std::int64_t oldValue = 0;
		if (source->bit_count)
		{
			if (!ValidBits(source, 32)) return 0;
			const std::uint64_t bits = ReadBits(sourceData, source->bit_offset,
				source->bit_count);
			const bool unsignedBool = source->bit_count == 1 &&
				sourceType->name && std::strcmp(sourceType->name, "bool") == 0;
			oldValue = unsignedBool ? static_cast<std::int64_t>(bits) :
				SignExtend(bits, source->bit_count);
		}
		else if (source->size == 1 || source->size == 2 || source->size == 4)
			oldValue = static_cast<std::int64_t>(ReadUnsigned(sourceData, source->size));
		else
			return 0;

		SYMBOL oldEntry = sourceType->datatype;
		while (oldEntry && oldEntry->constval.intnum != oldValue)
			oldEntry = oldEntry->link;
		if (!oldEntry)
		{
			if (!source->bit_count)
				std::memcpy(destinationData, sourceData, copySize);
			return equalSize;
		}
		for (SYMBOL newEntry = destinationType->datatype; newEntry;
			newEntry = newEntry->link)
		{
			if (std::strcmp(newEntry->name, oldEntry->name) != 0) continue;
			const std::uint64_t newValue =
				static_cast<std::uint64_t>(newEntry->constval.intnum);
			if (destination->bit_count)
			{
				if (!ValidBits(destination, 32)) return 0;
				WriteBits(destinationData, destination->bit_offset,
					destination->bit_count, newValue);
			}
			else if (destination->size == 1 || destination->size == 2 ||
				destination->size == 4)
				WriteUnsigned(destinationData, destination->size, newValue);
			else
				return 0;
			return equalSize;
		}
		return 0;
	}
	return 0;
}
