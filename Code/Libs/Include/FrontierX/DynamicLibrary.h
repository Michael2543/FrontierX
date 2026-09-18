#ifndef FRONTIERX_DYNAMIC_LIBRARY_H
#define FRONTIERX_DYNAMIC_LIBRARY_H

#include <string>

namespace FrontierX
{
class DynamicLibrary
{
public:
    DynamicLibrary() = default;
    ~DynamicLibrary();
    DynamicLibrary(const DynamicLibrary &) = delete;
    DynamicLibrary & operator=(const DynamicLibrary &) = delete;
    DynamicLibrary(DynamicLibrary && other) noexcept;
    DynamicLibrary & operator=(DynamicLibrary && other) noexcept;

    bool Open(const char * path, std::string & error) noexcept;
    void * Symbol(const char * name, std::string & error) const noexcept;
    bool Close(std::string & error) noexcept;
    bool IsOpen() const noexcept;
    void Abandon() noexcept;

private:
    void * handle_ = nullptr;
};
}

#endif
