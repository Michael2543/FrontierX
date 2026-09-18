#ifndef FRONTIERX_PROFILE_DOCUMENT_H
#define FRONTIERX_PROFILE_DOCUMENT_H
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace FrontierX
{
// Owning, read-only ASCII configuration document for native startup.
// No borrowed handles escape. Failed loads preserve the previous document.
class ProfileDocument
{
public:
    static constexpr std::size_t MaxBytes = 1024 * 1024;
    bool LoadFile(const std::filesystem::path & path, std::string & error);
    bool LoadText(std::string_view text, std::string & error);
    std::vector<std::string> Sections() const;
    std::vector<std::string> Lines(std::string_view section) const;
    std::vector<std::string> Keys(std::string_view section) const;
    std::optional<std::string> Value(std::string_view section, std::string_view key) const;
private:
    std::vector<char> storage_;
};
}
#endif
