#ifndef FRONTIERX_SDL_WINDOW_H
#define FRONTIERX_SDL_WINDOW_H

#include "FrontierX/WindowEvent.h"

#include <cstdint>
#include <string>

namespace FrontierX
{
class SdlWindow
{
public:
    SdlWindow() = default;
    ~SdlWindow();
    SdlWindow(const SdlWindow &) = delete;
    SdlWindow & operator=(const SdlWindow &) = delete;

    bool Open(const char * title, int width, int height, bool hidden, std::string & error);
    bool StartTextInput(std::string & error);
    bool PollEvent(WindowEvent & event);
    std::uint32_t WindowId() const;
    std::string VideoDriverName() const;
    void Close();

private:
    void * window_ = nullptr;
    bool video_initialized_ = false;
};
}

#endif
