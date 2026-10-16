#include "MaterialTextureFixture.h"

#include "DACOM.H"

#include <cstring>
#include <fstream>
#include <vector>

namespace FrontierXTest
{
bool WriteMaterialFixture(const std::filesystem::path & path,
                          const std::string & simpleType,
                          const std::string & mipDirectory)
{
    std::string names("/", 2);
    const U32 materialName = static_cast<U32>(names.size());
    names.append("Material library", sizeof("Material library"));
    const U32 entryName = static_cast<U32>(names.size());
    names.append("Loaded", sizeof("Loaded"));
    const U32 typeName = static_cast<U32>(names.size());
    names.append("Type", sizeof("Type"));
    const U32 otherName = static_cast<U32>(names.size());
    names.append("Other", sizeof("Other"));
    const U32 otherTypeName = static_cast<U32>(names.size());
    names.append("Type", sizeof("Type"));
    const U32 colorName = static_cast<U32>(names.size());
    names.append("C0", sizeof("C0"));
    const U32 alphaName = static_cast<U32>(names.size());
    names.append("C1", sizeof("C1"));
    const U32 textureReferenceName = static_cast<U32>(names.size());
    names.append("T0_name", sizeof("T0_name"));
    const U32 textureLibraryName = static_cast<U32>(names.size());
    names.append("Texture library", sizeof("Texture library"));
    const U32 tileName = static_cast<U32>(names.size());
    names.append("Tile", sizeof("Tile"));
    const U32 widthName = static_cast<U32>(names.size());
    names.append("Image X size", sizeof("Image X size"));
    const U32 heightName = static_cast<U32>(names.size());
    names.append("Image Y size", sizeof("Image Y size"));
    const U32 rgbName = static_cast<U32>(names.size());
    names.append("True RGB 565", sizeof("True RGB 565"));
    const U32 mipName = static_cast<U32>(names.size());
    names.append(mipDirectory.c_str(), mipDirectory.size() + 1U);
    const U32 pixelsName = static_cast<U32>(names.size());
    names.append("Image colors", sizeof("Image colors"));
    const std::string type("StateMaterial");
    std::vector<unsigned char> bytes;
    auto word = [&](U32 value) {
        for (unsigned shift = 0; shift != 32; shift += 8)
            bytes.push_back(static_cast<unsigned char>(value >> shift));
    };
    constexpr U32 directoryOffset = 56U;
    constexpr U32 recordSize = 44U;
    constexpr U32 nameOffset = directoryOffset + 16U * recordSize;
    for (U32 value : {0x20465455U, 0x101U, directoryOffset, 16U * recordSize,
                      0U, recordSize, nameOffset, static_cast<U32>(names.size()),
                      static_cast<U32>(names.size()),
                      nameOffset + static_cast<U32>(names.size()), 0U, 0U, 0U, 0U})
        word(value);
    auto record = [&](U32 next, U32 name, U32 attributes, U32 child, U32 size) {
        for (U32 value : {next, name, attributes, 0U, child, size, size, size,
                          0U, 0U, 0U}) word(value);
    };
    record(0U, 0U, 0x10U, recordSize, 0U);
    record(9U * recordSize, materialName, 0x10U, 2U * recordSize, 0U);
    record(4U * recordSize, entryName, 0x10U, 3U * recordSize, 0U);
    record(0U, typeName, 0U, 0U, static_cast<U32>(type.size()));
    record(0U, otherName, 0x10U, 5U * recordSize, 0U);
    record(6U * recordSize, otherTypeName, 0U, static_cast<U32>(type.size()),
           static_cast<U32>(simpleType.size()));
    record(7U * recordSize, colorName, 0U,
           static_cast<U32>(type.size() + simpleType.size()), 12U);
    record(8U * recordSize, alphaName, 0U,
           static_cast<U32>(type.size() + simpleType.size() + 12U), 4U);
    const U32 textureReferenceData = static_cast<U32>(type.size() + simpleType.size() + 16U);
    record(0U, textureReferenceName, 0U, textureReferenceData, 5U);
    const U32 textureData = textureReferenceData + 5U;
    record(0U, textureLibraryName, 0x10U, 10U * recordSize, 0U);
    record(0U, tileName, 0x10U, 11U * recordSize, 0U);
    record(12U * recordSize, widthName, 0U, textureData, 4U);
    record(13U * recordSize, heightName, 0U, textureData + 4U, 4U);
    record(0U, rgbName, 0x10U, 14U * recordSize, 0U);
    record(0U, mipName, 0x10U, 15U * recordSize, 0U);
    record(0U, pixelsName, 0U, textureData + 8U, 8U);
    bytes.insert(bytes.end(), names.begin(), names.end());
    bytes.insert(bytes.end(), type.begin(), type.end());
    bytes.insert(bytes.end(), simpleType.begin(), simpleType.end());
    auto floatValue = [&](float value) {
        U32 bits = 0;
        std::memcpy(&bits, &value, sizeof(bits));
        word(bits);
    };
    floatValue(0.25F);
    floatValue(0.5F);
    floatValue(0.75F);
    floatValue(0.5F);
    bytes.insert(bytes.end(), {'T', 'i', 'l', 'e', '\0'});
    word(2U);
    word(2U);
    for (U32 pixel : {0xf800U, 0x07e0U, 0x001fU, 0xffffU}) {
        bytes.push_back(static_cast<unsigned char>(pixel));
        bytes.push_back(static_cast<unsigned char>(pixel >> 8U));
    }
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    file.write(reinterpret_cast<const char *>(bytes.data()),
               static_cast<std::streamsize>(bytes.size()));
    return file.good();
}

bool WriteMipTextureFixture(const std::filesystem::path &path, bool alphaPlanes,
                            int levelTwoAlphaExtentDelta, bool rgb888)
{
    struct Entry
    {
        std::string name;
        std::vector<unsigned char> data;
        std::vector<U32> children;
        bool directory;
    };
    std::vector<Entry> entries{{"/", {}, {}, true}};
    auto directory = [&](U32 parent, const std::string &name) {
        const U32 index = static_cast<U32>(entries.size());
        entries.push_back({name, {}, {}, true});
        entries[parent].children.push_back(index);
        return index;
    };
    auto file = [&](U32 parent, const std::string &name, std::vector<unsigned char> data) {
        const U32 index = static_cast<U32>(entries.size());
        entries.push_back({name, std::move(data), {}, false});
        entries[parent].children.push_back(index);
    };
    auto wordBytes = [](U32 value) {
        return std::vector<unsigned char>{static_cast<unsigned char>(value),
            static_cast<unsigned char>(value >> 8), static_cast<unsigned char>(value >> 16),
            static_cast<unsigned char>(value >> 24)};
    };
    const U32 library = directory(0, "Texture library");
    // BlackHole's original ring uses Format_TRUE_3__8_8_8/MIP0 plus a separate
    // alpha plane. A small two-row image pins its channel order and stride.
    const U32 tile = directory(library, rgb888 ? "Rgb888Tile" : alphaPlanes ? "AlphaMipTile" : "MipTile");
    file(tile, "Image X size", wordBytes(rgb888 ? 2 : 256));
    file(tile, "Image Y size", wordBytes(rgb888 ? 2 : 256));
    const U32 palette = directory(tile, rgb888 ? "Format_TRUE_3__8_8_8" : "Palette 8 Bit");
    if (!rgb888)
    {
    file(palette, "Palette color count", wordBytes(9));
    // Each source-authored mip has a distinct color; no generated mip substitute.
    file(palette, "Palette RGB 888", {240,40,80, 40,240,80, 40,80,240,
        200,160,40, 160,40,200, 40,200,160, 120,80,40, 80,40,120, 40,120,80});
    }
    for (U32 level = 0; level < (rgb888 ? 1U : 9U); ++level)
    {
        const U32 mip = directory(palette, "MIP" + std::to_string(level));
        const U32 dimension = (rgb888 ? 2U : 256U) >> level;
        if (rgb888)
            file(mip, "Image colors", {17,34,51, 68,85,102, 119,136,153, 170,187,204});
        else
            file(mip, "Image indices", std::vector<unsigned char>(dimension * dimension,
                                                                   static_cast<unsigned char>(level)));
        if (alphaPlanes)
        {
            std::vector<unsigned char> alpha(dimension * dimension);
            for (U32 pixel = 0U; pixel < dimension * dimension; ++pixel)
                alpha[pixel] = pixel % 3U == 0U ? 0U : pixel % 3U == 1U ? 96U : 255U;
            if (level == (rgb888 ? 0U : 2U))
            {
                const int extent = static_cast<int>(alpha.size()) + levelTwoAlphaExtentDelta;
                if (extent < 0) return false;
                alpha.resize(static_cast<std::size_t>(extent), 255U);
            }
            file(mip, "Alpha 8 bit", std::move(alpha));
        }
    }
    std::string names;
    std::vector<U32> nameOffsets, dataOffsets;
    std::vector<unsigned char> data;
    for (const auto &entry : entries)
    {
        nameOffsets.push_back(static_cast<U32>(names.size()));
        names.append(entry.name.c_str(), entry.name.size() + 1);
        dataOffsets.push_back(static_cast<U32>(data.size()));
        data.insert(data.end(), entry.data.begin(), entry.data.end());
    }
    constexpr U32 headerSize = 56, recordSize = 44;
    const U32 recordsBytes = static_cast<U32>(entries.size()) * recordSize;
    std::vector<unsigned char> bytes;
    auto word = [&](U32 value) {
        const auto encoded = wordBytes(value);
        bytes.insert(bytes.end(), encoded.begin(), encoded.end());
    };
    for (U32 value : {0x20465455U, 0x101U, headerSize, recordsBytes, 0U, recordSize,
        headerSize + recordsBytes, static_cast<U32>(names.size()), static_cast<U32>(names.size()),
        headerSize + recordsBytes + static_cast<U32>(names.size()), 0U, 0U, 0U, 0U}) word(value);
    std::vector<U32> next(entries.size());
    for (const auto &entry : entries)
        for (std::size_t i = 1; i < entry.children.size(); ++i)
            next[entry.children[i - 1]] = entry.children[i] * recordSize;
    for (std::size_t i = 0; i < entries.size(); ++i)
    {
        const auto &entry = entries[i];
        const U32 size = static_cast<U32>(entry.data.size());
        const U32 child = entry.directory ?
            (entry.children.empty() ? 0U : entry.children[0] * recordSize) : dataOffsets[i];
        for (U32 value : {next[i], nameOffsets[i], entry.directory ? 0x10U : 0U, 0U,
            child, size, size, size, 0U, 0U, 0U}) word(value);
    }
    bytes.insert(bytes.end(), names.begin(), names.end());
    bytes.insert(bytes.end(), data.begin(), data.end());
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    output.write(reinterpret_cast<const char *>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    return output.good();
}

}
