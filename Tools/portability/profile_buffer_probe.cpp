#include <FrontierX/ProfileBuffer.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace
{
std::vector<char> prepare(const char * text)
{
    std::vector<char> buffer(std::strlen(text) + 8, 0);
    std::memcpy(buffer.data() + 1, text, std::strlen(text));
    FrontierX::ProfileBuffer::MarkSections(buffer.data());
    return buffer;
}

void require(bool condition, const char * message)
{
    if (!condition)
        throw std::string(message);
}

void checkSections(const char * text, const std::vector<std::string> & expected)
{
    const auto buffer = prepare(text);
    const char * section = FrontierX::ProfileBuffer::GetSection(buffer.data(), 1);
    for (const auto & name : expected)
    {
        require(section && name == section, "section order/name mismatch");
        section = FrontierX::ProfileBuffer::GetSection(section, 1);
    }
    require(section == nullptr, "unexpected extra section");
}
}

int main()
{
    using namespace FrontierX::ProfileBuffer;
    try
    {
        char output[8] = {'!', '!', '!', '!', '!', '!', '!', '!'};
        require(CopyLine("abcdef\r\nignored", output, 4) == 3 &&
                std::strcmp(output, "abc") == 0 && output[4] == '!', "truncation/canary failed");
        require(CopyLine("abc\r\n", output, sizeof(output)) == 3 &&
                std::strcmp(output, "abc") == 0, "CRLF copy failed");
        require(CopyLine("abc\r", output, sizeof(output)) == 4 &&
                std::strcmp(output, "abc\r") == 0, "bare CR must be preserved");
        require(CopyLine("", output, sizeof(output)) == 1 &&
                std::strcmp(output, " ") == 0, "empty-value sentinel lost");
        output[0] = '!';
        require(CopyLine("abc", output, 0) == 0 && output[0] == '!', "zero capacity wrote output");
        require(CopyLine("abc", output, 1) == 0 && output[0] == 0, "capacity one must terminate");
        require(CopyLine("abc", nullptr, 3) == 2, "length query must honor capacity");
        require(CopyLine("", nullptr, 1) == 1, "empty length query sentinel changed");
        require(CopyLine("", nullptr, 0) == 0, "zero-capacity query failed");
        const char * keys = "[Config]\n;key=comment\nkey_suffix=no\nkey = first ;literal\nkey=second\nempty=\n[Other]\nforeign=hidden";
        auto keyBuffer = prepare(keys);
        const char * config = GetSection(keyBuffer.data(), 1);
        auto symbol = [](char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                                         (c >= '0' && c <= '9') || c == '_'; };
        auto lookup = [&](const char * key) {
            return FindValue(keyBuffer.data(), std::strlen(keys) + 1, config, key,
                             [](const char * a, const char * b, std::size_t n) { return std::strncmp(a, b, n); }, symbol);
        };
        const char * value = lookup("key");
        require(value && std::strncmp(value, "first ;literal", 14) == 0,
                "first duplicate key or literal inline comment changed");
        require(lookup("foreign") == nullptr && lookup("absent") == nullptr,
                "missing key crossed section boundary");
        require(lookup("KEY") == nullptr, "comparison policy ignored");
        value = lookup("empty");
        require(value && CopyLine(value, output, sizeof(output)) == 1 && output[0] == ' ',
                "empty key value lost");
        require(lookup("this_key_is_longer_than_the_remaining_storage_buffer_size") == nullptr,
                "oversized key matched");
        checkSections("", {});
        checkSections("plain text\nno sections", {});
        checkSections("[First]\r\na=1\r\n[Second]\r\nb=2", {"First", "Second"});
        checkSections("  [First]\na=1\n  [First]\na=2\n", {"First", "First"});
        checkSections("[broken\ntext]\n[valid]\n", {"valid"});
        checkSections(";[comment]\n\t[tab]\n[real]\n", {"real"});
        checkSections("[Final]", {"Final"});
        auto callbacks = prepare("[One] suffix\r\nfirst = value\r\n[Two]\r\nlast=no-newline");
        std::vector<std::string> callbackSections;
        require(EnumerateSections(callbacks.data(),
                    [&](const char * name, const char * suffix) {
                        callbackSections.emplace_back(name);
                        if (callbackSections.size() == 1)
                            require(std::strncmp(suffix, " suffix", 7) == 0,
                                    "section suffix callback changed");
                        return true;
                    }) &&
                callbackSections == std::vector<std::string>({"One", "Two"}),
                "section callback order/completion changed");
        require(!EnumerateSections(callbacks.data(),
                    [](const char *, const char *) { return false; }),
                "section callback stop was ignored");
        auto noSections = prepare("plain text");
        require(EnumerateSections(noSections.data(),
                    [](const char *, const char *) { return false; }),
                "empty section enumeration did not complete");

        auto callbackKeys = prepare(
            "[Keys]\r\nfirst = value\r\n;comment=no\r\npunct-key!! = kept\r\nempty=\r\nlast=tail");
        const char * keySection = GetSection(callbackKeys.data(), 1);
        std::vector<std::string> names;
        std::vector<std::string> values;
        require(EnumerateKeys(keySection, symbol,
                    [&](const char * name, const char * valueText) {
                        names.emplace_back(name);
                        values.emplace_back(valueText);
                        return true;
                    }),
                "key callback did not complete");
        require(names == std::vector<std::string>({"first", "punct-key", "empty", "last"}) &&
                values == std::vector<std::string>({"value", "kept", "", "tail"}),
                "key callback trimming/order or final unterminated value changed");
        int callbackCount = 0;
        require(!EnumerateKeys(keySection, symbol,
                    [&](const char *, const char *) {
                        ++callbackCount;
                        return false;
                    }) && callbackCount == 1,
                "key callback stop was ignored");
        auto noKeys = prepare("[Empty]\r\nplain text");
        require(!EnumerateKeys(GetSection(noKeys.data(), 1), symbol,
                    [](const char *, const char *) { return true; }),
                "empty key enumeration unexpectedly succeeded");
        const std::string longEntry =
            "[Long]\n" + std::string(300, 'k') + "=" + std::string(300, 'v');
        auto longKeys = prepare(longEntry.c_str());
        require(EnumerateKeys(GetSection(longKeys.data(), 1), symbol,
                    [](const char * name, const char * valueText) {
                        return std::strlen(name) == 255 && std::strlen(valueText) == 255;
                    }),
                "key callback 255-byte cap changed");
        auto adjacent = prepare("[One]\r\nkey=1\r\n\r\n[Two]\r\n");
        const char * first = GetSection(adjacent.data(), 1);
        require(GetLine(GetLine(first, 1), 1) == nullptr,
                "blank CRLF immediately before a section must end navigation");
        auto buffer = prepare("[One]\r\n  key = value\r\n;comment\r\n\tTabbed\r\n\r\ntail\r\n[Two]\r\nlast=no-newline");
        const char * one = GetSection(buffer.data(), 1);
        require(GetLine(one, 0) == one, "zero line must preserve pointer");
        const char * line = GetLine(one, 1);
        require(line && std::strncmp(line, "key = value\r\n", 13) == 0, "space/CRLF handling changed");
        line = GetLine(line, 1);
        require(line && std::strncmp(line, ";comment", 8) == 0, "comments must remain visible to callers");
        line = GetLine(line, 1);
        require(line && *line == '\t', "tabs must not be stripped like spaces");
        line = GetLine(line, 1);
        require(line && *line == '\n', "blank CRLF line must expose LF after skipping CR");
        line = GetLine(line, 1);
        require(line && std::strncmp(line, "tail", 4) == 0, "line after blank was lost");
        require(GetLine(line, 1) == nullptr, "navigation crossed a section boundary");
        const char * two = GetSection(one, 1);
        line = GetLine(two, 1);
        require(line && std::strcmp(line, "last=no-newline") == 0, "final unterminated line lost");
        require(GetLine(line, 1) == nullptr, "end of buffer was not detected");
        std::cout << "Profile buffer section/line contract passed\n";
    }
    catch (const std::string & message)
    {
        std::cerr << message << '\n';
        return 1;
    }
    return 0;
}
