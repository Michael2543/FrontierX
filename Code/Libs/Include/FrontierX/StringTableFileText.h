#ifndef FRONTIERX_STRING_TABLE_FILE_TEXT_H
#define FRONTIERX_STRING_TABLE_FILE_TEXT_H

#include <cstddef>
#include <vector>
#include <string>

namespace FrontierX
{
// StringTable UTF children contain NUL-terminated UTF-16LE, independent of wchar_t.
inline bool DecodeStringTableFileText(const unsigned char * bytes, std::size_t size,
                                      std::wstring & text)
{
    if (!bytes || size < 2 || size % 2 != 0 || bytes[size - 2] || bytes[size - 1])
        return false;

    std::wstring decoded;
    for (std::size_t offset = 0; offset + 2 < size; offset += 2)
    {
        const unsigned int unit = static_cast<unsigned int>(bytes[offset]) |
            (static_cast<unsigned int>(bytes[offset + 1]) << 8);
        if (!unit) return false;
        if (sizeof(wchar_t) == 2)
        {
            decoded.push_back(static_cast<wchar_t>(unit));
        }
        else if (unit >= 0xd800 && unit <= 0xdbff)
        {
            if (offset + 4 >= size) return false;
            const unsigned int low = static_cast<unsigned int>(bytes[offset + 2]) |
                (static_cast<unsigned int>(bytes[offset + 3]) << 8);
            if (low < 0xdc00 || low > 0xdfff) return false;
            decoded.push_back(static_cast<wchar_t>(0x10000 +
                ((unit - 0xd800) << 10) + (low - 0xdc00)));
            offset += 2;
        }
        else
        {
            if (unit >= 0xdc00 && unit <= 0xdfff) return false;
            decoded.push_back(static_cast<wchar_t>(unit));
        }
    }
    text.swap(decoded);
    return true;
}

inline bool EncodeStringTableFileText(const wchar_t * text,
                                      std::vector<unsigned char> & bytes)
{
    if (!text) return false;
    std::vector<unsigned char> encoded;
    for (const wchar_t * cursor = text; *cursor; ++cursor)
    {
        unsigned int scalar = static_cast<unsigned int>(*cursor);
        unsigned int units[2] = { scalar, 0 };
        unsigned int count = 1;
        if (sizeof(wchar_t) != 2)
        {
            if (scalar > 0x10ffff || (scalar >= 0xd800 && scalar <= 0xdfff))
                return false;
            if (scalar > 0xffff)
            {
                scalar -= 0x10000;
                units[0] = 0xd800 + (scalar >> 10);
                units[1] = 0xdc00 + (scalar & 0x3ff);
                count = 2;
            }
        }
        for (unsigned int index = 0; index < count; ++index)
        {
            encoded.push_back(static_cast<unsigned char>(units[index] & 0xff));
            encoded.push_back(static_cast<unsigned char>(units[index] >> 8));
        }
    }
    encoded.push_back(0);
    encoded.push_back(0);
    bytes.swap(encoded);
    return true;
}
}

#endif
