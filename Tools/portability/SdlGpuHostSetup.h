#pragma once

#include <SDL3/SDL.h>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <unistd.h>

// Explicit GPU-probe host preparation, before renderer initialization. The
// regular test path and all resize/readback assertions remain unchanged.
inline bool WaitForSdlGpuHostSetup()
{
    const char * root = std::getenv("FRONTIERX_GPU_HOST_SETUP_DIR");
    if (!root) return true;
    const std::filesystem::path directory(root);
    if (!directory.is_absolute() || !std::filesystem::is_directory(directory))
        return false;
    const auto name = std::to_string(getpid());
    const auto request = directory / (name + ".requested");
    const auto ready = directory / (name + ".ready");
    if (std::filesystem::exists(request) || std::filesystem::exists(ready))
        return false;
    { std::ofstream stream(request); if (!stream) return false; }
    const Uint64 start = SDL_GetTicks();
    while (SDL_GetTicks() - start < 10000)
    {
        SDL_PumpEvents();
        if (std::filesystem::exists(ready))
        {
            std::filesystem::remove(request);
            std::filesystem::remove(ready);
            return true;
        }
        SDL_Delay(10);
    }
    return false;
}
