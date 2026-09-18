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
    bool Configure(int width, int height, bool fullscreen, bool resizable,
                   bool always_on_top, bool show, bool center, std::string & error);
    bool PollEvent(WindowEvent & event);
    bool WaitEvent(WindowEvent & event, std::string & error);
    bool WindowArea(int & x, int & y, int & width, int & height) const;
    bool ClientArea(int & x, int & y, int & width, int & height) const;
    void SetCursorPosition(int x, int y);
    void CursorPosition(int & x, int & y) const;
    std::uint32_t WindowId() const;
    std::string VideoDriverName() const;
    bool IsOpen() const;
    void Close();

private:
    void * window_ = nullptr;
    bool video_initialized_ = false;
};

std::string CurrentSdlVideoDriverName();
}

#endif
