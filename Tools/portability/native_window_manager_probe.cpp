#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/NativeWindowManager.h"
#include "FrontierX/NativeStreamer.h"
#include "FrontierX/NativeUtfFileSystem.h"
#include "Streamer.h"
#if defined(FRONTIERX_HAVE_VIDEO_DECODER)
#include "FrontierX/NativeVideoSystem.h"
#include "FrontierX/NativeRenderPipeline.h"
#include "FrontierX/NativeRenderPrimitive.h"
#include "VideoSystem.h"
#endif
#if defined(FRONTIERX_HAVE_IAC_DECODER)
#include "NativeIacWave.h"
#endif
#include <filesystem>
#include <fstream>
#include "FrontierX/LegacyInput.h"
#include "IConnection.h"
#include "SYSTEM.H"
#include "WindowManager.h"

#include <SDL3/SDL.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <array>
#include <map>
#include <algorithm>

namespace
{
struct MessageRecord
{
    HWND window;
    UINT message;
    WPARAM wparam;
    LPARAM lparam;
};

std::vector<MessageRecord> messages;
int exit_calls = 0;
IStreamer * closeOnInit = nullptr;

void DACOM_CDECL ExitHandler(int code)
{
    if (code == 0) ++exit_calls;
}

LRESULT COMAPI WindowCallback(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    messages.push_back({window, message, wparam, lparam});
    if (message == 0x500 && wparam == IStreamer::INITSUCCESS && closeOnInit)
        closeOnInit->CloseHandle(reinterpret_cast<HSTREAM>(lparam));
    return 0;
}

void Require(bool condition, const char * message)
{
    if (!condition) throw std::runtime_error(message);
}

void Push(SDL_Event & event)
{
    Require(SDL_PushEvent(&event), SDL_GetError());
}

// IStreamer blocks the source profile; SoundManager depends on deferred init and
// completion via this window callback. Extend this owner/pump check, not a mock.
void CheckStreamer(FrontierX::NativeComponentManager & manager, IWindowManager * window)
{
    Require(FrontierX::RegisterNativeStreamer(&manager), "streamer registration failed");
    AGGDESC descriptor("IStreamer", "Streamer");
    void * raw = nullptr;
    Require(manager.CreateInstance(&descriptor, &raw) == GR_OK, "streamer creation failed");
    auto * aggregate = static_cast<IAggregateComponent *>(raw);
    IStreamer * streamer = nullptr;
    Require(aggregate->QueryInterface(IID_IStreamer, reinterpret_cast<void **>(&streamer)) == GR_OK,
            "streamer query failed");
    Require(aggregate->Initialize() == GR_OK, "streamer aggregate init failed");
    STREAMERDESC init;
    init.hMainWindow = window->GetWindowHandle();
    init.uMsg = 0x500;
    Require(!streamer->Open("missing", nullptr, 0), "uninitialized stream accepted");
    STREAMERDESC invalid = init;
    invalid.hMainWindow = static_cast<HWND>(-1);
    Require(!streamer->Init(&invalid), "invalid notification window accepted");
    if (!streamer->Init(&init)) throw std::runtime_error(SDL_GetError());
    Require(!streamer->Init(&init), "duplicate initialization accepted");
    std::cout << "SDL audio driver: " << SDL_GetCurrentAudioDriver() << '\n';
    const auto root = std::filesystem::current_path() / "native_streamer_fixture";
    std::filesystem::create_directories(root);
    std::vector<unsigned char> wav(44 + 1600, 0);
    std::memcpy(wav.data(), "RIFF", 4);
    std::memcpy(wav.data() + 8, "WAVEfmt ", 8);
    auto put = [&](std::size_t offset, unsigned value, unsigned width) {
        for (unsigned i = 0; i < width; ++i) wav[offset + i] = static_cast<unsigned char>(value >> (8 * i));
    };
    put(4, static_cast<unsigned>(wav.size() - 8), 4);
    put(16, 16, 4); put(20, 1, 2); put(22, 1, 2); put(24, 8000, 4);
    put(28, 16000, 4); put(32, 2, 2); put(34, 16, 2);
    std::memcpy(wav.data() + 36, "data", 4); put(40, 1600, 4);
    for (std::size_t i = 44; i < wav.size(); i += 2) put(i, (i / 16) % 2 ? 1000 : 64536, 2);
    std::ofstream file(root / "tone.wav", std::ios::binary);
    file.write(reinterpret_cast<const char *>(wav.data()), static_cast<std::streamsize>(wav.size()));
    file.close();
    std::ofstream(root / "invalid.wav") << "not a wave";
    IFileSystem * source = nullptr;
    std::string error;
    Require(FrontierX::OpenNativeDirectoryFileSystem(root, &source, error), error.c_str());
    Require(!streamer->Open("missing.wav", source, 0) &&
            !streamer->Open("invalid.wav", source, 0), "bad audio accepted");
    const auto before = messages.size();
    HSTREAM stream = streamer->Open("TONE.WAV", source, 0);
    Require(stream && streamer->GetStatus(stream) == IStreamer::STOPPED && messages.size() == before,
            "Open must defer initialization notification");
    window->ServeMessageQueue(WMF_BACKGROUND);
    Require(messages.back().message == 0x500 && messages.back().wparam == IStreamer::INITSUCCESS &&
            messages.back().lparam == reinterpret_cast<LPARAM>(stream), "init message mismatch");
    S32 volume = 0;
    Require(streamer->SetVolume(stream, -2000) && streamer->GetVolume(stream, &volume) && volume == -2000 &&
            !streamer->SetVolume(stream, 1), "volume contract failed");
    Require(streamer->Restart(stream) && streamer->Stop(stream), "pause failed");
    SDL_Delay(150);
    window->ServeMessageQueue(WMF_BACKGROUND);
    Require(streamer->GetStatus(stream) == IStreamer::STOPPED, "paused stream completed");
    Require(streamer->Restart(stream), "resume failed");
    const Uint64 deadline = SDL_GetTicks() + 3000;
    while (streamer->GetStatus(stream) != IStreamer::COMPLETED && SDL_GetTicks() < deadline)
    { window->ServeMessageQueue(WMF_BACKGROUND); SDL_Delay(5); }
    Require(streamer->GetStatus(stream) == IStreamer::COMPLETED &&
            messages.back().wparam == IStreamer::COMPLETED && !streamer->Restart(stream),
            "playback did not complete or replay was accepted");
    std::vector<WPARAM> states;
    for (const auto & record : messages)
        if (record.message == 0x500 && record.lparam == reinterpret_cast<LPARAM>(stream))
            states.push_back(record.wparam);
    Require(states == std::vector<WPARAM>{IStreamer::INITSUCCESS, IStreamer::EOFREACHED, IStreamer::COMPLETED},
            "stream notification order or multiplicity changed");
    Require(streamer->CloseHandle(stream) && !streamer->CloseHandle(stream) &&
            streamer->GetStatus(stream) == IStreamer::INVALID, "stale handle accepted");
    HSTREAM loop = streamer->Open("tone.wav", source, STRMFL_LOOPING | STRMFL_PLAY);
    Require(loop && loop != stream, "loop handle was reused");
    const Uint64 loopUntil = SDL_GetTicks() + 350;
    while (SDL_GetTicks() < loopUntil) { window->ServeMessageQueue(WMF_BACKGROUND); SDL_Delay(5); }
    Require(streamer->GetStatus(loop) == IStreamer::PLAYING && streamer->CloseHandle(loop),
            "loop prematurely completed");
    HSTREAM cancelled = streamer->Open("tone.wav", source, STRMFL_PLAY);
    Require(cancelled != nullptr, "cancel test open failed");
    closeOnInit = streamer;
    window->ServeMessageQueue(WMF_BACKGROUND);
    closeOnInit = nullptr;
    Require(streamer->GetStatus(cancelled) == IStreamer::INVALID,
            "notification callback close did not invalidate stream");
    wav[20] = 2; wav[21] = 4;
    wav.resize(40); // Truncated IAC must fail even with an available decoder.
    std::ofstream unsupported(root / "iac.wav", std::ios::binary);
    unsupported.write(reinterpret_cast<const char *>(wav.data()), static_cast<std::streamsize>(wav.size()));
    unsupported.close();
    Require(!streamer->Open("iac.wav", source, 0), "truncated IAC falsely succeeded");
#if defined(FRONTIERX_HAVE_IAC_DECODER)
    SDL_AudioSpec rejectedSpec{SDL_AUDIO_U8, 2, 1234};
    std::vector<Uint8> rejectedSamples{1, 2, 3};
    Require(!FrontierX::DecodeNativeIacWave(wav, rejectedSpec, rejectedSamples) &&
            rejectedSamples == std::vector<Uint8>{1, 2, 3} && rejectedSpec.freq == 1234,
            "failed IAC decode mutated caller output");
#endif
    if (const char * archive = std::getenv("FRONTIERX_STREAMER_ARCHIVE"))
    {
        IFileSystem * external = nullptr;
        Require(FrontierX::OpenNativeUtfFileSystem(archive, &external, error), error.c_str());
        const char * entry = std::getenv("FRONTIERX_STREAMER_ENTRY");
        Require(entry && *entry, "external audio requires an explicit entry");
        Uint64 durationMs = 0;
#if defined(FRONTIERX_HAVE_IAC_DECODER)
        WIN32_FIND_DATA found{};
        HANDLE search = external->FindFirstFile(entry, &found);
        Require(search != INVALID_HANDLE_VALUE && external->FindClose(search), "external entry lookup failed");
        DAFILEDESC waveEntry(found.cFileName);
        HANDLE input = external->OpenChild(&waveEntry);
        Require(input != INVALID_HANDLE_VALUE, "external entry open failed");
        const DWORD size = external->GetFileSize(input);
        Require(size <= 64U * 1024U * 1024U, "external wave too large");
        std::vector<Uint8> compressed(size);
        DWORD read = 0;
        Require(external->ReadFile(input, compressed.data(), size, &read, nullptr) && read == size &&
                external->CloseHandle(input), "external wave read failed");
        SDL_AudioSpec decodedSpec{};
        std::vector<Uint8> decodedSamples;
        Require(FrontierX::DecodeNativeIacWave(compressed, decodedSpec, decodedSamples), "IAC decode failed");
        durationMs = 1000ULL * decodedSamples.size() /
            (sizeof(float) * static_cast<Uint64>(decodedSpec.channels) *
             static_cast<Uint64>(decodedSpec.freq));
        std::uint64_t hash = 14695981039346656037ULL;
        for (Uint8 byte : decodedSamples) { hash ^= byte; hash *= 1099511628211ULL; }
        std::cout << "IAC PCM: frequency " << decodedSpec.freq << "; channels " << decodedSpec.channels
                  << "; bytes " << decodedSamples.size() << "; FNV1a64 " << hash << '\n';
#endif
        const Uint64 speechStart = SDL_GetTicks();
        HSTREAM speech = streamer->Open(entry, external, STRMFL_PLAY);
        external->Release();
        Require(speech != nullptr, "external WAV failed to decode/open");
        if (durationMs > 45000)
        {
            const Uint64 limit = SDL_GetTicks() + 150;
            while (SDL_GetTicks() < limit)
            { window->ServeMessageQueue(WMF_BACKGROUND); SDL_Delay(5); }
            Require(streamer->GetStatus(speech) == IStreamer::PLAYING &&
                    streamer->Stop(speech) && streamer->CloseHandle(speech),
                    "external long WAV did not play and stop");
            std::cout << "External long WAV played and stopped through native IStreamer; elapsed-ms "
                      << SDL_GetTicks() - speechStart << '\n';
        }
        else
        {
            const Uint64 limit = SDL_GetTicks() + 45000;
            while (streamer->GetStatus(speech) != IStreamer::COMPLETED && SDL_GetTicks() < limit)
            { window->ServeMessageQueue(WMF_BACKGROUND); SDL_Delay(5); }
            Require(streamer->GetStatus(speech) == IStreamer::COMPLETED && streamer->CloseHandle(speech),
                    "external WAV did not complete");
            std::cout << "External WAV playback completed through native IStreamer; elapsed-ms "
                      << SDL_GetTicks() - speechStart << '\n';
        }
    }
    source->Release();
    streamer->Release();
    aggregate->Release();
    messages.clear();
    std::cout << "Native IStreamer: decoded WAV, deferred init, pause/resume, completion, loop and stale handles passed\n";
}

struct CloseCallback final : ISystemEventCallback
{
    GENRESULT QueryInterface(const C8 * interface_name, void ** instance) override
    {
        if (!instance) return GR_INVALID_PARMS;
        *instance = nullptr;
        if (interface_name && std::strcmp(interface_name, "ISystemEventCallback") == 0)
        {
            *instance = this;
            AddRef();
            return GR_OK;
        }
        return GR_INTERFACE_UNSUPPORTED;
    }

    U32 AddRef() override { return ++references; }
    U32 Release() override { return references ? --references : 0; }
    BOOL32 OnAppClose() override
    {
        ++calls;
        return veto ? TRUE : FALSE;
    }

    U32 references = 0;
    U32 calls = 0;
    bool veto = true;
};

#if defined(FRONTIERX_HAVE_VIDEO_DECODER)
#include "SyntheticVideoFixture.inc"
std::vector<std::uint8_t> DecodeSyntheticBase64(const char *input, std::size_t expected)
{
    constexpr char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::vector<std::uint8_t> bytes;
    U32 value = 0U; unsigned bits = 0U;
    for (const char *cursor = input; *cursor; ++cursor)
    {
        if (*cursor == '\n' || *cursor == '\r') continue;
        if (*cursor == '=') break;
        const char *digit = std::strchr(alphabet, *cursor);
        Require(digit != nullptr, "synthetic movie base64 invalid");
        value = (value << 6U) | U32(digit - alphabet); bits += 6U;
        if (bits >= 8U) { bits -= 8U; bytes.push_back(std::uint8_t((value >> bits) & 255U)); }
    }
    Require(bytes.size() == expected, "synthetic movie extent invalid");
    return bytes;
}
struct VideoTexture
{
    U32 width, height;
    std::vector<std::uint8_t> pixels;
};
struct VideoTextures
{
    U32 next = 1U;
    unsigned uploads = 0U, creates = 0U, destroys = 0U;
    std::map<U32, VideoTexture> textures;
    std::array<U32, 2> bindings{};
};
bool VideoTextureExists(void *context, U32 handle)
{ return static_cast<VideoTextures *>(context)->textures.count(handle) == 1U; }
GENRESULT CreateVideoTexture(void *context, U32 width, U32 height, U32 &handle)
{
    if (width != 16U || height != 16U) return GR_INVALID_PARMS;
    auto &owner = *static_cast<VideoTextures *>(context);
    handle = owner.next++;
    owner.textures.emplace(handle, VideoTexture{width, height, std::vector<std::uint8_t>(width * height * 4U)});
    ++owner.creates; return GR_OK;
}
GENRESULT UploadVideoTexture(void *context, U32 handle, const std::uint8_t *pixels,
                             U32 width, U32 height, std::size_t bytes)
{
    auto &owner = *static_cast<VideoTextures *>(context);
    const auto found = owner.textures.find(handle);
    if (found == owner.textures.end() || !pixels || found->second.width != width ||
        found->second.height != height || found->second.pixels.size() != bytes) return GR_INVALID_PARMS;
    std::copy_n(pixels, bytes, found->second.pixels.begin()); ++owner.uploads; return GR_OK;
}
GENRESULT DestroyVideoTexture(void *context, U32 handle)
{
    auto &owner = *static_cast<VideoTextures *>(context);
    if (std::find(owner.bindings.begin(), owner.bindings.end(), handle) != owner.bindings.end())
        return GR_INVALID_PARMS;
    if (owner.textures.erase(handle) != 1U) return GR_INVALID_PARMS;
    ++owner.destroys; return GR_OK;
}
GENRESULT DACOM_CDECL BindVideoTexture(void *context, U32 stage, U32 texture)
{
    auto &owner = *static_cast<VideoTextures *>(context);
    if (stage >= owner.bindings.size() || (texture && owner.textures.count(texture) != 1U)) return GR_INVALID_PARMS;
    owner.bindings[stage] = texture; return GR_OK;
}
GENRESULT DACOM_CDECL RejectVideoGeometry(void *, const FrontierX::RenderSubmissionView &)
{ return GR_NOT_IMPLEMENTED; }
void CheckVideo(FrontierX::NativeComponentManager &manager)
{
    // MovieScreen uses this real factory and legacy interface. Decoder-owned
    // textures/audio/timing must execute; registration alone cannot prove them.
    Require(FrontierX::RegisterNativeVideoSystem(&manager), "video registration failed");
    AGGDESC descriptor("IVideoSystem", "VideoSystem");
    void *raw = nullptr;
    Require(manager.CreateInstance(&descriptor, &raw) == GR_OK && raw, "video aggregate creation failed");
    auto *aggregate = static_cast<IAggregateComponent *>(raw);
    IVideoSystem *video = nullptr;
    Require(aggregate->QueryInterface(IID_IVideoSystem, reinterpret_cast<void **>(&video)) == GR_OK && video,
            "video legacy interface query failed");
    Require(aggregate->Initialize() == GR_OK, "video genuine decoder/license initialization failed");
    struct PrivateDirectory
    {
        std::filesystem::path path;
        ~PrivateDirectory() { std::error_code ec; std::filesystem::remove_all(path, ec); if (ec) std::abort(); }
    } privateDirectory;
    for (unsigned attempt = 0;; ++attempt)
    {
        privateDirectory.path = std::filesystem::temp_directory_path() /
            ("frontierx-native-video-" + std::to_string(SDL_GetTicksNS()) + '-' + std::to_string(attempt));
        if (std::filesystem::create_directory(privateDirectory.path)) break;
    }
    std::filesystem::permissions(privateDirectory.path, std::filesystem::perms::owner_all,
                                std::filesystem::perm_options::replace);
    const auto write = [&](const char *name, const std::vector<std::uint8_t> &bytes) {
        std::ofstream file(privateDirectory.path / name, std::ios::binary);
        file.write(reinterpret_cast<const char *>(bytes.data()), std::streamsize(bytes.size()));
        Require(bool(file), "synthetic movie write failed");
    };
    const auto movie = DecodeSyntheticBase64(kSyntheticMovieBase64, kSyntheticMovieBytes);
    write("movie.mpg", movie);
    write("video-only.mpg", DecodeSyntheticBase64(kSyntheticVideoOnlyBase64, kSyntheticVideoOnlyBytes));
    write("audio-only.mpg", DecodeSyntheticBase64(kSyntheticAudioOnlyBase64, kSyntheticAudioOnlyBytes));
    write("truncated.mpg", std::vector<std::uint8_t>(movie.begin(), movie.begin() + 16));
    write("invalid.mpg", {1U, 2U, 3U, 4U});
    IFileSystem *directory = nullptr;
    std::string error;
    Require(FrontierX::OpenNativeDirectoryFileSystem(privateDirectory.path, &directory, error), error.c_str());
    VideoTextures textures;
    FrontierX::NativeTextureOperations operations{};
    operations.context = &textures; operations.lookup = VideoTextureExists;
    operations.create = CreateVideoTexture; operations.upload = UploadVideoTexture; operations.destroy = DestroyVideoTexture;
    FrontierX::RegisterNativeRenderPrimitive(&manager);
    FrontierX::NativeRenderPrimitiveDesc primitiveDescriptor;
    primitiveDescriptor.callback = RejectVideoGeometry;
    primitiveDescriptor.context = &textures; primitiveDescriptor.textureBind = BindVideoTexture;
    void *primitiveRaw = nullptr;
    Require(manager.CreateInstance(&primitiveDescriptor, &primitiveRaw) == GR_OK && primitiveRaw,
            "video genuine native binding owner creation failed");
    auto *primitive = static_cast<IRenderPrimitive *>(primitiveRaw);
    auto *pipeline = new FrontierX::NativeRenderPipeline(primitive, operations);
    Require(video->Initialize(directory, pipeline) == GR_OK, "video actual filesystem/pipeline binding failed");
    Require(video->Initialize(directory, pipeline) != GR_OK, "video duplicate owner binding accepted");
    for (const char *name : {"missing.mpg", "invalid.mpg", "truncated.mpg", "video-only.mpg", "audio-only.mpg"})
        Require(!video->LoadVideoMovie(name, false) && textures.textures.empty(),
                "missing/corrupt/truncated or missing-stream movie succeeded or retained texture");
    const VIDEO_HANDLE handle = video->LoadVideoMovie("movie.mpg", false);
    Require(handle && !video->IsPlaying(handle) && video->GetNumFrames(handle) == 25U &&
            video->GetVideoWidth(handle) == 16U && video->GetVideoHeight(handle) == 16U,
            "video actual decoded dimensions/frame count invalid");
    FrontierX::NativeVideoDiagnostics diagnostics{};
    Require(FrontierX::GetNativeVideoDiagnostics(video, handle, diagnostics) &&
            diagnostics.sampleRate == 22050U && diagnostics.channels == 1U &&
            diagnostics.decodedFrames == 25U && diagnostics.decodedAudioSamples >= 22050U &&
            diagnostics.decodedAudioSamples <= 24255U && diagnostics.peak >= 0.10F && diagnostics.peak <= 0.16F &&
            diagnostics.rms >= 0.06F && diagnostics.rms <= 0.11F,
            "video genuine MP2 tone sample count/rate/channels/amplitude invalid");
    const U32 texture = video->GetVideoTexture(handle);
    Require(texture && pipeline->is_texture(texture) == GR_OK && textures.textures.count(texture) == 1U,
            "video actual decoded texture handle invalid");
    float u = 0.0F, v = 0.0F;
    video->GetVideoTexureCoords(handle, u, v);
    Require(u == 1.0F && v == 1.0F, "video authored texture extent invalid");
    const auto pixels = [&]() { return textures.textures.at(texture).pixels; };
    const auto color = [&](const std::vector<std::uint8_t> &rgba, bool red) {
        Require(rgba.size() == 16U * 16U * 4U, "video decoded pixel extent invalid");
        for (std::size_t index = 0; index < rgba.size(); index += 4U)
            Require(rgba[index + (red ? 0U : 2U)] >= 220U &&
                    rgba[index + (red ? 2U : 0U)] <= 30U && rgba[index + 1U] <= 30U && rgba[index + 3U] == 255U,
                    "video actual MPEG/YUV color or opaque alpha invalid");
    };
    const auto first = pixels(); color(first, true);
    video->SetVideoFrame(handle, 2U);
    const auto second = pixels(); color(second, false);
    Require(first != second, "one-based first/second video seeks did not select distinct actual frames");
    video->SetVideoFrame(handle, 1U);
    Require(pixels() == first, "first decoded video frame failed exact seek restoration");
    bool rejectedSeek = false;
    try { video->SetVideoFrame(handle, 0U); } catch (const std::runtime_error &) { rejectedSeek = true; }
    Require(rejectedSeek && pixels() == first, "invalid seek changed current video frame or succeeded");
    video->StartVideoMovie(handle);
    Require(video->IsPlaying(handle), "video actual playback did not start");
    const unsigned before = textures.uploads;
    const Uint64 advanceLimit = SDL_GetTicks() + 1500U;
    while (textures.uploads == before && SDL_GetTicks() < advanceLimit)
    { SDL_Delay(10); video->UpdateVideoFrame(handle); }
    Require(textures.uploads > before, "video real audio-master clock did not advance decoded frame uploads");
    video->StopVideoMovie(handle);
    const unsigned paused = textures.uploads;
    const auto pausedPixels = pixels();
    SDL_Delay(100); video->UpdateVideoFrame(handle);
    Require(!video->IsPlaying(handle) && paused == textures.uploads && pausedPixels == pixels(),
            "video pause did not preserve real frame output");
    video->SetVideoFrame(handle, 24U); color(pixels(), false);
    video->StartVideoMovie(handle);
    const Uint64 eofLimit = SDL_GetTicks() + 2500U;
    while (video->IsPlaying(handle) && SDL_GetTicks() < eofLimit)
    { SDL_Delay(10); video->UpdateVideoFrame(handle); }
    Require(!video->IsPlaying(handle), "video EOF/audio drain failed to stop playback");
    Require(pipeline->set_texture_stage_texture(0U, texture) == GR_OK &&
            pipeline->set_texture_stage_texture(1U, texture) == GR_OK,
            "video owner binding fixture requires actual both-stage bindings");
    video->ReleaseVideoMovie(handle);
    Require(textures.textures.empty() && textures.bindings == std::array<U32,2>{0U,0U} && pipeline->is_texture(texture) != GR_OK,
            "video release retained actual texture ownership");
    bool stale = false;
    try { (void)video->GetVideoWidth(handle); } catch (const std::runtime_error &failure)
    { stale = std::strcmp(failure.what(), "native video handle invalid/stale") == 0; }
    Require(stale, "released video handle accepted or returned placeholder metadata");
    const auto previousDiagnostics = diagnostics;
    Require(!FrontierX::GetNativeVideoDiagnostics(video, handle, diagnostics) &&
            diagnostics.decodedAudioSamples == previousDiagnostics.decodedAudioSamples &&
            diagnostics.sampleRate == previousDiagnostics.sampleRate && diagnostics.channels == previousDiagnostics.channels &&
            diagnostics.decodedFrames == previousDiagnostics.decodedFrames && diagnostics.peak == previousDiagnostics.peak &&
            diagnostics.rms == previousDiagnostics.rms,
            "stale video diagnostics changed output or returned placeholder success");
    const VIDEO_HANDLE loop = video->LoadVideoMovie("movie.mpg", true);
    Require(loop && loop != handle, "released video handle reused");
    video->SetVideoFrame(loop, 25U);
    const U32 loopTexture = video->GetVideoTexture(loop);
    color(textures.textures.at(loopTexture).pixels, true);
    const unsigned loopBefore = textures.uploads;
    bool loopReachedBlueFrame = false;
    video->StartVideoMovie(loop);
    const Uint64 loopLimit = SDL_GetTicks() + 1800U;
    while ((textures.uploads < loopBefore + 3U || !loopReachedBlueFrame) && SDL_GetTicks() < loopLimit)
    {
        SDL_Delay(10); video->UpdateVideoFrame(loop);
        const auto &loopPixels = textures.textures.at(loopTexture).pixels;
        if (loopPixels[2] >= 220U && loopPixels[0] <= 30U) loopReachedBlueFrame = true;
    }
    Require(video->IsPlaying(loop) && textures.uploads >= loopBefore + 3U && loopReachedBlueFrame,
            "video loop failed genuine rewind/continued frame uploads");
    U32 otherTexture = 0U;
    Require(pipeline->create_texture(16, 16, PixelFormat(PF_RGBA8), 1, 0, otherTexture) == GR_OK && otherTexture &&
            pipeline->set_texture_stage_texture(0U, video->GetVideoTexture(loop)) == GR_OK &&
            pipeline->set_texture_stage_texture(1U, otherTexture) == GR_OK,
            "video owner binding fixture requires actual independent nonmovie texture");
    video->Shutdown();
    Require(textures.bindings[0] == 0U && textures.bindings[1] == otherTexture &&
            pipeline->is_texture(otherTexture) == GR_OK,
            "video shutdown changed unrelated binding or retained its own texture");
    Require(pipeline->set_texture_stage_texture(1U, 0U) == GR_OK && pipeline->destroy_texture(otherTexture) == GR_OK,
            "video fixture failed independent texture release");
    Require(textures.textures.empty() && textures.creates == textures.destroys,
            "video shutdown retained decoded texture owners");
    stale = false;
    try { video->StartVideoMovie(loop); } catch (const std::runtime_error &) { stale = true; }
    Require(stale, "video shutdown retained live movie handles");
    directory->Release(); pipeline->Release(); primitive->Release(); video->Release(); aggregate->Release();
    std::cout << "Native IVideoSystem: real MPEG1/MP2 decode, frame pixels, exact seek, clock, pause, EOF, loop and owner rejection checked\n";
}
#endif

void CheckControlledInput()
{
    FrontierX::NativeComponentManager manager;
    FrontierX::RegisterNativeWindowManager(&manager);
    setenv("FRONTIERX_M3_CONTROLLED_INPUT", "1", 1);
    setenv("FRONTIERX_M3_TRACE", "generated-test-access", 1);
    AGGDESC descriptor("IWindowManager", "WindowManager");
    void *raw = nullptr;
    Require(manager.CreateInstance(&descriptor, &raw) == GR_OK && raw,
            "controlled input component creation failed");
    unsetenv("FRONTIERX_M3_CONTROLLED_INPUT");
    unsetenv("FRONTIERX_M3_TRACE");
    auto *aggregate = static_cast<IAggregateComponent *>(raw);
    IWindowManager *window = nullptr;
    Require(aggregate->QueryInterface(IID_IWindowManager,
            reinterpret_cast<void **>(&window)) == GR_OK && window,
            "controlled input window query failed");
    Require(window->Startup(nullptr, "controlled input", ExitHandler,
            WMF_ALLOW_WINDOW_RESIZE), "controlled input window startup failed");
    Require(window->SetWindowPos(400, 300, WMF_ALLOW_WINDOW_RESIZE),
            "controlled input window dimensions failed");
    window->SetCallback(WindowCallback);
    window->ServeMessageQueue(WMF_BACKGROUND);
    messages.clear();
    const auto id = static_cast<Uint32>(window->GetWindowHandle());
    SDL_Event event{};
    event.window.type = SDL_EVENT_WINDOW_FOCUS_LOST;
    event.window.windowID = id;
    Push(event);
    event = {};
    event.motion.type = SDL_EVENT_MOUSE_MOTION;
    event.motion.windowID = id;
    event.motion.x = 1; event.motion.y = 1;
    Push(event);
    event = {};
    event.key.type = SDL_EVENT_KEY_DOWN;
    event.key.windowID = id;
    event.key.key = SDLK_ESCAPE;
    Push(event);
    window->ServeMessageQueue(WMF_BACKGROUND);
    S32 x = 0, y = 0;
    window->GetCursorPos(x, y);
    Require(window->AppIsActive() && x == 200 && y == 150 && messages.empty(),
            "host focus/key/motion contaminated the explicit input fixture");
    // Completion callbacks must still reach the real source window pump.
    CheckStreamer(manager, window);
    messages.clear();
    event = {};
    event.window.type = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
    event.window.windowID = id;
    Push(event);
    window->ServeMessageQueue(0);
    Require(messages.size() == 1 && messages.front().message == FrontierX::LegacyMessage::Close,
            "controlled input swallowed an external close instead of failing the run");
    window->Release();
    aggregate->Release();
    Require(manager.ShutDown() == GR_OK, "controlled input component shutdown failed");
}
}

int main()
{
    try
    {
        FrontierX::NativeComponentManager manager;
        FrontierX::RegisterNativeWindowManager(&manager);
        Require(manager.RegisteredCount() == 1,
            "native window-manager factory registration failed");

        AGGDESC descriptor("IWindowManager", "WindowManager");
        void * raw_aggregate = nullptr;
        Require(manager.CreateInstance(&descriptor, &raw_aggregate) == GR_OK && raw_aggregate,
            "native window-manager aggregate creation failed");
        auto * aggregate = static_cast<IAggregateComponent *>(raw_aggregate);

        IWindowManager * window = nullptr;
        ISystemComponent * system = nullptr;
        Require(aggregate->QueryInterface(IID_IWindowManager,
            reinterpret_cast<void **>(&window)) == GR_OK && window,
            "IWindowManager query failed");
        Require(aggregate->QueryInterface(IID_ISystemComponent,
            reinterpret_cast<void **>(&system)) == GR_OK && system,
            "native window ISystemComponent query failed");
        Require(aggregate->Initialize() == GR_OK, "native window aggregate initialization failed");
        Require(window->Startup(nullptr, "FrontierX native window probe", ExitHandler,
            WMF_PREVENT_MULTIPLE, nullptr) == 1, "native window startup failed");
        Require(window->GetWindowHandle() != 0, "native window token is zero");
        Require(FrontierX::SetNativeWindowMinimumSize(window,400,300) &&
            !FrontierX::SetNativeWindowMinimumSize(window,0,300),"native minimum client bounds failed");
        Require(window->SetWindowPos(400, 300,
            WMF_ALLOW_WINDOW_RESIZE | WMF_CENTER) == TRUE,
            "native window configuration failed");

        WM_WINAREA client{};
        window->GetClientArea(client);
        Require(client.w == 400 && client.h == 300,
            "native client-area dimensions changed");
        FrontierX::NativeWindowMode mode;
        Require(FrontierX::GetNativeWindowMode(window,mode) && !mode.fullscreen &&
            mode.width==400 && mode.height==300,"native windowed mode query failed");
        mode.fullscreen=true;
        Require(FrontierX::SetNativeWindowMode(window,mode) &&
            FrontierX::GetNativeWindowMode(window,mode) && mode.fullscreen &&
            mode.width==400 && mode.height==300,"native fullscreen lost restored window size");
        mode.fullscreen=false;
        Require(FrontierX::SetNativeWindowMode(window,mode),"native windowed restoration failed");
        window->GetClientArea(client);
        Require(client.w==400 && client.h==300,"native restored client size changed");
        window->SetCursorPos(17, 23);
        S32 cursor_x = 0;
        S32 cursor_y = 0;
        window->GetCursorPos(cursor_x, cursor_y);
        Require(cursor_x == 17 && cursor_y == 23,
            "native cursor cache did not round trip");

        void * second_raw_aggregate = nullptr;
        Require(manager.CreateInstance(&descriptor, &second_raw_aggregate) == GR_OK &&
            second_raw_aggregate, "second native window aggregate creation failed");
        auto * second_aggregate = static_cast<IAggregateComponent *>(second_raw_aggregate);
        IWindowManager * second_window = nullptr;
        Require(second_aggregate->QueryInterface(IID_IWindowManager,
            reinterpret_cast<void **>(&second_window)) == GR_OK && second_window,
            "second IWindowManager query failed");
        Require(second_window->Startup(nullptr, "duplicate", ExitHandler,
            WMF_PREVENT_MULTIPLE, nullptr) == -1,
            "prevent-multiple startup contract changed");
        second_window->Release();
        second_aggregate->Release();

        IDAConnectionPoint * close_point = nullptr;
        Require(window->QueryOutgoingInterface("ISystemEventCallback", &close_point) == GR_OK &&
            close_point, "close callback connection-point query failed");
        CloseCallback close_callback;
        U32 close_token = 0;
        Require(close_point->Advise(&close_callback, &close_token) == GR_OK && close_token == 1,
            "close callback subscription failed");

        window->SetCallback(WindowCallback);
        SDL_FlushEvents(SDL_EVENT_FIRST, SDL_EVENT_LAST);
        const auto window_id = static_cast<SDL_WindowID>(window->GetWindowHandle());
        SDL_Event event{};
        event.window.type = SDL_EVENT_WINDOW_FOCUS_LOST;
        event.window.windowID = window_id;
        Push(event);
        event = {};
        event.motion.type = SDL_EVENT_MOUSE_MOTION;
        event.motion.windowID = window_id;
        event.motion.x = 31.0F;
        event.motion.y = 41.0F;
        Push(event);
        system->Update();
        Require(window->AppIsActive() == FALSE,
            "focus-lost event did not update native window state");
        window->GetCursorPos(cursor_x, cursor_y);
        Require(cursor_x == 31 && cursor_y == 41,
            "mouse motion did not update native cursor state");
        Require(messages.size() == 3 &&
            messages[0].message == FrontierX::LegacyMessage::KillFocus &&
            messages[1].message == FrontierX::LegacyMessage::ActivateApp &&
            messages[2].message == FrontierX::LegacyMessage::MouseMove,
            "native window callback dispatch order changed");

        event = {};
        event.window.type = SDL_EVENT_WINDOW_FOCUS_GAINED;
        event.window.windowID = window_id;
        Push(event);
        window->ServeMessageQueue(WMF_WAIT | WMF_BACKGROUND);
        Require(window->AppIsActive() == TRUE,
            "focus-gained event did not update native window state");

        const std::size_t before_close = messages.size();
        event = {};
        event.window.type = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
        event.window.windowID = window_id;
        Push(event);
        window->ServeMessageQueue(WMF_BACKGROUND);
        Require(close_callback.calls == 1 && messages.size() == before_close,
            "close veto did not suppress the window callback");

        // A normal background-window close must return to the modal owner,
        // without waiting for or consuming a later focus event.
        event.window.type = SDL_EVENT_WINDOW_FOCUS_LOST;
        Push(event);
        window->ServeMessageQueue(WMF_BACKGROUND);
        Require(window->AppIsActive() == FALSE, "close countercheck did not lose focus");
        const std::size_t inactive_before_close = messages.size();
        close_callback.veto = false;
        event.window.type = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
        Push(event);
        event.window.type = SDL_EVENT_WINDOW_FOCUS_GAINED;
        Push(event);
        window->ServeMessageQueue(0);
        Require(window->AppIsActive() == FALSE,
            "inactive accepted close waited for a later focus event");
        window->ServeMessageQueue(0);
        Require(window->AppIsActive() == FALSE,
            "a second pump waited for focus after accepted close");
        window->ServeMessageQueue(WMF_BACKGROUND);
        Require(window->AppIsActive() == TRUE, "queued focus event was lost");
        Require(close_callback.calls == 2 &&
            messages[inactive_before_close].message == FrontierX::LegacyMessage::Close,
            "accepted close did not reach the window callback");

        event = {};
        event.quit.type = SDL_EVENT_QUIT;
        Push(event);
        window->ServeMessageQueue(WMF_BACKGROUND);
        Require(exit_calls == 1, "quit event did not invoke the configured exit callback");

        CheckStreamer(manager, window);
#if defined(FRONTIERX_HAVE_VIDEO_DECODER)
        CheckVideo(manager);
#endif

        Require(close_point->Unadvise(close_token) == GR_OK,
            "close callback unsubscribe failed");
        close_point->Release();
        system->Release();
        window->Release();
        aggregate->Release();
        Require(manager.ShutDown() == GR_OK && manager.RegisteredCount() == 0,
            "native window manager shutdown failed");
        CheckControlledInput();

        std::cout << "native SDL IWindowManager component passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception & exception)
    {
        std::cerr << "native window-manager probe failed: " << exception.what() << '\n';
        return EXIT_FAILURE;
    }
}
