#ifndef FRONTIERX_UTF_METADATA_H
#define FRONTIERX_UTF_METADATA_H
#include <cstdint>
#include <filesystem>
#include <map>
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
struct UtfDirectoryEntry
{
    std::string name;
    bool directory = false;
    std::uint32_t size = 0;
};
// Runtime-only lookup table for an already validated disk archive.
struct UtfArchiveIndex
{
    struct File { std::uint64_t offset; std::uint32_t size; bool compressed; };
    std::map<std::string, std::vector<UtfDirectoryEntry>> directories;
    std::map<std::string, File> files;
    std::map<std::string, std::string> foldedDirectories;
    std::map<std::string, std::string> foldedFiles;
};
bool BuildUtfArchiveIndex(const std::filesystem::path & path,
                          UtfArchiveIndex & output, std::string & error);
bool ListIndexedUtfDirectory(const UtfArchiveIndex & index, const std::string & directory,
                             std::vector<UtfDirectoryEntry> & output, std::string & error);
bool ReadIndexedUtfEntry(const std::filesystem::path & path, const UtfArchiveIndex & index,
                         const std::string & entry, std::vector<unsigned char> & output,
                         std::string & error);
// Native IFileSystem emulates the Windows archive's case-insensitive lookup.
// Standalone metadata readers above retain exact-case semantics.
bool ResolveIndexedUtfDirectory(const UtfArchiveIndex & index, const std::string & requested,
                                std::string & exact);
bool ResolveIndexedUtfFile(const UtfArchiveIndex & index, const std::string & requested,
                           std::string & exact);
// Runtime input for a newly written UTF 1.01 container. Paths use exact-case
// '/'-separated components; directory items permit empty saved directories.
struct UtfArchiveItem
{
    std::string path;
    bool directory = false;
    std::vector<unsigned char> bytes;
};
// Encodes the fixed-width 56-byte header and 44-byte directory records.
// Output remains unchanged on failure; this does not write or flush a file.
bool EncodeUtfArchive(const std::vector<UtfArchiveItem> & items,
                      std::vector<unsigned char> & output, std::string & error);
// Checks UTF 1.01 metadata, reachable tree/name bounds and payload extents.
// Does not read/decompress payloads or validate archetype schemas.
bool InspectUtfMetadata(const std::filesystem::path & path, UtfMetadata & result, std::string & error);
// Exact case-sensitive virtual path, with '/' separators and no leading root separator.
// Reads at most 64 MiB; rejects unequal stored/logical lengths (no decompressor yet).
// Output remains unchanged on failure; container is validated and read on one open stream.
bool ReadUtfEntry(const std::filesystem::path & path, const std::string & entry,
                  std::vector<unsigned char> & output, std::string & error);
// Lists direct children of an exact-case virtual directory. Empty path means root.
// Output remains unchanged when metadata or the requested directory is invalid.
bool ListUtfDirectory(const std::filesystem::path & path, const std::string & directory,
                      std::vector<UtfDirectoryEntry> & output, std::string & error);
// Same validation for a nested archive read from an IFileSystem child (64 MiB maximum).
bool InspectUtfMetadata(const std::vector<unsigned char> & bytes, UtfMetadata & result,
                        std::string & error);
bool ReadUtfEntry(const std::vector<unsigned char> & bytes, const std::string & entry,
                  std::vector<unsigned char> & output, std::string & error);
bool ListUtfDirectory(const std::vector<unsigned char> & bytes, const std::string & directory,
                      std::vector<UtfDirectoryEntry> & output, std::string & error);
}
#endif
