#ifndef FRONTIERX_LEGACY_FILE_TYPES_H
#define FRONTIERX_LEGACY_FILE_TYPES_H

#if defined(_WIN32)
#error LegacyFileTypes.h is only for the non-Windows declaration path
#endif

#include <cstdint>

using BOOL = std::int32_t;
using LONG = std::int32_t;
using DWORD = std::uint32_t;
using HRESULT = std::int32_t;
using HANDLE = void *;
using LPVOID = void *;
using LPCVOID = const void *;
using LPDWORD = DWORD *;
using PDWORD = DWORD *;
using PLONG = LONG *;
using LPCTSTR = const char *;
using LPTSTR = char *;
using LPSTR = char *;

struct OVERLAPPED;
using LPOVERLAPPED = OVERLAPPED *;
struct SECURITY_ATTRIBUTES;
using LPSECURITY_ATTRIBUTES = SECURITY_ATTRIBUTES *;
struct FILETIME;
using LPFILETIME = FILETIME *;
struct WIN32_FIND_DATA;
using LPWIN32_FIND_DATA = WIN32_FIND_DATA *;

#ifndef CONST
#define CONST const
#endif

#define INVALID_HANDLE_VALUE ((HANDLE)(std::intptr_t)-1)

inline constexpr DWORD GENERIC_READ = 0x80000000U;
inline constexpr DWORD GENERIC_WRITE = 0x40000000U;
inline constexpr HRESULT S_OK = 0;
inline constexpr HRESULT E_FAIL = -2147467259;
inline constexpr DWORD FILE_SHARE_READ = 0x00000001U;
inline constexpr DWORD OPEN_EXISTING = 3U;
inline constexpr DWORD CREATE_ALWAYS = 2U;
inline constexpr DWORD FILE_ATTRIBUTE_NORMAL = 0x00000080U;
inline constexpr DWORD FILE_FLAG_SEQUENTIAL_SCAN = 0x08000000U;
inline constexpr DWORD PAGE_READONLY = 0x00000002U;
inline constexpr DWORD FILE_MAP_READ = 0x00000004U;
inline constexpr DWORD FILE_BEGIN = 0U;

#endif
