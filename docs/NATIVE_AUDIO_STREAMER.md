# Native IStreamer contract

## Exact Kalidon beacon restoration (2026-10-18)

At the user's explicit request, the native audio owner now resolves only TM8's
exact authored `specialDenied.wav` cue to the existing retail
`SFX/specialdenied.wav` entry when MSpeech has no corresponding entry. A supplied
mission-speech entry retains priority. The canonical lowercase retail spelling
also works on physical directory mounts; the mixed-case script name is unchanged.
Other speech names keep their original archive and failure result. Explicit
speech exclusion still excludes the cue. Playback retains the existing
PlayAudioMessage volume, playback-ID, highlight and cleanup contract; no generic
archive fallback, replacement recording, script/timer or gameplay change is added.
This supersedes the beacon presentation gap recorded in the earlier entry below.

**Synthetic automated execution:** the existing generated-PCM shell fixture
contains a separate lowercase SFX cue and an empty mission-speech source. It
checks disabled playback creates no stream/ID; enabled playback obtains a real
stream and speaker highlight and stops cleanly. A different missing speech name
that does exist in SFX still returns zero, proving the resolution remains bounded.
The prior owner fails the beacon playback check; the repaired owner passes. The
Python caller checks the explicit SFX-resolution diagnostic. Existing missing-file,
completion, portrait and ownership assertions remain enabled.

Full-media Debug GCC16.2.1 and Clang22.1.8, and fatal Clang ASan/UBSan with leak
detection, build `frontierx_native_shell` and pass:

```sh
ctest --test-dir "$BUILD" --output-on-failure -R '^portability\.(native_shell|layout|script08_schema)$'
```

**Lawfully external audio runtime:** the existing GCC default SDL-only
`frontierx_native_window_manager_probe` opens the real SFX UTF archive and exact
cue through IStreamer. The original mono 22050 Hz 8-bit PCM file completes
playback through PipeWire in approximately 1010 ms; no extracted media is added
to the repository. The probe's video driver is dummy, so no gameplay window or
input is involved. Executed command with privately supplied paths:

```sh
cmake --build out/build/linux-gcc-debug --target frontierx_native_window_manager_probe -j 6
SDL_VIDEODRIVER=dummy FRONTIERX_STREAMER_ARCHIVE="$RETAIL/DATA/sfx" FRONTIERX_STREAMER_ENTRY=specialDenied.wav out/build/linux-gcc-debug/frontierx_native_window_manager_probe
```

A new pinned full-media candidate retains the user's exact checkpoint and genuine
progress. In-mission cue timing and user audible confirmation remain pending,
as do native Windows/module ABI checks and the existing host-wide leak gates.
This establishes exact-asset playback plus the tested owner resolution, not full
mission/media acceptance or restoration of unrelated missing audio entries.


## Kalidon beacon audio-open abort (2026-10-18)

**User retail-input runtime:** the user reports that mission 9 ran for a long
period before closing. The actual candidate log selects `SCRIPT08.dll` and ends
at `Native mission speech failed to open: specialDenied.wav`; the retained last
quicksave contains original TM8 programs. Record the user's ordinal separately
from this observed Lost Sheep script identity. Both quicksaves and genuine
progress are copied and fingerprinted privately before further tests. No recent
systemd core is available; no segmentation fault, OOM or driver failure is inferred.

**Source and external-data review:** `TM8_FlashBeacon` requests this audio every
20 updates while Benson is unfound and the current system is Kalidon. The exact
filename exists in SFX, not MSpeech, in the supplied external installation.
`MScript::PlayAudio` calls `PlayAudioMessage`, which opens MSpeech. The original
Windows `SoundMan::PlayAudioMessage` returns playback ID zero when Open fails.
The native shared stream owner incorrectly aborted instead. It now preserves
that original failure result for non-communication opens as well, emitting a
separate failed-open counter and explicit unavailable-audio diagnostic. Failed
opens create no stream, success ID or speaker highlight; invalid parameters and
stream-start failures still fail explicitly. This supersedes the earlier native
strict-open policy described in the historical entry below. No script, mission
state, timing, audio filename, archive search order, simulation, public interface,
serialized record or Windows owner changes.

The missing beacon cue remains a presentation gap: it is not silently redirected
to another archive, replaced, muted or counted as successfully played. This
repair establishes original failure handling and continued gameplay, not restored
beacon audio or complete TM8/TM9 acceptance.

**Synthetic automated execution:** the existing real-stream shell lifecycle now
requests a missing mission-audio entry, checks result zero, unchanged last ID,
empty streams, no highlight and distinct communication/mission failure counters,
then successfully opens/stops a generated PCM message with normal highlight
ownership. Its Python caller checks the failure diagnostic. The old owner aborts
and fails this regression; the repair passes while existing completion, portrait,
volume and communication checks remain enabled. No new probe is added.

Linux x86_64 GCC16.2.1 and Clang22.1.8 full-media Debug configurations build
`frontierx_native_shell` and execute:

```sh
cmake --build "$BUILD" --target frontierx_native_shell -j 6
ctest --test-dir "$BUILD" --output-on-failure -R '^portability\.(native_shell|layout|script08_schema|script09_schema)$'
```

The same checks pass in fatal Clang ASan/UBSan with leak detection and an external
font face. These generated-PCM checks are synthetic runtime, not a continued
retail-data mission. A separately pinned full-media candidate retains the user's
latest TM8 quicksave and original progress for normal F8 continuation. Fixed-build
Kalidon gameplay confirmation, native Windows/module ABI and existing full-process
hardware/library leak gates remain pending. No new mission acceptance is inferred.


## User-run communication abort diagnosis (2026-10-14)

The user reported reaching the first Terran mission's victory and checking
save/load, followed by the game window closing. The system core confirms
`SIGABRT` in native `NativeMissionSound::Play`, called by the real
`UnitComm::update` communication queue. Its speaker is a Harvester; its
communication index is the source `planetDepleted` slot. This is a failed
speech open, not evidence of a completed normal victory teardown.

Source/resource review of the lawfully external StringPack and speech archives
identifies `HARVEST!!T_Harvester`'s depletion and redeployment requests as
`0108.wav` and `0119.wav`. Neither entry exists in the supplied speech or
mission-speech archive. The existing external-input window-manager probe
also fails actual lookup of `0108.wav` in both archives, before decoding.
No replacement name or substitute voice has been selected. Both missing
voices remain open P1b media blockers; similar-looking filenames are not
proof of the intended content. Raw core and retail records remain external.

The original Windows `SoundMan::PlayCommMessage` returns playback ID zero
when `IStreamer::Open` fails. The native owner incorrectly aborted instead.
Its communication path now retains that original failure result and emits
an explicit unavailable-communication diagnostic and failed-open counter.
It creates no stream, success ID or speaker highlight. Required scripted
audio and animated-message paths retain their strict failed-open handling.
There is no change to the Windows owner, public interfaces, serialized
records, simulation or RNG consumption.

Automated synthetic execution: rebuilt `frontierx_native_shell` in the
existing full-media Debug GCC 16.2.1, Clang 22.1.8 and Clang ASan/UBSan
graphs (native fonts, FFmpeg audio and video enabled). The existing
`portability.native_shell` CTest passes under each compiler/configuration.
Its generated-PCM sound lifecycle now explicitly checks that an absent
communication returns zero, allocates no stream or ID, records exactly one
failure and prints the failure diagnostic before successful real PCM
playback. The sanitizer run uses
`ASAN_OPTIONS=detect_leaks=1:halt_on_error=1` and
`UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`, without a report or skip.
This bounded owner check does not reproduce the user's complete mission
or establish that later victory/return paths are safe. Native Windows
Debug/Final, actual CI execution and renewed user victory-exit verification
remain pending.

Current unit voice checkpoint, 2026-10-10: original UnitComm and StringData
now connect normal selection/movement in the selected external `Sol_Train`
mission to native ISoundManager and production IStreamer. Separate visible
Wayland/Vulkan GCC/Clang optional-FFmpeg Debug runs started two unit voices,
completed one, stopped one and retained no active stream; both preserved
movement, objectives and source mission save/reload and exited zero.
This requires the reviewed external shared-LGPL IAC decoder and explicit
unit-alert/subtitle and mission-SFX exclusions. Next: restore in-window
text/teletype through the original owners. Windows/ABI, full mission
sanitizers, audible comparison and retail parity remain unverified. See
[the focused voice record](NATIVE_SINGLEPLAYER_SLICE.md#original-unit-voice-mission-checkpoint-2026-10-10).
Older entries below are historical.

Current mission audio boundary, 2026-10-10: the source `Sol_T` game-active
program requested `solarian_game14.wav`, then animated `STMG03.wav` speech.
The native mission sound owner now performs the source music fade request via
`IMusicManager::SetVolume(0)`. With the default SDL-only decoder, the real
`IStreamer` correctly fails the external IAC speech open. For a bounded
gameplay-owner diagnostic, the native shell explicitly used
`--skip-animated-portraits --skip-training-speech --skip-subtitles`; the
portrait exclusion otherwise retains real `IStreamer` speech playback.
The GDB-confirmed subtitle owner is still absent, and the default audio
build is not a complete training media path. No claim of speech playback in
this mission run is made. See
[the focused mission record](NATIVE_SINGLEPLAYER_SLICE.md#original-planet-owner-game-active-update-and-first-object-render-blocker-2026-10-10).
Entries below are historical snapshots.

Current native App sound-effect check, 2026-10-03: the native `ISFX`
owner now maps the source `SFX::BUTTONPRESS1` ID to the lawfully
external IAC effect and invokes required `IStreamer` for preload,
play, loop and stop. GCC and Clang FFmpeg-enabled shells actually
exercised it on SDL dummy audio; the mission remained unstarted.
Spatial attenuation and source priority are still unsupported.
See [the focused record](NATIVE_SINGLEPLAYER_SLICE.md#native-isfx-and-field-object-closure-2026-10-03).
Entries below are historical snapshots.

Current selected-SFX IAC check, 2026-10-03: the lawful external
`interface1.wav` used by source `SFX::BUTTONPRESS1` has WAVE tag
`0x0402`. Default GCC/Clang window-probe builds rejected its decode,
as their optional IAC adapter is disabled. The pinned official FFmpeg
9.0.1 source was fetched again outside the repository with the recorded
SHA-256 and rebuilt unmodified using the restricted shared LGPL
configuration below. Separate GCC 16.2.1 and Clang 22.1.8 Debug builds
of `frontierx_native_window_manager_probe` and `frontierx_native_shell`
passed CMake's LGPL query and focused shell/window checks. On SDL
dummy audio, both external-input probes decoded 30720 bytes of
22050-Hz mono float PCM with FNV1a64 `16092735175899199743` and
completed playback after 404/408 ms. The selected mission still has
no native `ISFX` owner or running gameplay; this is `IStreamer`
decoder/output evidence only. The FFmpeg-enabled full test matrix,
hardware audibility, native Windows/ABI and retail-runtime checks were
not performed. See [the focused mission record](NATIVE_SINGLEPLAYER_SLICE.md#external-sfx-source-binding-2026-10-03).


Latest App music owner, 2026-09-27: the original `MusicManager.cpp`
now creates and subscribes a native App owner after required
`IStreamer` startup in the selected external-mission shell. GCC and
Clang executed owner startup/teardown; no `PlayMusic` or mission
script ran. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#production-music-manager-owner-startup-gate-2026-09-27).

Latest selected-mission binding, 2026-09-27: the native GPU shell now
retains the production `IStreamer` aggregate and external Music source
for the App mission globals. It initializes SDL audio after the
window callback is live and rejects missing required owners. GCC and
Clang executed the lawful external `Sol_Train.dmission` source with
SDL dummy audio; no `MusicManager` or mission program ran. See
[the focused record](NATIVE_SINGLEPLAYER_SLICE.md#selected-mission-audio-ownership-gate-2026-09-27).

Latest selected-training music input, 2026-09-27: the source
`Sol_T` script asks for `solarian_game14.wav`. Its lawful external IAC
file expands to 160448512 bytes of F32 PCM, beyond the former 64-MiB
decoded-output bound. The FFmpeg-enabled owner now retains the 64-MiB
compressed-input bound and uses a 256-MiB decoded-output bound.
GCC/Clang probes decoded identical PCM and briefly started/stopped
the long track through SDL dummy audio; short training speech still
completed. This does not execute the App `MusicManager` or mission.
See [the focused record below](#selected-long-music-input-2026-09-27).

Current gate: the optional FFmpeg IAC path below supersedes the earlier
SDL-only rejection. External `STMG03.wav` now decodes and completes playback
through the production interface under GCC and Clang. This proves one retail
audio input, not the full App audio path, Windows sound parity or gameplay.

Historical status, before the FFmpeg and selected-music updates:
GCC/Clang compilation, synthetic dummy-device and host PipeWire
runtime; initial failed decoding of externally supplied training
speech. **This is not a completed retail audio path or gameplay
milestone.**

## Owner and integration decision

The unmodified source profile first failed to create `IStreamer`. The real
`SoundManager.cpp` caller opens a stopped stream, records its handle, then
handles `CQE_STREAMER` window messages: `INITSUCCESS` starts animated speech,
while `COMPLETED`/`INVALID` remove its pending node. `Sol_T.cpp` waits on
these message handles. A successful silent stub would change script behavior.
`IVideoSystem` owns a separate movie path; it does not replace talking-head
audio. No optional-media policy was selected.

The new [NativeStreamer.cpp](../Code/Libs/Src/Platform/NativeStreamer.cpp)
is an SDL-backed aggregate at the existing `IStreamer` interface, registered
by the shell before profile loading. Aggregate `Initialize` creates no sound
device, matching the two-stage contract; `IStreamer::Init` must succeed
separately. The full App's DirectSound initialization is not ported or invoked.
The native Init requires a null DirectSound pointer, a correctly sized
descriptor, and either no notification target or a live native window with a
callback and user message ID. Missing devices fail Init. Reinitialization
is rejected. SDL subsystem ownership is balanced on failure/destruction.
No vtable, persistent layout, simulation order or Windows implementation was
changed; `Streamer.h` now imports the existing non-Windows declaration types
and uses the actual case of `DACOM.H`.

Open reads through the supplied production `IFileSystem`, decodes through
SDL's WAV loader, and binds a paused SDL audio stream. It returns null for
missing, malformed or unsupported data. Input and decoded payloads are capped
at 64 MiB compressed and 256 MiB decoded respectively, with concurrent
streams at 64; allocation is currently whole-file,
not the legacy threaded read-ahead strategy. Descriptor buffer times are
legacy buffering hints, not simulation timing. No background thread calls
game logic. The native window pump updates streams on the application thread
and delivers deferred notifications through its existing window callback.
The existing `ISystemComponent::Update` can also perform that work.

Pause/resume, logarithmic DirectSound-volume conversion, queued looping,
EOF/completion and explicit close are implemented. Completion waits for the
SDL queue to drain plus a conservative two-device-buffer tail; this is not
sample-exact hardware completion or Windows timing parity. Loop replenishment
requires regular pumping; long stalls can cause underruns. Handles use
monotonic native-width tokens, never reused during the process. Callbacks may
close their own stream; iteration rechecks the handle before further access.
Teardown destroys audio streams before their device and subsystem.

Retail scripts spell speech names uppercase while the supplied archive uses
lowercase. If the exact open fails, the owner uses the existing case-insensitive
FindFirst/FindNext contract and accepts only one nondirectory match. Wildcard
and path-shaped fallback requests are rejected. Archive names are not modified.

## Validation and exact limits

The existing `portability.native_window_manager` check was extended because
it already owns the real DACOM aggregate, SDL window and callback pump.
It now exercises generated 8-kHz mono PCM WAV decoding, deferred initialization,
uppercase lookup, volume, stopped playback, resume, EOF/completion, looping,
invalid/stale handles and close from inside the initialization callback.
Malformed and unsupported-format input fails. An invalid notification window
fails Init. The shell test proves profile aggregation succeeds with IStreamer
and continues rejecting unavailable IVideoSystem. No new probe was added.

Executed on Linux x86_64, GCC 16.2.1 and Clang 22.1.8:

```sh
cmake --build --preset linux-gcc-debug -j4
ctest --preset linux-gcc-debug --output-on-failure
cmake --build --preset linux-clang-debug -j4
ctest --preset linux-clang-debug --output-on-failure
cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_native_shell frontierx_native_window_manager_probe -j4
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.native_(shell|window_manager)$' --output-on-failure
```

The focused CPU checks passed with dummy SDL video/audio. ASan/UBSan passed
with LeakSanitizer disabled. Both compiler probes also ran outside the command
sandbox, from their respective build directories, using
`SDL_VIDEODRIVER=dummy ./frontierx_native_window_manager_probe` and the host
PipeWire audio driver. Generated playback/callback checks passed. This is
actual host audio API execution, not microphone capture or proof of audibility.
An explicit `SDL_AUDIODRIVER=frontierx_invalid` run failed Init and exited 1;
no dummy fallback was silently substituted. GPU checks still skip in the
command sandbox and no new visible renderer run was needed for this change.

Both shells ran against the user's external Steam DATA with
`--material-root Data/mat --system-profile Code/App/Src/Conquest.ini
--hidden --smoke-ms 1` and dummy video. They mounted ten archives, created
IStreamer and stopped at `IVideoSystem`, exit 3. This is profile acquisition,
not audio initialization or mission startup.

The GCC probe additionally ran with `FRONTIERX_STREAMER_ARCHIVE` set to the
external `DATA/MSpeech` archive and `FRONTIERX_STREAMER_ENTRY=STMG03.wav`.
It failed Open, exit 1. A temporary reader outside the repository confirmed
the exact entry `stmg03.wav` is 93,040 bytes, mono, WAVE tag `0x0402`.
SDL_LoadWAV_IO rejected it with `Unknown WAVE format tag: 0x0402`.
[FFmpeg's RIFF codec table](https://github.com/FFmpeg/FFmpeg/blob/master/libavformat/riff.c)
identifies that tag as IAC (Indeo Audio Coder). This is a Linux decoder gap,
not evidence of damaged retail data or a CQ2 mission-layout defect. No retail
payload was copied into the repository. The opt-in probe path retains the
failure until a real decoder is integrated.

Next required audio work is IAC decoding behind this owner, with dependency
provenance/license review before linking a decoder. Installed FFmpeg libraries
were inventoried but not linked or added as dependencies. The selected training
mission cannot claim audio readiness from the generated PCM test. Also open:
full App initialization/event delivery, long-track buffering, device-loss
handling, pause/completion timing parity, native Windows rebuild/ABI checks,
mission loading, simulation and save/reload. `IVideoSystem` remains the next
profile-creation failure independently of the IAC requirement.


## Optional IAC decoder, 2026-09-27

`FRONTIERX_BUILD_FFMPEG_AUDIO=ON` adds the native IAC/WAV adapter to the existing
streamer owner. It requires SDL and external shared FFmpeg libraries. CMake
runs their license query and rejects anything other than LGPL 2.1-or-later;
static archives are rejected. Runtime repeats the library-license query.
Default builds leave this option off and continue returning failure for IAC.
There is no automatic download, system modification, transcoded asset cache
or subprocess decoder. See [dependency provenance](DEPENDENCIES.md#optional-native-iac-decoder-ffmpeg-901).

The independently written adapter validates RIFF/chunk bounds and supplies a
bounded custom memory reader to the WAV demuxer. It accepts only IAC, one or
two channels, a valid bounded sample rate, and finite planar float output.
It interleaves those floats for SDL, preserving rate/channels. Demuxing,
decoding, output limits and allocation failures reject the whole operation;
caller output is unchanged on rejection. Each decode has isolated packet,
frame, codec and I/O owners. Neither file formats nor simulation state change.
The existing window test covers truncated IAC and transactional rejection;
its opt-in external-input path now prints a PCM fingerprint before playback.

The official, unmodified FFmpeg 9.0.1 source was unpacked outside the repository
and built with GCC 16.2.1 using the following configure command, followed by
`make -j4` and `make install`:

```sh
./configure --prefix=/tmp/frontierx-ffmpeg-lgpl --disable-gpl --disable-nonfree --disable-version3 --disable-autodetect --disable-everything --disable-programs --disable-doc --disable-network --disable-static --enable-shared --disable-avdevice --disable-avfilter --disable-swscale --disable-swresample --disable-x86asm --enable-avcodec --enable-avformat --enable-decoder=iac --enable-demuxer=wav
```

The first configure attempt without `--disable-x86asm` failed because NASM
was unavailable. Before the LGPL prefix existed, the application configure
selected installed GPL libraries and correctly failed its license gate.
The successful configure reported `LGPL version 2.1 or later`; `ldd` on the
native executable resolved all three libav libraries to that external prefix.
No FFmpeg source code or binary entered the repository. Distribution remains
subject to the unresolved package-level license review.

Both application configurations and their full existing checks executed:

```sh
cmake --preset linux-gcc-debug -U '*FRONTIERX_AV*' -DFRONTIERX_BUILD_FFMPEG_AUDIO=ON -DCMAKE_PREFIX_PATH=/tmp/frontierx-ffmpeg-lgpl
cmake --build --preset linux-gcc-debug -j4
ctest --preset linux-gcc-debug --output-on-failure
cmake --preset linux-clang-debug -DFRONTIERX_BUILD_FFMPEG_AUDIO=ON -DCMAKE_PREFIX_PATH=/tmp/frontierx-ffmpeg-lgpl
cmake --build --preset linux-clang-debug -j4
ctest --preset linux-clang-debug --output-on-failure
```

The GCC cache reset removes the previously detected GPL pkg-config result.
In each build directory, outside the command sandbox, this command executed
with `SPEECH_ARCHIVE` set to the lawful external `DATA/MSpeech` archive:

```sh
SDL_VIDEODRIVER=dummy FRONTIERX_STREAMER_ARCHIVE="$SPEECH_ARCHIVE" FRONTIERX_STREAMER_ENTRY=STMG03.wav ./frontierx_native_window_manager_probe
```

Both runs selected PipeWire, decoded 1,487,872 bytes of float PCM (371,968 mono
samples, 22,050 Hz), and reported FNV-1a-64 `10538922693386034611`. GCC playback
completed after 16,968 ms; Clang after 16,970 ms. Both link the same GCC-built
FFmpeg libraries, so this compares application adapters, not independently
compiled codec arithmetic. The decoder emits warnings about potential band
coefficient problems but returns successful finite frames. These warnings
were retained in external logs, not suppressed or presented as Windows sound
parity. No microphone capture, wave-reference comparison or audibility claim
was made. No retail audio was copied into repository files.

Focused Clang ASan/UBSan covers the native adapter, stream owner and window
pump with LeakSanitizer disabled. The externally built FFmpeg libraries are
not sanitizer-instrumented. Configure also needs `ASAN_OPTIONS=detect_leaks=0`
in this sandbox: the initial license-query executable failed because
LeakSanitizer cannot run under its ptrace restriction, not because the LGPL
query failed. Repeating with that environment succeeded:

```sh
ASAN_OPTIONS=detect_leaks=0 cmake -S . -B /tmp/frontierx-texture-handle-asan -DFRONTIERX_BUILD_FFMPEG_AUDIO=ON -DCMAKE_PREFIX_PATH=/tmp/frontierx-ffmpeg-lgpl
ASAN_OPTIONS=detect_leaks=0 cmake --build /tmp/frontierx-texture-handle-asan --target frontierx_native_shell frontierx_native_window_manager_probe -j4
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 ctest --test-dir /tmp/frontierx-texture-handle-asan -R '^portability.native_(shell|window_manager)$' --output-on-failure
```

The sanitizer executable also ran the same external speech path with
`SDL_AUDIODRIVER=dummy`, `ASAN_OPTIONS=detect_leaks=0` and
`UBSAN_OPTIONS=halt_on_error=1`. It produced the same PCM fingerprint and
completed after 17,033 ms without native sanitizer diagnostics. This is
decoder/adapter execution with a dummy sink, not additional host audio evidence.
Both FFmpeg-enabled shells were rerun with the external DATA root and unmodified
source profile; both still reported `IVideoSystem returned [FAILED]`, exit 3.

A separate fresh GCC Debug build at `/tmp/frontierx-iac-disabled`, with SDL on,
GPU off and FFmpeg audio off, built and passed the existing window-manager
check. Its external speech attempt still returned failure, exit 1. This
protects explicit rejection in configurations lacking the decoder.
The GPU CTests skip in the command sandbox; no new GPU execution is claimed.
Native Windows builds/ABI checks remain unperformed. The next profile blocker
is still `IVideoSystem`, followed by the remaining production startup and
mission closure; App audio initialization must also connect to this owner.

## Selected long music input (2026-09-27)

The real `SolTrain_Start` program calls `MScript::PlayMusic` with
`solarian_game14.wav`. The bounded external-input mode of the existing
window-manager check initially rejected that lawful external Steam
archive entry. Read-only inspection measured a 10028080-byte IAC WAV,
22050 Hz stereo and about 909.6 seconds. The host FFmpeg CLI decoded
a temporary out-of-repository copy to a null sink without an error;
the copy was removed. The native decoder then succeeded after only its
decoded-output limit changed. This identifies the previous 64-MiB
limit as a Linux adapter restriction, not evidence of damaged retail
media or a CQ2 music-schema defect.

`NativeIacWave` still caps the compressed input at 64 MiB and now caps
decoded PCM at 256 MiB. `NativeStreamer::Open` accepts the same bounded
decoded size before creating its SDL stream. This whole-file strategy
can hold the decoded vector, retained loop buffer and SDL queue at once;
incremental streaming and an aggregate memory budget remain open for
larger or multiple tracks. A long external input in the existing probe
now checks decode fingerprint, `IStreamer::Open`, 150 ms of event-pumped
`PLAYING`, `Stop` and `CloseHandle` instead of waiting for a 15-minute
completion. Short external speech retains the completion check.

On Linux x86_64, GCC 16.2.1 and Clang 22.1.8 built the
FFmpeg-enabled `frontierx_native_window_manager_probe`. Each executed
the external music path with `SDL_VIDEODRIVER=dummy` and
`SDL_AUDIODRIVER=dummy`: 160448512 F32 PCM bytes at 22050 Hz stereo,
identical FNV1a64 `16776800164497827250`, followed by successful
start, stop and close. Each compiler also completed playback of the
selected `STMG03.wav` speech through the same native `IStreamer`, with
1487872 decoded bytes and identical FNV1a64
`10538922693386034611`. The two complete Linux Debug builds and CTest
presets passed; three command-sandbox cursor/GPU checks skipped per
preset and are not runtime evidence. Two additional host-audio runs
emitted a PulseAudio operation-permission error and remained active
for over a minute; they were interrupted with exit 130. Therefore
this change has SDL dummy-device runtime evidence, not audible host
hardware evidence. The productive App `MusicManager` and
`SolTrain_Start` did not execute. Native Windows Debug/Final, module
ABI and retail playback comparison were not run.
