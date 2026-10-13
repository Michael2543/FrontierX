#include "FrontierX/CompatibilityJournal.h"
#include "FrontierX/NativeStreamer.h"
#include "FrontierX/NativeWindowManager.h"
#include "Streamer.h"
#if defined(FRONTIERX_HAVE_IAC_DECODER)
#include "NativeIacWave.h"
#endif
#include "Filesys.h"
#include "SYSTEM.H"
#include "TComponent.h"
#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

namespace FrontierX
{
namespace
{
static CompatibilityJournal & M3StreamerJournal()
{
    static CompatibilityJournal journal("native-streamer");
    return journal;
}
class NativeStreamer;
std::vector<NativeStreamer *> owners;
std::uintptr_t nextHandle = 1;
std::uint32_t diagnosticTick = 0;
char interfaceName[] = "IStreamer";

class NativeStreamer : public ISystemComponent, public IStreamer
{
public:
    BEGIN_DACOM_MAP_INBOUND(NativeStreamer)
    DACOM_INTERFACE_ENTRY(ISystemComponent)
    DACOM_INTERFACE_ENTRY(IAggregateComponent)
    DACOM_INTERFACE_ENTRY(IStreamer)
    DACOM_INTERFACE_ENTRY2(IID_ISystemComponent, ISystemComponent)
    DACOM_INTERFACE_ENTRY2(IID_IAggregateComponent, IAggregateComponent)
    DACOM_INTERFACE_ENTRY2(IID_IStreamer, IStreamer)
    END_DACOM_MAP()

    struct Stream
    {
        SDL_AudioStream * audio = nullptr;
        std::vector<Uint8> pcm;
        HSTREAM handle = nullptr;
        STATUS status = STOPPED;
        S32 volume = 0;
        bool looping = false;
        bool notifyInit = true;
        bool eofSent = false;
        Uint64 drainedAt = 0;
        Uint64 tailMilliseconds = 0;
        ~Stream() { if (audio) SDL_DestroyAudioStream(audio); }
    };
    ~NativeStreamer()
    {
        owners.erase(std::remove(owners.begin(), owners.end(), this), owners.end());
        streams.clear();
        if (device) SDL_CloseAudioDevice(device);
        if (audioInitialized) SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
    GENRESULT init(AGGDESC *) { owners.push_back(this); return GR_OK; }
    GENRESULT Initialize() override { return GR_OK; }

    BOOL32 Init(STREAMERDESC * desc) override
    {
        if (!desc || desc->size != sizeof(*desc) || desc->lpDSound || device ||
            (desc->hMainWindow && (desc->uMsg < 0x400 ||
                !HasNativeWindowMessageTarget(desc->hMainWindow))) ||
            (!desc->hMainWindow && desc->uMsg) ||
            !std::isfinite(desc->readBufferTime) || desc->readBufferTime < 0 ||
            !std::isfinite(desc->soundBufferTime) || desc->soundBufferTime < 0) return FALSE;
        if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) return FALSE;
        audioInitialized = true;
        device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
        if (!device) { SDL_QuitSubSystem(SDL_INIT_AUDIO); audioInitialized = false; return FALSE; }
        window = desc->hMainWindow;
        message = desc->uMsg;
        return TRUE;
    }
    HSTREAM Open(const char * filename, IFileSystem * parent, DWORD flags) override try
    {
        if (!device || !parent || !filename || !*filename ||
            (flags & ~(STRMFL_PLAY | STRMFL_LOOPING)) || streams.size() >= 64 || !nextHandle)
            return nullptr;
        DAFILEDESC descriptor(filename);
        HANDLE file = parent->OpenChild(&descriptor);
        if (file == INVALID_HANDLE_VALUE && !std::strpbrk(filename, "*?/\\"))
        {
            // Campaign scripts use uppercase speech names; retail UTF stores lowercase.
            WIN32_FIND_DATA match{}, next{};
            HANDLE search = parent->FindFirstFile(filename, &match);
            if (search != INVALID_HANDLE_VALUE)
            {
                const bool ambiguous = parent->FindNextFile(search, &next) != FALSE;
                const bool searchClosed = parent->FindClose(search) != FALSE;
                if (!ambiguous && searchClosed && !(match.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    DAFILEDESC actual(match.cFileName);
                    file = parent->OpenChild(&actual);
                }
            }
        }
        M3StreamerJournal().Write("resource-resolved", 0, file != INVALID_HANDLE_VALUE, filename);
        if (file == INVALID_HANDLE_VALUE) return nullptr;
        struct FileGuard
        {
            IFileSystem * owner;
            HANDLE handle;
            ~FileGuard() { if (handle != INVALID_HANDLE_VALUE) owner->CloseHandle(handle); }
        } guard{parent, file};
        DWORD high = 0;
        const DWORD size = parent->GetFileSize(file, &high);
        if (high || size < 12 || size > 64U * 1024U * 1024U)
        { return nullptr; }
        std::vector<Uint8> bytes(size);
        DWORD read = 0;
        const bool loaded = parent->ReadFile(file, bytes.data(), size, &read, nullptr) && read == size;
        const bool closed = parent->CloseHandle(file) != FALSE;
        guard.handle = INVALID_HANDLE_VALUE;
        if (!loaded || !closed) return nullptr;
        SDL_AudioSpec spec{};
        Uint8 * pcm = nullptr;
        Uint32 length = 0;
        SDL_IOStream * io = SDL_IOFromConstMem(bytes.data(), bytes.size());
        if (!io) return nullptr;
        const bool waveLoaded = SDL_LoadWAV_IO(io, true, &spec, &pcm, &length);
        std::unique_ptr<Uint8, decltype(&SDL_free)> pcmOwner(pcm, SDL_free);
        std::vector<Uint8> decoded;
        if (!waveLoaded)
        {
#if defined(FRONTIERX_HAVE_IAC_DECODER)
            const bool decodedOk = DecodeNativeIacWave(bytes, spec, decoded);
            M3StreamerJournal().Write("iac-decoded", 0, decodedOk, filename);
            if (!decodedOk) return nullptr;
            pcm = decoded.data();
            length = static_cast<Uint32>(decoded.size());
#else
            return nullptr;
#endif
        }
        M3StreamerJournal().Write("pcm-decoded", 0, length, filename);
        if (!length || length > 256U * 1024U * 1024U) return nullptr;
        auto stream = std::make_unique<Stream>();
        stream->audio = SDL_OpenAudioDeviceStream(device, &spec, nullptr, nullptr);
        if (!stream->audio) return nullptr;
        SDL_AudioSpec output{};
        int frames = 0;
        if (!SDL_GetAudioDeviceFormat(SDL_GetAudioStreamDevice(stream->audio), &output, &frames) ||
            output.freq <= 0 || frames <= 0) return nullptr;
        // Queued input can be exhausted before the final device buffer is heard.
        stream->tailMilliseconds = (2000ULL * static_cast<Uint64>(frames) +
            static_cast<Uint64>(output.freq) - 1) / static_cast<Uint64>(output.freq);
        stream->pcm.assign(pcm, pcm + length);
        stream->looping = (flags & STRMFL_LOOPING) != 0;
        if (!SDL_PutAudioStreamData(stream->audio, pcm, static_cast<int>(length)) ||
            (!stream->looping && !SDL_FlushAudioStream(stream->audio))) return nullptr;
        stream->handle = reinterpret_cast<HSTREAM>(nextHandle++);
        if (flags & STRMFL_PLAY)
        {
            if (!SDL_ResumeAudioStreamDevice(stream->audio)) return nullptr;
            stream->status = PLAYING;
        }
        const HSTREAM handle = stream->handle;
        streams.push_back(std::move(stream));
        return handle;
    }
    catch (const std::bad_alloc &) { return nullptr; }
    BOOL32 CloseHandle(HSTREAM handle) override
    {
        const auto found = std::find_if(streams.begin(), streams.end(),
            [handle](const auto & s) { return s->handle == handle; });
        if (found == streams.end()) return FALSE;
        streams.erase(found);
        return TRUE;
    }
    BOOL32 Stop(HSTREAM handle) override
    {
        Stream * s = Find(handle);
        if (!s || s->status == COMPLETED || s->status == INVALID ||
            !SDL_PauseAudioStreamDevice(s->audio)) return FALSE;
        s->status = STOPPED;
        s->drainedAt = 0;
        return TRUE;
    }
    BOOL32 Restart(HSTREAM handle) override
    {
        Stream * s = Find(handle);
        if (!s || s->status == COMPLETED || s->status == INVALID ||
            !SDL_ResumeAudioStreamDevice(s->audio)) return FALSE;
        s->status = PLAYING;
        return TRUE;
    }
    BOOL32 SetVolume(HSTREAM handle, S32 volume) override
    {
        Stream * s = Find(handle);
        if (!s || volume < -10000 || volume > 0) return FALSE;
        const float gain = volume == -10000 ? 0.0F : std::pow(10.0F, static_cast<float>(volume) / 2000.0F);
        if (!SDL_SetAudioStreamGain(s->audio, gain)) return FALSE;
        s->volume = volume;
        return TRUE;
    }
    BOOL32 GetVolume(HSTREAM handle, S32 * volume) const override
    {
        const Stream * s = Find(handle);
        if (!s || !volume) return FALSE;
        *volume = s->volume;
        return TRUE;
    }
    STATUS GetStatus(HSTREAM handle) const override
    {
        const Stream * s = Find(handle);
        return s ? s->status : INVALID;
    }
    void Update() override
    {
        std::vector<HSTREAM> snapshot;
        for (const auto & s : streams) snapshot.push_back(s->handle);
        for (HSTREAM handle : snapshot)
        {
            Stream * s = Find(handle);
            if (!s) continue;
            if (s->notifyInit)
            {
                s->notifyInit = false;
                Notify(handle, INITSUCCESS);
                s = Find(handle); // The real caller can close or restart during a notification.
                if (!s) continue;
            }
            if (s->status != PLAYING && s->status != EOFREACHED) continue;
            const int queued = SDL_GetAudioStreamQueued(s->audio);
            const int available = SDL_GetAudioStreamAvailable(s->audio);
            if (queued < 0 || available < 0)
            { s->status = INVALID; Notify(handle, INVALID); continue; }
            if (s->looping)
            {
                if (queued < static_cast<int>(s->pcm.size()) &&
                    !SDL_PutAudioStreamData(s->audio, s->pcm.data(), static_cast<int>(s->pcm.size())))
                { s->status = INVALID; Notify(handle, INVALID); }
                continue;
            }
            if (!queued && !available)
            {
                if (!s->drainedAt) s->drainedAt = SDL_GetTicks() + 1;
                if (!s->eofSent)
                {
                    s->eofSent = true;
                    s->status = EOFREACHED;
                    Notify(handle, EOFREACHED);
                    s = Find(handle);
                    if (!s || s->status != EOFREACHED) continue;
                }
                if (SDL_GetTicks() >= s->drainedAt + s->tailMilliseconds)
                { s->status = COMPLETED; Notify(handle, COMPLETED); }
            }
        }
    }
private:
    Stream * Find(HSTREAM handle) const
    {
        for (const auto & s : streams) if (s->handle == handle) return s.get();
        return nullptr;
    }
    void Notify(HSTREAM handle, STATUS status)
    {
        M3StreamerJournal().Write("stream-notification-tick", 0, diagnosticTick);
        M3StreamerJournal().Write("stream-notification", 0, static_cast<unsigned int>(status));
        if (window && message)
            DispatchNativeWindowMessage(window, message, static_cast<LegacyWParam>(status),
                                        reinterpret_cast<LegacyLParam>(handle));
    }
    std::vector<std::unique_ptr<Stream>> streams;
    SDL_AudioDeviceID device = 0;
    HWND window = 0;
    UINT message = 0;
    bool audioInitialized = false;
};
}

bool RegisterNativeStreamer(ICOManager * manager)
{
    if (!manager) return false;
    auto * factory = new DAComponentFactory2<DAComponentAggregate<NativeStreamer>, AGGDESC>(interfaceName);
    const GENRESULT result = manager->RegisterComponent(factory, interfaceName);
    factory->Release();
    return result == GR_OK;
}
void UpdateNativeStreamers()
{
    const auto snapshot = owners;
    for (auto * owner : snapshot) static_cast<IStreamer *>(owner)->AddRef();
    for (auto * owner : snapshot)
    {
        owner->Update();
        static_cast<IStreamer *>(owner)->Release();
    }
}
void SetNativeStreamerDiagnosticTick(std::uint32_t tick)
{
    diagnosticTick = tick;
}
}
