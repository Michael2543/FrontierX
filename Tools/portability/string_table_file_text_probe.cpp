#include "FrontierX/StringTableFileText.h"
#include "StringTable.h"

#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

namespace
{
bool Require(bool condition, const char * message)
{
    if (!condition) std::cerr << message << '\n';
    return condition;
}
}

static_assert(sizeof(IStringEnum::StringInfo::idString) == 4,
              "StringTable enumeration IDs must retain Win32 width");
static_assert(offsetof(IStringEnum::StringInfo, idString) == 2 * sizeof(void *),
              "StringTable enumeration ID must follow two native pointers");
static_assert(std::is_same<decltype(IStringEnum::StringInfo::idString), U32>::value,
              "StringTable enumeration ID must use fixed-width U32");

int main()
{
    const wchar_t * text = L"A\u03a9\U0001f642";
    const unsigned char expected[] = {
        0x41, 0x00, 0xa9, 0x03, 0x3d, 0xd8, 0x42, 0xde, 0x00, 0x00
    };
    std::vector<unsigned char> encoded;
    if (!Require(FrontierX::EncodeStringTableFileText(text, encoded) &&
                 encoded.size() == sizeof(expected) &&
                 std::memcmp(encoded.data(), expected, sizeof(expected)) == 0,
                 "StringTable file encoding changed the Win32 UTF-16LE bytes")) return 1;

    std::wstring decoded;
    if (!Require(FrontierX::DecodeStringTableFileText(encoded.data(), encoded.size(), decoded) &&
                 decoded == text,
                 "StringTable file decoding did not recover native wide text")) return 2;

    const std::wstring retained = decoded;
    const unsigned char truncated[] = {0x41, 0x00, 0x00};
    const unsigned char missingNul[] = {0x41, 0x00};
    const unsigned char embeddedNul[] = {0x00, 0x00, 0x41, 0x00, 0x00, 0x00};
    const unsigned char loneHigh[] = {0x3d, 0xd8, 0x00, 0x00};
    if (!Require(!FrontierX::DecodeStringTableFileText(truncated, sizeof(truncated), decoded) &&
                 !FrontierX::DecodeStringTableFileText(missingNul, sizeof(missingNul), decoded) &&
                 !FrontierX::DecodeStringTableFileText(embeddedNul, sizeof(embeddedNul), decoded) &&
                 !FrontierX::DecodeStringTableFileText(loneHigh, sizeof(loneHigh), decoded) &&
                 decoded == retained,
                 "StringTable file decoder accepted malformed UTF-16LE or changed its output"))
        return 3;

    encoded.clear();
    if (!Require(FrontierX::EncodeStringTableFileText(L"", encoded) &&
                 encoded.size() == 2 && encoded[0] == 0 && encoded[1] == 0 &&
                 FrontierX::DecodeStringTableFileText(encoded.data(), encoded.size(), decoded) &&
                 decoded.empty(), "StringTable empty-file representation changed")) return 4;

    if (sizeof(wchar_t) == 4)
    {
        const wchar_t invalid[] = {static_cast<wchar_t>(0xd800), 0};
        if (!Require(!FrontierX::EncodeStringTableFileText(invalid, encoded) &&
                     encoded.size() == 2,
                     "StringTable encoder accepted an invalid native scalar")) return 5;
    }
    return 0;
}
