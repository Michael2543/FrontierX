#include <FrontierX/UtfMetadata.h>
#include <array>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

namespace FrontierX
{
static bool Inspect(std::istream & input, UtfMetadata & result, std::string & error,
                    const std::string * requested, std::vector<unsigned char> * output,
                    const std::string * requestedDirectory = nullptr,
                    std::vector<UtfDirectoryEntry> * listed = nullptr)
{
    input.seekg(0, std::ios::end);
    const auto end = input.tellg();
    if (end < std::streampos(56)) { error = "truncated UTF header"; return false; }
    const auto fileSize = static_cast<std::uint64_t>(static_cast<std::streamoff>(end));
    input.seekg(0);
    std::array<unsigned char, 56> bytes{};
    if (!input.read(reinterpret_cast<char *>(bytes.data()), bytes.size()))
    { error = "cannot read UTF header"; return false; }
    auto word = [&](std::size_t offset) {
        return std::uint32_t(bytes[offset]) | (std::uint32_t(bytes[offset + 1]) << 8) |
               (std::uint32_t(bytes[offset + 2]) << 16) | (std::uint32_t(bytes[offset + 3]) << 24);
    };
    if (word(0) != 0x20465455) { error = "invalid UTF signature"; return false; }
    if (word(4) != 0x101) { error = "unsupported UTF version"; return false; }
    const std::uint64_t directory = word(8), directorySize = word(12);
    const std::uint64_t names = word(24), namesSize = word(28), namesUsed = word(32);
    const std::uint64_t data = word(36);
    if (word(20) != 44 || directorySize == 0 || directorySize % 44 != 0)
    { error = "invalid UTF directory entry layout"; return false; }
    auto fits = [&](std::uint64_t offset, std::uint64_t size) {
        return offset >= 56 && offset <= fileSize && size <= fileSize - offset;
    };
    if (!fits(directory, directorySize) || !fits(names, namesSize) || namesUsed > namesSize || data > fileSize)
    { error = "UTF metadata range exceeds file bounds"; return false; }
    if (directory < names + namesSize && names < directory + directorySize)
    { error = "UTF directory and names overlap"; return false; }
    if (data < directory + directorySize || data < names + namesSize)
    { error = "UTF data overlaps metadata"; return false; }
    // The header stores an absolute free-entry offset; subsequent dwNext links are relative.
    const std::uint64_t freeAbsolute = word(16);
    if (freeAbsolute && (freeAbsolute < directory || freeAbsolute - directory >= directorySize ||
                         (freeAbsolute - directory) % 44 != 0))
    { error = "invalid UTF free-entry offset"; return false; }
    constexpr std::uint64_t MaxMetadataBytes = 64 * 1024 * 1024;
    if (directorySize + namesUsed > MaxMetadataBytes)
    { error = "UTF metadata exceeds inspector limit (64 MiB)"; return false; }
    std::vector<unsigned char> entries(static_cast<std::size_t>(directorySize));
    std::vector<unsigned char> nameTable(static_cast<std::size_t>(namesUsed));
    input.seekg(static_cast<std::streamoff>(directory));
    if (!input.read(reinterpret_cast<char *>(entries.data()), static_cast<std::streamsize>(entries.size())))
    { error = "truncated UTF directory"; return false; }
    input.seekg(static_cast<std::streamoff>(names));
    if (!nameTable.empty() && !input.read(reinterpret_cast<char *>(nameTable.data()), static_cast<std::streamsize>(nameTable.size())))
    { error = "truncated UTF name table"; return false; }
    auto field = [&](std::size_t offset) {
        return std::uint32_t(entries[offset]) | (std::uint32_t(entries[offset + 1]) << 8) |
               (std::uint32_t(entries[offset + 2]) << 16) | (std::uint32_t(entries[offset + 3]) << 24);
    };
    auto validLink = [&](std::uint32_t offset) { return offset % 44 == 0 && offset < directorySize; };
    if (field(8) == 0xffffffff || !(field(8) & 0x10) || field(0) != 0)
    { error = "invalid UTF root directory"; return false; }
    std::vector<bool> visited(entries.size() / 44, false);
    struct Pending { std::uint32_t offset; std::string parent; };
    std::vector<Pending> pending{{0, ""}};
    bool found = false;
    std::uint64_t selectedOffset = 0, selectedSize = 0;
    bool unsupported = false;
    std::uint32_t files = 0;
    bool directoryFound = false;
    std::vector<UtfDirectoryEntry> candidateEntries;
    while (!pending.empty())
    {
        const auto item = pending.back();
        const auto offset = item.offset;
        pending.pop_back();
        if (!validLink(offset) || visited[offset / 44])
        { error = "invalid, repeated or cyclic UTF entry link"; return false; }
        visited[offset / 44] = true;
        const auto attributes = field(offset + 8);
        if (attributes == 0xffffffff)
        { error = "live UTF tree references an unused entry"; return false; }
        const auto name = field(offset + 4);
        if (name >= nameTable.size() ||
            std::find(nameTable.begin() + name, nameTable.end(), 0) == nameTable.end())
        { error = "invalid or unterminated UTF name"; return false; }
        const auto next = field(offset);
        if (next) pending.push_back({next, item.parent});
        const bool usePaths = requested || requestedDirectory;
        const std::string nameText = usePaths ?
            std::string(reinterpret_cast<const char *>(nameTable.data() + name)) : "";
        const std::string virtualPath = offset == 0 ? "" : item.parent + nameText;
        if (usePaths && virtualPath.size() > 4096)
        { error = "UTF virtual path exceeds reader limit"; return false; }
        if (usePaths && offset && (nameText.empty() || nameText.find_first_of("/\\") != std::string::npos))
        { error = "UTF entry name cannot be represented as a virtual path"; return false; }
        if (requestedDirectory)
        {
            if (virtualPath == *requestedDirectory && (attributes & 0x10))
                directoryFound = true;
            const std::string parent = requestedDirectory->empty() ? "" :
                *requestedDirectory + "/";
            if (offset && item.parent == parent)
            {
                const auto duplicate = std::find_if(candidateEntries.begin(),
                    candidateEntries.end(), [&](const UtfDirectoryEntry & entry) {
                        return entry.name == nameText;
                    });
                if (duplicate != candidateEntries.end())
                { error = "ambiguous UTF directory child"; return false; }
                candidateEntries.push_back({nameText, (attributes & 0x10) != 0,
                    (attributes & 0x10) ? 0U : field(offset + 24)});
            }
        }
        const auto content = field(offset + 16);
        if (attributes & 0x10)
        {
            if (content) pending.push_back({content, virtualPath.empty() ? "" : virtualPath + "/"});
        }
        else
        {
            const std::uint64_t allocated = field(offset + 20), used = field(offset + 24);
            if (used > allocated || content > fileSize - data || allocated > fileSize - data - content)
            { error = "UTF payload range exceeds allocated/file bounds"; return false; }
            if (requested && virtualPath == *requested)
            {
                if (found) { error = "ambiguous UTF entry path"; return false; }
                found = true;
                selectedOffset = data + content;
                selectedSize = used;
                unsupported = used != field(offset + 28);
            }
            ++files;
        }
    }
    std::uint32_t unused = freeAbsolute ? static_cast<std::uint32_t>(freeAbsolute - directory) : 0;
    while (unused)
    {
        if (!validLink(unused) || visited[unused / 44] || field(unused + 8) != 0xffffffff)
        { error = "invalid UTF unused-entry chain"; return false; }
        visited[unused / 44] = true;
        unused = field(unused);
    }
    if (requested)
    {
        if (!found) { error = "UTF entry not found (exact case required)"; return false; }
        if (unsupported) { error = "UTF stored/logical sizes differ; decoding is not implemented"; return false; }
        if (selectedSize > 64 * 1024 * 1024) { error = "UTF entry exceeds reader limit (64 MiB)"; return false; }
        std::vector<unsigned char> payload(static_cast<std::size_t>(selectedSize));
        input.seekg(static_cast<std::streamoff>(selectedOffset));
        if (!input || (!payload.empty() && !input.read(reinterpret_cast<char *>(payload.data()),
                                                      static_cast<std::streamsize>(payload.size()))))
        { error = "UTF entry read failed"; return false; }
        output->swap(payload);
    }
    if (requestedDirectory)
    {
        if (!directoryFound)
        { error = "UTF directory not found (exact case required)"; return false; }
        listed->swap(candidateEntries);
    }
    result = {static_cast<std::uint32_t>(directorySize / 44), static_cast<std::uint32_t>(namesUsed), files};
    error.clear();
    return true;
}

static bool Inspect(const std::filesystem::path & path, UtfMetadata & result, std::string & error,
                    const std::string * requested, std::vector<unsigned char> * output,
                    const std::string * requestedDirectory = nullptr,
                    std::vector<UtfDirectoryEntry> * listed = nullptr)
{
    std::error_code statusError;
    if (!std::filesystem::is_regular_file(path, statusError) || statusError)
    { error = "UTF metadata is not an accessible regular file"; return false; }
    std::ifstream input(path, std::ios::binary);
    if (!input) { error = "cannot open UTF metadata"; return false; }
    return Inspect(input, result, error, requested, output, requestedDirectory, listed);
}

static bool Inspect(const std::vector<unsigned char> & bytes, UtfMetadata & result,
                    std::string & error, const std::string * requested,
                    std::vector<unsigned char> * output,
                    const std::string * requestedDirectory = nullptr,
                    std::vector<UtfDirectoryEntry> * listed = nullptr)
{
    if (bytes.size() > 64U * 1024U * 1024U)
    { error = "nested UTF exceeds reader limit (64 MiB)"; return false; }
    std::istringstream input(std::string(bytes.begin(), bytes.end()), std::ios::binary);
    return Inspect(input, result, error, requested, output, requestedDirectory, listed);
}

bool InspectUtfMetadata(const std::filesystem::path & path, UtfMetadata & result, std::string & error)
{
    return Inspect(path, result, error, nullptr, nullptr);
}
template<class Source>
static bool ReadEntry(const Source & path, const std::string & entry,
                  std::vector<unsigned char> & output, std::string & error)
{
    if (entry.empty()) { error = "empty UTF entry path"; return false; }
    if (entry.size() > 4096 || entry.front() == '/' || entry.back() == '/' ||
        entry.find("//") != std::string::npos || entry.find('\\') != std::string::npos)
    { error = "invalid UTF entry path"; return false; }
    UtfMetadata metadata;
    return Inspect(path, metadata, error, &entry, &output);
}
template<class Source>
static bool ListDirectory(const Source & path, const std::string & directory,
                      std::vector<UtfDirectoryEntry> & output, std::string & error)
{
    if (directory.size() > 4096 || (!directory.empty() &&
        (directory.front() == '/' || directory.back() == '/' ||
         directory.find("//") != std::string::npos ||
         directory.find('\\') != std::string::npos)))
    { error = "invalid UTF directory path"; return false; }
    UtfMetadata metadata;
    return Inspect(path, metadata, error, nullptr, nullptr, &directory, &output);
}

bool InspectUtfMetadata(const std::vector<unsigned char> & bytes, UtfMetadata & result,
                        std::string & error)
{
    return Inspect(bytes, result, error, nullptr, nullptr);
}
bool ReadUtfEntry(const std::filesystem::path & path, const std::string & entry,
                  std::vector<unsigned char> & output, std::string & error)
{ return ReadEntry(path, entry, output, error); }
bool ReadUtfEntry(const std::vector<unsigned char> & bytes, const std::string & entry,
                  std::vector<unsigned char> & output, std::string & error)
{ return ReadEntry(bytes, entry, output, error); }
bool ListUtfDirectory(const std::filesystem::path & path, const std::string & directory,
                      std::vector<UtfDirectoryEntry> & output, std::string & error)
{ return ListDirectory(path, directory, output, error); }
bool ListUtfDirectory(const std::vector<unsigned char> & bytes, const std::string & directory,
                      std::vector<UtfDirectoryEntry> & output, std::string & error)
{ return ListDirectory(bytes, directory, output, error); }
}
