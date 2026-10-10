#include "Include/NativeAppGlobals.h"
#include "Include/NativeAppMissionMedia.h"
#include <DACOM.H>
#include "SoundManager.h"
#include "MusicManager.h"
#include "resource.h"
#include "UnitComm.h"
#include <Filesys.h>
#include <Streamer.h>
#include <TComponent.h>
#include <EventSys.h>
#include <IConnection.h>
#include <TSmartPointer.h>
#include "ObjList.h"
#include "IObject.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <map>

namespace
{
struct NativeDisabledUnitComm : IUnitComm
{
    BEGIN_DACOM_MAP_INBOUND(NativeDisabledUnitComm)
    DACOM_INTERFACE_ENTRY(IUnitComm)
    END_DACOM_MAP()

    void PlayCommMessage(IBaseObject *, U32, U32, U32, U32, U32, DWORD) override
    {
        std::fprintf(stderr, "Native unit voice explicitly disabled\n");
    }
    void PlayCommMessage(IBaseObject *, U32, char *, U32, U32, U32, DWORD) override
    {
        std::fprintf(stderr, "Native unit voice explicitly disabled\n");
    }
};

[[noreturn]] void Unsupported(const char *method)
{
    std::fprintf(stderr, "Native mission sound method is not implemented: %s\n", method);
    std::abort();
}

struct VolumeSetting
{
    S32 level;
    bool muted = false;
};

struct NativeMissionSound : ISoundManager, IEventCallback
{
    BEGIN_DACOM_MAP_INBOUND(NativeMissionSound)
    DACOM_INTERFACE_ENTRY(ISoundManager)
    DACOM_INTERFACE_ENTRY(IEventCallback)
    END_DACOM_MAP()

    ~NativeMissionSound()
    {
        FlushStreams();
        if (EVENTSYS && eventHandle)
        {
            COMPTR<IDAConnectionPoint> connection;
            if (EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK)
                connection->Unadvise(eventHandle);
        }
    }

    bool Init()
    {
        COMPTR<IDAConnectionPoint> connection;
        return EVENTSYS && EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK &&
            connection->Advise(static_cast<IEventCallback *>(this), &eventHandle) == GR_OK;
    }

    struct StreamEntry
    {
        HSTREAM handle;
        U32 fromID;
        bool unitVoice;
    };

    GENRESULT COMAPI Notify(U32 message, void *) override
    {
        if (message == CQE_MISSION_CLOSE) { FlushStreams(); return GR_OK; }
        if (message != CQE_UPDATE) return GR_OK;
        for (auto it = streams.begin(); it != streams.end();)
        {
            const auto status = STREAMER->GetStatus(it->second.handle);
            if (status == IStreamer::COMPLETED || status == IStreamer::INVALID)
            {
                if (it->second.unitVoice && status == IStreamer::COMPLETED) ++stats.unitCompleted;
                ClearHighlight(it->second);
                if (!STREAMER->CloseHandle(it->second.handle)) Unsupported("completed stream close");
                it = streams.erase(it);
            }
            else ++it;
        }
        return GR_OK;
    }

    static void ClearHighlight(const StreamEntry &entry)
    {
        if (entry.fromID && OBJLIST)
            if (auto *object = OBJLIST->FindObject(entry.fromID)) object->bSpecialHighlight = false;
    }

    U32 PlayCommMessage(const char *filename, IFileSystem *parent,
                        IBaseObject *object, SINGLE volume) override
    {
        return Play(filename, parent ? parent : SPEECHDIR, volume, comms, object);
    }

    U32 PlayChatMessage(const char *filename, IFileSystem *parent,
                        SINGLE volume) override
    {
        return Play(filename, parent, volume, chat);
    }

    U32 PlayAudioMessage(const char *filename, IBaseObject *object,
                         SINGLE volume) override
    {
        if (object) Unsupported("PlayAudioMessage object highlight");
        return Play(filename, MSPEECHDIR, volume, chat);
    }

    U32 PlayAnimatedMessage(const char *filename, const char *animType, S32, S32,
                            IBaseObject *object, SINGLE volume) override
    {
        if (!skipAnimatedPortraits) Unsupported("PlayAnimatedMessage portrait");
        if (!animType || !*animType || object)
            Unsupported("PlayAnimatedMessage invalid animation or speaker highlight");
        return Play(filename, MSPEECHDIR, volume, comms);
    }

    U32 PlayMovie(const char *, S32, S32, SINGLE) override
    {
        Unsupported("PlayMovie position");
    }

    U32 PlayMovie(const char *, const RECT &, SINGLE) override
    {
        Unsupported("PlayMovie rectangle");
    }

    BOOL32 IsPlaying(U32 soundID) const override
    {
        const auto found = streams.find(soundID);
        if (found == streams.end()) return FALSE;
        return STREAMER->GetStatus(found->second.handle) == IStreamer::PLAYING;
    }

    BOOL32 StopPlayback(U32 soundID) override
    {
        const auto found = streams.find(soundID);
        if (found == streams.end()) return FALSE;
        const BOOL32 closed = STREAMER->CloseHandle(found->second.handle);
        if (closed)
        {
            ClearHighlight(found->second);
            if (found->second.unitVoice) ++stats.unitStopped;
            streams.erase(found);
        }
        return closed;
    }

    void FlushStreams() override
    {
        for (const auto &entry : streams)
        {
            ClearHighlight(entry.second);
            if (!STREAMER->CloseHandle(entry.second.handle)) Unsupported("FlushStreams close");
        }
        streams.clear();
    }

    bool IsChatEnabled() override { return false; }

    void GetMusicVolumeLevel(S32 &level, bool &muted) override
    { level = music.level; muted = music.muted; }
    void SetMusicVolumeLevel(const S32 &, bool) override
    { Unsupported("SetMusicVolumeLevel"); }
    void GetSfxVolumeLevel(S32 &level, bool &muted) override
    { level = effects.level; muted = effects.muted; }
    void SetSfxVolumeLevel(const S32 &, bool) override
    { Unsupported("SetSfxVolumeLevel"); }
    void GetCommVolumeLevel(S32 &level, bool &muted) override
    { level = comms.level; muted = comms.muted; }
    void SetCommVolumeLevel(const S32 &, bool) override
    { Unsupported("SetCommVolumeLevel"); }
    void GetChatVolumeLevel(S32 &level, bool &muted) override
    { level = chat.level; muted = chat.muted; }
    void SetChatVolumeLevel(const S32 &, bool) override
    { Unsupported("SetChatVolumeLevel"); }
    void TweekMusicVolume() override
    {
        if (!MUSICMANAGER) Unsupported("TweekMusicVolume music manager");
        MUSICMANAGER->SetVolume(0);
    }

    U32 GetLastStreamID() override { return lastID; }
    void SetLastStreamID(U32 id) override { lastID = id; }

    GENRESULT GetAnimation(U32, IAnimate **animation) override
    {
        if (animation) *animation = nullptr;
        return GR_NOT_IMPLEMENTED;
    }

    int BltMovies() override { return 0; }

    U32 Play(const char *filename, IFileSystem *source, SINGLE volume,
             const VolumeSetting &setting, IBaseObject *object = nullptr)
    {
        if (!filename || !filename[0] || !source || !STREAMER ||
            !std::isfinite(volume) || volume < 0.0F || volume > 1.0F ||
            lastID == std::numeric_limits<U32>::max())
            Unsupported("Play invalid source or parameters");
        if (skipSpeech && source == MSPEECHDIR)
        {
            std::fprintf(stderr, "Native mission speech explicitly disabled: %s\n", filename);
            return 0;
        }
        HSTREAM handle = STREAMER->Open(filename, source, 0);
        if (!handle)
        {
            std::fprintf(stderr, "Native mission speech failed to open: %s\n", filename);
            std::abort();
        }
        const SINGLE gain = setting.muted ? 0.0F :
            std::clamp(volume * (setting.level / 10.0F), 0.0F, 1.0F);
        if (!STREAMER->SetVolume(handle, ConvertFloatToDSound(gain)) ||
            !STREAMER->Restart(handle))
        {
            STREAMER->CloseHandle(handle);
            Unsupported("Play streamer start");
        }
        const U32 id = ++lastID;
        const bool unitVoice = source == SPEECHDIR;
        streams.emplace(id, StreamEntry{handle, object ? object->GetPartID() : 0U, unitVoice});
        if (object) object->bSpecialHighlight = true;
        if (unitVoice)
        {
            ++stats.unitStarted;
            std::fprintf(stderr, "Native unit voice started: %s; stream %u; speaker %u\n",
                         filename, unsigned(id), unsigned(object ? object->GetPartID() : 0U));
        }
        return id;
    }

    bool skipSpeech = false;
    bool skipAnimatedPortraits = false;
    U32 lastID = 0;
    VolumeSetting music{6};
    VolumeSetting effects{9};
    VolumeSetting comms{7};
    VolumeSetting chat{9};
    std::map<U32, StreamEntry> streams;
    U32 eventHandle = 0;
    FrontierX::NativeMissionAudioStats stats{};
};
}

namespace FrontierX
{
IUnitComm *CreateNativeDisabledUnitComm()
{
    return new DAComponent<NativeDisabledUnitComm>;
}

ISoundManager *CreateNativeMissionSoundManager(bool skipSpeech, bool skipAnimatedPortraits)
{
    if (!STREAMER || !MSPEECHDIR || !SPEECHDIR) return nullptr;
    auto *manager = new DAComponent<NativeMissionSound>;
    manager->skipSpeech = skipSpeech;
    manager->skipAnimatedPortraits = skipAnimatedPortraits;
    if (!manager->Init()) { manager->Release(); return nullptr; }
    return manager;
}

bool GetNativeMissionAudioStats(ISoundManager *manager, NativeMissionAudioStats &stats)
{
    auto *native = dynamic_cast<NativeMissionSound *>(manager);
    if (!native) return false;
    stats = native->stats;
    stats.activeStreams = static_cast<U32>(native->streams.size());
    return true;
}
}
