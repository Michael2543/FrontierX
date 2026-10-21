#include "FrontierX/CompatibilityJournal.h"
#include "FrontierX/PlatformTime.h"
#include "FrontierX/NativeStreamer.h"
#include "Include/NativeAppGlobals.h"
#include "Include/NativeAppMissionMedia.h"
#include "Include/MGlobals.h"
#include <DACOM.H>
#include "SoundManager.h"
#include "UserDefaults.h"
#include "MusicManager.h"
#include "SFX.h"
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
#include "LFParser.h"
#include "IAnimate.h"
#include "IBriefing.h"
#include "BaseHotRect.h"
#include "GenData.h"
#include <DAnimate.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <map>
#include <memory>
#include <cstring>

static FrontierX::CompatibilityJournal & M3NativeSoundJournal()
{
    static FrontierX::CompatibilityJournal journal("native-sound");
    return journal;
}

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
        if (!DEFAULTS) return false;
        const auto &sound=DEFAULTS->GetDefaults()->soundState;
        music={sound.music.volume,sound.music.bMute!=0};
        effects={sound.effects.volume,sound.effects.bMute!=0};
        comms={sound.comms.volume,sound.comms.bMute!=0};
        chat={sound.chat.volume,sound.chat.bMute!=0};
        COMPTR<IDAConnectionPoint> connection;
        return EVENTSYS && EVENTSYS->QueryOutgoingInterface("IEventCallback", connection) == GR_OK &&
            connection->Advise(static_cast<IEventCallback *>(this), &eventHandle) == GR_OK;
    }

    struct Portrait
    {
        COMPTR<IAnimate> animation;
        COMPTR<ILFParser> parser;
        bool ownershipTransferred = false;
        ~Portrait()
        {
            if (animation)
            {
                if (ownershipTransferred) animation->PauseAnim(true);
                else animation->DeferredDestruction();
                // Match SoundMan STREAM_NODE: the hot-rectangle owner releases
                // the animation after deferred destruction or ownership transfer.
                animation.ptr = nullptr;
            }
        }
    };

    struct StreamEntry
    {
        HSTREAM handle;
        U32 fromID;
        bool unitVoice;
        std::unique_ptr<Portrait> portrait;
        SINGLE volume;
        bool chatMessage;
    };

    GENRESULT COMAPI Notify(U32 message, void *parameter) override
    {
        if (message == CQE_MISSION_CLOSE) { FlushStreams(); return GR_OK; }
        if (message != CQE_UPDATE) return GR_OK;
        for (auto it = streams.begin(); it != streams.end();)
        {
            const auto status = STREAMER->GetStatus(it->second.handle);
            if (status == IStreamer::COMPLETED || status == IStreamer::INVALID)
            {
                M3NativeSoundJournal().Write("completion-observed-tick", it->first, MGlobals::GetUpdateCount());
                M3NativeSoundJournal().Write("completion-observed", it->first, status);
                if (it->second.unitVoice && status == IStreamer::COMPLETED) ++stats.unitCompleted;
                ClearHighlight(it->second);
                RecordPortrait(it->second);
                if (!STREAMER->CloseHandle(it->second.handle)) Unsupported("completed stream close");
                it = streams.erase(it);
            }
            else ++it;
        }
        UpdateVolumeState(static_cast<U32>(reinterpret_cast<std::uintptr_t>(parameter)) >> 10);
        return GR_OK;
    }

    static void ClearHighlight(const StreamEntry &entry)
    {
        if (entry.fromID && OBJLIST)
            if (auto *object = OBJLIST->FindObject(entry.fromID)) object->bSpecialHighlight = false;
    }

    void RecordPortrait(const StreamEntry &entry)
    {
        if (!entry.portrait) return;
        std::uint64_t changes = 0, draws = 0;
        if (!FrontierX::GetNativeAnimationCounts(entry.portrait->animation, changes, draws))
            Unsupported("portrait source animation diagnostics");
        stats.portraitFrameChanges += changes;
        stats.portraitDraws += draws;
    }

    U32 PlayCommMessage(const char *filename, IFileSystem *parent,
                        IBaseObject *object, SINGLE volume) override
    {
        return Play(filename, parent ? parent : SPEECHDIR, volume, comms, object, true);
    }

    U32 PlayChatMessage(const char *filename, IFileSystem *parent,
                        SINGLE volume) override
    {
        return Play(filename, parent, volume, chat);
    }

    U32 PlayAudioMessage(const char *filename, IBaseObject *object,
                         SINGLE volume) override
    {
        IFileSystem *source = MSPEECHDIR;
        const char *resolvedFilename = filename;
        // The authored TM8 beacon names this exact retail SFX entry as audio.
        // Prefer any mission-speech override; do not redirect other speech.
        if (!skipSpeech && filename && std::strcmp(filename, "specialDenied.wav") == 0 &&
            source && source->GetFileAttributes(filename) == 0xffffffffU)
        {
            constexpr const char *retailEntry = "specialdenied.wav";
            if (source->GetFileAttributes(retailEntry) != 0xffffffffU)
                resolvedFilename = retailEntry;
            else if (SFXDIR && SFXDIR->GetFileAttributes(retailEntry) != 0xffffffffU)
            {
                source = SFXDIR;
                resolvedFilename = retailEntry;
                std::fprintf(stderr, "Native mission beacon resolved from SFX: %s\n", filename);
            }
        }
        return Play(resolvedFilename, source, volume, chat, object);
    }

    U32 PlayAnimatedMessage(const char *filename, const char *animType, S32 x, S32 y,
                            IBaseObject *object, SINGLE volume) override
    {
        if (!animType || !*animType)
            Unsupported("PlayAnimatedMessage invalid animation");
        if (skipAnimatedPortraits)
            return Play(filename, MSPEECHDIR, volume, comms, object);
        if (!filename || !*filename || !GENDATA || !MSPEECHDIR)
            Unsupported("PlayAnimatedMessage invalid source");
        COMPTR<IDAComponent> component;
        auto portrait = std::make_unique<Portrait>();
        if (GENDATA->CreateInstance(animType, component) != GR_OK ||
            component->QueryInterface("IAnimate", portrait->animation) != GR_OK)
            Unsupported("PlayAnimatedMessage animation resource");
        char lipFile[36];
        std::strncpy(lipFile, filename, 32);
        lipFile[32] = 0;
        char *extension = std::strrchr(lipFile, '.');
        if (!extension) Unsupported("PlayAnimatedMessage filename extension");
        std::strcpy(extension + 1, "txt");
        if (CreateLFParser(portrait->parser) != GR_OK ||
            !portrait->parser->Initialize(lipFile))
            Unsupported("PlayAnimatedMessage lip resource");
        U32 *frames = nullptr;
        const S32 count = portrait->parser->ParseFile(&frames);
        // Native mounted filesystems complete reads synchronously. Never use
        // an invented frame sequence when parsing or I/O fails.
        if (count <= 0 || !frames) Unsupported("PlayAnimatedMessage lip parse");
        BaseHotRect *parent = FULLSCREEN;
        ANIMATE_DATA data{};
        data.xOrigin = x;
        data.yOrigin = y;
        data.bFuzzEffect = BRIEFING == nullptr;
        if (BRIEFING)
        {
            if (BRIEFING->QueryInterface("BaseHotRect", reinterpret_cast<void **>(&parent)) != GR_OK ||
                !parent) Unsupported("PlayAnimatedMessage briefing parent");
            parent->Release();
        }
        portrait->animation->InitAnimate(data, parent, frames, static_cast<U32>(count));
        portrait->animation->PauseAnim(true);
        const U32 id = Play(filename, MSPEECHDIR, volume, chat, object);
        if (id)
        {
            streams.at(id).portrait = std::move(portrait);
            streams.at(id).portrait->animation->PauseAnim(false);
            ++stats.portraitsStarted;
        }
        return id;
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
        // Original SoundMan retains registered streams until completion/error
        // handling or explicit stop. EOF and paused playback are still live.
        return streams.find(soundID) != streams.end();
    }

    BOOL32 StopPlayback(U32 soundID) override
    {
        const auto found = streams.find(soundID);
        if (found == streams.end()) return FALSE;
        const BOOL32 closed = STREAMER->CloseHandle(found->second.handle);
        if (closed)
        {
            ClearHighlight(found->second);
            RecordPortrait(found->second);
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
            RecordPortrait(entry.second);
            if (!STREAMER->CloseHandle(entry.second.handle)) Unsupported("FlushStreams close");
        }
        streams.clear();
    }

    bool IsChatEnabled() override { return false; }

    void GetMusicVolumeLevel(S32 &level, bool &muted) override
    { level = music.level; muted = music.muted; }
    void SetMusicVolumeLevel(const S32 &, bool) override;
    void GetSfxVolumeLevel(S32 &level, bool &muted) override
    { level = effects.level; muted = effects.muted; }
    void SetSfxVolumeLevel(const S32 &, bool) override;
    void GetCommVolumeLevel(S32 &level, bool &muted) override
    { level = comms.level; muted = comms.muted; }
    void SetCommVolumeLevel(const S32 &, bool) override;
    void GetChatVolumeLevel(S32 &level, bool &muted) override
    { level = chat.level; muted = chat.muted; }
    void SetChatVolumeLevel(const S32 &, bool) override;
    void TweekMusicVolume() override
    {
        if (!MUSICMANAGER) Unsupported("TweekMusicVolume music manager");
        currentMusicVolume = 0.0F;
        MUSICMANAGER->SetVolume(0);
    }

    U32 GetLastStreamID() override { return lastID; }
    void SetLastStreamID(U32 id) override { lastID = id; }

    GENRESULT GetAnimation(U32 soundID, IAnimate **animation) override
    {
        if (!animation) return GR_INVALID_PARMS;
        *animation = nullptr;
        const auto found = streams.find(soundID);
        if (found == streams.end() || !found->second.portrait) return GR_GENERIC;
        auto &portrait = *found->second.portrait;
        if (portrait.ownershipTransferred) Unsupported("GetAnimation duplicate ownership transfer");
        *animation = portrait.animation;
        portrait.ownershipTransferred = true;
        return GR_OK;
    }

    int BltMovies() override { return 0; }

    U32 Play(const char *filename, IFileSystem *source, SINGLE volume,
             const VolumeSetting &setting, IBaseObject *object = nullptr,
             bool unitCommunication = false)
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
        M3NativeSoundJournal().Write("stream-open", 0, handle != nullptr, filename);
        if (!handle)
        {
            // Original SoundMan returns zero on a failed Open, including
            // PlayAudioMessage. Do not invent a stream or abort the mission.
            if (unitCommunication)
            {
                ++failedCommunicationOpens;
                std::fprintf(stderr, "Native unit communication unavailable: %s; failed opens %llu; playback ID 0 (failure)\n",
                    filename, static_cast<unsigned long long>(failedCommunicationOpens));
            }
            else
            {
                ++failedMissionOpens;
                std::fprintf(stderr, "Native mission audio unavailable: %s; failed opens %llu; playback ID 0 (failure)\n",
                    filename, static_cast<unsigned long long>(failedMissionOpens));
            }
            return 0;
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
        M3NativeSoundJournal().Write("playback-started", id, 1, filename);
        const bool unitVoice = source == SPEECHDIR;
        streams.emplace(id, StreamEntry{handle, object ? object->GetPartID() : 0U,
            unitVoice, nullptr, volume, &setting == &chat});
        UpdateVolumeState(0);
        if (object) object->bSpecialHighlight = true;
        if (unitVoice)
        {
            ++stats.unitStarted;
            std::fprintf(stderr, "Native unit voice started: %s; stream %u; speaker %u\n",
                         filename, unsigned(id), unsigned(object ? object->GetPartID() : 0U));
        }
        return id;
    }

    void UpdateVolumeState(U32 milliseconds)
    {
        bool commPlaying = false, chatPlaying = false;
        for (const auto &entry : streams)
        {
            if (entry.second.chatMessage) chatPlaying = true;
            else commPlaying = true;
        }
        auto backgroundGain = [chatPlaying, commPlaying](const VolumeSetting &setting) {
            SINGLE gain = setting.muted ? 0.0F : setting.level / 10.0F;
            if (chatPlaying) gain = std::min(gain, 0.25F);
            else if (commPlaying) gain -= 0.25F;
            return std::max(gain, 0.0F);
        };
        auto ramp = [milliseconds](SINGLE current, SINGLE target) {
            if (target > current)
                current = std::min(target, current + static_cast<SINGLE>(milliseconds * 0.0005));
            else current = target;
            return std::max(current, 0.0F);
        };
        // Source SoundMan increases gain over two seconds and ducks immediately.
        // Enable(FALSE) remains with its owning explicit exclusion/pause route.
        currentMusicVolume = ramp(currentMusicVolume, backgroundGain(music));
        currentEffectsVolume = ramp(currentEffectsVolume, backgroundGain(effects));
        if (MUSICMANAGER) MUSICMANAGER->SetVolume(currentMusicVolume);
        if (SFXMANAGER) SFXMANAGER->SetSFXVolume(currentEffectsVolume);
        for (const auto &entry : streams)
        {
            const auto &stream = entry.second;
            const auto &setting = stream.chatMessage ? chat : comms;
            SINGLE gain = setting.muted ? 0.0F : stream.volume * (setting.level / 10.0F);
            if (!stream.chatMessage && chatPlaying) gain -= 0.75F;
            if (!STREAMER->SetVolume(stream.handle, ConvertFloatToDSound(std::clamp(gain, 0.0F, 1.0F))))
                Unsupported("UpdateVolumeState stream volume");
        }
    }

    void SetLevel(VolumeSetting &setting, S32 level, bool muted)
    {
        if (level < 0 || level > 10) Unsupported("SetVolumeLevel range");
        setting = {level, muted};
        // The original SoundMan holds a reference to USER_DEFAULTS::soundState.
        auto &sound=DEFAULTS->GetDefaults()->soundState;
        auto &item= &setting==&music ? sound.music : &setting==&effects ? sound.effects :
            &setting==&comms ? sound.comms : sound.chat;
        item.volume=level; item.bMute=muted;
        UpdateVolumeState(5000);
    }

    bool skipSpeech = false;
    bool skipAnimatedPortraits = false;
    U32 lastID = 0;
    VolumeSetting music{6};
    VolumeSetting effects{9};
    VolumeSetting comms{7};
    VolumeSetting chat{9};
    SINGLE currentMusicVolume = -0.00001F;
    SINGLE currentEffectsVolume = -0.00001F;
    std::map<U32, StreamEntry> streams;
    U32 eventHandle = 0;
    FrontierX::NativeMissionAudioStats stats{};
    std::uint64_t failedCommunicationOpens = 0;
    std::uint64_t failedMissionOpens = 0;
};

void NativeMissionSound::SetMusicVolumeLevel(const S32 &level, bool muted)
{ SetLevel(music, level, muted); }
void NativeMissionSound::SetSfxVolumeLevel(const S32 &level, bool muted)
{ SetLevel(effects, level, muted); }
void NativeMissionSound::SetCommVolumeLevel(const S32 &level, bool muted)
{ SetLevel(comms, level, muted); }
void NativeMissionSound::SetChatVolumeLevel(const S32 &level, bool muted)
{ SetLevel(chat, level, muted); }
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
    for (const auto &entry : native->streams)
    {
        if (!entry.second.portrait) continue;
        std::uint64_t changes = 0, draws = 0;
        if (!GetNativeAnimationCounts(entry.second.portrait->animation, changes, draws)) return false;
        stats.portraitFrameChanges += changes;
        stats.portraitDraws += draws;
    }
    return true;
}

// Existing shell smoke: real generated PCM through the real stream/device and
// mission sound owner, without a mission or fabricated completion status.
bool CheckNativeMissionSoundCompletion()
{
    auto failed = [](const char *step) {
        std::fprintf(stderr, "Native synthetic sound lifecycle failed: %s\n", step);
        return false;
    };
    if (!STREAMER || !SFXDIR) return failed("owners");
    if (!CheckNativeStereoPan()) return failed("stereo pan sample gains");
    struct ObjectBinding
    {
        IObjectList *previous = OBJLIST;
        ~ObjectBinding() { OBJLIST = previous; }
    } objectBinding;
    COMPTR<IObjectList> fixtureObjects;
    if (!OBJLIST)
    {
        fixtureObjects = CreateNativeSoundCheckObjectList();
        fixtureObjects->Release();
        OBJLIST = fixtureObjects;
    }
    COMPTR<ISoundManager> sound;
    auto *owner = new DAComponent<NativeMissionSound>;
    sound = owner;
    owner->Release();
    // Real UnitComm can request an absent speech entry from a retail StringPack.
    // Failure must retain the original zero result, without a stream or success ID.
    if (owner->PlayCommMessage("frontierx-missing-communication.wav", SFXDIR, nullptr, 1.0F) != 0 ||
        owner->lastID != 0 || !owner->streams.empty() || owner->failedCommunicationOpens != 1)
        return failed("missing communication failure");
    const U32 id = owner->PlayCommMessage("interface1.wav", SFXDIR, nullptr, 1.0F);
    if (!id || !owner->IsPlaying(id)) return failed("initial stream");
    const HSTREAM stream = owner->streams.at(id).handle;
    COMPTR<INativeSpatialAudio> spatialAudio;
    if (STREAMER->QueryInterface("INativeSpatialAudio", spatialAudio) != GR_OK ||
        !spatialAudio->SetPan(stream, 6000) || !spatialAudio->SetPan(stream, -6000) ||
        spatialAudio->SetPan(stream, 10001) || !spatialAudio->SetPan(stream, 0))
        return failed("stereo pan stream binding");
    S32 appliedVolume = 0, level = 0;
    bool muted = true;
    owner->SetCommVolumeLevel(4, false);
    owner->GetCommVolumeLevel(level, muted);
    if (level != 4 || muted || !STREAMER->GetVolume(stream, &appliedVolume) ||
        appliedVolume != ConvertFloatToDSound(0.4F)) return failed("communication volume");
    owner->SetCommVolumeLevel(7, false);
    if (!STREAMER->Stop(stream) || STREAMER->GetStatus(stream) != IStreamer::STOPPED ||
            !owner->IsPlaying(id) || !STREAMER->Restart(stream)) return failed("stop/restart");
    const auto until = Platform::MonotonicMicroseconds() + 3000000;
    auto waitFor = [&](IStreamer::STATUS state) {
        while (Platform::MonotonicMicroseconds() < until)
        {
            UpdateNativeStreamers();
            const auto actual = STREAMER->GetStatus(stream);
            if (actual == state) return true;
            if (actual == IStreamer::INVALID || actual == IStreamer::COMPLETED) return false;
            Platform::SleepForMilliseconds(1);
        }
        return false;
    };
    if (!waitFor(IStreamer::EOFREACHED) || !owner->IsPlaying(id) ||
            !waitFor(IStreamer::COMPLETED)) return failed("EOF/completion");
    owner->Notify(CQE_UPDATE, nullptr);
    if (owner->IsPlaying(id) || !owner->streams.empty()) return failed("completed cleanup");
    const U32 stopped = owner->PlayCommMessage("interface1.wav", SFXDIR, nullptr, 1.0F);
    if (!stopped || !owner->IsPlaying(stopped) || !owner->StopPlayback(stopped) ||
        owner->IsPlaying(stopped) || !owner->streams.empty() || !OBJLIST) return failed("explicit stop");
    // TM1's first Blackwell message needs the same registered speaker highlight
    // as UnitComm. Reuse the actual generated PCM/stream fixture, with portraits
    // explicitly excluded; no mission state, objective or fake EOF is supplied.
    struct Speaker : IBaseObject
    {
        void *QueryInterface(OBJID, OBJPTR<IBaseObject> &, U32) override { return nullptr; }
        U32 GetPartID() const override { return 0xfffffff1U; }
    } speaker{};
    struct Binding
    {
        Speaker &speaker;
        IFileSystem *speech = MSPEECHDIR;
        ~Binding()
        {
            OBJLIST->RemovePartID(&speaker, speaker.GetPartID());
            OBJLIST->RemoveObject(&speaker);
            MSPEECHDIR = speech;
        }
    } binding{speaker};
    if (!OBJLIST->AddObject(&speaker) ||
        !OBJLIST->AddPartID(&speaker, speaker.GetPartID())) return failed("speaker registration");
    MSPEECHDIR = SFXDIR;
    // TM8's beacon requests an unavailable MSpeech entry. Original
    // SoundMan::PlayAudioMessage returns zero without a stream or highlight.
    const U32 beforeMissingAudio = owner->lastID;
    if (owner->PlayAudioMessage("frontierx-missing-mission-audio.wav", &speaker, 1.0F) != 0 ||
        owner->lastID != beforeMissingAudio || !owner->streams.empty() || speaker.bSpecialHighlight ||
        owner->failedMissionOpens != 1 || owner->failedCommunicationOpens != 1)
        return failed("missing mission audio failure");
    const U32 audio = owner->PlayAudioMessage("interface1.wav", &speaker, 1.0F);
    if (!audio || !owner->IsPlaying(audio) || !speaker.bSpecialHighlight ||
        !owner->StopPlayback(audio) || speaker.bSpecialHighlight || !owner->streams.empty())
        return failed("mission audio after missing entry");
    // The authored TM8 beacon names an exact SFX asset, absent from MSpeech.
    // Keep unrelated missing speech out of the effect archive.
    if (!TEXTURESDIR || TEXTURESDIR->GetFileAttributes("specialDenied.wav") != 0xffffffffU)
        return failed("separate beacon source fixture");
    MSPEECHDIR = TEXTURESDIR;
    const U32 beforeBeacon = owner->lastID;
    owner->skipSpeech = true;
    if (owner->PlayAudioMessage("specialDenied.wav", nullptr, 1.0F) != 0 ||
        owner->lastID != beforeBeacon || !owner->streams.empty()) return failed("disabled beacon");
    owner->skipSpeech = false;
    const U32 beacon = owner->PlayAudioMessage("specialDenied.wav", &speaker, 1.0F);
    if (!beacon || !owner->IsPlaying(beacon) || !speaker.bSpecialHighlight ||
        !owner->StopPlayback(beacon) || speaker.bSpecialHighlight || !owner->streams.empty())
        return failed("beacon exact effect archive playback");
    if (owner->PlayAudioMessage("interface1.wav", nullptr, 1.0F) != 0 ||
        owner->lastID != beacon || !owner->streams.empty())
        return failed("unrelated speech must not fall back to effects");
    MSPEECHDIR = SFXDIR;
    COMPTR<ILFParser> lips;
    if (CreateLFParser(lips) != GR_OK || !lips->Initialize("interface1.txt"))
    { std::fputs("Native synthetic lip resource initialization failed\n", stderr); return false; }
    U32 *frames = nullptr;
    if (lips->ParseFile(&frames) != 3 || !frames ||
        frames[0] != 3 || frames[1] != 3 || frames[2] != 12)
    { std::fputs("Native synthetic lip frame sequence failed\n", stderr); return false; }
    // The parser owns the lip-frame array; reinitialization must release it.
    if (!lips->Initialize("INTERFACE1.TXT") || lips->ParseFile(&frames) != 3 ||
        !frames || frames[0] != 3 || frames[1] != 3 || frames[2] != 12)
    { std::fputs("Native synthetic lip reinitialization failed\n", stderr); return false; }
    owner->skipAnimatedPortraits = true;
    const U32 story = owner->PlayAnimatedMessage("interface1.wav", "Animate!!SyntheticSpeech",
                                               0, 0, &speaker, 1.0F);
    return story && speaker.bSpecialHighlight && owner->IsPlaying(story) &&
        owner->StopPlayback(story) && !speaker.bSpecialHighlight && owner->streams.empty();
}
}
