#include "Include/NativeAppGlobals.h"
#include "Include/NativeAppSfx.h"
#include "SFX.h"
#include "Sfxid.h"
#include "GenData.h"
#include "Streamer.h"
#include "Filesys.h"
#include "TComponent.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <utility>
#include <vector>

struct NativeSfxEntry
{
    U32 id;
    SINGLE volume;
    const char *filename;
    SINGLE cutoff;
};

#include "NativeSfxTable.inc"

struct SoundEffect
{
    U32 id = 0;
    U32 references = 1;
    SINGLE volume = 1.0F;
    HSTREAM stream = nullptr;
    U32 flags = 0;
    bool started = false;
};

struct NativeSfx : ISFX
{
    BEGIN_DACOM_MAP_INBOUND(NativeSfx)
    DACOM_INTERFACE_ENTRY(ISFX)
    END_DACOM_MAP()

    ~NativeSfx()
    {
        for (SoundEffect *effect : handles)
        {
            if (effect->stream) streamer->CloseHandle(effect->stream);
            delete effect;
        }
        for (const auto &entry : preloaded) streamer->CloseHandle(entry.second);
        if (source) source->Release();
        if (streamer) streamer->Release();
    }

    bool Attach(IStreamer *activeStreamer, IFileSystem *effects)
    {
        if (!activeStreamer || !effects || streamer || source) return false;
        activeStreamer->AddRef();
        effects->AddRef();
        streamer = activeStreamer;
        source = effects;
        return true;
    }

    BOOL32 __stdcall Preload(SFX::ID id) override
    {
        const NativeSfxEntry *entry = FindEntry(id);
        if (!entry || !streamer || !source || !active) return FALSE;
        for (const auto &cached : preloaded)
            if (cached.first == entry->id) return TRUE;
        for (const SoundEffect *effect : handles)
            if (effect->id == entry->id && effect->stream) return TRUE;
        const HSTREAM stream = streamer->Open(entry->filename, source, 0);
        if (!stream) return FALSE;
        preloaded.emplace_back(entry->id, stream);
        return TRUE;
    }

    HSOUND __stdcall Open(SFX::ID id) override
    {
        const NativeSfxEntry *entry = FindEntry(id);
        if (!entry || !streamer || !source) return HSOUND();
        HSTREAM stream = nullptr;
        const auto cached = std::find_if(preloaded.begin(), preloaded.end(),
            [entry](const auto &value) { return value.first == entry->id; });
        if (cached != preloaded.end())
        {
            stream = cached->second;
            preloaded.erase(cached);
        }
        else if (active) stream = streamer->Open(entry->filename, source, 0);
        if (active && !stream) return HSOUND();
        auto *effect = new SoundEffect;
        effect->id = entry->id;
        effect->stream = stream;
        handles.push_back(effect);
        return HSOUND(effect);
    }

    BOOL32 __stdcall CloseHandle(HSOUND &sound) override
    {
        SoundEffect *effect = FindHandle(sound);
        if (!effect || !effect->references) return FALSE;
        sound.ptr = nullptr;
        if (--effect->references) return TRUE;
        if (effect->stream) streamer->CloseHandle(effect->stream);
        handles.erase(std::find(handles.begin(), handles.end(), effect));
        delete effect;
        return TRUE;
    }

    BOOL32 __stdcall AddRef(const HSOUND &sound) override
    {
        SoundEffect *effect = FindHandle(sound);
        if (!effect || effect->references == std::numeric_limits<U32>::max()) return FALSE;
        ++effect->references;
        return TRUE;
    }

    BOOL32 __stdcall Play(const HSOUND &sound, U32, const Vector *position,
                          U32 flags) override
    {
        SoundEffect *effect = FindHandle(sound);
        if (!effect) return FALSE;
        if (!active) return TRUE; // Source Enable(FALSE) bypasses spatial playback.
        if (position)
        {
            std::fputs("Native SFX spatial playback requires camera/fog routing\n", stderr);
            std::abort();
        }
        if (flags & ~(SFXPLAYF_LOOPING | SFXPLAYF_NORESTART |
                      SFXPLAYF_NOFOG | SFXPLAYF_NODROP)) return FALSE;
        const NativeSfxEntry *entry = FindEntry(static_cast<SFX::ID>(effect->id));
        if (!entry) return FALSE;
        if ((flags & SFXPLAYF_NORESTART) && effect->stream && effect->started &&
            streamer->GetStatus(effect->stream) == IStreamer::PLAYING) return TRUE;
        const bool looping = (flags & SFXPLAYF_LOOPING) != 0;
        if (!effect->stream || effect->started || looping)
        {
            if (effect->stream) streamer->CloseHandle(effect->stream);
            effect->stream = streamer->Open(entry->filename, source,
                looping ? STRMFL_LOOPING : 0);
            effect->started = false;
        }
        if (!effect->stream) return FALSE;
        const SINGLE gain = std::clamp(entry->volume * globalVolume * effect->volume, 0.0F, 1.0F);
        if (!streamer->SetVolume(effect->stream, ConvertFloatToDSound(gain)) ||
            !streamer->Restart(effect->stream)) return FALSE;
        effect->flags = flags & ~SFXPLAYF_NORESTART;
        effect->started = true;
        return TRUE;
    }

    BOOL32 __stdcall Stop(const HSOUND &sound) override
    {
        SoundEffect *effect = FindHandle(sound);
        if (!effect) return FALSE;
        effect->flags = 0;
        effect->started = false;
        if (effect->stream)
        {
            const BOOL32 closed = streamer->CloseHandle(effect->stream);
            effect->stream = nullptr;
            return closed;
        }
        return TRUE;
    }

    BOOL32 __stdcall SetVolume(const HSOUND &sound, SINGLE volume) override
    {
        SoundEffect *effect = FindHandle(sound);
        if (!effect || !std::isfinite(volume)) return FALSE;
        effect->volume = std::clamp(volume, 0.0F, 1.0F);
        if (!effect->stream || !effect->started) return TRUE;
        const NativeSfxEntry *entry = FindEntry(static_cast<SFX::ID>(effect->id));
        if (!entry) return FALSE;
        const SINGLE gain = std::clamp(entry->volume * globalVolume * effect->volume, 0.0F, 1.0F);
        return streamer->SetVolume(effect->stream, ConvertFloatToDSound(gain));
    }

    BOOL32 __stdcall GetVolume(const HSOUND &sound, SINGLE *volume) const override
    {
        const SoundEffect *effect = FindHandle(sound);
        if (!effect || !volume) return FALSE;
        *volume = effect->volume;
        return TRUE;
    }

    void __stdcall Enable(BOOL32 enabled) override
    {
        active = enabled != FALSE;
        if (active) return;
        for (const auto &entry : preloaded) streamer->CloseHandle(entry.second);
        preloaded.clear();
        for (SoundEffect *effect : handles)
        {
            if (effect->stream) streamer->CloseHandle(effect->stream);
            effect->stream = nullptr;
            effect->started = false;
            effect->flags = 0;
        }
    }

    void __stdcall SetSFXVolume(SINGLE volume) override
    {
        if (!std::isfinite(volume))
        {
            std::fputs("Native SFX global volume is invalid\n", stderr);
            std::abort();
        }
        globalVolume = std::clamp(volume, 0.0F, 1.0F);
        for (SoundEffect *effect : handles)
        {
            if (!effect->stream || !effect->started) continue;
            const NativeSfxEntry *entry = FindEntry(static_cast<SFX::ID>(effect->id));
            if (!entry || !streamer->SetVolume(effect->stream,
                ConvertFloatToDSound(std::clamp(entry->volume * globalVolume * effect->volume,
                                                0.0F, 1.0F))))
            {
                std::fputs("Native SFX active volume update failed\n", stderr);
                std::abort();
            }
        }
    }

    const GT_GLOBAL_SOUNDS & __stdcall GetGlobalSounds() override
    {
        const void *data = GENDATA ? GENDATA->GetArchetypeData("GlobalSounds") : nullptr;
        if (!data)
        {
            std::fputs("Native SFX GlobalSounds archetype unavailable\n", stderr);
            std::abort();
        }
        return *static_cast<const GT_GLOBAL_SOUNDS *>(data);
    }

    U32 DEBUG_GetNumHandles() override { return static_cast<U32>(handles.size()); }

private:
    static const NativeSfxEntry *FindEntry(SFX::ID id)
    {
        const U32 number = static_cast<U32>(id);
        if (!number || number >= nativeSfxLast) return nullptr;
        for (const NativeSfxEntry &entry : nativeSfxEntries)
            if (entry.id == number) return &entry;
        return nullptr;
    }

    SoundEffect *FindHandle(const HSOUND &sound) const
    {
        const auto found = std::find(handles.begin(), handles.end(), sound.ptr);
        return found != handles.end() ? *found : nullptr;
    }

    IStreamer *streamer = nullptr;
    IFileSystem *source = nullptr;
    std::vector<SoundEffect *> handles;
    std::vector<std::pair<U32, HSTREAM>> preloaded;
    SINGLE globalVolume = 1.0F;
    bool active = true;
};

namespace FrontierX
{
ISFX *CreateNativeSfxManager()
{
    if (!STREAMER || !SFXDIR) return nullptr;
    auto *manager = new DAComponent<NativeSfx>;
    if (!manager->Attach(STREAMER, SFXDIR))
    {
        static_cast<ISFX *>(manager)->Release();
        return nullptr;
    }
    return manager;
}
}
