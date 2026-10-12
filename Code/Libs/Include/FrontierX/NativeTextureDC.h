#ifndef FRONTIERX_NATIVE_TEXTURE_DC_H
#define FRONTIERX_NATIVE_TEXTURE_DC_H

// CPU RGBA drawing at the original texture-DC boundary. No window or UI owner.
#include "FrontierX/LegacyWindowTypes.h"
#include "FrontierX/LegacyFileTypes.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <new>

namespace FrontierX::TextureDC
{
struct Object { bool pen; bool hollow; bool hatch; std::uint32_t color; int width; bool stock; };
using HGDIOBJ = Object *;
using HBRUSH = Object *;
using HPEN = Object *;
constexpr int BLACK_BRUSH = 4, NULL_BRUSH = 5, NULL_PEN = 8;
constexpr int PS_SOLID = 0, HS_DIAGCROSS = 5, TRANSPARENT = 1;
constexpr std::uint32_t SRCCOPY = 0x00cc0020;
inline Object blackBrush{false, false, false, 0, 0, true};
inline Object nullBrush{false, true, false, 0, 0, true};
inline Object nullPen{true, true, false, 0, 0, true};
struct Context
{
    unsigned char *pixels = nullptr;
    int width = 0, height = 0, pitch = 0;
    Object *brush = &blackBrush, *pen = &nullPen;
    int backgroundMode = TRANSPARENT;
    int cursorX = 0, cursorY = 0;
};
inline HGDIOBJ GetStockObject(int id)
{ return id == BLACK_BRUSH ? &blackBrush : id == NULL_BRUSH ? &nullBrush : id == NULL_PEN ? &nullPen : nullptr; }
inline HBRUSH CreateSolidBrush(std::uint32_t color)
{ return new (std::nothrow) Object{false, false, false, color, 0, false}; }
inline HBRUSH CreateHatchBrush(int style, std::uint32_t color)
{ return style == HS_DIAGCROSS ? new (std::nothrow) Object{false, false, true, color, 0, false} : nullptr; }
inline HPEN CreatePen(int style, int width, std::uint32_t color)
{ return style == PS_SOLID && width > 0 ? new (std::nothrow) Object{true, false, false, color, width, false} : nullptr; }
inline HGDIOBJ SelectObject(HDC dc, HGDIOBJ object)
{
    if (!dc || !object) return nullptr;
    auto &context = *static_cast<Context *>(dc);
    auto &slot = object->pen ? context.pen : context.brush;
    auto *previous = slot; slot = object; return previous;
}
inline bool DeleteObject(HGDIOBJ object)
{ if (!object || object->stock) return false; delete object; return true; }
inline int SetBkMode(HDC dc, int mode)
{
    if (!dc || mode != TRANSPARENT) return 0;
    auto &context = *static_cast<Context *>(dc);
    int previous = context.backgroundMode; context.backgroundMode = mode; return previous;
}
inline void Pixel(Context &context, int x, int y, const Object &object)
{
    if (object.hollow || x < 0 || y < 0 || x >= context.width || y >= context.height ||
        (object.hatch && ((x + y) % 8 != 0) && ((x - y) % 8 != 0))) return;
    unsigned char *pixel = context.pixels + static_cast<std::size_t>(y) * context.pitch + static_cast<std::size_t>(x) * 4;
    pixel[0] = static_cast<unsigned char>(object.color);
    pixel[1] = static_cast<unsigned char>(object.color >> 8);
    pixel[2] = static_cast<unsigned char>(object.color >> 16);
    pixel[3] = 255;
}
inline bool FillRect(HDC dc, const RECT *rect, HBRUSH brush)
{
    if (!dc || !rect || !brush) return false;
    auto &context = *static_cast<Context *>(dc);
    for (int y = std::max(0, rect->top); y < std::min(context.height, rect->bottom); ++y)
        for (int x = std::max(0, rect->left); x < std::min(context.width, rect->right); ++x)
            Pixel(context, x, y, *brush);
    return true;
}
inline bool Rectangle(HDC dc, int left, int top, int right, int bottom)
{
    if (!dc || left >= right || top >= bottom) return false;
    auto &context = *static_cast<Context *>(dc);
    RECT rect{left, top, right, bottom}; FillRect(dc, &rect, context.brush);
    const int width = context.pen->width;
    for (int y = std::max(0, top); y < std::min(context.height, bottom); ++y)
        for (int x = std::max(0, left); x < std::min(context.width, right); ++x)
            if (static_cast<std::int64_t>(x) - left < width || static_cast<std::int64_t>(right) - x <= width || static_cast<std::int64_t>(y) - top < width || static_cast<std::int64_t>(bottom) - y <= width)
                Pixel(context, x, y, *context.pen);
    return true;
}
inline bool Oval(HDC dc, int left, int top, int right, int bottom,
                 bool arc, double start = 0, double extent = 0)
{
    if (!dc || left >= right || top >= bottom) return false;
    auto &context = *static_cast<Context *>(dc);
    const double rx = (static_cast<double>(right) - left) / 2;
    const double ry = (static_cast<double>(bottom) - top) / 2;
    const double cx = left + rx, cy = top + ry;
    const double ix = std::max(0.0, rx - context.pen->width);
    const double iy = std::max(0.0, ry - context.pen->width);
    constexpr double turn = 6.283185307179586;
    for (int y = std::max(0, top); y < std::min(context.height, bottom); ++y)
        for (int x = std::max(0, left); x < std::min(context.width, right); ++x)
        {
            const double dx = x + 0.5 - cx, dy = y + 0.5 - cy;
            if (dx * dx / (rx * rx) + dy * dy / (ry * ry) > 1) continue;
            if (arc)
            {
                double angle = std::atan2(-dy / ry, dx / rx) - start;
                angle = std::fmod(angle + turn * 2, turn);
                if (angle > extent) continue;
            }
            else Pixel(context, x, y, *context.brush);
            if (ix == 0 || iy == 0 || dx * dx / (ix * ix) + dy * dy / (iy * iy) >= 1)
                Pixel(context, x, y, *context.pen);
        }
    return true;
}
inline bool Ellipse(HDC dc, int left, int top, int right, int bottom)
{ return Oval(dc, left, top, right, bottom, false); }
inline bool MoveToEx(HDC dc, int x, int y, void *previous)
{
    if (!dc || previous) return false;
    auto &context = *static_cast<Context *>(dc);
    context.cursorX = x; context.cursorY = y; return true;
}
inline bool LineTo(HDC dc, int x, int y)
{
    if (!dc) return false;
    auto &context = *static_cast<Context *>(dc);
    const double ax = context.cursorX, ay = context.cursorY;
    const double vx = static_cast<double>(x) - ax, vy = static_cast<double>(y) - ay;
    context.cursorX = x; context.cursorY = y;
    if (context.pen->hollow) return true;
    const double length2 = vx * vx + vy * vy;
    const double radius = context.pen->width * 0.5;
    const auto lower = [](double value, int limit) { return static_cast<int>(std::clamp(value, 0.0, static_cast<double>(limit))); };
    const int left = lower(std::min(ax, static_cast<double>(x)) - radius, context.width);
    const int top = lower(std::min(ay, static_cast<double>(y)) - radius, context.height);
    const int right = lower(std::max(ax, static_cast<double>(x)) + radius + 1, context.width);
    const int bottom = lower(std::max(ay, static_cast<double>(y)) + radius + 1, context.height);
    for (int py = top; py < bottom; ++py)
        for (int px = left; px < right; ++px)
        {
            const double projection = length2 == 0 ? 0 : std::clamp(((px - ax) * vx + (py - ay) * vy) / length2, 0.0, 1.0);
            const double dx = px - ax - projection * vx, dy = py - ay - projection * vy;
            if (dx * dx + dy * dy <= radius * radius) Pixel(context, px, py, *context.pen);
        }
    return true;
}
inline bool Arc(HDC dc, int left, int top, int right, int bottom, int x1, int y1, int x2, int y2)
{
    const double cx = (static_cast<double>(left) + right) / 2;
    const double cy = (static_cast<double>(top) + bottom) / 2;
    const double start = std::atan2(cy - y1, x1 - cx);
    double extent = std::atan2(cy - y2, x2 - cx) - start;
    if (extent <= 0) extent += 6.283185307179586;
    return Oval(dc, left, top, right, bottom, true, start, extent);
}
inline bool StretchBlt(HDC destination, int dx, int dy, int dw, int dh,
                       HDC source, int sx, int sy, int sw, int sh, std::uint32_t operation)
{
    if (!destination || !source || destination == source || operation != SRCCOPY ||
        dw <= 0 || dh <= 0 || sw <= 0 || sh <= 0) return false;
    auto &to = *static_cast<Context *>(destination);
    const auto &from = *static_cast<const Context *>(source);
    if (sx < 0 || sy < 0 || sw > from.width - sx || sh > from.height - sy) return false;
    for (int y = std::max(0, dy); y < std::min(to.height, dy + dh); ++y)
        for (int x = std::max(0, dx); x < std::min(to.width, dx + dw); ++x)
        {
            const auto *pixel = from.pixels + (sy + static_cast<std::int64_t>(y - dy) * sh / dh) * from.pitch +
                (sx + static_cast<std::int64_t>(x - dx) * sw / dw) * 4;
            auto *out = to.pixels + static_cast<std::size_t>(y) * to.pitch + static_cast<std::size_t>(x) * 4;
            std::copy_n(pixel, 4, out);
        }
    return true;
}
}
#endif
