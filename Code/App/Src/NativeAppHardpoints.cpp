#include "Include/NativeAppGlobals.h"

#if !defined(_WIN32)
#include <ICamera.h>
#include <renderer.h>
#include <ENGINE.H>
#include <IHardPoint.h>
#include "HPEnum.h"
#include "ArchHolder.h"
#include "CQImage.h"

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <string>

namespace
{
void RequireHardpointServices()
{
    if (ENGINE && HARDPOINT) return;
    ICQImage::Bomb("native App hardpoint services unavailable");
    std::abort();
}

bool MatchPattern(const char *name, const char *pattern)
{
    if (*pattern == '*')
    {
        ++pattern;
        do { if (MatchPattern(name, pattern)) return true; }
        while (*name++);
        return false;
    }
    if (!*pattern) return !*name;
    if (!*name) return false;
    if (*pattern != '?' &&
        std::toupper(static_cast<unsigned char>(*pattern)) !=
        std::toupper(static_cast<unsigned char>(*name))) return false;
    return MatchPattern(name + 1, pattern + 1);
}

S32 FindChildPath(const std::string &path, S32 parent)
{
    if (parent == INVALID_INSTANCE_INDEX) return parent;
    size_t start = !path.empty() && path[0] == '\\' ? 1U : 0U;
    while (start < path.size())
    {
        const size_t end = path.find('\\', start);
        const std::string part = path.substr(start, end - start);
        if (part.empty()) return INVALID_INSTANCE_INDEX;
        S32 child = INVALID_INSTANCE_INDEX;
        bool found = false;
        while ((child = ENGINE->get_instance_child_next(parent,
                    EN_DONT_RECURSE, child)) != INVALID_INSTANCE_INDEX)
        {
            const char *name = ENGINE->get_instance_part_name(child);
            if (name && part == name) { parent = child; found = true; break; }
        }
        if (!found) return INVALID_INSTANCE_INDEX;
        if (end == std::string::npos) return parent;
        start = end + 1;
    }
    return INVALID_INSTANCE_INDEX;
}

struct EnumState
{
    const char *pattern;
    S32 instance;
    S32 archetype;
    IHPEnumerator *callback;
    bool done;
};

void HardpointCallback(const char *name, void *context)
{
    auto &state = *static_cast<EnumState *>(context);
    if (state.done || !name || !MatchPattern(name, state.pattern)) return;
    HPENUMINFO info{};
    if (!HARDPOINT->retrieve_hardpoint_info(state.archetype, name,
                                             info.hardpointinfo)) return;
    info.index = state.instance;
    info.name = name;
    state.done = !state.callback->EnumerateHardpoint(info);
}

bool EnumerateTree(S32 parent, const char *pattern, IHPEnumerator *callback)
{
    HARCH archetype = parent;
    EnumState state{pattern, parent, archetype, callback, false};
    HARDPOINT->enumerate_hardpoints(HardpointCallback, archetype, &state);
    if (state.done) return false;
    S32 child = INVALID_INSTANCE_INDEX;
    while ((child = ENGINE->get_instance_child_next(parent,
                EN_DONT_RECURSE, child)) != INVALID_INSTANCE_INDEX)
        if (!EnumerateTree(child, pattern, callback)) return false;
    return true;
}
}

S32 __stdcall FindChild(const char *pathname, S32 parent)
{
    RequireHardpointServices();
    if (!pathname) return INVALID_INSTANCE_INDEX;
    return FindChildPath(pathname, parent);
}

void __stdcall EnumerateHardpoints(S32 parent, const char *pathName,
                                   IHPEnumerator *callback)
{
    RequireHardpointServices();
    if (!pathName || !callback || parent == INVALID_INSTANCE_INDEX)
    {
        ICQImage::Bomb("native App hardpoint enumeration arguments invalid");
        std::abort();
    }
    const std::string path(pathName);
    const size_t separator = path.rfind('\\');
    if (separator != std::string::npos)
    {
        if (separator > 0) parent = FindChildPath(path.substr(0, separator), parent);
        pathName += separator + 1;
    }
    if (parent == INVALID_INSTANCE_INDEX) return;
    EnumerateTree(parent, pathName, callback);
}

BOOL32 __stdcall FindHardpoint(const char *pathName, S32 &index,
                               HardpointInfo &info, S32 parent)
{
    RequireHardpointServices();
    index = INVALID_INSTANCE_INDEX;
    if (!pathName || parent == INVALID_INSTANCE_INDEX)
    {
        ICQImage::Bomb("native App hardpoint lookup arguments invalid");
        std::abort();
    }
    const std::string path(pathName);
    const size_t separator = path.rfind('\\');
    if (separator == std::string::npos) return FALSE;
    index = separator ? FindChildPath(path.substr(0, separator), parent) : parent;
    if (index == INVALID_INSTANCE_INDEX) return FALSE;
    HARCH archetype = index;
    if (HARDPOINT->retrieve_hardpoint_info(archetype, path.c_str() + separator + 1,
                                            info)) return TRUE;
    index = INVALID_INSTANCE_INDEX;
    return FALSE;
}
#endif
