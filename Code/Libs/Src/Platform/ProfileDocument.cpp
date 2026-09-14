#include <FrontierX/ProfileDocument.h>
#include <FrontierX/ProfileBuffer.h>
#include <fstream>

namespace
{
unsigned char fold(unsigned char c)
{
    return c >= 'A' && c <= 'Z' ? static_cast<unsigned char>(c + ('a' - 'A')) : c;
}
int compare(const char * a, const char * b, std::size_t count)
{
    for (std::size_t i = 0; i < count; ++i)
    {
        const auto left = fold(static_cast<unsigned char>(a[i]));
        const auto right = fold(static_cast<unsigned char>(b[i]));
        if (left != right) return left < right ? -1 : 1;
        if (!left) return 0;
    }
    return 0;
}
bool ascii(std::string_view text)
{
    for (unsigned char c : text)
        if (c == 0 || c > 127) return false;
    return true;
}
}
namespace FrontierX
{
bool ProfileDocument::LoadText(std::string_view text, std::string & error)
{
    if (text.size() > MaxBytes) { error = "configuration exceeds 1 MiB"; return false; }
    if (!ascii(text)) { error = "configuration must be ASCII without embedded NUL bytes"; return false; }
    std::vector<char> next(text.size() + 8, 0);
    if (!text.empty()) std::memcpy(next.data() + 1, text.data(), text.size());
    ProfileBuffer::MarkSections(next.data());
    storage_.swap(next);
    error.clear();
    return true;
}
bool ProfileDocument::LoadFile(const std::filesystem::path & path, std::string & error)
{
    std::error_code statusError;
    if (!std::filesystem::is_regular_file(path, statusError) || statusError)
    { error = "configuration is not an accessible regular file"; return false; }
    std::ifstream input(path, std::ios::binary);
    if (!input) { error = "cannot open configuration"; return false; }
    std::string text;
    char chunk[4096];
    while (input.read(chunk, sizeof(chunk)) || input.gcount() > 0)
    {
        const auto count = static_cast<std::size_t>(input.gcount());
        if (count > MaxBytes - text.size()) { error = "configuration exceeds 1 MiB"; return false; }
        text.append(chunk, count);
    }
    if (input.bad() || !input.eof()) { error = "configuration read failed"; return false; }
    return LoadText(text, error);
}
std::vector<std::string> ProfileDocument::Sections() const
{
    std::vector<std::string> result;
    if (storage_.empty()) return result;
    const char * section = storage_.data();
    while ((section = ProfileBuffer::GetSection(section, 1))) result.emplace_back(section);
    return result;
}
std::optional<std::string> ProfileDocument::Value(std::string_view name, std::string_view key) const
{
    if (storage_.empty() || !ascii(name) || !ascii(key)) return std::nullopt;
    const std::string keyText(key);
    const char * section = storage_.data();
    while ((section = ProfileBuffer::GetSection(section, 1)))
    {
        if (std::strlen(section) != name.size() || compare(section, name.data(), name.size())) continue;
        const char * value = ProfileBuffer::FindValue(storage_.data(), storage_.size() - 7,
            section, keyText.c_str(), compare, [](char c) {
                return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                       (c >= '0' && c <= '9') || c == '_';
            });
        if (!value) return std::nullopt;
        std::vector<char> output(storage_.size(), 0);
        const auto count = ProfileBuffer::CopyLine(value, output.data(), output.size());
        return std::string(output.data(), count);
    }
    return std::nullopt;
}
}
