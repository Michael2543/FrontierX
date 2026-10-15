#include "FrontierX/NativeVideoSystem.h"
#include "SYSTEM.H"
#include "TComponent.h"
#include "Filesys.h"
#include "VideoSystem.h"
#include "rendpipeline.h"
#include <SDL3/SDL.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libswscale/swscale.h>
}
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

namespace FrontierX {
namespace {
constexpr std::size_t MaxMovieBytes = 256U * 1024U * 1024U;
constexpr std::size_t MaxAudioSamples = 128U * 1024U * 1024U;
char interfaceName[] = "IVideoSystem";
struct VideoError : std::runtime_error { using std::runtime_error::runtime_error; };
void Require(bool condition, const char *message) { if (!condition) throw VideoError(message); }
struct Reader {
    const std::vector<Uint8> &bytes;
    std::size_t position = 0;
};
int Read(void *opaque, uint8_t *out, int count) {
    auto &r = *static_cast<Reader *>(opaque);
    if (count <= 0) return AVERROR(EINVAL);
    const auto size = std::min(std::size_t(count), r.bytes.size() - r.position);
    if (!size) return AVERROR_EOF;
    std::memcpy(out, r.bytes.data() + r.position, size); r.position += size;
    return int(size);
}
int64_t Seek(void *opaque, int64_t offset, int origin) {
    auto &r = *static_cast<Reader *>(opaque);
    if (origin == AVSEEK_SIZE) return int64_t(r.bytes.size());
    origin &= ~AVSEEK_FORCE;
    const int64_t base = origin == SEEK_CUR ? int64_t(r.position) :
        origin == SEEK_END ? int64_t(r.bytes.size()) : 0;
    if ((origin != SEEK_SET && origin != SEEK_CUR && origin != SEEK_END) ||
        offset < -base || offset > int64_t(r.bytes.size()) - base) return AVERROR(EINVAL);
    r.position = std::size_t(base + offset); return base + offset;
}
struct Decoder {
    Reader reader;
    AVIOContext *io = nullptr;
    AVFormatContext *format = nullptr;
    AVCodecContext *codec = nullptr;
    AVPacket *packet = nullptr;
    AVFrame *frame = nullptr;
    int stream = -1;
    bool draining = false;
    explicit Decoder(const std::vector<Uint8> &bytes) : reader{bytes} {}
    ~Decoder() {
        av_packet_free(&packet); av_frame_free(&frame); avcodec_free_context(&codec);
        avformat_close_input(&format);
        if (io) { av_freep(&io->buffer); avio_context_free(&io); }
    }
    void Open(AVMediaType type) {
        auto *buffer = static_cast<unsigned char *>(av_malloc(32768));
        Require(buffer != nullptr, "movie input buffer allocation failed");
        io = avio_alloc_context(buffer, 32768, 0, &reader, Read, nullptr, Seek);
        if (!io) { av_free(buffer); throw VideoError("movie input allocation failed"); }
        format = avformat_alloc_context(); Require(format != nullptr, "movie demux allocation failed");
        format->pb = io; format->flags |= AVFMT_FLAG_CUSTOM_IO;
        const auto *demuxer = av_find_input_format("mpeg");
        Require(demuxer && avformat_open_input(&format, nullptr, demuxer, nullptr) >= 0,
                "movie MPEG program demux failed");
        Require(avformat_find_stream_info(format, nullptr) >= 0, "movie stream discovery failed");
        stream = av_find_best_stream(format, type, -1, -1, nullptr, 0);
        Require(stream >= 0, "movie required audio/video stream missing");
        const auto *parameters = format->streams[stream]->codecpar;
        Require(parameters->codec_id == (type == AVMEDIA_TYPE_VIDEO ? AV_CODEC_ID_MPEG1VIDEO : AV_CODEC_ID_MP2),
                "movie codec is outside the reviewed MPEG-1/MP2 scope");
        const auto *provider = avcodec_find_decoder(parameters->codec_id);
        Require(provider != nullptr, "movie required decoder unavailable");
        codec = avcodec_alloc_context3(provider); Require(codec != nullptr, "movie decoder allocation failed");
        Require(avcodec_parameters_to_context(codec, parameters) >= 0, "movie decoder parameters invalid");
        codec->pkt_timebase = format->streams[stream]->time_base;
        Require(avcodec_open2(codec, provider, nullptr) >= 0, "movie decoder open failed");
        packet = av_packet_alloc(); frame = av_frame_alloc();
        Require(packet && frame, "movie packet/frame allocation failed");
    }
    bool Next() {
        for (;;) {
            int result = avcodec_receive_frame(codec, frame);
            if (!result) return true;
            if (result == AVERROR_EOF) return false;
            Require(result == AVERROR(EAGAIN), "movie frame decode failed");
            Require(!draining, "movie decoder stalled while draining");
            do {
                av_packet_unref(packet);
                result = av_read_frame(format, packet);
                if (result == AVERROR_EOF) {
                    draining = true;
                    Require(avcodec_send_packet(codec, nullptr) >= 0, "movie decoder drain failed");
                    break;
                }
                Require(result >= 0, "movie packet read failed");
            } while (packet->stream_index != stream);
            if (!draining) Require(avcodec_send_packet(codec, packet) >= 0, "movie packet decode failed");
        }
    }
    void Rewind() {
        const auto *source = format->streams[stream];
        const int64_t start = source->start_time == AV_NOPTS_VALUE ? 0 : source->start_time;
        Require(av_seek_frame(format, stream, start, AVSEEK_FLAG_BACKWARD) >= 0, "movie rewind failed");
        avcodec_flush_buffers(codec); av_packet_unref(packet); av_frame_unref(frame); draining = false;
    }
};
struct Movie {
    std::vector<Uint8> bytes;
    std::unique_ptr<Decoder> video;
    std::vector<float> pcm;
    std::vector<double> frameTimes;
    SDL_AudioSpec spec{};
    SDL_AudioStream *audio = nullptr;
    SwsContext *conversion = nullptr;
    IRenderPipeline *pipeline = nullptr;
    U32 texture = 0, width = 0, height = 0, frameCount = 0;
    double fps = 0, audioOrigin = 0;
    double position = 0, queuedOrigin = 0, duration = 0, audioDuration = 0, pendingTime = 0;
    bool pending = false, videoEof = false, playing = false, looping = false, hasTimeline = false;
    double lastDecodedTime = 0;
    Uint64 drainedAt = 0;
    ~Movie() {
        if (audio) SDL_DestroyAudioStream(audio);
        sws_freeContext(conversion);
        if (texture && pipeline) {
            for (U32 stage = 0; stage < 2; ++stage) {
                U32 bound = 0;
                if (pipeline->get_texture_stage_texture(stage, &bound) != GR_OK ||
                    (bound == texture && pipeline->set_texture_stage_texture(stage, 0) != GR_OK)) {
                    std::fputs("Native video owned texture unbind failed\n", stderr); std::abort();
                }
            }
            if (pipeline->destroy_texture(texture) != GR_OK) {
                std::fputs("Native video texture release failed\n", stderr); std::abort();
            }
        }
    }
    double Clock() {
        if (!playing) return position;
        const int queued = SDL_GetAudioStreamQueued(audio);
        const int available = SDL_GetAudioStreamAvailable(audio);
        Require(queued >= 0 && available >= 0, "movie audio queue query failed");
        // Available is the resampled view of the SAME queued input, not
        // another queue. Count only source bytes; use Available for drain.
        const double inputSeconds = double(queued) / (sizeof(float) * spec.channels * spec.freq);
        SDL_AudioSpec deviceSpec{}; int deviceFrames = 0;
        Require(SDL_GetAudioDeviceFormat(SDL_GetAudioStreamDevice(audio), &deviceSpec, &deviceFrames) && deviceSpec.freq > 0,
                "movie audio device clock unavailable");
        const double latency = double(deviceFrames) / deviceSpec.freq;
        double clock = audioDuration - inputSeconds - latency;
        if (!queued && !available) {
            if (!drainedAt) drainedAt = SDL_GetTicksNS();
            // Audio is the master while samples remain. A video tail after the
            // last audible sample continues on the monotonic device-drain clock.
            clock = std::max(audioDuration, queuedOrigin) + double(SDL_GetTicksNS() - drainedAt) / 1e9 - latency;
        } else drainedAt = 0;
        position = std::clamp(clock, std::max(position, queuedOrigin), duration);
        return position;
    }
    void QueueAudio(double seconds) {
        Require(SDL_PauseAudioStreamDevice(audio) && SDL_ClearAudioStream(audio), "movie audio reset failed");
        const std::size_t frame = std::min(std::size_t(seconds * spec.freq), pcm.size() / std::size_t(spec.channels));
        const std::size_t begin = frame * std::size_t(spec.channels);
        const std::size_t bytesLeft = (pcm.size() - begin) * sizeof(float);
        Require(bytesLeft <= std::size_t(std::numeric_limits<int>::max()), "movie audio extent exceeds SDL queue");
        if (bytesLeft) Require(SDL_PutAudioStreamData(audio, pcm.data() + begin, int(bytesLeft)), "movie audio queue failed");
        Require(SDL_FlushAudioStream(audio), "movie audio flush failed");
        queuedOrigin = position = seconds; drainedAt = 0;
    }
    bool NextFrame() {
        pending = video->Next();
        if (!pending) { videoEof = true; return false; }
        const auto *stream = video->format->streams[video->stream];
        const int64_t pts = video->frame->best_effort_timestamp;
        if (pts != AV_NOPTS_VALUE) pendingTime = double(pts) * av_q2d(stream->time_base) - audioOrigin;
        else {
            // MPEG's delayed final picture can have no packet timestamp. Its
            // presentation interval is still supplied by the decoded cadence.
            Require(hasTimeline, "movie initial video timestamp missing");
            const double interval = video->frame->duration > 0 ?
                double(video->frame->duration) * av_q2d(stream->time_base) : 1.0 / fps;
            Require(std::isfinite(interval) && interval > 0, "movie video cadence invalid");
            pendingTime = lastDecodedTime + interval;
        }
        Require(std::isfinite(pendingTime) && (!hasTimeline || pendingTime >= lastDecodedTime),
                "movie video timestamp invalid/nonmonotonic");
        lastDecodedTime = pendingTime; hasTimeline = true;
        return true;
    }
    void Upload() {
        AVFrame *frame = video->frame;
        Require(frame->width == int(width) && frame->height == int(height), "movie dimensions changed midstream");
        conversion = sws_getCachedContext(conversion, frame->width, frame->height,
            AVPixelFormat(frame->format), int(width), int(height), AV_PIX_FMT_RGBA,
            SWS_BILINEAR, nullptr, nullptr, nullptr);
        Require(conversion != nullptr, "movie color conversion unavailable");
        RPLOCKDATA lock{};
        Require(pipeline->lock_texture(texture, 0, &lock) == GR_OK, "movie texture lock failed");
        bool converted = lock.pixels && lock.width == width && lock.height == height && lock.pitch >= width * 4U &&
            lock.pitch <= U32(std::numeric_limits<int>::max());
        if (converted) {
            uint8_t *target[] = {static_cast<uint8_t *>(lock.pixels), nullptr, nullptr, nullptr};
            int pitch[] = {int(lock.pitch), 0, 0, 0};
            converted = sws_scale(conversion, frame->data, frame->linesize, 0, int(height), target, pitch) == int(height);
        }
        const bool unlocked = pipeline->unlock_texture(texture, 0) == GR_OK;
        Require(converted && unlocked, "movie color conversion/upload failed");
    }
    void SetPosition(double seconds, U32 requestedFrame = 0) {
        Require(std::isfinite(seconds) && seconds >= 0 && seconds <= duration, "movie seek position invalid");
        video->Rewind(); pending = false; videoEof = false; hasTimeline = false;
        U32 decodedFrame = 0;
        while (NextFrame()) {
            Upload(); ++decodedFrame;
            if (requestedFrame ? decodedFrame == requestedFrame : pendingTime >= seconds) break;
        }
        Require(!requestedFrame || decodedFrame == requestedFrame, "movie requested frame unavailable after rewind");
        QueueAudio(seconds);
        if (playing) Require(SDL_ResumeAudioStreamDevice(audio), "movie audio resume failed");
    }
    void Update() {
        if (!playing) return;
        const double clock = Clock();
        while (pending || NextFrame()) {
            if (pendingTime > clock) break;
            Upload(); pending = false;
        }
        if (videoEof && clock >= duration) {
            if (looping) SetPosition(0, 1);
            else {
                Require(SDL_PauseAudioStreamDevice(audio), "movie EOF audio pause failed");
                playing = false;
            }
        }
    }
};
void DecodeAudio(Movie &movie) {
    Decoder audio(movie.bytes); audio.Open(AVMEDIA_TYPE_AUDIO);
    bool first = true;
    while (audio.Next()) {
        const AVFrame *f = audio.frame;
        Require(f->sample_rate > 0 && f->sample_rate <= 192000 && f->ch_layout.nb_channels >= 1 && f->ch_layout.nb_channels <= 2,
                "movie audio format unsupported");
        if (first) {
            movie.spec = {SDL_AUDIO_F32, f->ch_layout.nb_channels, f->sample_rate};
            Require(f->best_effort_timestamp != AV_NOPTS_VALUE, "movie audio timestamp missing");
            movie.audioOrigin = double(f->best_effort_timestamp) * av_q2d(audio.format->streams[audio.stream]->time_base);
            first = false;
        }
        Require(f->sample_rate == movie.spec.freq && f->ch_layout.nb_channels == movie.spec.channels && f->nb_samples > 0,
                "movie audio format changed");
        const std::size_t count = std::size_t(f->nb_samples) * std::size_t(movie.spec.channels);
        Require(count <= MaxAudioSamples - movie.pcm.size(), "movie decoded audio exceeds bound");
        const auto format = AVSampleFormat(f->format);
        Require(format == AV_SAMPLE_FMT_S16P || format == AV_SAMPLE_FMT_S16 || format == AV_SAMPLE_FMT_FLTP || format == AV_SAMPLE_FMT_FLT,
                "movie decoded audio sample format unsupported");
        const bool planar = av_sample_fmt_is_planar(format) != 0;
        for (int sample = 0; sample < f->nb_samples; ++sample) for (int channel = 0; channel < movie.spec.channels; ++channel) {
            const auto *data = f->extended_data[planar ? channel : 0];
            const int index = planar ? sample : sample * movie.spec.channels + channel;
            float value = format == AV_SAMPLE_FMT_S16P || format == AV_SAMPLE_FMT_S16 ?
                float(reinterpret_cast<const int16_t *>(data)[index]) / 32768.0F : reinterpret_cast<const float *>(data)[index];
            Require(std::isfinite(value), "movie decoded audio sample invalid");
            movie.pcm.push_back(value);
        }
    }
    Require(!first && !movie.pcm.empty(), "movie decoded audio is empty");
    movie.audioDuration = movie.duration = double(movie.pcm.size()) / (movie.spec.channels * movie.spec.freq);
}
class NativeVideoSystem : public ISystemComponent, public IVideoSystem {
    IFileSystem *directory = nullptr;
    IRenderPipeline *pipeline = nullptr;
    std::map<VIDEO_HANDLE, std::unique_ptr<Movie>> movies;
    VIDEO_HANDLE nextHandle = 1;
    bool audioInitialized = false;
    Movie &Get(VIDEO_HANDLE handle) {
        const auto found = movies.find(handle);
        Require(found != movies.end(), "native video handle invalid/stale"); return *found->second;
    }
public:
    BEGIN_DACOM_MAP_INBOUND(NativeVideoSystem)
    DACOM_INTERFACE_ENTRY(ISystemComponent)
    DACOM_INTERFACE_ENTRY(IAggregateComponent)
    DACOM_INTERFACE_ENTRY(IVideoSystem)
    DACOM_INTERFACE_ENTRY2(IID_ISystemComponent, ISystemComponent)
    DACOM_INTERFACE_ENTRY2(IID_IAggregateComponent, IAggregateComponent)
    DACOM_INTERFACE_ENTRY2(IID_IVideoSystem, IVideoSystem)
    END_DACOM_MAP()
    ~NativeVideoSystem() { Shutdown(); }
    GENRESULT init(AGGDESC *desc) { return desc && (!desc->description || !std::strcmp(desc->description, "VideoSystem")) ? GR_OK : GR_INTERFACE_UNSUPPORTED; }
    GENRESULT Initialize() override {
        for (const char *license : {avcodec_license(), avformat_license(), avutil_license(), swscale_license()})
            if (!license || std::strcmp(license, "LGPL version 2.1 or later")) return GR_GENERIC;
        AVCodecParserContext *videoParser = av_parser_init(AV_CODEC_ID_MPEG1VIDEO);
        AVCodecParserContext *audioParser = av_parser_init(AV_CODEC_ID_MP2);
        const bool ready = videoParser && audioParser && avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO) &&
            avcodec_find_decoder(AV_CODEC_ID_MP2) && av_find_input_format("mpeg");
        if (videoParser) av_parser_close(videoParser);
        if (audioParser) av_parser_close(audioParser);
        return ready ? GR_OK : GR_NOT_IMPLEMENTED;
    }
    void Update() override {} // MovieScreen owns frame updates through the original interface.
    GENRESULT Initialize(IFileSystem *fs, IRenderPipeline *render) override {
        if (!fs || !render || directory || pipeline || Initialize() != GR_OK) return GR_INVALID_PARMS;
        if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) return GR_GENERIC;
        audioInitialized = true; directory = fs; pipeline = render; directory->AddRef(); pipeline->AddRef(); return GR_OK;
    }
    void Shutdown() override {
        movies.clear();
        if (directory) { directory->Release(); directory = nullptr; }
        if (pipeline) { pipeline->Release(); pipeline = nullptr; }
        if (audioInitialized) { SDL_QuitSubSystem(SDL_INIT_AUDIO); audioInitialized = false; }
    }
    VIDEO_HANDLE LoadVideoMovie(const char *name, bool looping) override try {
        Require(directory && pipeline && name && *name && nextHandle && movies.size() < 8, "native video load arguments invalid");
        DAFILEDESC desc(name); HANDLE file = directory->OpenChild(&desc);
        Require(file != INVALID_HANDLE_VALUE, "native video resource missing");
        struct FileScope { IFileSystem *fs; HANDLE handle; ~FileScope() { if (handle != INVALID_HANDLE_VALUE && !fs->CloseHandle(handle)) std::abort(); } } scope{directory, file};
        DWORD high = 0; const DWORD size = directory->GetFileSize(file, &high);
        Require(!high && size && size <= MaxMovieBytes, "native video file extent invalid");
        auto movie = std::make_unique<Movie>(); movie->bytes.resize(size);
        DWORD read = 0;
        Require(directory->ReadFile(file, movie->bytes.data(), size, &read) && read == size, "native video file read failed/short");
        Require(directory->CloseHandle(file) != FALSE, "native video file close failed"); scope.handle = INVALID_HANDLE_VALUE;
        movie->pipeline = pipeline; movie->looping = looping;
        DecodeAudio(*movie);
        movie->video = std::make_unique<Decoder>(movie->bytes); movie->video->Open(AVMEDIA_TYPE_VIDEO);
        const auto *stream = movie->video->format->streams[movie->video->stream];
        const auto rate = stream->avg_frame_rate.num ? stream->avg_frame_rate : stream->r_frame_rate;
        movie->fps = av_q2d(rate);
        movie->width = U32(movie->video->codec->width); movie->height = U32(movie->video->codec->height);
        Require(movie->width && movie->height && movie->width <= 4096 && movie->height <= 4096 && std::isfinite(movie->fps) && movie->fps > 0 && movie->fps <= 120,
                "native video dimensions/rate invalid");
        double lastTime = -std::numeric_limits<double>::infinity();
        while (movie->NextFrame()) {
            Require(movie->frameCount < 720000U && movie->pendingTime >= lastTime,
                    "native video frame count/timestamp order invalid");
            ++movie->frameCount; lastTime = movie->pendingTime;
            movie->frameTimes.push_back(lastTime);
        }
        Require(movie->frameCount != 0, "native video decoded frame count is empty");
        movie->duration = std::max(movie->audioDuration, lastTime + 1.0 / movie->fps);
        movie->video->Rewind(); movie->pending = false; movie->videoEof = false; movie->hasTimeline = false;
        Require(pipeline->create_texture(int(movie->width), int(movie->height), PixelFormat(PF_RGBA8), 1, 0, movie->texture) == GR_OK && movie->texture,
                "native video texture creation failed");
        movie->audio = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &movie->spec, nullptr, nullptr);
        Require(movie->audio != nullptr, "native video audio stream open failed");
        Require(movie->NextFrame(), "native video has no frames"); movie->Upload(); movie->pending = false;
        movie->QueueAudio(0);
        const VIDEO_HANDLE handle = nextHandle++;
        movies.emplace(handle, std::move(movie)); return handle;
    } catch (const std::exception &error) { std::fprintf(stderr, "Native video load failed: %s; resource: %s\n", error.what(), name ? name : "<null>"); return 0; }
    bool Diagnostics(VIDEO_HANDLE handle, NativeVideoDiagnostics &out) const {
        const auto found = movies.find(handle);
        if (found == movies.end()) return false;
        const auto &m = *found->second;
        NativeVideoDiagnostics result;
        result.decodedAudioSamples = m.pcm.size();
        result.sampleRate = std::uint32_t(m.spec.freq);
        result.channels = std::uint32_t(m.spec.channels);
        result.decodedFrames = m.frameCount;
        double energy = 0;
        for (float sample : m.pcm) {
            result.peak = std::max(result.peak, std::fabs(sample));
            energy += double(sample) * sample;
        }
        result.rms = m.pcm.empty() ? 0.0F : float(std::sqrt(energy / double(m.pcm.size())));
        out = result; return true;
    }
    void ReleaseVideoMovie(VIDEO_HANDLE handle) override { Get(handle); movies.erase(handle); }
    void StartVideoMovie(VIDEO_HANDLE handle) override { auto &m = Get(handle); if (!m.playing) { m.QueueAudio(m.position); Require(SDL_ResumeAudioStreamDevice(m.audio), "movie audio start failed"); m.playing = true; } }
    void StopVideoMovie(VIDEO_HANDLE handle) override { auto &m = Get(handle); if (m.playing) { m.Clock(); Require(SDL_PauseAudioStreamDevice(m.audio), "movie audio stop failed"); m.playing = false; } }
    bool IsPlaying(VIDEO_HANDLE handle) override { return Get(handle).playing; }
    U32 GetVideoTexture(VIDEO_HANDLE handle) override { return Get(handle).texture; }
    void GetVideoTexureCoords(VIDEO_HANDLE handle, SINGLE &u, SINGLE &v) override { Get(handle); u = v = 1.0F; }
    void SetVideoFrame(VIDEO_HANDLE handle, U32 frame) override { auto &m = Get(handle); Require(frame >= 1 && frame <= m.frameCount, "movie frame seek out of range"); m.SetPosition(frame == 1 ? 0.0 : std::max(0.0, m.frameTimes[frame - 1]), frame); }
    void UpdateVideoFrame(VIDEO_HANDLE handle) override { Get(handle).Update(); }
    U32 GetNumFrames(VIDEO_HANDLE handle) override { return Get(handle).frameCount; }
    U32 GetVideoWidth(VIDEO_HANDLE handle) override { return Get(handle).width; }
    U32 GetVideoHeight(VIDEO_HANDLE handle) override { return Get(handle).height; }
};
}
bool GetNativeVideoDiagnostics(IVideoSystem *owner, std::uint32_t handle, NativeVideoDiagnostics &out) {
    const auto *native = dynamic_cast<const NativeVideoSystem *>(owner);
    return native && native->Diagnostics(handle, out);
}
bool RegisterNativeVideoSystem(ICOManager *manager) {
    if (!manager) return false;
    auto *factory = new DAComponentFactory2<DAComponentAggregate<NativeVideoSystem>, AGGDESC>(interfaceName);
    if (!factory) return false;
    const GENRESULT result = manager->RegisterComponent(factory, interfaceName); factory->Release(); return result == GR_OK;
}
}
