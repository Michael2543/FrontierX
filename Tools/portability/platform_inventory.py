"""Inventory native-port touchpoints by category and owning production subsystem."""
import argparse
from collections import defaultdict
import json
from pathlib import Path
import re
import sys


SOURCE_SUFFIXES = {'.c', '.cc', '.cpp', '.cxx', '.h', '.hh', '.hpp', '.hxx', '.inl'}
EXCLUDED_PARTS = {'Tools', 'Viewer', 'Obsolete', 'Samples', 'Example', 'Examples'}
SCAN_ROOTS = ('Code/App/Src', 'Code/Libs/Src', 'Code/Libs/Include')

CATEGORIES = {
    'win32_headers': (
        'Direct includes of Windows platform headers.',
        r'#\s*include\s*[<"](?:windowsx?|mmsystem|shellapi|shlobj|commctrl|winsock2?|'
        r'process)\.h[>"]'),
    'win32_api': (
        'Win32 types and calls that require a platform boundary or Windows implementation.',
        r'\b(?:HWND|HINSTANCE|HKEY|WPARAM|LPARAM|LRESULT|WNDPROC|CRITICAL_SECTION|'
        r'CreateFileA?|ReadFile|WriteFile|CloseHandle|GetLastError|CreateThread|Sleep|'
        r'QueryPerformanceCounter|QueryPerformanceFrequency|PostMessageA?|SendMessageA?|'
        r'PeekMessageA?|GetMessageA?|DefWindowProcA?|RegisterClassExA?|CreateWindowExA?)\b'),
    'msvc_extensions': (
        'MSVC language, calling-convention, pragma, and intrinsic extensions.',
        r'\b(?:__declspec|__stdcall|__cdecl|__fastcall|__int8|__int16|__int32|__int64|_MSC_VER)\b|'
        r'#\s*pragma\s+(?:warning|comment|intrinsic)'),
    'inline_assembly': (
        'Inline assembly that cannot compile unchanged on x86_64 GCC or Clang.',
        r'\b(?:__asm|_asm)\b|\basm\s*\('),
    'legacy_integer_casts': (
        'Explicit casts to legacy-width integer aliases; each is a pointer-width audit candidate.',
        r'\(\s*(?:U32|S32|DWORD|LONG)\s*\)|'
        r'(?:static_cast|reinterpret_cast)\s*<\s*(?:U32|S32|DWORD|LONG)\s*>|'
        r'\b(?:U32|S32|DWORD|LONG)\s*\('),
    'pointer_integer_casts': (
        'High-confidence object/address casts to 32-bit legacy integer representations.',
        r'(?:\(\s*(?:U32|S32|DWORD|LONG)\s*\)|\b(?:U32|S32|DWORD|LONG)\s*\()\s*'
        r'(?:this\b|&\s*[A-Za-z_]\w*)'),
    'encoded_handle_casts': (
        'Opaque HANDLE values cast to legacy integers; classify index tokens separately from '
        'native OS handles.',
        r'(?:\(\s*(?:U32|S32|DWORD|LONG)\s*\)|'
        r'\b(?:U32|S32|DWORD|LONG)\s*\()\s*'
        r'(?:[A-Za-z_]\w*[Hh]andle\w*|handle\b|hFile\b|hFindFile\b|'
        r'p(?:Read|Write)->hFileHandle\b)'),
    'packing': (
        'Explicit structure packing or packed attributes affecting storage or module ABI.',
        r'#\s*pragma\s+pack\b|__attribute__\s*\(\(\s*packed\s*\)\)'),
    'varargs': (
        'C varargs whose width and format contracts require LP64 review.',
        r'\b(?:va_list|va_start|va_arg|va_end|vsnprintf|vsprintf|vfprintf)\b'),
    'native_long': (
        'Native long declarations/usages that differ between Win32 and LP64 Linux.',
        r'\b(?:unsigned\s+long|signed\s+long|long)\b'),
    'd3d9': (
        'D3D9-facing device, resource, state, or shader surface excluding D3DX names.',
        r'\b(?:IDirect3D(?!X)[A-Za-z0-9_]*|D3D(?!X)[A-Z][A-Za-z0-9_]*)\b|'
        r'#\s*include\s*[<"]d3d9\.h[>"]'),
    'd3dx9': (
        'D3DX9 effects, texture, shader, mesh, or math helpers requiring explicit replacement.',
        r'\bD3DX[A-Za-z0-9_]*\b|#\s*include\s*[<"]d3dx9[^>"]*\.h[>"]'),
    'directsound': (
        'DirectSound and WinMM audio surface requiring a portable audio backend.',
        r'\b(?:IDirectSound[A-Za-z0-9_]*|DirectSoundCreate[0-9A-Za-z_]*|DSB[A-Z][A-Z0-9_]*|'
        r'WAVEFORMATEX|waveOut[A-Za-z0-9_]*)\b|#\s*include\s*[<"](?:dsound|mmeapi)\.h[>"]'),
    'legacy_video': (
        'Bink or DirectShow multimedia surface requiring the planned portable video path.',
        r'\b(?:Bink[A-Za-z0-9_]*|IAMMultiMediaStream|IMediaStream|IDirectDrawMediaStream|'
        r'IDirectDrawStreamSample|AMMultiMediaStream)\b|binkw32|#\s*include\s*[<"]amstream\.h[>"]'),
    'dynamic_loading': (
        'Runtime module-loading calls requiring a portable loader and ABI policy.',
        r'\b(?:LoadLibraryA?|FreeLibrary|GetProcAddress|GetModuleHandleA?|SetDllDirectoryA?)\b'),
    'registry': (
        'Windows registry access requiring portable configuration/storage policy.',
        r'\b(?:HKEY_[A-Z_]+|RegOpenKeyExA?|RegCreateKeyExA?|RegQueryValueExA?|'
        r'RegSetValueExA?|RegCloseKey|RegDeleteKeyA?)\b'),
}

COMPILED_CATEGORIES = {name: re.compile(pattern) for name, (_, pattern) in CATEGORIES.items()}


def strip_comments(text):
    """Remove C/C++ comments while retaining strings and newlines for include/token scans."""
    output = []
    index = 0
    state = 'code'
    while index < len(text):
        char = text[index]
        next_char = text[index + 1] if index + 1 < len(text) else ''
        if state == 'code':
            if char == '/' and next_char == '/':
                output.extend('  ')
                index += 2
                state = 'line_comment'
                continue
            if char == '/' and next_char == '*':
                output.extend('  ')
                index += 2
                state = 'block_comment'
                continue
            output.append(char)
            if char == '"':
                state = 'string'
            elif char == "'":
                state = 'character'
            index += 1
            continue
        if state == 'line_comment':
            if char == '\n':
                output.append(char)
                state = 'code'
            else:
                output.append(' ')
            index += 1
            continue
        if state == 'block_comment':
            if char == '*' and next_char == '/':
                output.extend('  ')
                index += 2
                state = 'code'
            else:
                output.append('\n' if char == '\n' else ' ')
                index += 1
            continue
        output.append(char)
        if char == '\\' and index + 1 < len(text):
            output.append(text[index + 1])
            index += 2
        else:
            if (state == 'string' and char == '"') or (state == 'character' and char == "'"):
                state = 'code'
            index += 1
    return ''.join(output)


def owner_for(relative_path):
    parts = relative_path.parts
    if parts[:3] == ('Code', 'App', 'Src'):
        return 'App/Scripts' if len(parts) > 3 and parts[3] == 'Scripts' else 'App'
    if parts[:3] == ('Code', 'Libs', 'Src') and len(parts) > 3:
        return f'Libs/{parts[3]}'
    if parts[:3] == ('Code', 'Libs', 'Include'):
        return 'Libs/Include'
    return 'Other'


def source_files(root):
    root = root.resolve()
    files = []
    for scan_root in SCAN_ROOTS:
        base = root / scan_root
        if not base.is_dir():
            raise ValueError(f'Missing scan root: {scan_root}')
        for path in base.rglob('*'):
            relative = path.relative_to(root)
            if path.is_file() and path.suffix.lower() in SOURCE_SUFFIXES and not (
                    set(relative.parts) & EXCLUDED_PARTS):
                files.append(path)
    return sorted(set(files))


def inventory(root):
    root = root.resolve()
    files = source_files(root)
    category_files = {name: [] for name in CATEGORIES}
    for path in files:
        relative = path.relative_to(root)
        text = strip_comments(path.read_text(encoding='utf-8', errors='replace'))
        for name, pattern in COMPILED_CATEGORIES.items():
            count = sum(1 for _ in pattern.finditer(text))
            if count:
                category_files[name].append({
                    'path': relative.as_posix(),
                    'owner': owner_for(relative),
                    'occurrences': count,
                })

    categories = {}
    for name, (description, _) in CATEGORIES.items():
        entries = sorted(category_files[name], key=lambda item: item['path'])
        owner_counts = defaultdict(lambda: {'occurrences': 0, 'files': 0})
        for entry in entries:
            owner_counts[entry['owner']]['occurrences'] += entry['occurrences']
            owner_counts[entry['owner']]['files'] += 1
        owners = [{'owner': owner, **counts} for owner, counts in owner_counts.items()]
        owners.sort(key=lambda item: (-item['occurrences'], item['owner']))
        categories[name] = {
            'description': description,
            'occurrences': sum(item['occurrences'] for item in entries),
            'file_count': len(entries),
            'owners': owners,
            'files': entries,
        }
    return {
        'schema_version': 1,
        'scope': ('C/C++ sources and headers under Code/App/Src, Code/Libs/Src and '
                  'Code/Libs/Include, excluding Tools, Viewer, Obsolete, Samples and Examples. '
                  'Comments are ignored; results are lexical audit candidates, not compile '
                  'errors.'),
        'source_files_scanned': len(files),
        'categories': categories,
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[2])
    args = parser.parse_args()
    try:
        print(json.dumps(inventory(args.root), indent=2, sort_keys=True))
    except (OSError, ValueError) as error:
        print(f'Platform inventory failed: {error}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
