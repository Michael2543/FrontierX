#ifndef FRONTIERX_LEGACY_FILE_TYPES_H
#define FRONTIERX_LEGACY_FILE_TYPES_H

#if defined(_WIN32)
#error LegacyFileTypes.h is only for the non-Windows declaration path
#endif

#include <cstddef>
#include <cstdint>

using BOOL = std::int32_t;
using LONG = std::int32_t;
using WORD = std::uint16_t;
using LANGID = std::uint16_t;
using DWORD = std::uint32_t;
using DWORDLONG = std::uint64_t;
using COLORREF = std::uint32_t;
using HRESULT = std::int32_t;
using HANDLE = void *;
using PVOID = void *;
using HMENU = void *;
using HCURSOR = void *;
using LPVOID = void *;
using LPCVOID = const void *;
using LPDWORD = DWORD *;
using PDWORD = DWORD *;
using PLONG = LONG *;
using LPCTSTR = const char *;
using LPTSTR = char *;
using LPSTR = char *;

#ifndef __fastcall
#define __fastcall
#endif
#ifndef __stdcall
#define __stdcall
#endif
#ifndef __cdecl
#define __cdecl
#endif

struct OVERLAPPED;
using LPOVERLAPPED = OVERLAPPED *;
struct SECURITY_ATTRIBUTES;
using LPSECURITY_ATTRIBUTES = SECURITY_ATTRIBUTES *;
struct FILETIME
{
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
};
using LPFILETIME = FILETIME *;
struct WIN32_FIND_DATA
{
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    char cFileName[260];
    char cAlternateFileName[14];
};
using LPWIN32_FIND_DATA = WIN32_FIND_DATA *;
static_assert(sizeof(WIN32_FIND_DATA) == 320);
struct RGBQUAD
{
    std::uint8_t rgbBlue;
    std::uint8_t rgbGreen;
    std::uint8_t rgbRed;
    std::uint8_t rgbReserved;
};
static_assert(sizeof(RGBQUAD) == 4);
struct BITMAPINFOHEADER
{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    std::uint16_t biPlanes;
    std::uint16_t biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
};
static_assert(sizeof(BITMAPINFOHEADER) == 40);
static_assert(offsetof(BITMAPINFOHEADER, biClrImportant) + sizeof(DWORD) == 40);

#define RGB(red, green, blue) \
    (static_cast<COLORREF>(static_cast<std::uint8_t>(red)) | \
     (static_cast<COLORREF>(static_cast<std::uint8_t>(green)) << 8) | \
     (static_cast<COLORREF>(static_cast<std::uint8_t>(blue)) << 16))

#ifndef CONST
#define CONST const
#endif

#define INVALID_HANDLE_VALUE ((HANDLE)(std::intptr_t)-1)

inline constexpr DWORD GENERIC_READ = 0x80000000U;
inline constexpr DWORD GENERIC_WRITE = 0x40000000U;
inline constexpr HRESULT S_OK = 0;
inline constexpr HRESULT E_FAIL = -2147467259;
inline constexpr DWORD FILE_SHARE_READ = 0x00000001U;
inline constexpr DWORD ERROR_FILE_NOT_FOUND = 2U;
inline constexpr DWORD ERROR_NO_MORE_FILES = 18U;
inline constexpr DWORD OPEN_EXISTING = 3U;
inline constexpr DWORD CREATE_NEW = 1U;
inline constexpr DWORD CREATE_ALWAYS = 2U;
inline constexpr DWORD FILE_ATTRIBUTE_NORMAL = 0x00000080U;
inline constexpr DWORD FILE_ATTRIBUTE_DIRECTORY = 0x00000010U;
inline constexpr DWORD FILE_FLAG_SEQUENTIAL_SCAN = 0x08000000U;
inline constexpr DWORD PAGE_READONLY = 0x00000002U;
inline constexpr DWORD PAGE_READWRITE = 0x00000004U;
inline constexpr DWORD FILE_MAP_READ = 0x00000004U;
inline constexpr DWORD FILE_BEGIN = 0U;

#ifndef SUCCEEDED
#define SUCCEEDED(result) (static_cast<HRESULT>(result) >= 0)
#endif
#ifndef FAILED
#define FAILED(result) (static_cast<HRESULT>(result) < 0)
#endif
#ifndef MAKE_HRESULT
#define MAKE_HRESULT(severity, facility, code) \
    static_cast<HRESULT>((static_cast<DWORD>(severity) << 31) | \
                         (static_cast<DWORD>(facility) << 16) | \
                         static_cast<DWORD>(code))
#endif
inline constexpr DWORD SEVERITY_SUCCESS = 0;
inline constexpr DWORD MAX_PATH = 260;

#endif
