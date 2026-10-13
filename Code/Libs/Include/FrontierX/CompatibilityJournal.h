#ifndef FRONTIERX_COMPATIBILITY_JOURNAL_H
#define FRONTIERX_COMPATIBILITY_JOURNAL_H
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace FrontierX
{
// Private, opt-in owner journals. No runtime pointers or resource contents.
// Instantiate once per owner/TU to avoid cross-DLL CRT/file ownership.
class CompatibilityJournal
{
    FILE *file = nullptr;
public:
    explicit CompatibilityJournal(const char *owner)
    {
        const char *base = std::getenv("FRONTIERX_M3_JOURNAL");
        if (!base || !*base) return;
        char path[1024];
        const int count = std::snprintf(path, sizeof(path), "%s.%s.jsonl", base, owner);
        if (count < 0 || count >= int(sizeof(path)) || !(file = std::fopen(path, "wb")))
        { std::fputs("M3 private journal open failed\n", stderr); std::abort(); }
    }
    ~CompatibilityJournal() { if (file) std::fclose(file); }
    CompatibilityJournal(const CompatibilityJournal &) = delete;
    CompatibilityJournal &operator=(const CompatibilityJournal &) = delete;
    void Write(const char *stage, unsigned int id, unsigned int value,
               const char *resource = "")
    {
        if (!file) return;
        if (!resource) resource = "";
        // Only a bounded resource basename is permitted; never print a path.
        char name[65] = {};
        size_t length = 0;
        for (; resource[length] && length < sizeof(name) - 1; ++length)
        {
            const char c = resource[length];
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                  (c >= '0' && c <= '9') || c == '_' || c == '.' || c == '!'))
            { length = 0; break; }
            name[length] = c;
        }
        name[length] = 0;
        std::fprintf(file, "{\"stage\":\"%s\",\"id\":%u,\"value\":%u,\"resource\":\"%s\"}\n",
            stage, id, value, name);
        if (std::fflush(file) || std::ferror(file))
        { std::fputs("M3 private journal write failed\n", stderr); std::abort(); }
    }
};
}
#endif
