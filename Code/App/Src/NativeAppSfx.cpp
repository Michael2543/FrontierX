#include "Include/NativeAppGlobals.h"
#include "Include/NativeAppSfx.h"
#include "SFX.h"
#include "Sfxid.h"
#include "GenData.h"
#include "Streamer.h"
#include "Filesys.h"
#include "TComponent.h"
#include "Camera.h"
#include "Sector.h"
#include "FogOfWar.h"
#include "FrontierX/NativeStreamer.h"
#include <EventSys.h>
#include <IConnection.h>
#include <TSmartPointer.h>

#include <algorithm>
#include <array>
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
    Vector position{0.0F, 0.0F, 0.0F};
    U32 systemID = 0;
    SINGLE spatialGain = 1.0F;
    int instance = -1;
};

constexpr U32 PositionValid = 0x10U;
constexpr U32 Offscreen = 0x20U;
constexpr U32 Distant = 0x40U;

struct NativeSfx : ISFX, IEventCallback
{
    struct Instance
    {
        U32 id = 0;
        HSTREAM stream = nullptr;
        SoundEffect *owner = nullptr;
        unsigned previous = 0, next = 0;
    };
    BEGIN_DACOM_MAP_INBOUND(NativeSfx)
    DACOM_INTERFACE_ENTRY(ISFX)
    DACOM_INTERFACE_ENTRY(IEventCallback)
    END_DACOM_MAP()

    NativeSfx()
    {
        for (unsigned i = 0; i < instances.size(); ++i)
        {
            instances[i].previous = (i + instances.size() - 1) % instances.size();
            instances[i].next = (i + 1) % instances.size();
        }
    }

    ~NativeSfx()
    {
        if (EVENTSYS && eventHandle)
        {
            COMPTR<IDAConnectionPoint> connection;
            if (EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
                connection->Unadvise(eventHandle);
        }
        for (SoundEffect *effect : handles)
            delete effect;
        for (auto &instance : instances)
            if (instance.stream && !streamer->CloseHandle(instance.stream))
                Failure("instance stream close");
        if (spatialAudio && source) spatialAudio->ForgetSamples(source);
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
        COMPTR<IDAConnectionPoint> connection;
        if (streamer->QueryInterface("INativeSpatialAudio", spatialAudio) != GR_OK ||
            !EVENTSYS || EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) != GR_OK ||
            connection->Advise(static_cast<IEventCallback *>(this), &eventHandle) != GR_OK)
            return false;
        return true;
    }

    GENRESULT COMAPI Notify(U32 message, void *) override
    {
        if (message == CQE_CAMERA_MOVED) cameraMoved = true;
        if (message != CQE_UPDATE || !cameraMoved || !active) return GR_OK;
        cameraMoved = false;
        RefreshInstances();
        if (CAMERA && SECTOR)
        {
            currentSystemID = SECTOR->GetCurrentSystem();
            Vector topLeft, topRight, bottomRight, bottomLeft;
            if (!CAMERA->PaneToPoints(topLeft, topRight, bottomRight, bottomLeft))
                Failure("camera pane projection");
            bottom = bottomLeft.y;
            top = bottom + (topLeft.y - bottomLeft.y);
            left = (topLeft.x + bottomLeft.x) / 2;
            right = (topRight.x + bottomRight.x) / 2;
        }
        for (auto it = handles.rbegin(); it != handles.rend(); ++it)
        {
            SoundEffect *effect = *it;
            if (!effect->started && !(effect->flags & SFXPLAYF_LOOPING)) continue;
            HSOUND sound(effect);
            const Vector *position = (effect->flags & PositionValid) ? &effect->position : nullptr;
            const BOOL32 played = Play(sound, effect->systemID, position,
                effect->flags | SFXPLAYF_NORESTART);
            sound.ptr = nullptr;
            if (!played) Failure("active spatial sound update");
        }
        gameActive = CQFLAGS.bGameActive != 0;
        return GR_OK;
    }

    BOOL32 __stdcall Preload(SFX::ID id) override
    {
        const NativeSfxEntry *entry = FindEntry(id);
        if (!entry || !streamer || !source || !active) return FALSE;
        return spatialAudio->PreloadSample(entry->filename, source);
    }

    HSOUND __stdcall Open(SFX::ID id) override
    {
        const NativeSfxEntry *entry = FindEntry(id);
        if (!entry || !streamer || !source) return HSOUND();
        if (active && !spatialAudio->PreloadSample(entry->filename, source)) return HSOUND();
        auto *effect = new SoundEffect;
        effect->id = entry->id;
        handles.push_back(effect);
        return HSOUND(effect);
    }

    BOOL32 __stdcall CloseHandle(HSOUND &sound) override
    {
        SoundEffect *effect = FindHandle(sound);
        if (!effect || !effect->references) return FALSE;
        sound.ptr = nullptr;
        if (--effect->references) return TRUE;
        if (effect->instance >= 0)
        {
            auto &instance = instances[effect->instance];
            if ((effect->flags & SFXPLAYF_LOOPING) && instance.stream)
            {
                if (!streamer->CloseHandle(instance.stream)) return FALSE;
                instance.stream = nullptr;
            }
            instance.owner = nullptr;
            effect->instance = -1;
            effect->stream = nullptr;
        }
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

    BOOL32 __stdcall Play(const HSOUND &sound, U32 systemID, const Vector *position,
                          U32 flags) override
    {
        SoundEffect *effect = FindHandle(sound);
        if (!effect) return FALSE;
        if (!active) return TRUE; // Source Enable(FALSE) bypasses spatial playback.
        if (flags & ~(SFXPLAYF_LOOPING | SFXPLAYF_NORESTART |
                      SFXPLAYF_NOFOG | SFXPLAYF_NODROP | PositionValid | Offscreen | Distant)) return FALSE;
        flags &= ~(Offscreen | Distant);
        bool inSystem = true;
        S32 pan = 0;
        effect->spatialGain = 1.0F;
        if (position)
        {
            flags |= PositionValid;
            effect->position = *position;
            effect->systemID = systemID;
            if (systemID != currentSystemID || !gameActive) inSystem = false;
            else
            {
                if (!CAMERA || !FOGOFWAR) Failure("spatial camera/fog owner");
                if (((flags & SFXPLAYF_NOFOG) &&
                     !FOGOFWAR->CheckHardFogVisibilityForThisPlayer(systemID, *position)) ||
                    (!(flags & SFXPLAYF_NOFOG) && !FOGOFWAR->CheckVisiblePosition(*position)))
                    inSystem = false;
                else
                {
                    if (right == left || top == bottom) Failure("spatial camera extent");
                    const Vector rotated = CAMERA->GetInverseWorldTransform()->rotate_translate(*position);
                    const SINGLE normalX = 2 * (rotated.x - (left + right) / 2) / (right - left);
                    const SINGLE normalY = 2 * (rotated.y - (top + bottom) / 2) / (top - bottom);
                    const SINGLE x = std::fabs(normalX);
                    const SINGLE y = std::fabs(normalY > 0 ? normalY * 1.5 : normalY * 0.75);
                    if (x > 1.0F || y > 1.0F) flags |= Offscreen;
                    SINGLE distance = std::max(x, y) - 1.0;
                    if (distance > 0)
                    {
                        distance = (2.0F - distance) / 2.0F;
                        if (distance < 0.2F) { flags |= Distant; distance = 0.2F; }
                        effect->spatialGain = distance;
                    }
                    SINGLE balance = std::fabs(normalX) - 0.70;
                    balance = balance <= 0 ? 1.0F : std::max(1.0F - balance, 0.0F);
                    pan = ConvertFloatToDSound(balance);
                    if (normalX > 0) pan = -pan;
                }
            }
        }
        if (!inSystem && (flags & SFXPLAYF_NODROP))
        {
            flags |= Offscreen | Distant;
            effect->spatialGain = 0.0F;
            inSystem = true;
        }
        effect->flags = flags & ~SFXPLAYF_NORESTART;
        if (!inSystem)
        {
            ReleaseInstance(*effect, true);
            return TRUE; // Original spatial visibility drop, not playback evidence.
        }
        const NativeSfxEntry *entry = FindEntry(static_cast<SFX::ID>(effect->id));
        if (!entry) return FALSE;
        if ((flags & SFXPLAYF_NORESTART) && effect->stream && effect->started &&
            streamer->GetStatus(effect->stream) == IStreamer::PLAYING)
            return ApplyGain(*effect, *entry, pan);
        if (effect->instance < 0)
        {
            RefreshInstances();
            const int chosen = FindInstance(effect->id, flags);
            if (chosen < 0) return TRUE; // Original priority/duplicate-limit drop.
            auto &instance = instances[chosen];
            if (instance.owner) ReleaseInstance(*instance.owner, false);
            if (instance.stream && !streamer->CloseHandle(instance.stream)) return FALSE;
            instance.stream = nullptr;
            instance.owner = effect;
            instance.id = effect->id;
            effect->instance = chosen;
            MoveInstanceToFront(static_cast<unsigned>(chosen));
        }
        const bool looping = (flags & SFXPLAYF_LOOPING) != 0;
        if (!effect->stream || !(flags & SFXPLAYF_NORESTART))
        {
            auto &instance = instances[effect->instance];
            if (instance.stream && !streamer->CloseHandle(instance.stream)) return FALSE;
            effect->stream = streamer->Open(entry->filename, source,
                looping ? STRMFL_LOOPING : 0);
            instance.stream = effect->stream;
            effect->started = false;
        }
        if (!effect->stream)
        {
            instances[effect->instance].id = 0;
            ReleaseInstance(*effect, false);
            return FALSE;
        }
        if (!ApplyGain(*effect, *entry, pan) ||
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
            return streamer->Stop(effect->stream);
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
        const SINGLE gain = std::clamp(entry->volume * globalVolume * effect->volume * effect->spatialGain, 0.0F, 1.0F);
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
        if (active) { cameraMoved = true; return; }
        for (SoundEffect *effect : handles)
        {
            ReleaseInstance(*effect, true);
        }
        for (auto &instance : instances)
        {
            if (instance.stream && !streamer->CloseHandle(instance.stream))
                Failure("disabled unowned instance close");
            instance.stream = nullptr;
        }
    }

    void __stdcall SetSFXVolume(SINGLE volume) override
    {
        if (!std::isfinite(volume))
        {
            std::fputs("Native SFX global volume is invalid\n", stderr);
            std::abort();
        }
        const SINGLE checkedVolume = std::clamp(volume, 0.0F, 1.0F);
        if (globalVolume == checkedVolume) return;
        globalVolume = checkedVolume;
        cameraMoved = true;
        for (SoundEffect *effect : handles)
        {
            if (!effect->stream || !effect->started) continue;
            const NativeSfxEntry *entry = FindEntry(static_cast<SFX::ID>(effect->id));
            if (!entry || !streamer->SetVolume(effect->stream,
                ConvertFloatToDSound(std::clamp(entry->volume * globalVolume * effect->volume * effect->spatialGain,
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

    bool CheckPool()
    {
        if (!active) return false;
        const U32 before = DEBUG_GetNumHandles();
        {
            std::vector<HSOUND> logical;
            for (unsigned i = 0; i < 70; ++i)
            {
                HSOUND sound = Open(SFX::BUTTONPRESS1);
                if (!sound) return false;
                logical.push_back(sound);
            }
            if (DEBUG_GetNumHandles() != before + 70) return false;
        }
        if (DEBUG_GetNumHandles() != before) return false;
        HSOUND first = Open(SFX::BUTTONPRESS1), second = Open(SFX::BUTTONPRESS1);
        if (!first || !second) return false;
        constexpr unsigned seed = 0x1b10U;
        std::srand(seed);
        (void)std::rand(); (void)std::rand();
        const int expectedNext = std::rand();
        std::srand(seed);
        if (!Play(first, 0, nullptr, SFXPLAYF_LOOPING) ||
            !Play(second, 0, nullptr, SFXPLAYF_LOOPING)) return false;
        const int actualNext = std::rand();
        auto *firstEffect = FindHandle(first), *secondEffect = FindHandle(second);
        if (!firstEffect || !secondEffect || firstEffect->instance < 0 ||
            secondEffect->instance != -1 || actualNext != expectedNext) return false;
        const int owned = firstEffect->instance;
        // A live NORESTART owner must not allocate or consume another random slot.
        std::srand(seed);
        const int expectedUnchanged = std::rand();
        std::srand(seed);
        if (!Play(first, 0, nullptr, SFXPLAYF_LOOPING | SFXPLAYF_NORESTART) ||
            firstEffect->instance != owned || std::rand() != expectedUnchanged ||
            !Stop(first)) return false;
        return CloseHandle(first) && CloseHandle(second) && DEBUG_GetNumHandles() == before;
    }

private:
    void ReleaseInstance(SoundEffect &effect, bool stop)
    {
        if (effect.instance >= 0)
        {
            auto &instance = instances[effect.instance];
            if (stop && instance.stream)
            {
                if (!streamer->CloseHandle(instance.stream)) Failure("released instance stream close");
                instance.stream = nullptr;
            }
            instance.owner = nullptr;
        }
        effect.instance = -1;
        effect.stream = nullptr;
        effect.started = false;
    }

    void RefreshInstances()
    {
        for (auto &instance : instances)
        {
            if (!instance.owner || !instance.stream ||
                (instance.owner->flags & SFXPLAYF_LOOPING)) continue;
            const auto status = streamer->GetStatus(instance.stream);
            if (status == IStreamer::INVALID) Failure("sound instance invalid");
            SINGLE fraction = 0;
            const auto *entry = FindEntry(static_cast<SFX::ID>(instance.id));
            if (!entry || !spatialAudio->GetPlaybackFraction(instance.stream, &fraction))
                Failure("sound instance playback position");
            if ((status != IStreamer::PLAYING && status != IStreamer::EOFREACHED) ||
                fraction > entry->cutoff)
                ReleaseInstance(*instance.owner, false);
        }
    }

    int FindInstance(U32 id, U32 flags)
    {
        int matchingUnowned = -1, anyUnowned = -1, matchingOwned = -1, anyOwned = -1;
        unsigned usage = 0;
        for (const auto &instance : instances) if (instance.id == id) ++usage;
        const unsigned duplicateLimit = (flags & SFXPLAYF_LOOPING) ? 1U : 2U;
        // Preserve the source allocation path's single random starting slot.
        unsigned index = static_cast<unsigned>(std::rand()) % instances.size();
        for (unsigned count = 0; count < instances.size(); ++count)
        {
            const auto &instance = instances[index];
            if (!instance.owner)
            {
                if (instance.id == id) matchingUnowned = index;
                else if (usage < duplicateLimit) anyUnowned = index;
            }
            else if (!(flags & Offscreen))
            {
                if ((flags & SFXPLAYF_LOOPING) &&
                    (instance.owner->flags & (SFXPLAYF_LOOPING | Offscreen)) == (SFXPLAYF_LOOPING | Offscreen))
                {
                    if (instance.id == id) matchingOwned = index;
                }
                else if ((instance.owner->flags & (SFXPLAYF_LOOPING | Offscreen)) == Offscreen)
                {
                    if (instance.id == id) matchingOwned = index;
                    else if (usage < duplicateLimit) anyOwned = index;
                }
            }
            else if (!(flags & Distant))
            {
                if ((flags & SFXPLAYF_LOOPING) &&
                    (instance.owner->flags & (SFXPLAYF_LOOPING | Distant)) == (SFXPLAYF_LOOPING | Distant))
                {
                    if (instance.id == id) matchingOwned = index;
                }
                else if ((instance.owner->flags & (SFXPLAYF_LOOPING | Distant)) == Distant)
                {
                    if (instance.id == id) matchingOwned = index;
                    else if (usage < duplicateLimit) anyOwned = index;
                }
            }
            index = instance.next;
        }
        return matchingUnowned >= 0 ? matchingUnowned : anyUnowned >= 0 ? anyUnowned :
            matchingOwned >= 0 ? matchingOwned : anyOwned;
    }

    void MoveInstanceToFront(unsigned index)
    {
        if (index == firstInstance) return;
        auto &instance = instances[index];
        instances[instance.previous].next = instance.next;
        instances[instance.next].previous = instance.previous;
        instance.previous = instances[firstInstance].previous;
        instance.next = firstInstance;
        instances[instance.previous].next = index;
        instances[firstInstance].previous = index;
        firstInstance = index;
    }
    [[noreturn]] static void Failure(const char *reason)
    {
        std::fprintf(stderr, "Native SFX: %s\n", reason);
        std::abort();
    }

    BOOL32 ApplyGain(SoundEffect &effect, const NativeSfxEntry &entry, S32 pan)
    {
        const SINGLE gain = std::clamp(entry.volume * globalVolume * effect.volume * effect.spatialGain,
                                      0.0F, 1.0F);
        return streamer->SetVolume(effect.stream, ConvertFloatToDSound(gain)) &&
            spatialAudio->SetPan(effect.stream, pan);
    }
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
    std::array<Instance, 10> instances{};
    unsigned firstInstance = 0;
    SINGLE globalVolume = 1.0F;
    bool active = true;
    COMPTR<FrontierX::INativeSpatialAudio> spatialAudio;
    U32 eventHandle = 0;
    U32 currentSystemID = 0;
    bool gameActive = false;
    bool cameraMoved = true;
    SINGLE top = 0.0F, bottom = 0.0F, left = 0.0F, right = 0.0F;
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
bool CheckNativeSfxPool()
{
    auto *owner = dynamic_cast<NativeSfx *>(SFXMANAGER);
    return owner && owner->CheckPool();
}
}
