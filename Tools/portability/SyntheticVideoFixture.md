# Synthetic native movie fixture

`SyntheticVideoFixture.inc` contains independently generated mathematical colors
and a 440 Hz tone. It contains no retail data, captured output, executable, SDK or
third-party source. The base64 payloads are decoded into the existing probe's
build directory at execution. The checked-in bytes require no runtime encoder.

Generated on 2026-10-14 with the installed system FFmpeg n9.0.1 executable. Its
build configuration does not affect the linked game provider: the generator is
not conveyed or linked. Runtime decoding uses the separately reviewed external
shared LGPL FFmpeg prefix, including MPEG video/audio parsers and libswscale.

Generation command (the output path was a private temporary directory):

```sh
ffmpeg -hide_banner -loglevel error \
  -f lavfi -i "nullsrc=s=16x16:r=25:d=1,format=gbrp,geq=r='if(mod(N,2),0,255)':g=0:b='if(mod(N,2),255,0)'" \
  -f lavfi -i 'sine=frequency=440:sample_rate=22050:duration=1' \
  -map 0:v -map 1:a -c:v mpeg1video -b:v 64k -g 1 -bf 0 \
  -pix_fmt yuv420p -r 25 -c:a mp2 -b:a 32k -ar 22050 -f mpeg video.mpg
ffmpeg -hide_banner -loglevel error -i video.mpg -map 0:v -c copy -f mpeg video-only.mpg
ffmpeg -hide_banner -loglevel error -i video.mpg -map 0:a -c copy -f mpeg audio-only.mpg
```

The full stream has 25 alternating red/blue 16×16 frames. The first two decoded
frames must differ; one-based seeks must restore the corresponding pixel data.
RGB tolerances accommodate MPEG/YUV conversion; alpha must remain opaque.
Video-only/audio-only copies protect required-stream rejection. Truncated input
is a short prefix of the full mathematical fixture, not a separate asset.

| Payload | Bytes | SHA-256 |
|---|---:|---|
| Full movie | 8192 | `c0ef9dd70fdea7a72c18272603f73d9d86443942569021a0eefa34c3898688dc` |
| Video only | 2048 | `391c4a577e1b9e2225f44ffdc448308bebedfb652c4805ce2120c683da229ea1` |
| Audio only | 6144 | `bcf7b7125aa63fa9d1ddbf50f062b58f3d72674f1bbfa1abd122f8e2cc03b1fa` |

The production owner is `NativeVideoSystem` through the original `IVideoSystem`
interface; its real caller is `MovieScreen`. The existing window-manager probe
already owns SDL audio and directory filesystems. Its extension must disprove
codec/resource/frame/timing/ownership blockers before enabling movies in playable
CI. CPU texture uploads and dummy audio do not establish MovieScreen's visible
composition or audible retail playback; those require separate actual-host and
user evidence.
