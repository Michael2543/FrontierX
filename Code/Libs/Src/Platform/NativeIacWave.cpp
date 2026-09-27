#include "NativeIacWave.h"
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
}
#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <limits>

namespace FrontierX
{
namespace
{
constexpr std::size_t MaximumInputBytes = 64U * 1024U * 1024U;
// The selected 15-minute IAC music track expands to roughly 160 MiB of F32 PCM.
constexpr std::size_t MaximumDecodedBytes = 256U * 1024U * 1024U;
struct MemoryReader
{
    const std::vector<Uint8> & bytes;
    std::size_t position = 0;
};
int Read(void * opaque, uint8_t * output, int count)
{
    auto & reader = *static_cast<MemoryReader *>(opaque);
    if (count <= 0) return AVERROR(EINVAL);
    const auto size = std::min(static_cast<std::size_t>(count), reader.bytes.size() - reader.position);
    if (!size) return AVERROR_EOF;
    std::memcpy(output, reader.bytes.data() + reader.position, size);
    reader.position += size;
    return static_cast<int>(size);
}
int64_t Seek(void * opaque, int64_t offset, int origin)
{
    auto & reader = *static_cast<MemoryReader *>(opaque);
    if (origin == AVSEEK_SIZE) return static_cast<int64_t>(reader.bytes.size());
    int64_t base = 0;
    if (origin == SEEK_CUR) base = static_cast<int64_t>(reader.position);
    else if (origin == SEEK_END) base = static_cast<int64_t>(reader.bytes.size());
    else if (origin != SEEK_SET) return AVERROR(EINVAL);
    if (offset < -base || offset > static_cast<int64_t>(reader.bytes.size()) - base)
        return AVERROR(EINVAL);
    reader.position = static_cast<std::size_t>(base + offset);
    return base + offset;
}
struct Decoder
{
    AVFormatContext * format = nullptr;
    AVIOContext * io = nullptr;
    AVCodecContext * codec = nullptr;
    AVFrame * frame = nullptr;
    AVPacket * packet = nullptr;
    ~Decoder()
    {
        av_packet_free(&packet);
        av_frame_free(&frame);
        avcodec_free_context(&codec);
        avformat_close_input(&format);
        if (io) { av_freep(&io->buffer); avio_context_free(&io); }
    }
};
}

bool DecodeNativeIacWave(const std::vector<Uint8> & bytes, SDL_AudioSpec & spec,
                         std::vector<Uint8> & samples)
{
    if (bytes.size() < 12 || bytes.size() > MaximumInputBytes ||
        std::memcmp(bytes.data(), "RIFF", 4) || std::memcmp(bytes.data() + 8, "WAVE", 4))
        return false;
    auto word = [&](std::size_t at) {
        return static_cast<std::uint32_t>(bytes[at]) |
            (static_cast<std::uint32_t>(bytes[at + 1]) << 8) |
            (static_cast<std::uint32_t>(bytes[at + 2]) << 16) |
            (static_cast<std::uint32_t>(bytes[at + 3]) << 24);
    };
    const std::size_t declared = word(4);
    if (declared < 4 || declared > bytes.size() - 8) return false;
    const std::size_t end = declared + 8;
    bool hasData = false;
    for (std::size_t at = 12; at < end;)
    {
        if (end - at < 8) return false;
        const std::size_t length = word(at + 4);
        if (length > end - at - 8) return false;
        if (!std::memcmp(bytes.data() + at, "data", 4)) hasData = length != 0;
        at += 8 + length;
        if (length & 1U) { if (at == end) return false; ++at; }
    }
    if (!hasData) return false;
    // The selected native path is replaceable shared LGPL libraries only.
    for (const char * license : {avcodec_license(), avformat_license(), avutil_license()})
        if (!license || std::strcmp(license, "LGPL version 2.1 or later")) return false;
    MemoryReader reader{bytes};
    Decoder decoder;
    auto * buffer = static_cast<unsigned char *>(av_malloc(4096));
    if (!buffer) return false;
    decoder.io = avio_alloc_context(buffer, 4096, 0, &reader, Read, nullptr, Seek);
    if (!decoder.io) { av_free(buffer); return false; }
    decoder.format = avformat_alloc_context();
    if (!decoder.format) return false;
    decoder.format->pb = decoder.io;
    decoder.format->flags |= AVFMT_FLAG_CUSTOM_IO;
    const auto * wave = av_find_input_format("wav");
    if (!wave || avformat_open_input(&decoder.format, nullptr, wave, nullptr) < 0 ||
        decoder.format->nb_streams != 1) return false;
    const auto * parameters = decoder.format->streams[0]->codecpar;
    if (parameters->codec_id != AV_CODEC_ID_IAC || parameters->codec_type != AVMEDIA_TYPE_AUDIO ||
        parameters->sample_rate <= 0 || parameters->sample_rate > 192000 ||
        parameters->ch_layout.nb_channels < 1 || parameters->ch_layout.nb_channels > 2)
        return false;
    const auto * codec = avcodec_find_decoder(AV_CODEC_ID_IAC);
    if (!codec) return false;
    decoder.codec = avcodec_alloc_context3(codec);
    if (!decoder.codec || avcodec_parameters_to_context(decoder.codec, parameters) < 0)
        return false;
    decoder.codec->thread_count = 1;
    decoder.codec->err_recognition = AV_EF_EXPLODE;
    if (avcodec_open2(decoder.codec, codec, nullptr) < 0) return false;
    decoder.frame = av_frame_alloc();
    decoder.packet = av_packet_alloc();
    if (!decoder.frame || !decoder.packet) return false;
    SDL_AudioSpec result{};
    result.format = SDL_AUDIO_F32;
    result.freq = parameters->sample_rate;
    result.channels = parameters->ch_layout.nb_channels;
    std::vector<Uint8> decoded;
    auto receive = [&]() {
        for (;;)
        {
            const int status = avcodec_receive_frame(decoder.codec, decoder.frame);
            if (status == AVERROR(EAGAIN) || status == AVERROR_EOF) return true;
            if (status < 0) return false;
            AVFrame & frame = *decoder.frame;
            if (frame.format != AV_SAMPLE_FMT_FLTP || frame.sample_rate != result.freq ||
                frame.ch_layout.nb_channels != result.channels || frame.nb_samples <= 0)
                return false;
            const std::size_t count = static_cast<std::size_t>(frame.nb_samples) *
                static_cast<std::size_t>(result.channels);
            if (count > (MaximumDecodedBytes - decoded.size()) / sizeof(float)) return false;
            const std::size_t begin = decoded.size();
            decoded.resize(begin + count * sizeof(float));
            for (int sample = 0; sample < frame.nb_samples; ++sample)
                for (int channel = 0; channel < result.channels; ++channel)
                {
                    const float value = reinterpret_cast<const float *>(frame.extended_data[channel])[sample];
                    if (!std::isfinite(value)) return false;
                    const std::size_t index = static_cast<std::size_t>(sample) *
                        static_cast<std::size_t>(result.channels) + static_cast<std::size_t>(channel);
                    std::memcpy(decoded.data() + begin + index * sizeof(float), &value, sizeof(value));
                }
            av_frame_unref(decoder.frame);
        }
    };
    int status = 0;
    while ((status = av_read_frame(decoder.format, decoder.packet)) >= 0)
    {
        if (decoder.packet->stream_index != 0 ||
            avcodec_send_packet(decoder.codec, decoder.packet) < 0 || !receive()) return false;
        av_packet_unref(decoder.packet);
    }
    if (status != AVERROR_EOF || avcodec_send_packet(decoder.codec, nullptr) < 0 ||
        !receive() || decoded.empty()) return false;
    spec = result;
    samples.swap(decoded);
    return true;
}
}
