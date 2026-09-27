#pragma once
#include <SDL3/SDL_audio.h>
#include <vector>

namespace FrontierX
{
// Decode only IAC inside WAV, from bounded in-memory input. Output is transactional.
bool DecodeNativeIacWave(const std::vector<Uint8> & bytes, SDL_AudioSpec & spec,
                         std::vector<Uint8> & samples);
}
