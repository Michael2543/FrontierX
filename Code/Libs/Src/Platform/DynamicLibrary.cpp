#include "FrontierX/DynamicLibrary.h"

#include <dlfcn.h>

#include <utility>

namespace FrontierX
{
namespace
{
void AssignError(std::string & error, const char * detail) noexcept
{
    try
    {
        error = detail;
    }
    catch (...)
    {
        error.clear();
    }
}

void AssignLoaderError(std::string & error, const char * fallback) noexcept
{
    const char * detail = dlerror();
    AssignError(error, detail ? detail : fallback);
}
}

DynamicLibrary::~DynamicLibrary()
{
    if (handle_) dlclose(handle_);
}

DynamicLibrary::DynamicLibrary(DynamicLibrary && other) noexcept :
    handle_(std::exchange(other.handle_, nullptr))
{
}

DynamicLibrary & DynamicLibrary::operator=(DynamicLibrary && other) noexcept
{
    if (this == &other) return *this;
    if (handle_) dlclose(handle_);
    handle_ = std::exchange(other.handle_, nullptr);
    return *this;
}

bool DynamicLibrary::Open(const char * path, std::string & error) noexcept
{
    error.clear();
    if (!path || !*path)
    {
        AssignError(error, "library path is empty");
        return false;
    }
    if (handle_)
    {
        AssignError(error, "library is already open");
        return false;
    }

    dlerror();
    handle_ = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (!handle_)
    {
        AssignLoaderError(error, "unable to open library");
        return false;
    }
    return true;
}

void * DynamicLibrary::Symbol(const char * name, std::string & error) const noexcept
{
    error.clear();
    if (!handle_)
    {
        AssignError(error, "library is not open");
        return nullptr;
    }
    if (!name || !*name)
    {
        AssignError(error, "symbol name is empty");
        return nullptr;
    }

    dlerror();
    void * symbol = dlsym(handle_, name);
    const char * detail = dlerror();
    if (detail)
    {
        AssignError(error, detail);
        return nullptr;
    }
    return symbol;
}

bool DynamicLibrary::Close(std::string & error) noexcept
{
    error.clear();
    if (!handle_) return true;
    void * handle = std::exchange(handle_, nullptr);
    dlerror();
    if (dlclose(handle) != 0)
    {
        handle_ = handle;
        AssignLoaderError(error, "unable to close library");
        return false;
    }
    return true;
}

bool DynamicLibrary::IsOpen() const noexcept
{
    return handle_ != nullptr;
}

void DynamicLibrary::Abandon() noexcept
{
    handle_ = nullptr;
}
}
