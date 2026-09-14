// Extracted from DACOM/ProfileParser.cpp.
// COPYRIGHT (C) 1997 BY DIGITAL ANVIL, INC.
#ifndef FRONTIERX_PROFILE_BUFFER_H
#define FRONTIERX_PROFILE_BUFFER_H

#include <cstring>

namespace FrontierX
{
namespace ProfileBuffer
{

// Internal parser representation, not a disk format or module interface.
// Input storage must be zero-filled, with text at offset 1 and at least seven
// trailing zero bytes, matching ProfileParser::Initialize's fileSize + 8 allocation.
// Navigation accepts only pointers into that live storage, with nonnegative counts.
// MarkSections is called once before navigation. Returned pointers borrow the buffer.
inline const char * GetLine(const char * buffer, int line)
{
	const char * tmp;

	while (line>0)
	{
		if ((tmp = std::strchr(buffer, '\n')) == 0)
		{
			const std::size_t len = std::strlen(buffer);

			buffer += len+1;
			if (buffer[0] == 0)
			{
				buffer = 0;
				goto Done;
			}
		}
		else
		{
			buffer = tmp+1;
			switch (*buffer)
			{
			case 0:
				buffer = 0;
				goto Done;
			case '\r':
				buffer++;
				break;
			}

			while (*buffer == ' ')
				buffer++;
			if (*buffer == 0)
			{
				buffer = 0;
				goto Done;
			}

			line--;
		}

	}

Done:	
	return buffer;
}

inline const char * GetSection(const char * buffer, int count)
{
	std::size_t len;

	while (count>0)
	{
		len = std::strlen(buffer);
		buffer += len;
		if (buffer[1] != 0)		// found a ']'
		{
			buffer++;
		}
		else
		if (buffer[2] != 0)		// found a '['
		{
			count--;
			buffer += 2;
		}
		else
			return 0;			// end of the line
	}

	return buffer;
}

// The caller supplies its existing case/locale policy. section must point into storage;
// textSize includes the leading sentinel, as in ProfileParser::bufferSize.
template<class Compare, class IsSymbol>
const char * FindValue(const char * storage, std::size_t textSize, const char * section,
                      const char * key, Compare compare, IsSymbol isSymbol)
{
	const std::size_t keyLength = std::strlen(key);
	const char * line = section;
	while ((line = GetLine(line, 1)) != 0)
	{
		const std::size_t offset = static_cast<std::size_t>(line - storage);
		if (offset <= textSize && keyLength <= textSize - offset &&
		    !isSymbol(line[keyLength]) && compare(line, key, keyLength) == 0)
		{
			// Preserve the legacy search extent: strchr scans to the next NUL marker.
			const char * value = std::strchr(line, '=');
			if (value)
			{
				do { ++value; } while (*value == ' ');
				return value;
			}
		}
	}
	return 0;
}

// Copy one logical line/value. A zero-capacity request cannot produce output.
// A null destination queries the capped length using the legacy empty-value sentinel.
inline std::size_t CopyLine(const char * text, char * output, std::size_t capacity)
{
	if (capacity == 0)
		return 0;
	const char * newline = std::strchr(text, '\n');
	std::size_t length = newline ? static_cast<std::size_t>(newline - text) : std::strlen(text);
	if (newline && length && text[length - 1] == '\r')
		--length;
	if (length >= capacity)
		length = capacity - 1;
	if (output)
	{
		if (length == 0 && capacity > 1)
		{
			output[0] = ' ';
			length = 1;
		}
		else
			std::memcpy(output, text, length);
		output[length] = 0;
	}
	else if (length == 0)
		length = 1;
	return length;
}

inline void MarkSections(char * fileBuffer)
{
	char * oldBuffer = fileBuffer+1;
	while (*oldBuffer == ' ')
		oldBuffer++;			// skip white space
	do
	{
		if (*oldBuffer == '[')      // next section
		{
			char * tmp, * tmp2;

			if ((tmp = std::strchr(oldBuffer, ']')) != 0)
			{
				tmp2 = std::strchr(oldBuffer, '\n');			// if no return between [ ... ]

				if (tmp2 == 0 || tmp2 > tmp)
				{
					// assume that previous character is not important,
					// put double 00 for '[', single 0 for ']'

					oldBuffer[-1] = 0;
					*oldBuffer = 0;		// mark it
					*tmp = 0;
					oldBuffer = tmp+1;
				}
			}
		}
	} while ((oldBuffer = (char *) GetLine(oldBuffer, 1)) != 0);
	
}

}
}
#endif
