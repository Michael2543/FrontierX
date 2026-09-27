#ifndef FRONTIERX_NATIVE_GAME_SCENE_H
#define FRONTIERX_NATIVE_GAME_SCENE_H

#include <cstdint>
#include <filesystem>
#include <string>

struct ICOManager;

namespace FrontierX
{
struct NativeGameSystem;
struct SdlGpuReadback;

// A startup composition of production game owners over one UTF mesh.
// The callback pumps shell events and returns false when the window should close.
using NativeSceneContinue = bool (*)(void * context);
bool RenderNativeGameScene(ICOManager * components, NativeGameSystem * gameSystem,
                           std::uint32_t windowToken,
                           const std::filesystem::path & materials,
                           const std::filesystem::path & mesh,
                           const char * vertexShader,
                           const char * fragmentShader,
                           const char * extendedVertexShader,
                           const char * gameVertexShader,
                           NativeSceneContinue continueScene, void * loopContext,
                           std::uint32_t & presentedFrames,
                           SdlGpuReadback & image, std::string & error);
}

#endif
