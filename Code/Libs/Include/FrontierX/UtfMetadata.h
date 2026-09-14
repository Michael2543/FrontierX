#ifndef FRONTIERX_UTF_METADATA_H
#define FRONTIERX_UTF_METADATA_H
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace FrontierX
{
// Decoded summary only; never map this runtime type onto file bytes.
struct UtfMetadata
{
    std::uint32_t directoryEntries = 0;
    std::uint32_t nameBytesUsed = 0;
    std::uint32_t reachableFiles = 0;
};
// Checks UTF 1.01 metadata, reachable tree/name bounds and payload extents.
// Does not read/decompress payloads or validate archetype schemas.
bool InspectUtfMetadata(const std::filesystem::path & path, UtfMetadata & result, std::string & error);
// Exact case-sensitive virtual path, with '/' separators and no leading root separator.
// Reads at most 64 MiB; rejects unequal stored/logical lengths (no decompressor yet).
// Output remains unchanged on failure; container is validated and read on one open stream.
bool ReadUtfEntry(const std::filesystem::path & path, const std::string & entry,
                  std::vector<unsigned char> & output, std::string & error);
}
#endif
