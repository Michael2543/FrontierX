#include "CQTrace.h"
#include "MaterialTextureFixture.h"
#include "NativeAppImage.h"
#include "FrontierX/GameMaterialManager.h"
#include "FrontierX/GameTextureManager.h"
#include "FrontierX/NativeEngine.h"
#include "FrontierX/NativeHardpoint.h"
#include "FrontierX/NativeMeshManager.h"
#include "FrontierX/NativeAnimation.h"
#include "FrontierX/NativeChannel.h"
#include "FrontierX/LegacyRenderSubmission.h"
#include "FrontierX/NativeGameVertexBuffer.h"
#include "FrontierX/NativeGameIndexBuffer.h"
#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/NativeMathEngine.h"
#include "FrontierX/NativeRenderPipeline.h"
#include "FrontierX/NativeRenderPrimitive.h"
#include "FrontierX/NativeRenderManager.h"
#include "FrontierX/NativePolyMesh.h"
#include "FrontierX/NativeUtfFileSystem.h"
#include "FrontierX/UtfMetadata.h"
#include "Include/NativeAppGlobals.h"
#include "Include/NativeShipVisualTransform.h"
#include "Include/NativeShipWarpInTransform.h"
#include "Include/NativeWeaponOrientation.h"
#include "Include/NativeObjectControlFrame.h"
#include "Include/NativeObjectSavedYaw.h"
#include "Include/NativeAsteroidDistanceOrder.h"
#include "BRadix.h"
#include <DEffectOpts.h>
#include "MeshRender.h"
#include "MyVertex.h"
#include "IVertexBuffer.h"
#include "3dmath.h"
#include "FDump.h"
#include "ICamera.h"
#include "Camera.h"
#include "IHardPoint.h"
#include "IAnim.h"
#include "IMeshManager.h"
#include "SysConsumerDesc.h"
#include "SYSTEM.H"
#include "HPEnum.h"
#include "ENGINE.H"
#include "anim2d.h"
#include "IMaterialManager.h"
#include "IImageReader.h"
#include "IProfileParser.h"
#include "ITextureLibrary.h"
#include "lightman.h"
#include "IRenderPrimitive.h"
#include "renderer.h"
#include "ArchHolder.h"
#include "corners.h"
#include "IRenderComponent.h"
#include "RPUL/MTPrimitiveBuilder.h"
#include "PolyMeshFixture.h"
#include "TComponent.h"
#include "TManager.h"
#include "View2d.h"
#include "xmesh.h"
#include "matrix4.h"
#if defined(FRONTIERX_GAME_GPU)
#include "FrontierX/RenderSmokeScene.h"
#include "FrontierX/SdlGpuRenderer.h"
#include <SDL3/SDL.h>
#endif

#include <array>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <iostream>
#include <iterator>
#include <string>
#include <stdexcept>
#include <vector>

void UpdateRenderEffects(SINGLE dt);
extern SINGLE puffPercent;

void COMAPI CreateTGAReader(IImageReader **reader);
void COMAPI CreateBMPReader(IImageReader **reader);

namespace
{
struct ProbeAppVertexBufferManager : ICQ_VB_Manager
{
    IVertexBufferOwner *owner = nullptr;
    unsigned adds = 0U;
    unsigned deletes = 0U;

    void Add(IVertexBufferOwner *value) override
    {
        owner = value;
        ++adds;
    }

    void Delete(IVertexBufferOwner *value) override
    {
        if (owner == value) owner = nullptr;
        ++deletes;
    }
};

struct AppStateCapture
{
    FrontierX::RenderStateSnapshot render{};
    FrontierX::TextureStageSnapshot stage{};
    FrontierX::SamplerStateSnapshot sampler{};
    U32 texture = 0U;
    unsigned stageCalls = 0U;
    unsigned samplerCalls = 0U;
    unsigned rectangleDraws = 0U;
    std::size_t rectangleVertices = 0U;
    std::vector<FrontierX::RenderVertexP3DiffuseT2> rectangleGeometry;
    unsigned lineDraws = 0U;
    std::array<FrontierX::RenderVertexP3DiffuseT2, 2> lineVertices{};
    Matrix4 projection{true};
    std::array<int, 4> viewport{{0, 0, 0, 0}};
};

GENRESULT DACOM_CDECL CaptureApp2DDraw(
    void * context, const FrontierX::RenderSubmissionView & view)
{
    auto & capture = *static_cast<AppStateCapture *>(context);
    if (!view.IsValid()) return GR_INVALID_PARMS;
    if (view.topology == FrontierX::PrimitiveTopology::LineList)
    {
        if (view.vertexCount != 2U) return GR_INVALID_PARMS;
        ++capture.lineDraws;
        std::copy(view.vertices, view.vertices + 2U, capture.lineVertices.begin());
    }
    else
    {
        ++capture.rectangleDraws;
        capture.rectangleVertices = view.vertexCount;
        capture.rectangleGeometry.insert(capture.rectangleGeometry.end(),
            view.vertices, view.vertices + view.vertexCount);
    }
    return GR_OK;
}

GENRESULT DACOM_CDECL CaptureApp2DModelview(void *, const Transform &)
{
    return GR_OK;
}

GENRESULT DACOM_CDECL CaptureApp2DProjection(void * context, const Matrix4 & value)
{
    static_cast<AppStateCapture *>(context)->projection = value;
    return GR_OK;
}

GENRESULT DACOM_CDECL CaptureApp2DViewport(
    void * context, int x, int y, int width, int height)
{
    static_cast<AppStateCapture *>(context)->viewport = {{x, y, width, height}};
    return GR_OK;
}

GENRESULT DACOM_CDECL CaptureAppRenderState(
    void * context, const FrontierX::RenderStateSnapshot & value)
{
    static_cast<AppStateCapture *>(context)->render = value;
    return GR_OK;
}

GENRESULT DACOM_CDECL CaptureAppTextureStage(
    void * context, const FrontierX::TextureStageSnapshot & value)
{
    auto &capture = *static_cast<AppStateCapture *>(context);
    capture.stage = value;
    ++capture.stageCalls;
    return GR_OK;
}

GENRESULT DACOM_CDECL CaptureAppTextureBind(void * context, U32 stage, U32 texture)
{
    auto &capture = *static_cast<AppStateCapture *>(context);
    if (stage > 1U || (stage == 1U && texture != 0U)) return GR_NOT_IMPLEMENTED;
    if (stage == 0U) capture.texture = texture;
    return GR_OK;
}

GENRESULT DACOM_CDECL CaptureAppSamplerState(
    void * context, const FrontierX::SamplerStateSnapshot & value)
{
    auto &capture = *static_cast<AppStateCapture *>(context);
    capture.sampler = value;
    ++capture.samplerCalls;
    return GR_OK;
}

struct TextureCapture
{
    std::vector<U32> liveHandles;
    std::array<std::uint8_t, 1024> pixels{};
    unsigned creates = 0U;
    unsigned uploads = 0U;
    unsigned destroys = 0U;
    U32 lastUploadWidth = 0U, lastUploadHeight = 0U;
    std::size_t lastUploadBytes = 0U;
    U32 firstHandle = 0U;
#if defined(FRONTIERX_GAME_GPU)
    FrontierX::SdlGpuRenderer * gpu = nullptr;
#endif
};

GENRESULT CreateTexture(void *context, U32 width, U32 height, U32 &handle)
{
    auto & capture = *static_cast<TextureCapture *>(context);
#if defined(FRONTIERX_GAME_GPU)
    if (capture.gpu)
    {
        const GENRESULT result = capture.gpu->CreateTexture(width, height, handle);
        if (result != GR_OK) return result;
        if (capture.firstHandle == 0U) capture.firstHandle = handle;
        ++capture.creates;
        return GR_OK;
    }
#endif
    if (!((width == 4U && height == 4U) || (width == 2U && height == 2U) || (width == 16U && height == 16U)))
        return GR_INVALID_PARMS;
    handle = 64U;
    while (std::find(capture.liveHandles.begin(), capture.liveHandles.end(), handle) !=
           capture.liveHandles.end()) ++handle;
    capture.liveHandles.push_back(handle);
    ++capture.creates;
    if (!capture.firstHandle) capture.firstHandle = handle;
    return GR_OK;
}

GENRESULT UploadTexture(void *context, U32 handle, const std::uint8_t *pixels,
                        U32 width, U32 height, std::size_t byteCount)
{
    auto &capture = *static_cast<TextureCapture *>(context);
#if defined(FRONTIERX_GAME_GPU)
    if (capture.gpu)
    {
        const GENRESULT result = capture.gpu->ReplaceTexture(handle,
            {pixels, width, height, byteCount});
        if (result != GR_OK) return result;
        ++capture.uploads;
        capture.lastUploadWidth = width;
        capture.lastUploadHeight = height;
        capture.lastUploadBytes = byteCount;
        if ((width == 4U && height == 4U && byteCount == 64U) ||
            (width == 2U && height == 2U && byteCount == 16U) ||
            (width == 16U && height == 16U && byteCount == 1024U))
            std::memcpy(capture.pixels.data(), pixels, byteCount);
        return GR_OK;
    }
#endif
    if (std::find(capture.liveHandles.begin(), capture.liveHandles.end(), handle) ==
            capture.liveHandles.end() ||
        !((width == 4U && height == 4U && byteCount == 64U) ||
          (width == 2U && height == 2U && byteCount == 16U) ||
            (width == 16U && height == 16U && byteCount == 1024U)))
        return GR_INVALID_PARMS;
    ++capture.uploads;
    capture.lastUploadWidth = width;
    capture.lastUploadHeight = height;
    capture.lastUploadBytes = byteCount;
    std::memcpy(capture.pixels.data(), pixels, byteCount);
    return GR_OK;
}

GENRESULT DestroyTexture(void *context, U32 handle)
{
    auto & capture = *static_cast<TextureCapture *>(context);
#if defined(FRONTIERX_GAME_GPU)
    if (capture.gpu)
    {
        const GENRESULT result = capture.gpu->DestroyTexture(handle);
        if (result == GR_OK) ++capture.destroys;
        return result;
    }
#endif
    const auto found = std::find(capture.liveHandles.begin(), capture.liveHandles.end(), handle);
    if (found == capture.liveHandles.end()) return GR_INVALID_PARMS;
    capture.liveHandles.erase(found);
    ++capture.destroys;
    return GR_OK;
}

unsigned gameDrawCalls = 0;
FrontierX::RenderStateSnapshot gameMeshStateAtSubmit;
extern FrontierX::RenderStateSnapshot capturedLegacyState;
unsigned gameMeshDrawCalls = 0;
unsigned gameStandardDrawCalls = 0;
unsigned nativeLegacyDrawCalls = 0;
std::vector<FrontierX::RenderVertexP3DiffuseT4> nativeLegacyVertices;
std::size_t gameMeshIndices = 0U;
bool gameMeshSecondUvMatches = false;
bool acceptPolyMeshCpuDraw = false;
std::vector<U32> gameMeshDiffuse;
std::vector<FrontierX::RenderVertexP3N3DiffuseT5> gameStandardVertices;
std::vector<U16> gameStandardIndices;

static_assert(sizeof(MTVERTEX) ==
                  sizeof(FrontierX::RenderVertexP3DiffuseSpecularT4));
static_assert(offsetof(MTVERTEX, pos) ==
                  offsetof(FrontierX::RenderVertexP3DiffuseSpecularT4, x));
static_assert(offsetof(MTVERTEX, color) ==
                  offsetof(FrontierX::RenderVertexP3DiffuseSpecularT4, diffuseBgra));
static_assert(offsetof(MTVERTEX, scolor) ==
                  offsetof(FrontierX::RenderVertexP3DiffuseSpecularT4, specularBgra));
static_assert(offsetof(MTVERTEX, u) ==
                  offsetof(FrontierX::RenderVertexP3DiffuseSpecularT4, u));
static_assert(offsetof(MTVERTEX, u2) ==
                  offsetof(FrontierX::RenderVertexP3DiffuseSpecularT4, u2));
static_assert(offsetof(MTVERTEX, v2) + sizeof(float) == sizeof(MTVERTEX));
static_assert(MTVERTEX_FVFFLAGS == FrontierX::LegacyD3d9::kMtVertexFvf);

GENRESULT DACOM_CDECL CaptureGameMeshDraw(
    void *, const FrontierX::RenderSubmissionView3 & submission)
{
    if (!submission.IsValid()) return GR_INVALID_PARMS;
    ++gameMeshDrawCalls;
    gameMeshStateAtSubmit = capturedLegacyState;
    gameMeshIndices = submission.indexCount;
    gameMeshSecondUvMatches = true;
    gameMeshDiffuse.clear();
    for (std::size_t index = 0U; index < submission.vertexCount; ++index)
    {
        gameMeshDiffuse.push_back(submission.vertices[index].diffuseBgra);
        gameMeshSecondUvMatches &=
            submission.vertices[index].u2 == submission.vertices[index].u &&
            submission.vertices[index].v2 == submission.vertices[index].v;
    }
    return acceptPolyMeshCpuDraw ? GR_OK : GR_NOT_IMPLEMENTED;
}

GENRESULT DACOM_CDECL CaptureGameStandardDraw(
    void *, const FrontierX::RenderSubmissionView4 & submission)
{
    if (!submission.IsValid()) return GR_INVALID_PARMS;
    ++gameStandardDrawCalls;
    gameStandardVertices.assign(submission.vertices,
                                submission.vertices + submission.vertexCount);
    gameStandardIndices.assign(submission.indices,
                               submission.indices + submission.indexCount);
    return GR_OK;
}
bool rejectNativeLegacyDraw = false;
GENRESULT DACOM_CDECL CaptureNativeLegacyDraw(
    void *, const FrontierX::RenderSubmissionView2 &submission)
{
    if (!submission.IsValid()) return GR_INVALID_PARMS;
    ++nativeLegacyDrawCalls;
    nativeLegacyVertices.assign(submission.vertices,
                               submission.vertices + submission.vertexCount);
    return rejectNativeLegacyDraw ? GR_GENERIC : GR_OK;
}
// The checked native primitive validates each state before forwarding to this
// CPU receiver. The old VB fixture had no state receiver; the original legacy
// caller now requires an explicit executable backend for those source states.
FrontierX::RenderStateSnapshot capturedLegacyState;
FrontierX::TextureStageSnapshot capturedLegacyStage;
FrontierX::SamplerStateSnapshot capturedLegacySampler[2];
U32 capturedLegacyTextures[2]{};
unsigned capturedLegacyStateCalls = 0U;
bool cpuMaterialBackendLive = false;
unsigned rejectCpuStageCall = 0U;
GENRESULT DACOM_CDECL CaptureLegacyState(void *, const FrontierX::RenderStateSnapshot &state)
{ if (!cpuMaterialBackendLive) return GR_NOT_IMPLEMENTED;
  capturedLegacyState = state; ++capturedLegacyStateCalls; return GR_OK; }
GENRESULT DACOM_CDECL CaptureLegacyStage(void *, const FrontierX::TextureStageSnapshot &state)
{ if (!cpuMaterialBackendLive) return GR_NOT_IMPLEMENTED;
  if (rejectCpuStageCall && --rejectCpuStageCall == 0U) return GR_GENERIC;
  capturedLegacyStage = state; return GR_OK; }
GENRESULT DACOM_CDECL CaptureLegacySampler(void *, const FrontierX::SamplerStateSnapshot &state)
{ if (!cpuMaterialBackendLive) return GR_NOT_IMPLEMENTED;
  capturedLegacySampler[state.stage] = state; return GR_OK; }
GENRESULT DACOM_CDECL CaptureLegacyTexture(void *, U32 stage, U32 texture)
{ if (!cpuMaterialBackendLive) return GR_NOT_IMPLEMENTED;
  capturedLegacyTextures[stage] = texture; return GR_OK; }
struct GameTransformCapture
{
    unsigned modelviewCalls = 0U;
    unsigned projectionCalls = 0U;
    Vector translation{};
    float projectionX = 0.0F;
#if defined(FRONTIERX_GAME_GPU)
    FrontierX::SdlGpuRenderer * gpu = nullptr;
    bool submitAppGpu = false;
    bool submitNativeLegacyGpu = false;
    bool capturePolyMeshGpu = false;
    FrontierX::SdlGpuReadback polyMeshImage;
    FrontierX::SdlGpuReadback nativeLegacyImage;
    unsigned rejectStageAfter = 0U;
    IRenderPrimitive * primitive = nullptr;
    bool appStateAtSubmit = false;
    FrontierX::SdlGpuReadback appImageAtSubmit;
    bool appPresentedAtSubmit = false;
    std::string appGpuError;
#endif
};

#if defined(FRONTIERX_GAME_GPU)
GENRESULT DACOM_CDECL SubmitNativeLegacyGpu(
    void *context, const FrontierX::RenderSubmissionView2 &submission)
{
    const GENRESULT result = CaptureNativeLegacyDraw(context, submission);
    if (result != GR_OK) return result;
    auto *capture = static_cast<GameTransformCapture *>(context);
    if (!capture || !capture->submitNativeLegacyGpu || !capture->gpu) return GR_OK;
    const GENRESULT submitted = capture->gpu->Submit(submission);
    if (submitted != GR_OK) return submitted;
    return capture->gpu->Readback64(capture->nativeLegacyImage, capture->appGpuError)
        ? GR_OK : GR_GENERIC;
}
GENRESULT DACOM_CDECL SubmitGameStandardGpu(
    void * context, const FrontierX::RenderSubmissionView4 & submission)
{
    const GENRESULT captured = CaptureGameStandardDraw(context, submission);
    if (captured != GR_OK) return captured;
    auto * capture = static_cast<GameTransformCapture *>(context);
    if (capture && capture->submitAppGpu && capture->primitive)
    {
        U32 cull = 0U;
        U32 factor = 0U;
        U32 colorArgument = 0U;
        U32 alphaArgument = 0U;
        capture->appStateAtSubmit =
            capture->primitive->get_render_state(D3DRS_CULLMODE, &cull) == GR_OK &&
            capture->primitive->get_render_state(D3DRS_TEXTUREFACTOR, &factor) == GR_OK &&
            capture->primitive->get_texture_stage_state(
                0U, D3DTSS_COLORARG1, &colorArgument) == GR_OK &&
            capture->primitive->get_texture_stage_state(
                0U, D3DTSS_ALPHAARG1, &alphaArgument) == GR_OK &&
            cull == D3DCULL_CW && factor == 0xFF40A0E0U &&
            colorArgument == D3DTA_TFACTOR && alphaArgument == D3DTA_TFACTOR;
    }
    if (!capture || !capture->gpu || !capture->submitAppGpu) return GR_OK;
    const GENRESULT result = capture->gpu->Submit(submission);
    if (result != GR_OK) return result;
    return capture->gpu->Readback64(capture->appImageAtSubmit,
                                   capture->appGpuError) &&
                   capture->gpu->Present(capture->appPresentedAtSubmit,
                                         capture->appGpuError)
               ? GR_OK : GR_GENERIC;
}

GENRESULT DACOM_CDECL SubmitGameMeshGpu(
    void * context, const FrontierX::RenderSubmissionView3 & submission)
{
    const GENRESULT captured = CaptureGameMeshDraw(context, submission);
    if (captured == GR_INVALID_PARMS) return captured;
    auto * capture = static_cast<GameTransformCapture *>(context);
    if (!capture || !capture->gpu) return GR_INVALID_PARMS;
    const GENRESULT result = capture->gpu->Submit(submission);
    if (result != GR_OK || !capture->capturePolyMeshGpu) return result;
    return capture->gpu->Readback64(capture->polyMeshImage, capture->appGpuError)
        ? GR_OK : GR_GENERIC;
}

GENRESULT DACOM_CDECL ApplyGameStateGpu(
    void * context, const FrontierX::RenderStateSnapshot & state)
{
    capturedLegacyState = state;
    return static_cast<GameTransformCapture *>(context)->gpu->SetRenderState(state);
}

GENRESULT DACOM_CDECL ApplyGameStageGpu(
    void * context, const FrontierX::TextureStageSnapshot & state)
{
    auto * capture = static_cast<GameTransformCapture *>(context);
    if (capture->rejectStageAfter && --capture->rejectStageAfter == 0U)
    {
        return GR_GENERIC;
    }
    return capture->gpu->SetTextureStageState(state);
}

GENRESULT DACOM_CDECL ApplyGameTextureGpu(void * context, U32 stage, U32 texture)
{
    return static_cast<GameTransformCapture *>(context)->gpu->BindTexture(stage, texture);
}

GENRESULT DACOM_CDECL ApplyGameSamplerGpu(
    void * context, const FrontierX::SamplerStateSnapshot & state)
{
    return static_cast<GameTransformCapture *>(context)->gpu->SetSamplerState(state);
}

struct GameGpuRuntime
{
    SDL_Window * window = nullptr;
    bool videoInitialized = false;
    FrontierX::SdlGpuRenderer renderer;
    ~GameGpuRuntime()
    {
        renderer.Shutdown();
        if (window) SDL_DestroyWindow(window);
        if (videoInitialized) SDL_Quit();
    }
};

bool WriteP6(const std::filesystem::path & path,
             const FrontierX::SdlGpuReadback & image)
{
    if (!image.rgbValid) return false;
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    output << "P6\n" << FrontierX::SdlGpuReadback::Width << ' '
           << FrontierX::SdlGpuReadback::Height << "\n255\n";
    output.write(reinterpret_cast<const char *>(image.rgb8.data()),
                 static_cast<std::streamsize>(image.rgb8.size()));
    output.close();
    return output.good();
}
#endif

GENRESULT DACOM_CDECL CaptureGameModelview(void * context,
                                            const Transform & modelview)
{
    if (!context) return GR_INVALID_PARMS;
    auto & capture = *static_cast<GameTransformCapture *>(context);
    ++capture.modelviewCalls;
    capture.translation = modelview.translation;
#if defined(FRONTIERX_GAME_GPU)
    if (capture.gpu) return capture.gpu->SetModelview(modelview);
#endif
    return GR_OK;
}

GENRESULT DACOM_CDECL CaptureGameProjection(void * context,
                                             const Matrix4 & projection)
{
    if (!context) return GR_INVALID_PARMS;
    auto & capture = *static_cast<GameTransformCapture *>(context);
    ++capture.projectionCalls;
    capture.projectionX = projection.d[0][0];
#if defined(FRONTIERX_GAME_GPU)
    if (capture.gpu) return capture.gpu->SetProjection(projection);
#endif
    return GR_OK;
}

GENRESULT DACOM_CDECL RejectGameDraw(void *, const FrontierX::RenderSubmissionView &)
{
    ++gameDrawCalls;
    return GR_NOT_IMPLEMENTED;
}

struct ProbeCamera final : ICamera
{
    U32 references = 1U;
    mutable unsigned inverseQueries = 0U;
    Transform transform;
    Transform inverse;
    Vector position{0.0F, 0.0F, 5.0F};
    Vector look{0.0F, 0.0F, 0.0F};
    ViewRect pane{0, 0, 63, 63};
    bool projectCenter = false;

    ProbeCamera()
    {
        transform.translation.z = 5.0F;
        inverse.translation.z = -5.0F;
    }

    GENRESULT COMAPI QueryInterface(const C8 *name, void **result) override
    {
        if (!result) return GR_INVALID_PARMS;
        *result = nullptr;
        if (!name || std::strcmp(name, IID_ICamera) != 0) return GR_INTERFACE_UNSUPPORTED;
        *result = static_cast<ICamera *>(this);
        AddRef();
        return GR_OK;
    }
    U32 COMAPI AddRef() override { return ++references; }
    U32 COMAPI Release() override { return --references; }
    const Vector & COMAPI get_position() const override { return position; }
    const Transform & COMAPI get_transform() const override { return transform; }
    const Vector & COMAPI get_look_pos() const override { return look; }
    const ViewRect * COMAPI get_pane() const override { return &pane; }
    SINGLE COMAPI get_fovx() const override { return 1.0F; }
    SINGLE COMAPI get_fovy() const override { return 1.0F; }
    SINGLE COMAPI get_znear() const override { return 1.0F; }
    SINGLE COMAPI get_zfar() const override { return 100.0F; }
    SINGLE COMAPI get_aspect() const override { return 1.0F; }
    SINGLE COMAPI get_hpc() const override { return 32.0F; }
    SINGLE COMAPI get_vpc() const override { return -32.0F; }
    bool COMAPI point_to_screen(float &x, float &y, float &depth, const Vector &) const override
    {
        if (!projectCenter) return false;
        x = 32.0F;
        y = 32.0F;
        depth = 5.0F;
        return true;
    }
    void COMAPI screen_to_point(Vector &point, float, float) const override
    {
        point = look;
    }
    vis_state COMAPI object_visibility(const Vector &, float) const override
    {
        return VS_FULLY_VISIBLE;
    }
    Transform COMAPI get_inverse_transform() const override
    {
        ++inverseQueries;
        return inverse;
    }
};

// Typed receiver for TreeRender's original CAMERA position query. Unsupported
// interface slots fail explicitly; it does not stand in for gameplay Camera.
struct PreviewCameraPosition final : IBaseCamera
{
    const ProbeCamera &source;
    mutable unsigned positionQueries = 0;
    explicit PreviewCameraPosition(const ProbeCamera &camera) : source(camera) {}
    GENRESULT COMAPI QueryInterface(const C8 *, void **out) override
    { if (out) *out = nullptr; return GR_INTERFACE_UNSUPPORTED; }
    U32 COMAPI AddRef() override { std::abort(); }
    U32 COMAPI Release() override { std::abort(); }
    _pane *COMAPI GetPane() const override { return nullptr; }
    BOOL32 COMAPI SetPane(_pane *, BOOL32) override { return FALSE; }
    BOOL32 COMAPI SetPaneRef(_pane *, BOOL32) override { return FALSE; }
    BOOL32 COMAPI SetInterfaceBarHeight(U32) override { return FALSE; }
    const Transform *COMAPI GetTransform() const override { return &source.transform; }
    const Transform *COMAPI GetModelTransform() const override { return nullptr; }
    const Transform *COMAPI GetInverseTransform() const override { return &source.inverse; }
    const Transform *COMAPI GetInverseWorldTransform() const override { return nullptr; }
    const Transform *COMAPI GetWorldTransform() const override { return nullptr; }
    const Transform *COMAPI GetInverseSectorTransform() const override { return nullptr; }
    const Transform *COMAPI GetSectorTransform() const override { return nullptr; }
    SINGLE COMAPI GetHorizontalFOV() const override { return source.get_fovx(); }
    BOOL32 COMAPI SetHorizontalFOV(SINGLE, BOOL32) override { return FALSE; }
    SINGLE COMAPI GetVerticalFOV() const override { return source.get_fovy(); }
    BOOL32 COMAPI SetVerticalFOV(SINGLE, BOOL32) override { return FALSE; }
    BOOL32 COMAPI SetOrbitPosition() override { return FALSE; }
    BOOL32 COMAPI SetWorldRotation(SINGLE, BOOL32) override { return FALSE; }
    BOOL32 COMAPI SetWorldRotationPitchRoll(SINGLE, SINGLE, SINGLE, BOOL32) override { return FALSE; }
    SINGLE COMAPI GetWorldRotation() const override { std::abort(); }
    void COMAPI GetOrientation(SINGLE *, SINGLE *, SINGLE *) const override { std::abort(); }
    BOOL32 COMAPI SetOrientation(SINGLE, SINGLE, SINGLE, BOOL32) override { return FALSE; }
    Vector COMAPI GetPosition() const override { ++positionQueries; return source.position; }
    BOOL32 COMAPI SetRotatedPosition(const Vector *, BOOL32) override { return FALSE; }
    Vector COMAPI GetRotatedPosition() const override { std::abort(); }
    BOOL32 COMAPI SetPosition(const Vector *, BOOL32) override { return FALSE; }
    BOOL32 COMAPI MoveForward(SINGLE, BOOL32) override { return FALSE; }
    BOOL32 COMAPI AddToPitch(SINGLE, BOOL32) override { return FALSE; }
    TRANSRESULT COMAPI PointToScreen(const Vector &, S32 *, S32 *, const Transform *) const override { return OUT_PANE; }
    BOOL32 COMAPI PaneToPoints(Vector &, Vector &, Vector &, Vector &) const override { return FALSE; }
    BOOL32 COMAPI ScreenToPoint(SINGLE &, SINGLE &, SINGLE) const override { return FALSE; }
    U32 COMAPI GetStateInfo(CAMERA_DATA *) const override { return 0; }
    BOOL32 COMAPI SetStateInfo(const CAMERA_DATA *, BOOL32) override { return FALSE; }
    GENRESULT COMAPI SetPerspective() override { return GR_NOT_IMPLEMENTED; }
    GENRESULT COMAPI SetModelView(const Transform *) override { return GR_NOT_IMPLEMENTED; }
    BOOL32 COMAPI SetLookAtPosition(const Vector &) override { return FALSE; }
    Vector COMAPI GetLookAtPosition() const override { return source.look; }
    BOOL32 COMAPI SnapToTargetRotation() override { return FALSE; }
    SINGLE COMAPI GetCameraLOD() override { std::abort(); }
    bool SphereInFrustrum(const Vector &, float, float &, float &, float &, float &) override { return false; }
    bool SphereInFrustrumFast(const Vector &, float) override { return false; }
    void ShakeCamera(SINGLE, SINGLE) override { std::abort(); }
    void COMAPI SetCameraDefaults(CAMERA_DATA &) const override { std::abort(); }
    void COMAPI SetCameraToDefaults() override { std::abort(); }
};

void Word(std::vector<unsigned char> &bytes, U32 value)
{
    for (unsigned shift = 0; shift < 32; shift += 8)
        bytes.push_back(static_cast<unsigned char>(value >> shift));
}

// MaterialManager consumes the same generated input from unpacked source data.
// Required Cloud/Stasis/Mine/Harvest graphics use this production ANM owner.
// Existing timing/RNG checks cannot detect unresolved frames or malformed UV IO.
bool WriteWeaverRootFixture(const std::filesystem::path &path)
{
    const auto channel = path / "Animation/Script/Sc_build/Object map 0/Channel";
    std::filesystem::create_directories(channel);
    std::ofstream parent(channel.parent_path() / "Parent name", std::ios::binary);
    parent.write("Root", 5);
    const U32 header[3] = {2U, 0x3F800000U, 6U}; // two vector/quaternion frames, one second
    std::ofstream head(channel / "Header", std::ios::binary);
    head.write(reinterpret_cast<const char *>(header), sizeof(header));
    // Independently generated root pitch reproduces the Weaver's authored
    // 65-degree build motion without copying retail frame data into the test.
    const float halfAngle = 32.5F * MUL_DEG_TO_RAD;
    const float frames[14] = {0,0,0, 1,0,0,0,
                             0,0,0, std::cos(halfAngle),std::sin(halfAngle),0,0};
    std::ofstream frame(channel / "Frames", std::ios::binary);
    frame.write(reinterpret_cast<const char *>(frames), sizeof(frames));
    const auto jointChannel = path / "Animation/Script/Sc_build/Joint map 0/Channel";
    std::filesystem::create_directories(jointChannel);
    std::ofstream jointParent(jointChannel.parent_path() / "Parent name", std::ios::binary);
    jointParent.write("Root", 5);
    std::ofstream jointChild(jointChannel.parent_path() / "Child name", std::ios::binary);
    jointChild.write("Child", 6);
    std::ofstream jointHead(jointChannel / "Header", std::ios::binary);
    jointHead.write(reinterpret_cast<const char *>(header), sizeof(header));
    std::ofstream jointFrame(jointChannel / "Frames", std::ios::binary);
    jointFrame.write(reinterpret_cast<const char *>(frames), sizeof(frames));
    const auto moveChannel = path / "Animation/Script/Sc_move/Object map 0/Channel";
    std::filesystem::create_directories(moveChannel);
    std::ofstream moveParent(moveChannel.parent_path() / "Parent name", std::ios::binary);
    moveParent.write("Root", 5);
    std::ofstream moveHead(moveChannel / "Header", std::ios::binary);
    moveHead.write(reinterpret_cast<const char *>(header), sizeof(header));
    float motion[14];
    std::copy(frames, frames + 14, motion);
    motion[7] = 2; motion[8] = 3; motion[9] = 4;
    std::ofstream moveFrame(moveChannel / "Frames", std::ios::binary);
    moveFrame.write(reinterpret_cast<const char *>(motion), sizeof(motion));
    return parent && head && frame && jointParent && jointChild && jointHead &&
        jointFrame && moveParent && moveHead && moveFrame;
}

bool WriteAnim2DFixture(const std::filesystem::path &path, unsigned mode)
{
    constexpr U32 recordSize = 44U, headerSize = 56U;
    const bool flat = mode >= 6U;
    const U32 recordCount = flat ? 5U : 6U;
    const char *labels[6] = {"/", "Frequency", "UV Set", "Texture names", "UV Rects", "Texture library"};
    if (flat) { labels[2] = mode == 7U ? "Missing texture names" : "Texture names";
        labels[3] = "UV Rects"; labels[4] = "Texture library"; }
    std::string names;
    U32 offsets[6]{};
    for (U32 i = 0; i < recordCount; ++i) { offsets[i] = U32(names.size()); names.append(labels[i], std::strlen(labels[i]) + 1U); }
    std::vector<unsigned char> data, bytes;
    const auto word = [](std::vector<unsigned char> &out, U32 value) {
        for (unsigned shift = 0; shift < 32; shift += 8) out.push_back(static_cast<unsigned char>(value >> shift));
    };
    const auto scalar = [&](float value) { U32 bits; std::memcpy(&bits, &value, sizeof(bits)); word(data, bits); };
    scalar(12.0F);
    const std::string texture = mode == 1U ? "RequiredAnim2DMissing" : "AnimSharedTile";
    for (unsigned name = 0U; name < (mode == 5U ? 65U : 1U); ++name)
    {
        data.insert(data.end(), texture.begin(), texture.end());
        if (mode != 4U) data.push_back(0);
    }
    const U32 nameLength = U32(data.size()) - 4U;
    const U32 uvOffset = U32(data.size());
    word(data, mode == 2U ? 1U : 0U);
    scalar(0.125F); scalar(0.25F); scalar(0.875F); scalar(0.75F);
    if (mode == 3U) data.pop_back();
    const U32 uvLength = U32(data.size()) - uvOffset;
    for (U32 value : {0x20465455U, 0x101U, headerSize, recordCount * recordSize,
        0U, recordSize, headerSize + recordCount * recordSize, U32(names.size()), U32(names.size()),
        headerSize + recordCount * recordSize + U32(names.size()), 0U, 0U, 0U, 0U}) word(bytes, value);
    const auto record = [&](U32 i, U32 next, bool directory, U32 child, U32 size) {
        for (U32 value : {next * recordSize, offsets[i], directory ? 0x10U : 0U, 0U,
            child, size, size, size, 0U, 0U, 0U}) word(bytes, value);
    };
    record(0,0,true,recordSize,0); record(1,2,false,0,4);
    if (flat) { record(2,3,false,4,nameLength); record(3,4,false,uvOffset,uvLength); record(4,0,true,0,0); }
    else { record(2,0,true,3U * recordSize,0); record(3,4,false,4,nameLength);
        record(4,5,false,uvOffset,uvLength); record(5,0,true,0,0); }
    bytes.insert(bytes.end(), names.begin(), names.end()); bytes.insert(bytes.end(), data.begin(), data.end());
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    output.write(reinterpret_cast<const char *>(bytes.data()), std::streamsize(bytes.size()));
    return bool(output);
}

bool WriteDirectoryFixture(const std::filesystem::path &archive, const std::filesystem::path &root)
{
    std::string error;
    for (const char *directory : {"shaders", "textures", "materials"})
    {
        std::filesystem::create_directories(root / directory);
        std::vector<FrontierX::UtfDirectoryEntry> entries;
        if (!FrontierX::ListUtfDirectory(archive, directory, entries, error)) return false;
        for (const auto &entry : entries)
        {
            if (entry.directory) return false;
            std::vector<unsigned char> bytes;
            if (!FrontierX::ReadUtfEntry(archive, std::string(directory) + "/" + entry.name,
                    bytes, error)) return false;
            std::ofstream output(root / directory / entry.name, std::ios::binary | std::ios::trunc);
            output.write(reinterpret_cast<const char *>(bytes.data()),
                static_cast<std::streamsize>(bytes.size()));
            if (!output) return false;
        }
    }
    return true;
}

bool WriteFixture(const std::filesystem::path &path, bool crlf, bool materialEnd)
{
    const char *names[] = {"/", "shaders", "textures", "materials",
                           "Synthetic.fx", "Base.txt", "Tile.tga",
                           "VertexColor.fx", "VertexColor.txt"};
    constexpr U32 recordSize = 44U;
    constexpr U32 headerSize = 56U;
    constexpr U32 count = 9U;
    const std::string ending = crlf ? "\r\n" : "\r";
    const std::string shader = "BEGIN_MAT_SCRIPT" + ending +
        "NAME \"Synthetic\"" + ending +
        "PARAM \"Gain\" FLOATC 0 0 1 0.25" + ending +
        "PARAM \"Diffuse\" TEXTURE 0" + ending +
        "END_MAT_SCRIPT" + ending;
    const std::string material = "1\nSynthetic\nFLOAT 0 0.75 0 \"Gain\"\n"
        "TEXTURE 0 \"Tile.tga\" \"Diffuse\"\n" +
        std::string(materialEnd ? "END\n" : "");
    const std::string vertexColorMaterial =
        "1\nVertex Color with Texture\n"
        "TEXTURE 0 \"Tile.tga\" \"Diffuse + Opacity\"\nEND\n";
    std::ifstream vertexColorInput(FRONTIERX_VERTEX_COLOR_SOURCE,
                                   std::ios::binary);
    if (!vertexColorInput) return false;
    const std::string vertexColorSource{
        std::istreambuf_iterator<char>(vertexColorInput),
        std::istreambuf_iterator<char>()};
    if (vertexColorInput.bad() || vertexColorSource.empty()) return false;
    std::string vertexColor;
    for (char value : vertexColorSource)
    {
        if (value == '\n') vertexColor += ending;
        else vertexColor += value;
    }
    std::vector<unsigned char> tga(18U, 0U);
    tga[2] = 2U;
    tga[12] = 4U;
    tga[14] = 4U;
    tga[16] = 32U;
    tga[17] = 0x20U;
    for (U32 pixel = 0; pixel < 16U; ++pixel)
        tga.insert(tga.end(), {51U, 34U, 17U, 255U});
    std::vector<unsigned char> nameBytes;
    U32 nameOffsets[count]{};
    for (U32 index = 0; index < count; ++index)
    {
        nameOffsets[index] = static_cast<U32>(nameBytes.size());
        const char *name = names[index];
        nameBytes.insert(nameBytes.end(), name, name + std::strlen(name) + 1);
    }
    std::vector<unsigned char> bytes;
    for (U32 value : {0x20465455U, 0x101U, headerSize, count * recordSize,
                      0U, recordSize, headerSize + count * recordSize,
                      static_cast<U32>(nameBytes.size()),
                      static_cast<U32>(nameBytes.size()),
                      headerSize + count * recordSize + static_cast<U32>(nameBytes.size()),
                      0U, 0U, 0U, 0U}) Word(bytes, value);
    for (U32 index = 0; index < count; ++index)
    {
        const bool directory = index < 4U;
        const U32 next = index == 1U || index == 2U ? (index + 1U) * recordSize :
            index == 4U ? 7U * recordSize :
            index == 5U ? 8U * recordSize : 0U;
        const U32 child = index == 0U ? recordSize :
            index == 1U ? 4U * recordSize : index == 2U ? 6U * recordSize :
            index == 3U ? 5U * recordSize :
            index == 5U ? static_cast<U32>(shader.size()) :
            index == 6U ? static_cast<U32>(shader.size() + material.size()) :
            index == 7U ? static_cast<U32>(shader.size() + material.size() +
                                           tga.size()) :
            index == 8U ? static_cast<U32>(shader.size() + material.size() +
                                           tga.size() + vertexColor.size()) : 0U;
        const U32 size = directory ? 0U : index == 4U ?
            static_cast<U32>(shader.size()) :
            index == 5U ? static_cast<U32>(material.size()) :
            index == 6U ? static_cast<U32>(tga.size()) :
            index == 7U ? static_cast<U32>(vertexColor.size()) :
            static_cast<U32>(vertexColorMaterial.size());
        for (U32 value : {next, nameOffsets[index], directory ? 0x10U : 0U,
                          0U, child, size, size, size, 0U, 0U, 0U}) Word(bytes, value);
    }
    bytes.insert(bytes.end(), nameBytes.begin(), nameBytes.end());
    bytes.insert(bytes.end(), shader.begin(), shader.end());
    bytes.insert(bytes.end(), material.begin(), material.end());
    bytes.insert(bytes.end(), tga.begin(), tga.end());
    bytes.insert(bytes.end(), vertexColor.begin(), vertexColor.end());
    bytes.insert(bytes.end(), vertexColorMaterial.begin(),
                 vertexColorMaterial.end());
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    output.write(reinterpret_cast<const char *>(bytes.data()),
                 static_cast<std::streamsize>(bytes.size()));
    return output.good();
}

bool Expect(bool condition, const char *message)
{
    if (!condition) std::cerr << message << '\n';
    return condition;
}

struct VertexConstantCapture
{
    UINT firstRegister = 0U;
    UINT registerCount = 0U;
#if defined(FRONTIERX_GAME_GPU)
    FrontierX::SdlGpuRenderer * gpu = nullptr;
#endif
};

GENRESULT DACOM_CDECL CaptureVertexConstants(
    void * context, UINT firstRegister, const FLOAT * values,
    UINT registerCount)
{
    auto * capture = static_cast<VertexConstantCapture *>(context);
    if (!capture || !values || !registerCount) return GR_INVALID_PARMS;
    capture->firstRegister = firstRegister;
    capture->registerCount = registerCount;
#if defined(FRONTIERX_GAME_GPU)
    if (capture->gpu)
        return capture->gpu->SetVertexConstants(firstRegister, values,
                                                registerCount);
#endif
    return GR_OK;
}

struct ProbeOuter : IDAComponent
{
    U32 references = 1U;
    IRenderPipeline *pipe = nullptr;
    IRenderPrimitive *primitive = nullptr;
    ITextureLibrary *textureLibrary = nullptr;
    IMaterialManager *materialManager = nullptr;
    ILightManager *lightManager = nullptr;
    GENRESULT COMAPI QueryInterface(const C8 *name, void **result) override
    {
        if (!result) return GR_INVALID_PARMS;
        *result = nullptr;
        if (name && std::strcmp(name, IID_IRenderPipeline) == 0 && pipe)
        {
            *result = pipe;
            pipe->AddRef();
            return GR_OK;
        }
        if (name && std::strcmp(name, IID_IRenderPrimitive) == 0 && primitive)
        {
            *result = primitive;
            primitive->AddRef();
            return GR_OK;
        }
        if (name && std::strcmp(name, IID_ITextureLibrary) == 0 && textureLibrary)
        {
            *result = textureLibrary;
            textureLibrary->AddRef();
            return GR_OK;
        }
        if (name && std::strcmp(name, IID_IMaterialManager) == 0 && materialManager)
        {
            *result = materialManager;
            materialManager->AddRef();
            return GR_OK;
        }
        if (name && std::strcmp(name, IID_ILightManager) == 0 && lightManager)
        {
            *result = lightManager;
            lightManager->AddRef();
            return GR_OK;
        }
        return GR_INTERFACE_UNSUPPORTED;
    }
    U32 COMAPI AddRef() override { return ++references; }
    U32 COMAPI Release() override { return --references; }
};
int DACOM_CDECL IgnoreDiagnostic(ErrorCode, const C8 *, ...) { return 0; }
}

extern "C"
{
DA_ERROR_HANDLER FDUMP = IgnoreDiagnostic;
}
extern bool RegisterNativeTextureLibrary(ICOManager *manager);
extern bool RegisterNativeLightManager(ICOManager *manager);

int main(int argc, char **argv)
{
    // Original asteroid background sorting floors nonnegative distances and
    // keeps original index order for ties. The real F8 camera exposed a key
    // above U32 range; compare bounded behavior with the actual old radix owner.
    const std::array<float, 8> boundedDistances{{9.9F, 0.9F, 9.1F, 2.2F,
                                                0.1F, 2.9F, 4000000.0F, 1.5F}};
    const auto untouchedDistances = boundedDistances;
    std::array<U32, 8> oldKeys{};
    for (std::size_t i = 0; i < oldKeys.size(); ++i)
        oldKeys[i] = static_cast<U32>(boundedDistances[i]);
    RadixSort::set_sort_size(static_cast<U32>(oldKeys.size()));
    RadixSort::sort(oldKeys.data(), static_cast<U32>(oldKeys.size()));
    const std::vector<std::uint32_t> oldOrder(RadixSort::index_list,
                                             RadixSort::index_list + oldKeys.size());
    RadixSort::free_arrays();
    auto sourceAsteroidOrder = [](const auto &distances) {
        std::vector<U32> keys;
        for (float distance : distances) keys.push_back(NativeAsteroidDistanceKey(distance));
        RadixSort::set_sort_size(static_cast<U32>(keys.size()));
        RadixSort::sort(keys.data(), static_cast<U32>(keys.size()));
        const std::vector<U32> order(RadixSort::index_list, RadixSort::index_list + keys.size());
        RadixSort::free_arrays();
        return order;
    };
    const auto asteroidOrder = sourceAsteroidOrder(boundedDistances);
    if (!Expect(asteroidOrder == oldOrder && boundedDistances == untouchedDistances &&
                asteroidOrder == std::vector<U32>({1U, 4U, 7U, 3U, 5U, 0U, 2U, 6U}),
                "native asteroid keys retain actual original radix integer-floor ties without mutating distances"))
        return 1;
    const std::array<float, 9> largeDistances{{5.17852e10F, 2.9F,
        std::numeric_limits<float>::max(), 5.17852e10F, 2.1F, 4294967296.0F, 0.0F, -0.0F, 0.99F}};
    if (!Expect(sourceAsteroidOrder(largeDistances) ==
                    std::vector<U32>({6U, 7U, 8U, 1U, 4U, 5U, 0U, 3U, 2U}) &&
                NativeAsteroidDistanceKey(0.0F) == NativeAsteroidDistanceKey(-0.0F),
                "genuine radix preserves large finite asteroid distance order and original floor/zero ties")) return 1;
    for (float invalidDistance : {-1.0F, std::numeric_limits<float>::infinity(),
                                  -std::numeric_limits<float>::infinity(),
                                  std::numeric_limits<float>::quiet_NaN()})
    {
        U32 unchangedKey = 0xABCD1234U;
        bool rejected = false;
        try { unchangedKey = NativeAsteroidDistanceKey(invalidDistance); }
        catch (const std::runtime_error &) { rejected = true; }
        if (!Expect(rejected && unchangedKey == 0xABCD1234U,
                    "invalid asteroid distance rejects before publishing a key")) return 1;
    }
    std::cout << "Original asteroid radix floor/ties and large-distance failure boundaries checked\n";
    PixelFormat opaqueFormat;
    opaqueFormat.awidth = std::numeric_limits<int>::min();
    DDPIXELFORMAT rgbx{};
    rgbx.dwSize = sizeof(rgbx);
    rgbx.dwFlags = DDPF_RGB;
    rgbx.dwRGBBitCount = 32;
    rgbx.dwRBitMask = 0x000000ff;
    rgbx.dwGBitMask = 0x0000ff00;
    rgbx.dwBBitMask = 0x00ff0000;
    opaqueFormat.init(rgbx);
    if (!Expect(opaqueFormat.rwidth == 8 && opaqueFormat.gwidth == 8 &&
            opaqueFormat.bwidth == 8 && opaqueFormat.ar == 0 &&
            opaqueFormat.al == 0 && opaqueFormat.awidth == 0,
            "opaque map texture masks do not read the previous absent alpha width")) return 1;
    if (!Expect(FrontierX::CheckNativeVfxReader(), "original SHP literal/run/skip/empty bounds and truncated input")) return 1;
	PROPERTY numericProperty{"NativeCount", 42U, 0.0F, DAVT_U32};
	DACOM_VARIANT numericVariant(numericProperty);
	PROPERTY pointerProperty{"LegacyPointer", 0x1234U, 0.0F, DAVT_PVOID};
	DACOM_VARIANT pointerVariant(pointerProperty);
	if (!Expect(numericVariant.variantType == DAVT_U32 && numericVariant.longVal == 42U &&
			pointerVariant.variantType == DAVT_EMPTY && pointerVariant.pVoid == nullptr,
			"native 32-bit property values cannot manufacture pointers")) return 1;
	std::array<unsigned char, 18> truncatedTga{};
	truncatedTga[2] = 2U;
	truncatedTga[12] = 4U;
	truncatedTga[14] = 4U;
	truncatedTga[16] = 32U;
	IImageReader *boundedReader = nullptr;
	CreateTGAReader(&boundedReader);
	if (!Expect(boundedReader && boundedReader->LoadImage(truncatedTga.data(),
			static_cast<U32>(truncatedTga.size()), 0U) != GR_OK,
			"production TGA reader rejects truncated mapped pixels")) return 1;
	// Original Static!!Briefing uses uncompressed 8-bit indices and a 24-bit palette.
	std::vector<unsigned char> indexedTga(18U, 0U);
	indexedTga[1] = indexedTga[2] = 1;
	indexedTga[5] = 2;
	indexedTga[7] = 24;
	indexedTga[12] = 2;
	indexedTga[14] = 1;
	indexedTga[16] = 8;
	indexedTga[17] = 0x20;
	indexedTga.insert(indexedTga.end(), {51,34,17,102,85,68,0,1});
	std::array<unsigned char,8> indexedPixels{};
	if (!Expect(boundedReader->LoadImage(indexedTga.data(), indexedTga.size()-1, 0) != GR_OK,
			"briefing TGA rejects truncated index data") ||
		!Expect(boundedReader->LoadImage(indexedTga.data(), indexedTga.size(), 0) == GR_OK &&
			boundedReader->GetImage(PF_RGBA, indexedPixels.data(), nullptr) == GR_OK &&
			indexedPixels == std::array<unsigned char,8>{17,34,51,255,68,85,102,255},
			"original briefing palette decoder preserves both colors")) return 1;
	indexedTga.back() = 2;
	if (!Expect(boundedReader->LoadImage(indexedTga.data(), indexedTga.size(), 0) != GR_OK,
			"briefing TGA rejects an index outside the palette")) return 1;
	boundedReader->Release();
	std::vector<unsigned char> unalignedBmp(1U + 40U + 12U, 0U);
	unsigned char *bmp = unalignedBmp.data() + 1;
	bmp[0] = 40; bmp[4] = 4; bmp[8] = 1; bmp[12] = 1; bmp[14] = 24;
	for (unsigned i = 0; i < 4; ++i)
	{
		bmp[40+i*3] = 51; bmp[41+i*3] = 34; bmp[42+i*3] = 17;
	}
	IImageReader *bmpReader = nullptr;
	CreateBMPReader(&bmpReader);
	std::array<unsigned char,16> bmpPixels{};
	if (!Expect(bmpReader && bmpReader->LoadImage(bmp, 52, 0) == GR_OK &&
			bmpReader->GetImage(PF_RGBA, bmpPixels.data(), nullptr) == GR_OK &&
			bmpPixels[0] == 17 && bmpPixels[1] == 34 && bmpPixels[2] == 51 && bmpPixels[3] == 255,
			"original BMP decoder accepts an unaligned mapped header") ||
		!Expect(bmpReader->LoadImage(bmp, 39, 0) != GR_OK,
			"native BMP rejects a truncated mapped header")) return 1;
	bmpReader->Release();
    const bool cr = argc == 3 && std::strcmp(argv[2], "cr") == 0;
    const bool missingEnd = argc == 3 && std::strcmp(argv[2], "missing-end") == 0;
#if defined(FRONTIERX_GAME_GPU)
    const bool captureRequested = argc == 9 &&
        std::strcmp(argv[7], "--capture-prefix") == 0 && argv[8][0] != '\0';
    const bool gpuMode = (argc == 7 || captureRequested) &&
        std::strcmp(argv[2], "--gpu") == 0;
    GameGpuRuntime gpu;
    if (gpuMode)
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            std::cerr << "SDL video unavailable: " << SDL_GetError() << '\n';
            return 77;
        }
        gpu.videoInitialized = true;
        gpu.window = SDL_CreateWindow("FrontierX game mesh probe", 640, 360,
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_NOT_FOCUSABLE);
        if (!gpu.window)
        {
            std::cerr << "SDL window unavailable: " << SDL_GetError() << '\n';
            return 77;
        }
        const FrontierX::SdlGpuTextureView smokeTexture{
            FrontierX::RenderSmokeTextureRgba.data(),
            FrontierX::RenderSmokeTextureWidth,
            FrontierX::RenderSmokeTextureHeight,
            FrontierX::RenderSmokeTextureRgba.size()};
        std::string gpuError;
        if (!gpu.renderer.Initialize(SDL_GetWindowID(gpu.window), argv[3], argv[4],
                smokeTexture, gpuError, argv[5], argv[6]))
        {
            std::cerr << "SDL GPU unavailable: " << gpuError << '\n';
            return 77;
        }
    }
#else
    constexpr bool gpuMode = false;
#endif
    if (!Expect((argc == 2 || cr || missingEnd || gpuMode) &&
                WriteFixture(argv[1], !cr, !missingEnd), "material fixture creation")) return 1;
    IFileSystem *archive = nullptr;
    std::string error;
    if (!Expect(FrontierX::OpenNativeUtfFileSystem(argv[1], &archive, error) && archive,
                "native material UTF open"))
    {
        std::cerr << error << '\n';
        return 1;
    }

    ICOManager *components = DACOM_Acquire();
    if (!Expect(components && FrontierX::RegisterNativeMathEngine(components) &&
                FrontierX::RegisterNativeGameMaterialManager(components) &&
                FrontierX::RegisterNativeGameTextureManager(components) &&
                RegisterNativeTextureLibrary(components) &&
                FrontierX::RegisterNativePolyMesh(components) &&
                FrontierX::RegisterNativeEngine(components) &&
                FrontierX::RegisterNativeHardpoint(components) &&
                FrontierX::RegisterNativeMeshManager(components) &&
                FrontierX::RegisterNativeAnimation(components) &&
                FrontierX::RegisterNativeChannel(components) &&
                FrontierX::RegisterNativeRenderManager(components),
                "production game material manager registration")) return 1;
    const Transform mathLeft(Vector(1.0F, 2.0F, 3.0F));
    const Transform mathRight(Vector(4.0F, 5.0F, 6.0F));
    const Transform mathCombined = mathLeft * mathRight;
    if (!Expect(MATH_ENGINE() != nullptr &&
                mathCombined.translation.x == 5.0F &&
                mathCombined.translation.y == 7.0F &&
                mathCombined.translation.z == 9.0F,
                "production math provider composes engine transforms")) return 1;
    // Ship hulls, hardpoint glows and Harvester tanks share the original direct
    // Engine Rx90/Ry180 model space. Check basis/attachment numerically so a
    // mirrored helper call alone cannot conceal perpendicular cargo or offset
    // exhaust; simulation world transforms must remain unchanged.
    Transform shipWorld;
    shipWorld.d[0][0] = 0; shipWorld.d[0][1] = -1; shipWorld.d[0][2] = 0;
    shipWorld.d[1][0] = 1; shipWorld.d[1][1] = 0; shipWorld.d[1][2] = 0;
    shipWorld.d[2][0] = 0; shipWorld.d[2][1] = 0; shipWorld.d[2][2] = 1;
    shipWorld.translation = Vector(10,20,30);
    const Transform originalWorld = shipWorld;
    const Transform visualWorld = NativeShipVisualTransform(shipWorld, true);
    const Transform uncorrectedWorld = NativeShipVisualTransform(shipWorld, false);
    const float expectedVisual[3][3] = {{0,0,-1},{-1,0,0},{0,1,0}};
    bool shipBasisValid = true;
    for (unsigned row = 0; row < 3; ++row)
        for (unsigned column = 0; column < 3; ++column)
            shipBasisValid &= std::fabs(visualWorld.d[row][column] - expectedVisual[row][column]) < 1e-5F;
    Transform authoredAttachment(Vector(1,2,3));
    const Transform attached = visualWorld * authoredAttachment;
    Vector cargoLongAxis;
    MATH_ENGINE()->rotate(cargoLongAxis, visualWorld, Vector(0,0,1));
    if (!Expect(shipBasisValid && std::memcmp(&shipWorld, &originalWorld, sizeof(shipWorld)) == 0 &&
                std::memcmp(&uncorrectedWorld, &originalWorld, sizeof(shipWorld)) == 0 &&
                visualWorld.translation.x == 10 && visualWorld.translation.y == 20 && visualWorld.translation.z == 30 &&
                std::fabs(attached.translation.x - 7) < 1e-5F &&
                std::fabs(attached.translation.y - 19) < 1e-5F &&
                std::fabs(attached.translation.z - 32) < 1e-5F &&
                std::fabs(cargoLongAxis.x + 1) < 1e-5F &&
                std::fabs(cargoLongAxis.y) < 1e-5F && std::fabs(cargoLongAxis.z) < 1e-5F,
                "original ship visual axes align attachment and cargo without mutating simulation or nondirect hull")) return 1;
    // ObjectDamage::damagePostRender places smoke at a raw mesh-extent point.
    // The hull correction must move that point without rotating the plume a
    // second time. Existing glow/muzzle checks do not cover that distinction.
    Transform smokeLocal = NativeShipModelAxisCorrection();
    smokeLocal.translation = Vector(1,2,3);
    const Transform originalSmokeLocal = smokeLocal;
    const Transform smokeWorld = NativeShipDamageSmokeTransform(shipWorld, smokeLocal, true);
    const Transform smokeLegacy = shipWorld * smokeLocal;
    const Transform smokeUncorrected = NativeShipDamageSmokeTransform(shipWorld, smokeLocal, false);
    bool smokeOrientationValid = true;
    for (unsigned row = 0; row < 3; ++row)
        for (unsigned column = 0; column < 3; ++column)
            smokeOrientationValid &= smokeWorld.d[row][column] == smokeLegacy.d[row][column];
    if (!Expect(std::fabs(smokeWorld.translation.x - 7) < 1e-5F &&
                std::fabs(smokeWorld.translation.y - 19) < 1e-5F &&
                std::fabs(smokeWorld.translation.z - 32) < 1e-5F &&
                smokeOrientationValid && smokeLegacy.translation.z == 33 &&
                std::memcmp(&smokeUncorrected, &smokeLegacy, sizeof(Transform)) == 0 &&
                std::memcmp(&shipWorld, &originalWorld, sizeof(Transform)) == 0 &&
                std::memcmp(&smokeLocal, &originalSmokeLocal, sizeof(Transform)) == 0,
                "damage smoke anchors to corrected hull extents without rotating plume or changing simulation/nondirect frames")) return 1;
    // TM2's Mantis Frigate fixed turrets feed this frame to PlasmaBolt. The
    // previous world-node*hardpoint origin missed the visible corrected hull.
    // A scaled/rotated child must retain its local frame with no second root
    // translation or model correction. This extends the existing axis check.
    Transform barrelLocal;
    barrelLocal.set_identity();
    barrelLocal.d[0][0] = 0; barrelLocal.d[0][1] = -3;
    barrelLocal.d[1][0] = 2; barrelLocal.d[1][1] = 0; barrelLocal.d[2][2] = 4;
    barrelLocal.translation = Vector(5,7,11);
    const Transform barrelWorld = shipWorld * barrelLocal;
    Transform muzzle;
    muzzle.set_identity();
    muzzle.rotate_about_i(0.31F);
    muzzle.translation = Vector(1,2,3);
    const Transform originalBarrel = barrelWorld, originalMuzzleFrame = muzzle;
    const Transform correctedMuzzle = NativeShipHardpointTransform(shipWorld, barrelWorld, muzzle, true);
    const Transform unchangedMuzzle = NativeShipHardpointTransform(shipWorld, barrelWorld, muzzle, false);
    const Transform legacyMuzzle = barrelWorld * muzzle;
    bool muzzleFrameValid = true;
    for (const Vector &point : {Vector(0,0,0), Vector(1,0,0), Vector(0,1,0), Vector(0,0,1)})
    {
        const Vector local = barrelLocal * (muzzle * point);
        // Numeric product of the independently pinned shipWorld and original
        // Rx90/Ry180 basis above: (x,y,z) maps to (-z,-x,y)+(10,20,30).
        const Vector expected(10-local.z, 20-local.x, 30+local.y);
        const Vector actual = correctedMuzzle * point;
        muzzleFrameValid &= std::fabs(actual.x-expected.x) < 1e-4F &&
            std::fabs(actual.y-expected.y) < 1e-4F && std::fabs(actual.z-expected.z) < 1e-4F;
    }
    if (!Expect(muzzleFrameValid &&
                std::fabs(correctedMuzzle.translation.x + 13) < 1e-4F &&
                std::fabs(correctedMuzzle.translation.y - 21) < 1e-4F &&
                std::fabs(correctedMuzzle.translation.z - 39) < 1e-4F &&
                std::memcmp(&unchangedMuzzle, &legacyMuzzle, sizeof(Transform)) == 0 &&
                std::memcmp(&shipWorld, &originalWorld, sizeof(Transform)) == 0 &&
                std::memcmp(&barrelWorld, &originalBarrel, sizeof(Transform)) == 0 &&
                std::memcmp(&muzzle, &originalMuzzleFrame, sizeof(Transform)) == 0,
                "fixed ship muzzle shares hull correction exactly once and preserves scaled child/nondirect frames")) return 1;
    // TM5 Midas has model-Y revolute turret joints and model -K muzzles.
    // Omitting the hull correction makes that muzzle point down world Z and
    // measures its joint yaw in the wrong plane. Exercise the shared frame
    // used by Turret's sight, swivel and target-leading consumers.
    for (const float heading : {-2.4F, -0.7F, 0.0F, 0.7F, 2.4F})
    for (const float angle : {-2.0F, -0.4F, 0.0F, 0.4F, 2.0F})
    {
        TRANSFORM ship;
        ship.rotate_about_k(heading);
        ship.translation = Vector(100, 200, 300);
        const Matrix jointRotation(Quaternion(Vector(0,1,0), angle));
        Transform joint(jointRotation);
        joint.translation = Vector(5, 20, -30);
        const Transform nodeWorld = ship * joint;
        const TRANSFORM node(NativeShipHardpointTransform(ship, nodeWorld, Transform(), true));
        const Vector ray = -node.get_k();
        const Vector expected = ship.rotate(Vector(std::sin(angle), -std::cos(angle), 0));
        const Vector target = node.translation + expected * 5000;
        const SINGLE error = std::remainder(TRANSFORM::get_yaw(target-node.translation) -
                                            node.get_yaw_zbased(), 2*PI);
        const Vector origin = ship.rotate_translate(Vector(-5, -30, 20));
        if (!Expect((ray-expected).magnitude() < 1e-4F && std::fabs(error) < 1e-4F &&
                    (node.translation-origin).magnitude() < 1e-4F,
                    "jointed ship muzzle, target yaw and model-Y pivot share the visible hull frame")) return 1;
    }
    // Harvester hp_tractor is a model-space muzzle. A model-Z displacement
    // must follow the corrected hull axis instead of lowering the beam in world Z.
    // Keep the original endpoint/transform available to simulation and fast hulls.
    // Regression: ObjectWarp::startWarpIn feeds this basis directly into
    // ObjectControl and the corrected Engine hull. The prior legacy basis
    // pitches current controls by 90 degrees; asset-load checks miss that.
    for (const SINGLE heading : {0.0F, 0.73F, -1.21F, 3.14159265F})
    {
        const Vector gate(13, -27, 9);
        const TRANSFORM warpWorld = NativeShipWarpInTransform(heading, gate);
        const Transform warpHull = NativeShipVisualTransform(warpWorld, true);
        const Vector hullUp = warpHull.get_j();
        const Vector hullForward = -warpHull.get_k();
        const SINGLE yawDifference = warpWorld.get_yaw() - heading;
        const SINGLE wrappedDifference = std::atan2(std::sin(yawDifference), std::cos(yawDifference));
        const Vector handedness = cross_product(warpWorld.get_i(), warpWorld.get_j());
        TRANSFORM legacyWarp;
        const Vector facing(std::sin(heading), std::cos(heading), 0);
        legacyWarp.set_i(Vector(facing.y, -facing.x, 0));
        legacyWarp.set_j(Vector(0, 0, 1));
        legacyWarp.set_k(-facing);
        if (!Expect(std::fabs(wrappedDifference) < 1e-5F &&
                    std::fabs(warpWorld.get_pitch()) < 1e-5F &&
                    std::fabs(warpWorld.get_roll()) < 1e-5F &&
                    std::fabs(hullUp.x) < 1e-5F && std::fabs(hullUp.y) < 1e-5F &&
                    std::fabs(hullUp.z - 1) < 1e-5F &&
                    std::fabs(hullForward.x - facing.x) < 1e-5F &&
                    std::fabs(hullForward.y - facing.y) < 1e-5F &&
                    std::fabs(hullForward.z) < 1e-5F &&
                    std::fabs(handedness.z - 1) < 1e-5F &&
                    warpWorld.translation.x == gate.x && warpWorld.translation.y == gate.y &&
                    warpWorld.translation.z == gate.z &&
                    std::fabs(legacyWarp.get_pitch()) > 1.5F,
                    "warp-in current control basis preserves heading, level hull and gate position; rejects legacy basis")) return 1;
    }
    // SpaceShip's speaking marker must spin around its own below-hull center,
    // not tilt about the navigation J axis and swing that offset sideways.
    // Use asymmetric offsets and actual production math across the full phase.
    for (const float angle : {-1.0F, 0.0F, 0.7F, 1.0F})
    {
        TRANSFORM marker(shipWorld);
        marker = TRANSFORM(NativeShipSpeakerMarkerTransform(
            shipWorld, Vector(2, 3, -4), angle));
        bool planar = true;
        for (const Vector corner : {Vector(-5, -5, 0), Vector(-5, 5, 0),
                                    Vector(5, -5, 0), Vector(5, 5, 0)})
            planar &= std::fabs((marker * corner).z - 26) < 1e-5F;
        if (!Expect(planar && std::fabs(marker.translation.x - 7) < 1e-5F &&
                    std::fabs(marker.translation.y - 22) < 1e-5F &&
                    std::fabs(marker.translation.z - 26) < 1e-5F &&
                    std::fabs(marker.get_k().z - 1) < 1e-5F &&
                    std::memcmp(&shipWorld, &originalWorld, sizeof(shipWorld)) == 0,
                    "speaking marker phase keeps below-hull center and plane fixed without modifying ship")) return 1;
    }
    const Vector authoredMuzzle(2, 5, -11);
    const Vector renderedMuzzle = visualWorld * authoredMuzzle;
    const Vector originalMuzzle = shipWorld * authoredMuzzle;
    const Vector fastHullMuzzle = uncorrectedWorld * authoredMuzzle;
    if (!Expect(std::fabs(renderedMuzzle.x - 21) < 1e-5F &&
                std::fabs(renderedMuzzle.y - 18) < 1e-5F &&
                std::fabs(renderedMuzzle.z - 35) < 1e-5F &&
                std::fabs(originalMuzzle.x - 5) < 1e-5F &&
                std::fabs(originalMuzzle.y - 22) < 1e-5F &&
                std::fabs(originalMuzzle.z - 19) < 1e-5F &&
                fastHullMuzzle.x == originalMuzzle.x &&
                fastHullMuzzle.y == originalMuzzle.y &&
                fastHullMuzzle.z == originalMuzzle.z &&
                std::memcmp(&shipWorld, &originalWorld, sizeof(shipWorld)) == 0,
                "Harvester rendered muzzle follows hull axes while original/fast endpoints remain intact")) return 1;
    RendCompDesc polyMeshDescriptor;
    polyMeshDescriptor.interface_name = "PolyMesh";
    IRenderComponent *polyMesh = nullptr;
    if (!Expect(components->CreateInstance(&polyMeshDescriptor,
                    reinterpret_cast<void **>(&polyMesh)) == GR_INVALID_PARMS && !polyMesh,
                "production PolyMesh factory rejects missing engine services")) return 1;
    AGGDESC descriptor("IMaterialManager");
    ProbeOuter outer;
    AGGDESC lightDescriptor("LightManager");
    ILightManager *lights = nullptr;
    IAggregateComponent *lightAggregate = nullptr;
    if (!Expect(RegisterNativeLightManager(components) &&
                components->CreateInstance(&lightDescriptor,
                    reinterpret_cast<void **>(&lights)) == GR_OK && lights &&
                lights->QueryInterface(IID_IAggregateComponent,
                    reinterpret_cast<void **>(&lightAggregate)) == GR_OK &&
                lightAggregate && lightAggregate->Initialize() == GR_OK,
                "production PolyMesh light service creation")) return 1;
    lights->set_ambient_light(255, 255, 255);
    outer.lightManager = lights;
    TextureCapture textureCapture;
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode) textureCapture.gpu = &gpu.renderer;
#endif
    FrontierX::NativeTextureOperations textureOperations{};
    textureOperations.context = &textureCapture;
    textureOperations.create = CreateTexture;
    textureOperations.upload = UploadTexture;
    textureOperations.destroy = DestroyTexture;
    FrontierX::RegisterNativeRenderPrimitive(components);
    FrontierX::NativeRenderPrimitiveDesc materialPrimitiveDescriptor(
        RejectGameDraw, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, CaptureGameStandardDraw);
    IRenderPrimitive * materialPrimitive = nullptr;
    if (!Expect(components->CreateInstance(&materialPrimitiveDescriptor,
                    reinterpret_cast<void **>(&materialPrimitive)) == GR_OK &&
                    materialPrimitive,
                "native game material draw receiver creation")) return 1;
    auto *pipeline = new FrontierX::NativeRenderPipeline(materialPrimitive, textureOperations);
    outer.pipe = pipeline;
    AGGDESC gameTextureDescriptor("ITManager");
    gameTextureDescriptor.outer = &outer;
    IDAComponent *gameTextureInner = nullptr;
    gameTextureDescriptor.inner = &gameTextureInner;
    void *rawGameTextures = nullptr;
    if (!Expect(components->CreateInstance(&gameTextureDescriptor, &rawGameTextures) == GR_OK &&
                rawGameTextures && gameTextureInner,
                "production game texture manager creation")) return 1;
    ITManager *gameTextures = nullptr;
    if (!Expect(gameTextureInner->QueryInterface(IID_ITManager,
                    reinterpret_cast<void **>(&gameTextures)) == GR_OK && gameTextures,
                "production game texture manager interface")) return 1;
    ITManager::InitInfo gameTextureInit{};
    gameTextureInit.PIPE = pipeline;
    gameTextures->Initialize(gameTextureInit);
    descriptor.outer = &outer;
    IDAComponent *inner = nullptr;
    descriptor.inner = &inner;
    void *raw = nullptr;
    const GENRESULT createResult = components->CreateInstance(&descriptor, &raw);
    if (!Expect(createResult == GR_OK && raw && inner,
                "production game material manager creation"))
    {
        std::cerr << "result=" << createResult << " raw=" << raw
                  << " inner=" << inner << '\n';
        return 1;
    }
    IMaterialManager *materials = nullptr;
    if (!Expect(inner->QueryInterface(IID_IMaterialManager,
                    reinterpret_cast<void **>(&materials)) == GR_OK && materials,
                "production game material manager interface")) return 1;
    IMaterialManager::InitInfo init{};
    init.MATDIR = archive;
    init.PIPE = pipeline;
    init.TMANAGER = gameTextures;
    if (!Expect(materials->Initialize(init) == GR_OK,
                "production game material directory initialization")) return 1;
    const std::filesystem::path materialDirectoryPath = std::string(argv[1]) + ".directory";
    IFileSystem *materialDirectory = nullptr;
    if (!Expect(WriteDirectoryFixture(argv[1], materialDirectoryPath) &&
                FrontierX::OpenNativeDirectoryFileSystem(materialDirectoryPath, &materialDirectory, error),
                "native physical material source")) return 1;
    DAFILEDESC traversal("../unused");
    if (!Expect(materialDirectory->OpenChild(&traversal) == INVALID_HANDLE_VALUE,
                "physical source rejects parent traversal")) return 1;
    std::error_code linkError;
    const auto escape = materialDirectoryPath / "escape";
    std::filesystem::remove(escape, linkError);
    std::filesystem::create_symlink(std::filesystem::absolute(argv[1]), escape, linkError);
    DAFILEDESC escapeDescriptor("escape");
    if (!Expect(!linkError && materialDirectory->OpenChild(&escapeDescriptor) == INVALID_HANDLE_VALUE,
                "physical source rejects a symlink outside its root")) return 1;
    std::filesystem::remove(escape, linkError);
    DAFILEDESC absoluteMaterial("\\materials\\Base.txt");
    DAFILEDESC escapedMaterial("\\materials\\..\\Base.txt");
    if (!Expect(materialDirectory->SetCurrentDirectory("shaders") &&
                materialDirectory->OpenChild(&escapedMaterial) == INVALID_HANDLE_VALUE,
                "physical source rejects absolute-path traversal")) return 1;
    const HANDLE absoluteFile = materialDirectory->OpenChild(&absoluteMaterial);
    if (!Expect(absoluteFile != INVALID_HANDLE_VALUE &&
                materialDirectory->GetFileSize(absoluteFile, nullptr) > 0 &&
                materialDirectory->CloseHandle(absoluteFile) &&
                materialDirectory->SetCurrentDirectory("\\materials") &&
                materialDirectory->SetCurrentDirectory("\\"),
                "physical source resolves a root-absolute child within its root")) return 1;
    init.MATDIR = materialDirectory;
    // Profile-owned managers outlive a temporary GPU scene. Detach its resources,
    // then reload through the same owner before any mesh borrows a material.
    if (!Expect(!FrontierX::ResetNativeGameMaterialManager(nullptr) &&
                FrontierX::ResetNativeGameMaterialManager(materials) &&
                materials->FindMaterial("Base.txt") == nullptr &&
                FrontierX::ResetNativeGameMaterialManager(materials) &&
                materials->Initialize(init) == GR_OK,
                "profile material owner reset and reuse")) return 1;
    IMaterial *material = materials->FindMaterial("Base.txt");
    if (!Expect(material && std::strcmp(material->GetName(), "Base.txt") == 0 &&
                material->GetFloat("Gain") == 0.75F &&
                materials->FindMaterial("Missing.txt") == nullptr,
                "production game material script lookup"))
    {
        IMaterial *first = nullptr;
        const GENRESULT firstResult = materials->FindFirstMaterial(&first);
        std::cerr << "first=" << firstResult << " material=" << first
                  << " name=" << (first ? first->GetName() : "")
                  << " gain=" << (first ? first->GetFloat("Gain") : -1.0F)
                  << " register=" << (first ? first->GetFloat(MRT_CODE, 0U) : -1.0F)
                  << '\n';
        return 1;
    }
    const std::filesystem::path meshPath = std::string(argv[1]) + ".mesh";
    if (!Expect(FrontierXTest::WriteGameMeshFixture(meshPath),
                "CQ2 game mesh fixture creation")) return 1;
    IFileSystem *meshArchive = nullptr;
    if (!Expect(FrontierX::OpenNativeUtfFileSystem(meshPath, &meshArchive, error) &&
                meshArchive, "CQ2 game mesh UTF open")) return 1;
    DAFILEDESC meshDirectoryDescriptor("openFLAME 3D N-mesh");
    IFileSystem *meshDirectory = nullptr;
    if (!Expect(meshArchive->CreateInstance(&meshDirectoryDescriptor,
                    reinterpret_cast<void **>(&meshDirectory)) == GR_OK && meshDirectory,
                "production PolyMesh directory view")) return 1;

    AGGDESC textureDescriptor("TextureLibrary");
    textureDescriptor.outer = &outer;
    IDAComponent *textureInner = nullptr;
    textureDescriptor.inner = &textureInner;
    void *rawTextures = nullptr;
    if (!Expect(components->CreateInstance(&textureDescriptor, &rawTextures) == GR_OK &&
                rawTextures && textureInner, "production game texture creation")) return 1;
    IAggregateComponent *textureAggregate = nullptr;
    ITextureLibrary *textures = nullptr;
    if (!Expect(textureInner->QueryInterface(IID_IAggregateComponent,
                    reinterpret_cast<void **>(&textureAggregate)) == GR_OK &&
                textureAggregate && textureAggregate->Initialize() == GR_OK &&
                textureInner->QueryInterface(IID_ITextureLibrary,
                    reinterpret_cast<void **>(&textures)) == GR_OK && textures,
                "production game texture initialization")) return 1;
    {
        XMesh mesh;
        if (!Expect(mesh.read(meshDirectory, textures, materials, 0U) &&
                    mesh.object_vertex_cnt == 5 && mesh.face_cnt == 2 &&
                    mesh.face_group_cnt == 1 && mesh.face_groups &&
                    mesh.face_groups[0].material == 0 &&
                    mesh.imaterial_cnt == 1 && mesh.imaterial_list &&
                    mesh.imaterial_list[0] == material && material->IsRealized() &&
                    material->GetFloat("Gain") == 0.75F &&
                    std::strcmp(material->GetTextureName("Diffuse"), "Tile.tga") == 0 &&
                    gameTextures->GetFirstTexture() == textureCapture.firstHandle &&
                    textureCapture.creates == 1U && textureCapture.uploads == 1U &&
                    textureCapture.pixels[0] == 17U &&
                    textureCapture.pixels[1] == 34U &&
                    textureCapture.pixels[2] == 51U &&
                    textureCapture.pixels[3] == 255U,
                    "production XMesh resolves the actual game material")) return 1;
        for (std::size_t pixel = 0; pixel < 16U; ++pixel)
        {
            const std::size_t offset = pixel * 4U;
            if (!Expect(textureCapture.pixels[offset] == 17U &&
                        textureCapture.pixels[offset + 1U] == 34U &&
                        textureCapture.pixels[offset + 2U] == 51U &&
                        textureCapture.pixels[offset + 3U] == 255U,
                        "production TGA owner uploads every RGBA pixel")) return 1;
        }
    }
    // get_texture_id may supply an unresolved-name placeholder. The actual
    // material/XMesh owner must reject it before it becomes drawable geometry.
    // Material::set_texture_library queries its aggregate outer, as in Engine.
    outer.textureLibrary = textures;
    for (unsigned failureMode = 0U; failureMode < 3U; ++failureMode)
    {
        const auto failurePath = meshPath.parent_path() /
            ("required_material_failure_" + std::to_string(failureMode) + ".utf");
        IFileSystem *failureArchive = nullptr;
        IFileSystem *failureDirectory = nullptr;
        if (!Expect(FrontierXTest::WriteRequiredMaterialFailureFixture(failurePath, failureMode) &&
                    FrontierX::OpenNativeUtfFileSystem(failurePath, &failureArchive, error) &&
                    failureArchive->CreateInstance(&meshDirectoryDescriptor,
                        reinterpret_cast<void **>(&failureDirectory)) == GR_OK,
                    "required material failure fixture opens through production UTF view")) return 1;
        const unsigned before = gameMeshDrawCalls;
        bool accepted = false;
        {
            XMesh rejected;
            try
            {
                accepted = rejected.read(failureDirectory, textures, materials, 0U);
            }
            catch (const std::runtime_error &failure)
            {
                if (std::string(failure.what()).find("Native material required texture") != 0U)
                    throw;
                accepted = false;
            }
            if (accepted) std::cerr << "required material result legacy_count=" << rejected.material_cnt
                << " cq2_count=" << rejected.imaterial_cnt << " legacy_owner=" << static_cast<void *>(rejected.material_list) << '\n';
            if (accepted && rejected.material_list) std::cerr << "required material texture ref="
                << rejected.material_list[0].diffuse_texture_ref << " handle=" << rejected.material_list[0].texture_id << '\n';
        }
        failureDirectory->Release();
        failureArchive->Release();
        if (accepted) std::cerr << "required material failure unexpectedly accepted mode=" << failureMode << '\n';
        if (!Expect(!accepted && gameMeshDrawCalls == before,
                    "original material owner rejects missing legacy/nested map or incomplete declared entry before draw")) return 1;
    }
    IRP_VERTEXBUFFERHANDLE materialBuffer = 0U;
    void * materialBytes = nullptr;
    U32 materialCapacity = 0U;
    std::array<StandardVertex, 4U> materialVertices{};
    materialVertices[1].pos.set(-1.0F, -1.0F, 0.5F);
    materialVertices[1].norm.set(0.0F, 0.0F, 1.0F);
    materialVertices[1].r = 0x12U;
    materialVertices[1].u = 0.25F;
    materialVertices[2].u2 = 0.5F;
    materialVertices[3].tanZ = 0.75F;
    const U16 materialIndices[3] = {0U, 1U, 2U};
    IDirect3DIndexBuffer9 * materialIndexBuffer = nullptr;
    void * materialIndexBytes = nullptr;
    if (!Expect(pipeline->create_vertex_buffer(D3DFVF_STANDARD, 4, 0U,
                    &materialBuffer) == GR_OK && materialBuffer != 0U &&
                    pipeline->lock_vertex_buffer(materialBuffer, 0U,
                        &materialBytes, &materialCapacity) == GR_OK &&
                    materialCapacity == materialVertices.size(),
                "production material StandardVertex buffer creation")) return 1;
    std::memcpy(materialBytes, materialVertices.data(), sizeof(materialVertices));
    if (!Expect(pipeline->unlock_vertex_buffer(materialBuffer) == GR_OK &&
                    pipeline->create_index_buffer(sizeof(materialIndices),
                        &materialIndexBuffer) == GR_OK && materialIndexBuffer &&
                    materialIndexBuffer->Lock(0U, 0U, &materialIndexBytes, 0U) == GR_OK,
                "production material index buffer creation")) return 1;
    std::memcpy(materialIndexBytes, materialIndices, sizeof(materialIndices));
    if (!Expect(materialIndexBuffer->Unlock() == GR_OK,
                "production material index buffer unlock")) return 1;
    IDirect3DVertexBuffer9 materialVertexBuffer{materialBuffer};
    material->DrawVB(&materialVertexBuffer, materialIndexBuffer,
                     1, 3, 0, 3, nullptr);
    if (!Expect(gameStandardDrawCalls == 0U,
                "CQ2 material rejects draw without a realized native effect")) return 1;
    if (!Expect(pipeline->draw_indexed_primitive_vb(D3DPT_TRIANGLELIST,
                    materialBuffer, 1, 3, materialIndexBuffer, 3, 0U) == GR_OK &&
                    gameStandardDrawCalls == 1U &&
                    gameStandardVertices.size() == 3U &&
                    gameStandardIndices == std::vector<U16>({0U, 1U, 2U}) &&
                    std::memcmp(gameStandardVertices.data(),
                                materialVertices.data() + 1U,
                                3U * sizeof(StandardVertex)) == 0,
                "native StandardVertex geometry remains separately executable") ||
        !Expect(materialIndexBuffer->Release() == 0U &&
                    pipeline->destroy_vertex_buffer(materialBuffer) == GR_OK,
                "production material draw buffers released")) return 1;
    gameTextures->Flush();
    if (!Expect(textureCapture.destroys == 1U,
                "production game texture manager releases the uploaded texture")) return 1;
    DAFILEDESC textureDirectoryDescriptor("textures");
    IFileSystem *textureDirectory = nullptr;
    if (!Expect(archive->CreateInstance(&textureDirectoryDescriptor,
                    reinterpret_cast<void **>(&textureDirectory)) == GR_OK &&
                textureDirectory, "production game texture directory view")) return 1;
    const U32 reloaded = gameTextures->CreateTextureFromFile(
        "Tile.tga", textureDirectory, DA::UNKTYPE, PF_4CC_DAA8);
    textureDirectory->Release();
    if (!Expect(reloaded != 0U &&
                (gpuMode || reloaded == 64U) && textureCapture.creates == 2U &&
                textureCapture.uploads == 2U,
                "production game texture manager infers the TGA suffix")) return 1;
    gameTextures->Flush();
    if (!Expect(textureCapture.destroys == 2U,
                "production game texture manager releases the inferred texture")) return 1;
    // SysMap::loadTextures uses the real ITManager for opaque map images
    // larger than the previous four-pixel fixture; exercise that caller.
    std::vector<unsigned char> mapImage(18U, 0U);
    mapImage[2] = 2U;
    mapImage[12] = mapImage[14] = 16U;
    mapImage[16] = 32U;
    mapImage[17] = 0x28U;
    for (unsigned pixel = 0; pixel < 256U; ++pixel)
        mapImage.insert(mapImage.end(), {51U, 34U, 17U, 0U});
    const auto mapPath = materialDirectoryPath / "NativeMap.tga";
    {
        std::ofstream output(mapPath, std::ios::binary | std::ios::trunc);
        output.write(reinterpret_cast<const char *>(mapImage.data()), static_cast<std::streamsize>(mapImage.size()));
        if (!Expect(output.good(), "synthetic original map texture written")) return 1;
    }
    const bool previousTexturePolicy = FrontierX::SetNativeGameTextureBaseLevelOnly(true);
    U32 mapTexture = 0U;
    {
        FrontierX::NativeAppDataSources textureSources;
        FrontierX::NativeAppDataSources conflictingSources;
        if (!Expect(!textureSources.BindTextures(nullptr) &&
                    textureSources.BindTextures(materialDirectory) &&
                    TEXTURESDIR == materialDirectory &&
                    !textureSources.BindTextures(materialDirectory) &&
                    !conflictingSources.BindTextures(materialDirectory),
                    "original texture directory has a single retained native owner")) return 1;
        mapTexture = gameTextures->CreateTextureFromFile(
            "NativeMap.tga", TEXTURESDIR, DA::TGA, PF_4CC_DAOT);
    }
    FrontierX::SetNativeGameTextureBaseLevelOnly(previousTexturePolicy);
    if (!Expect(TEXTURESDIR == nullptr, "original texture directory cleared on owner release")) return 1;
    if (!Expect(mapTexture != 0U, "original opaque map texture reaches native pipeline")) return 1;
    for (std::size_t offset = 0; offset < 1024U; offset += 4U)
        if (!Expect(textureCapture.pixels[offset] == 17U &&
                    textureCapture.pixels[offset + 1U] == 34U &&
                    textureCapture.pixels[offset + 2U] == 51U &&
                    textureCapture.pixels[offset + 3U] == 255U,
                    "opaque map texture preserves RGB and discards source alpha")) return 1;
    PixelFormat mapFormat;
    RPLOCKDATA mapLock{};
    if (!Expect(pipeline->get_texture_format(mapTexture, &mapFormat) == GR_OK &&
                mapFormat.num_bits() == 32U && mapFormat.num_a_bits() == 0U &&
                pipeline->lock_texture(mapTexture, 0, &mapLock) == GR_OK,
                "opaque map format is inherited by original canvas callers")) return 1;
    auto * mapPixels = static_cast<unsigned char *>(mapLock.pixels);
    mapPixels[0] = 91U;
    mapPixels[3] = 0U; // original player-icon recolouring writes RGB only
    if (!Expect(pipeline->unlock_texture(mapTexture, 0) == GR_OK &&
                textureCapture.pixels[0] == 91U && textureCapture.pixels[3] == 255U,
                "opaque icon lock/upload preserves opacity after RGB writes")) return 1;
    U32 unsupportedMipTexture = 0;
    if (!Expect(pipeline->create_texture(16, 16, PF_4CC_DAOT, 3, 0,
                                        unsupportedMipTexture) != GR_OK &&
                unsupportedMipTexture == 0U,
                "native mip chains remain explicitly unsupported")) return 1;
    gameTextures->Flush();
    // The supported D3D path permits RGBA8 for both of these requested classes
    // (planet ownership art, compass and ring); retain source alpha here.
    for (const PFenum alphaClass : {PF_4CC_DAA1, PF_4CC_DAA4})
    {
        const bool previous = FrontierX::SetNativeGameTextureBaseLevelOnly(true);
        const U32 alphaMap = gameTextures->CreateTextureFromFile(
            "NativeMap.tga", materialDirectory, DA::TGA, alphaClass);
        FrontierX::SetNativeGameTextureBaseLevelOnly(previous);
        if (!Expect(alphaMap != 0U && textureCapture.pixels[0] == 17U &&
                    textureCapture.pixels[1] == 34U && textureCapture.pixels[2] == 51U &&
                    textureCapture.pixels[3] == 0U,
                    "original map alpha classes use their supported RGBA8 representation")) return 1;
        gameTextures->Flush();
    }
    DACOMDESC engineDescriptor("IEngine");
    IEngine *engine = nullptr;
    if (!Expect(components->CreateInstance(&engineDescriptor,
                    reinterpret_cast<void **>(&engine)) == GR_OK && engine,
                "production native engine service creation")) return 1;
    if (!Expect(engine->create_archetype("unclaimed.3db", meshArchive) ==
                    INVALID_ARCHETYPE_INDEX &&
                engine->get_archetype_by_name("unclaimed.3db") ==
                    INVALID_ARCHETYPE_INDEX,
                "production engine releases an unclaimed archetype")) return 1;
    GameTransformCapture transformCapture;
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode) transformCapture.gpu = &gpu.renderer;
#endif
    FrontierX::NativeRenderPrimitiveDesc primitiveDescriptor(
        RejectGameDraw, &transformCapture, CaptureNativeLegacyDraw, CaptureLegacyState, nullptr, nullptr,
        CaptureLegacyStage, CaptureLegacyTexture, CaptureGameModelview, CaptureGameProjection,
        CaptureGameMeshDraw, CaptureGameStandardDraw, CaptureLegacySampler);
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode)
    {
        primitiveDescriptor.callback2 = SubmitNativeLegacyGpu;
        primitiveDescriptor.callback3 = SubmitGameMeshGpu;
        primitiveDescriptor.callback4 = SubmitGameStandardGpu;
        primitiveDescriptor.state = ApplyGameStateGpu;
        primitiveDescriptor.textureStage = ApplyGameStageGpu;
        primitiveDescriptor.textureBind = ApplyGameTextureGpu;
        primitiveDescriptor.sampler = ApplyGameSamplerGpu;
    }
#endif
    IRenderPrimitive *primitive = nullptr;
    if (!Expect(components->CreateInstance(&primitiveDescriptor,
                    reinterpret_cast<void **>(&primitive)) == GR_OK && primitive,
                "native render primitive service creation")) return 1;
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode) transformCapture.primitive = primitive;
#endif
    outer.primitive = primitive;
    Matrix4 testProjection(true);
    testProjection.d[0][0] = gpuMode ? 0.5F : 0.75F;
    if (gpuMode)
    {
        testProjection.d[1][1] = 0.5F;
        testProjection.d[2][2] = -0.1F;
    }
    Matrix4 observedProjection(true);
    if (!Expect(primitive->set_projection(testProjection) == GR_OK &&
                    primitive->get_projection(observedProjection) == GR_OK &&
                    observedProjection.d[0][0] == testProjection.d[0][0] &&
                    transformCapture.projectionCalls == 1U &&
                    transformCapture.projectionX == testProjection.d[0][0],
                "native primitive forwards and caches projection")) return 1;
    outer.textureLibrary = textures;
    outer.materialManager = materials;
    PROFPARSEDESC2 profileDescriptor;
    IAggregateComponent *profileAggregate = nullptr;
    if (!Expect(components->CreateInstance(&profileDescriptor,
                    reinterpret_cast<void **>(&profileAggregate)) == GR_OK &&
                profileAggregate, "production engine profile creation")) return 1;
    IProfileParser2 *profile2 = nullptr;
    IProfileParser *profile = nullptr;
    constexpr char engineProfile[] = "[Engine]\nIHardpoint\nIRenderer\n[RenderManager]\n  PolyMesh\n";
    if (!Expect(profileAggregate->QueryInterface(IID_IProfileParser2,
                    reinterpret_cast<void **>(&profile2)) == GR_OK && profile2 &&
                profile2->Initialize2(engineProfile, sizeof(engineProfile) - 1U) == GR_OK &&
                profileAggregate->QueryInterface(IID_IProfileParser,
                    reinterpret_cast<void **>(&profile)) == GR_OK && profile,
                "production engine profile initialization")) return 1;
    static_cast<FrontierX::NativeComponentManager *>(components)->SetProfileParser(profile);
    profile->Release();
    profile2->Release();
    profileAggregate->Release();
    polyMeshDescriptor.system_services = &outer;
    polyMeshDescriptor.engine_services = engine;
    if (!Expect(components->CreateInstance(&polyMeshDescriptor,
                    reinterpret_cast<void **>(&polyMesh)) == GR_OK && polyMesh,
                "production PolyMesh initializes with engine and material services")) return 1;
    if (!Expect(polyMesh->create_archetype(1, meshArchive) &&
                    polyMesh->get_archetype_mesh(1) != nullptr,
                "production PolyMesh creates the CQ2 game mesh archetype")) return 1;
    polyMesh->destroy_archetype(1);
    polyMesh->Release();
    if (!Expect(engine->load_engine_components(&outer) == GR_OK &&
                engine->enumerate_engine_components(nullptr) == 2,
                "production engine loads RenderMgr and PolyMesh from profile")) return 1;
    // A failed declared compound previously fell back to its simple mesh and
    // silently omitted required submodels. Use real UTF parts and Fix joints.
    const auto absentArchetype = [engine](const char *name) {
        const auto index = engine->get_archetype_by_name(name);
        if (index != INVALID_ARCHETYPE_INDEX) engine->release_archetype(index);
        return index == INVALID_ARCHETYPE_INDEX;
    };
    const auto locatorPath = meshPath.parent_path() / "hardpoint_only_fixture.utf";
    IFileSystem *locatorArchive = nullptr;
    IHardpoint *claimHardpoints = nullptr;
    if (!Expect(FrontierXTest::WriteRendererClaimGameMeshFixture(locatorPath, false) &&
                FrontierX::OpenNativeUtfFileSystem(locatorPath, &locatorArchive, error) &&
                engine->QueryInterface(IID_IHardpoint, reinterpret_cast<void **>(&claimHardpoints)) == GR_OK,
                "genuine hardpoint-only fixture binds the original Engine component")) return 1;
    const auto locatorIndex = engine->create_archetype("hardpoint_only.3db", locatorArchive);
    locatorArchive->Release();
    HardpointInfo locatorInfo{};
    if (!Expect(locatorIndex != INVALID_ARCHETYPE_INDEX &&
                claimHardpoints->retrieve_hardpoint_info(locatorIndex, "Locator", locatorInfo) &&
                locatorInfo.type == JT_FIXED && locatorInfo.point.x == 0.25F &&
                locatorInfo.point.y == 0.5F && locatorInfo.point.z == 0.75F &&
                locatorInfo.orientation.d[0][0] == 1.0F && locatorInfo.orientation.d[1][1] == 1.0F &&
                locatorInfo.orientation.d[2][2] == 1.0F,
                "legitimate no-mesh node retains its genuine source hardpoint claim")) return 1;
    const auto locatorInstance = engine->create_instance2(locatorIndex, nullptr);
    if (!Expect(locatorInstance != INVALID_INSTANCE_INDEX,
                "legitimate hardpoint-only archetype creates an original instance")) return 1;
    engine->destroy_instance(locatorInstance);
    engine->release_archetype(locatorIndex);
    if (!Expect(absentArchetype("hardpoint_only.3db"), "released hardpoint-only node retains no archetype")) return 1;
    for (const unsigned mode : {0U, 6U, 1U, 2U, 3U, 4U, 5U})
    {
        const auto compoundPath = meshPath.parent_path() /
            ("required_compound_" + std::to_string(mode) + ".utf");
        const std::string name = "required_compound_" + std::to_string(mode) + ".cmp";
        IFileSystem *compoundArchive = nullptr;
        if (!Expect(FrontierXTest::WriteCompoundGameMeshFixture(compoundPath, mode) &&
                    FrontierX::OpenNativeUtfFileSystem(compoundPath, &compoundArchive, error),
                    "original compound fixture opens real nested UTF parts")) return 1;
        const unsigned before = gameMeshDrawCalls;
        const auto index = engine->create_archetype(name.c_str(), compoundArchive);
        compoundArchive->Release();
        if (mode == 0U || mode == 6U)
        {
            unsigned partCount = 0U, jointCount = 0U;
            const auto count = [](ARCHETYPE_INDEX, ARCHETYPE_INDEX, void *context) {
                ++*static_cast<unsigned *>(context);
                return true;
            };
            if (!Expect(index != INVALID_ARCHETYPE_INDEX && engine->is_archetype_compound(index) &&
                        engine->enumerate_archetype_parts(index, count, &partCount) == GR_OK && partCount == 2U &&
                        engine->enumerate_archetype_joints(index, count, &jointCount) == GR_OK && jointCount == 1U,
                        "original Engine compound retains both required parts and the authored Fix joint")) return 1;
            if (mode == 6U)
            {
                const auto rootIndex = engine->get_archetype_by_name("CompoundRoot.3db");
                const bool actualLocator = rootIndex != INVALID_ARCHETYPE_INDEX &&
                    claimHardpoints->retrieve_hardpoint_info(rootIndex, "Locator", locatorInfo);
                if (rootIndex != INVALID_ARCHETYPE_INDEX) engine->release_archetype(rootIndex);
                if (!Expect(actualLocator, "compound permits a genuine hardpoint-only locator root with rendered child")) return 1;
            }
            const auto instance = engine->create_instance2(index, nullptr);
            const auto child = engine->get_instance_child_next(instance, EN_DONT_RECURSE, INVALID_INSTANCE_INDEX);
            if (!Expect(instance != INVALID_INSTANCE_INDEX && child != INVALID_INSTANCE_INDEX &&
                        engine->get_instance_parent(child) == instance &&
                        std::strcmp(engine->get_instance_part_name(instance), "Root") == 0 &&
                        std::strcmp(engine->get_instance_part_name(child), "Child") == 0 &&
                        engine->get_joint_info(child) != nullptr,
                        "original compound instance connects the actual root and child production meshes")) return 1;
            engine->destroy_instance(instance);
            const auto remainingChild = engine->get_instance_archetype(child);
            if (remainingChild != INVALID_ARCHETYPE_INDEX) engine->release_archetype(remainingChild);
            engine->release_archetype(index);
            if (!Expect(remainingChild == INVALID_ARCHETYPE_INDEX,
                        "compound destruction releases the connected child instance")) return 1;
        }
        else if (!Expect(index == INVALID_ARCHETYPE_INDEX,
                         "missing or failed declared compound root/part rejects despite other component claims"))
        {
            std::cerr << "required compound unexpectedly accepted mode=" << mode << '\n';
            return 1;
        }
        if (!Expect(absentArchetype(name.c_str()) && absentArchetype("CompoundRoot.3db") &&
                    absentArchetype("CompoundChild.3db") && gameMeshDrawCalls == before,
                    "failed or released compound retains no acquired part archetypes or draws")) return 1;
    }
    const auto failedClaimPath = meshPath.parent_path() / "declared_mesh_and_hardpoint_failure.utf";
    IFileSystem *failedClaimArchive = nullptr;
    if (!Expect(FrontierXTest::WriteRendererClaimGameMeshFixture(failedClaimPath, true) &&
                FrontierX::OpenNativeUtfFileSystem(failedClaimPath, &failedClaimArchive, error),
                "declared mesh failure with genuine other component fixture opens")) return 1;
    const auto failedClaimIndex = engine->create_archetype("declared_mesh_failure.3db", failedClaimArchive);
    failedClaimArchive->Release();
    claimHardpoints->Release();
    if (!Expect(failedClaimIndex == INVALID_ARCHETYPE_INDEX && absentArchetype("declared_mesh_failure.3db"),
                "failed declared renderer cannot become a successful hardpoint-only archetype")) return 1;
    std::cout << "Original compound required parts, connected instances and failed-load ownership checked\n";
    const auto legacyMeshPath = meshPath.parent_path() / "legacy_game_mesh_fixture.utf";
    IFileSystem *legacyMeshArchive = nullptr;
    if (!Expect(FrontierXTest::WriteLegacyGameMeshFixture(legacyMeshPath, gpuMode) &&
                    FrontierX::OpenNativeUtfFileSystem(legacyMeshPath,
                        &legacyMeshArchive, error) && legacyMeshArchive,
                "legacy PolyMesh render fixture open")) return 1;
    const ARCHETYPE_INDEX engineArchetype =
        engine->create_archetype("fixture.3db", legacyMeshArchive);
    if (!Expect(engineArchetype != INVALID_ARCHETYPE_INDEX &&
                std::strcmp(engine->get_archetype_name(engineArchetype), "fixture.3db") == 0,
                "production engine dispatches the game mesh archetype")) return 1;
    // ENGINE.H returns a held archetype, including for observational checks.
    const auto instanceHasArchetype = [engine](INSTANCE_INDEX instance,
                                              ARCHETYPE_INDEX expected)
    {
        const ARCHETYPE_INDEX actual = engine->get_instance_archetype(instance);
        const bool matches = actual == expected;
        if (actual != INVALID_ARCHETYPE_INDEX) engine->release_archetype(actual);
        return matches;
    };
    const INSTANCE_INDEX engineInstance = engine->create_instance2(engineArchetype, nullptr);
    if (!Expect(engineInstance != INVALID_INSTANCE_INDEX &&
                instanceHasArchetype(engineInstance, engineArchetype),
                "production engine creates a PolyMesh instance")) return 1;
    IRenderer *renderer = nullptr;
    if (!Expect(engine->QueryInterface(IID_IRenderer,
                    reinterpret_cast<void **>(&renderer)) == GR_OK && renderer &&
                renderer->get_instance_mesh(engineInstance, nullptr) != nullptr,
                "production RenderMgr exposes the PolyMesh instance")) return 1;
    // Direct SpaceShip/Platform draws bypass CQ2 team modifiers. Exercise
    // their actual unique-mesh owner, including isolation from other players.
    Mesh *teamArchetype = renderer->get_archetype_mesh(engineArchetype);
    const std::string originalMaterialName = teamArchetype->material_list[0].name;
    const auto originalDiffuse = teamArchetype->material_list[0].diffuse;
    teamArchetype->material_list[0].set_name("fixture_markings");
    const INSTANCE_INDEX redTeam = engine->create_instance2(engineArchetype, nullptr);
    const INSTANCE_INDEX blueTeam = engine->create_instance2(engineArchetype, nullptr);
    SetNativeLegacyTeamColor(engine, renderer, redTeam, RGB(191, 37, 11));
    SetNativeLegacyTeamColor(engine, renderer, blueTeam, RGB(13, 41, 223));
    const Mesh *redMesh = renderer->get_instance_mesh(redTeam);
    const Mesh *blueMesh = renderer->get_instance_mesh(blueTeam);
    if (!Expect(redMesh != blueMesh && redMesh != teamArchetype && blueMesh != teamArchetype &&
                redMesh->material_list[0].diffuse.r == 191 &&
                redMesh->material_list[0].diffuse.g == 37 &&
                blueMesh->material_list[0].diffuse.b == 223 &&
                teamArchetype->material_list[0].diffuse.r == originalDiffuse.r &&
                teamArchetype->material_list[0].diffuse.g == originalDiffuse.g &&
                redMesh->material_list[0].texture_id == teamArchetype->material_list[0].texture_id,
                "legacy team markings are per-instance, preserve textures and leave archetype unchanged")) return 1;
    SetNativeLegacyTeamColor(engine, renderer, redTeam, RGB(29, 179, 47));
    if (!Expect(redMesh->material_list[0].diffuse.g == 179 &&
                blueMesh->material_list[0].diffuse.r == 13 &&
                blueMesh->material_list[0].diffuse.b == 223,
                "team-color changes cannot recolor another player's instance")) return 1;
    engine->destroy_instance(redTeam);
    engine->destroy_instance(blueTeam);
    teamArchetype->material_list[0].set_name(originalMaterialName.c_str());
    std::cout << "Production legacy player-color mesh isolation checked\n";
    ENGINE = engine;
    REND = renderer;
    IHardpoint *hardpoints = nullptr;
    if (!Expect(engine->QueryInterface(IID_IHardpoint,
                    reinterpret_cast<void **>(&hardpoints)) == GR_OK && hardpoints,
                "production Engine hardpoint owner")) return 1;
    const INSTANCE_INDEX hardpointSibling = engine->create_instance2(engineArchetype, nullptr);
    const INSTANCE_INDEX hardpointInstance = engine->create_instance2(engineArchetype, nullptr);
    HardpointInfo fixtureHardpoint{};
    fixtureHardpoint.type = JT_FIXED;
    fixtureHardpoint.point = Vector(3, 4, 5);
    fixtureHardpoint.orientation.set_identity();
    if (!Expect(hardpointInstance != INVALID_INSTANCE_INDEX && hardpointInstance != engineArchetype &&
                hardpoints->set_hardpoint_info(engineArchetype, "hp_fixture", fixtureHardpoint),
                "hardpoint fixture separates instance from archetype IDs")) return 1;
    // Original ObjectSelection::initSelection uses ComputeCorners. Instance
    // IDs must resolve their actual archetype before the renderer bounds query.
    // This existing Engine closure already allocates divergent IDs; passing an
    // instance straight to RenderMgr otherwise selects unrelated/missing bounds.
    std::array<float, 6> selectionBounds{}, authoredBounds{};
    engine->update_instance(hardpointInstance, 0.0F, 0U);
    ComputeCorners(selectionBounds.data(), hardpointInstance);
    if (!Expect(renderer->get_archetype_bounding_box(engineArchetype, 1.0F,
                    authoredBounds.data()), "selection fixture resolves authored renderer bounds")) return 1;
    for (std::size_t axis = 0; axis < authoredBounds.size(); ++axis)
        authoredBounds[axis] = axis % 2U == 0U ? (std::max)(0.0F, authoredBounds[axis]) :
            (std::min)(0.0F, authoredBounds[axis]);
    if (!Expect(selectionBounds == authoredBounds && selectionBounds[0] > selectionBounds[1] &&
                selectionBounds[2] > selectionBounds[3] &&
                instanceHasArchetype(hardpointInstance, engineArchetype),
                "production selection corners use held archetype bounds for divergent instance IDs")) return 1;
    // Direct SpaceShip hulls render model Z along world Y. The marker's
    // unchanged corner-width rule must use that footprint, not thin model Y.
    const std::array<float, 6> asymmetricBounds{{7, -3, 1, -2, 30, -10}};
    const auto unchangedBounds = asymmetricBounds;
    std::array<float, 6> visualBounds{}, ordinaryBounds{};
    NativeShipSelectionBounds(asymmetricBounds.data(), true, visualBounds.data());
    NativeShipSelectionBounds(asymmetricBounds.data(), false, ordinaryBounds.data());
    const std::array<float, 6> correctedBounds{{3, -7, 30, -10, 1, -2}};
    bool selectionAxesValid = true;
    for (std::size_t axis = 0; axis < visualBounds.size(); ++axis)
        selectionAxesValid &= std::fabs(visualBounds[axis] - correctedBounds[axis]) < 1e-4F;
    if (!Expect(selectionAxesValid && asymmetricBounds == unchangedBounds &&
                ordinaryBounds == asymmetricBounds &&
                std::fabs((visualBounds[2] - visualBounds[3]) - 40.0F) < 1e-4F &&
                asymmetricBounds[2] - asymmetricBounds[3] == 3.0F,
                "selection draw footprint matches corrected hull axes without widening cached gameplay bounds")) return 1;
    std::array<float, 6> engineVisualBounds{};
    NativeShipSelectionBounds(selectionBounds.data(), true, engineVisualBounds.data());
    if (!Expect(std::fabs(engineVisualBounds[0] + selectionBounds[1]) < 1e-4F &&
                std::fabs(engineVisualBounds[1] + selectionBounds[0]) < 1e-4F &&
                std::fabs(engineVisualBounds[2] - selectionBounds[4]) < 1e-4F &&
                std::fabs(engineVisualBounds[3] - selectionBounds[5]) < 1e-4F,
                "production ComputeCorners bounds feed the same direct-hull visual footprint")) return 1;
    std::cout << "Selection bounds: original held-archetype corners; direct-hull X/Z footprint; cached bounds unchanged\n";
    HARDPOINT = hardpoints;
    S32 foundHardpoint = INVALID_INSTANCE_INDEX;
    HardpointInfo foundHardpointInfo{};
    const bool hardpointFound = FindHardpoint("\\hp_fixture", foundHardpoint,
                                              foundHardpointInfo, hardpointInstance);
    struct HardpointEnumeration : IHPEnumerator
    {
        unsigned count = 0;
        S32 instance = INVALID_INSTANCE_INDEX;
        bool EnumerateHardpoint(const HPENUMINFO &info) override
        {
            ++count;
            instance = info.index;
            return false;
        }
    } hardpointEnumeration;
    EnumerateHardpoints(hardpointInstance, "\\hp_*", &hardpointEnumeration);
    HARDPOINT = nullptr;
    hardpoints->Release();
    engine->destroy_instance(hardpointInstance);
    engine->destroy_instance(hardpointSibling);
    if (!Expect(hardpointFound && foundHardpoint == hardpointInstance &&
                foundHardpointInfo.point.z == 5 && hardpointEnumeration.count == 1 &&
                hardpointEnumeration.instance == hardpointInstance,
                "App hardpoint lookup/enumeration uses the held archetype for the instance")) return 1;
    IMeshInfoTree *meshInfoTree = CreateMeshInfoTree(engineInstance);
    MeshInfo *meshInfo = meshInfoTree ? meshInfoTree->GetMeshInfo() : nullptr;
    const bool meshInfoValid = meshInfo && meshInfo->instanceIndex == engineInstance &&
        meshInfo->bHasMesh && meshInfo->bWhole && !meshInfo->bOwnsInstance &&
        meshInfo->radius > 0.0F;
    if (meshInfoTree) DestroyMeshInfoTree(meshInfoTree);
    ENGINE = nullptr;
    REND = nullptr;
    if (!Expect(meshInfoValid &&
                instanceHasArchetype(engineInstance, engineArchetype),
                "production App MeshInfo tree preserves borrowed Engine instance")) return 1;
    ProbeCamera camera;
    if (!gpuMode)
    {
        camera.projectCenter = true;
        ViewPoint projectedHull[64]{};
        int hullPoints = 64;
        float hullDepth = 0.0F;
        if (!Expect(renderer->get_instance_projected_bounding_polygon(engineInstance,
                        &camera, 1.0F, hullPoints, projectedHull, 64, hullDepth) &&
                    hullPoints >= 3,
                    "production RenderMgr projects a representable selection hull")) return 1;
        camera.inverse.translation.y = 1.0e10F;
        hullPoints = 64;
        if (!Expect(!renderer->get_instance_projected_bounding_polygon(engineInstance,
                        &camera, 1.0F, hullPoints, projectedHull, 64, hullDepth),
                    "production RenderMgr rejects unrepresentable selection coordinates")) return 1;
        camera.inverse.translation.y = 0.0F;
        camera.projectCenter = false;
    }
    U32 primitiveStateId = 0U;
    U32 cullMode = 0U;
    if (!Expect((!gpuMode ||
                (primitive->set_render_state(D3DRS_CULLMODE, D3DCULL_NONE) == GR_OK &&
                 primitive->get_render_state(D3DRS_CULLMODE, &cullMode) == GR_OK &&
                 cullMode == D3DCULL_NONE)) &&
                primitive->set_state(RPR_STATE_ID, 73U) == GR_OK &&
                primitive->get_state(RPR_STATE_ID, &primitiveStateId) == GR_OK &&
                primitiveStateId == 73U &&
                primitive->set_state(RPR_STATE_ID, 0U) == GR_OK,
                "production PolyMesh fixed culling and immediate state key")) return 1;
    if (gpuMode)
    {
        engine->update(0.0F);
        if (!Expect(textureCapture.creates >= 3U &&
                    textureCapture.uploads >= 3U,
                    "production game mesh texture library uploads RGB565 frame"))
        {
            std::cerr << "creates=" << textureCapture.creates
                      << " uploads=" << textureCapture.uploads << '\n';
            return 1;
        }
    }
    cpuMaterialBackendLive = true;
    engine->render_instance(&camera, engineInstance, EN_DONT_RECURSE, 1.0F,
        RF_DONT_CULL_FRUSTUM | RF_DONT_CULL_FACES, nullptr);
    const std::vector<U32> fullAmbientDiffuse = gameMeshDiffuse;
    if (!gpuMode)
    {
        lights->set_ambient_light(32, 48, 64);
        engine->render_instance(&camera, engineInstance, EN_DONT_RECURSE, 1.0F,
            RF_DONT_CULL_FRUSTUM | RF_DONT_CULL_FACES, nullptr);
        if (!Expect(!fullAmbientDiffuse.empty() &&
                    gameMeshDiffuse.size() == fullAmbientDiffuse.size() &&
                    gameMeshDiffuse != fullAmbientDiffuse,
                    "production PolyMesh submits ambient-dependent vertex colors")) return 1;
        lights->set_ambient_light(255, 255, 255);
    }
    // Planet hover passes opacity per Engine instance. The shared archetype
    // retains its authored transparency; the next instance must render normally.
    auto *opacityMesh = renderer->get_instance_mesh(engineInstance, nullptr);
    if (!Expect(opacityMesh && opacityMesh->material_list,
                "instance opacity uses the real shared PolyMesh material")) return 1;
    const U8 authoredAlpha = opacityMesh->material_list[0].transparency;
    const U32 authoredFlags = opacityMesh->material_list[0].flags;
    const auto opacitySibling = engine->create_instance2(engineArchetype, nullptr);
    if (!Expect(opacitySibling != INVALID_INSTANCE_INDEX,
                "opacity sibling shares the actual Engine archetype")) return 1;
    const U32 ordinaryFlags = RF_DONT_CULL_FRUSTUM | RF_DONT_CULL_FACES;
    const bool oldAcceptPolyMesh = acceptPolyMeshCpuDraw;
    acceptPolyMeshCpuDraw = true;
    engine->render_instance(&camera, engineInstance, EN_DONT_RECURSE, 1.0F, ordinaryFlags, nullptr);
    const auto opaqueColors = gameMeshDiffuse;
    const auto opaqueState = gameMeshStateAtSubmit;
    if (!Expect(!opaqueColors.empty() && (opaqueColors[0] >> 24) == authoredAlpha,
                "ordinary instance retains authored material alpha")) return 1;
#if defined(FRONTIERX_GAME_GPU)
    FrontierX::SdlGpuReadback ordinaryOpacityImage;
    std::string opacityGpuError;
    if (gpuMode && !Expect(gpu.renderer.Readback64(ordinaryOpacityImage, opacityGpuError) &&
        ordinaryOpacityImage.rgbValid && ordinaryOpacityImage.changedPixels > 0,
        "ordinary shared-instance opacity reaches Vulkan readback")) return 1;
#endif
    for (const U8 opacity : {U8(160), U8(0), U8(255), U8(160)})
    {
        const auto drawsBefore = gameMeshDrawCalls;
        engine->render_instance(&camera, engineInstance, EN_DONT_RECURSE, 1.0F,
            ordinaryFlags | FrontierX::EncodeNativeInstanceOpacity(opacity), nullptr);
        const U32 expectedAlpha = U32(authoredAlpha) * opacity / 255;
        bool colorsMatch = gameMeshDiffuse.size() == opaqueColors.size();
        for (std::size_t i = 0; colorsMatch && i != gameMeshDiffuse.size(); ++i)
            colorsMatch = gameMeshDiffuse[i] == ((opaqueColors[i] & 0xffffffU) | (expectedAlpha << 24));
        if (!Expect(gameMeshDrawCalls == drawsBefore + 1 && colorsMatch &&
            opacityMesh->material_list[0].transparency == authoredAlpha &&
            opacityMesh->material_list[0].flags == authoredFlags &&
            (opacity == 255 || (gameMeshStateAtSubmit.alphaBlendEnabled &&
                gameMeshStateAtSubmit.sourceBlend == D3DBLEND_SRCALPHA &&
                gameMeshStateAtSubmit.destinationBlend == D3DBLEND_INVSRCALPHA &&
                !gameMeshStateAtSubmit.depthWriteEnabled)),
            "scoped instance opacity multiplies authored alpha and exposes geometry behind without shared mutation")) return 1;
#if defined(FRONTIERX_GAME_GPU)
        if (gpuMode)
        {
            FrontierX::SdlGpuReadback fadedImage;
            if (!Expect(gpu.renderer.Readback64(fadedImage, opacityGpuError) && fadedImage.rgbValid &&
                (opacity == 255 ? fadedImage.checksum == ordinaryOpacityImage.checksum :
                    fadedImage.checksum != ordinaryOpacityImage.checksum) &&
                (opacity != 0 || fadedImage.changedPixels == 0),
                "Vulkan instance opacity exposes background and full opacity restores original image")) return 1;
        }
#endif
        engine->render_instance(&camera, opacitySibling, EN_DONT_RECURSE, 1.0F, ordinaryFlags, nullptr);
        if (!Expect(gameMeshDiffuse == opaqueColors &&
            gameMeshStateAtSubmit.depthWriteEnabled == opaqueState.depthWriteEnabled &&
            gameMeshStateAtSubmit.alphaBlendEnabled == opaqueState.alphaBlendEnabled &&
            gameMeshStateAtSubmit.sourceBlend == opaqueState.sourceBlend &&
            gameMeshStateAtSubmit.destinationBlend == opaqueState.destinationBlend,
            "adjacent shared-archetype instance restores ordinary color, blending and depth writes")) return 1;
#if defined(FRONTIERX_GAME_GPU)
        if (gpuMode)
        {
            FrontierX::SdlGpuReadback restoredImage;
            if (!Expect(gpu.renderer.Readback64(restoredImage, opacityGpuError) && restoredImage.rgbValid &&
                restoredImage.checksum == ordinaryOpacityImage.checksum,
                "adjacent ordinary instance restores the original Vulkan image")) return 1;
        }
#endif
    }
    engine->destroy_instance(opacitySibling);
    acceptPolyMeshCpuDraw = oldAcceptPolyMesh;
    std::cout << "Original Engine/RenderMgr/PolyMesh instance opacity and shared-material restoration checked\n";
    Transform observedModelview(false);
    if (!Expect(camera.inverseQueries != 0U &&
                    transformCapture.modelviewCalls != 0U &&
                    transformCapture.translation.z == -5.0F &&
                    primitive->get_modelview(observedModelview) == GR_OK &&
                    observedModelview.translation.z == -5.0F &&
                    gameDrawCalls == 0U && gameMeshDrawCalls != 0U &&
                    gameMeshIndices == 6U && gameMeshSecondUvMatches,
                "production PolyMesh indexed draw reaches the native format boundary")) return 1;
#if defined(FRONTIERX_GAME_GPU)
    std::uint64_t gameGpuChecksum = 0U;
    if (gpuMode)
    {
        FrontierX::SdlGpuReadback image;
        FrontierX::SdlGpuReadback untexturedImage;
        std::string gpuError;
        bool presented = false;
        const U32 gameTextureHandle = gpu.renderer.CurrentTextureHandle();
        if (!Expect(gameTextureHandle != 0U &&
                    gpu.renderer.IsTexture(gameTextureHandle) &&
                    gpu.renderer.Readback64(image, gpuError) && image.rgbValid &&
                    image.changedPixels > 0U &&
                    gpu.renderer.BindTexture(0U, 0U) == GR_OK &&
                    gpu.renderer.Readback64(untexturedImage, gpuError) &&
                    untexturedImage.rgbValid &&
                    untexturedImage.checksum != image.checksum &&
                    gpu.renderer.BindTexture(0U, gameTextureHandle) == GR_OK &&
                    gpu.renderer.Present(presented, gpuError) && presented,
                    "production Engine PolyMesh texture reaches visible Vulkan output"))
        {
            std::cerr << gpuError << " handle=" << gameTextureHandle
                      << " changed=" << image.changedPixels
                      << " plain=" << untexturedImage.checksum
                      << " texture=" << image.checksum << '\n';
            return 1;
        }
        std::cout << "game_gpu checksum=" << image.checksum
                  << " untextured=" << untexturedImage.checksum
                  << " changed=" << image.changedPixels
                  << " texture=" << gameTextureHandle << '\n';
        gameGpuChecksum = image.checksum;
        if (captureRequested && !Expect(WriteP6(
                std::string(argv[8]) + "-game.ppm", image),
                "bounded production PolyMesh P6 capture")) return 1;
    }
#endif
    ProbeAppVertexBufferManager appBuffers;
    cpuMaterialBackendLive = false;
    VertexConstantCapture appVertexConstants;
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode) appVertexConstants.gpu = &gpu.renderer;
#endif
    auto * appPipeline = new FrontierX::NativeRenderPipeline(
        primitive, textureOperations, CaptureVertexConstants,
        &appVertexConstants);
    ENGINE = engine;
    REND = renderer;
    PIPE = appPipeline;
    TEXLIB = textures;
    const auto animTexturesPath = std::filesystem::path(meshPath.string() + ".anim-shared-textures");
    IFileSystem *animTexturesArchive = nullptr;
    if (!Expect(FrontierXTest::WriteAnim2DSharedTextureFixture(animTexturesPath) &&
                FrontierX::OpenNativeUtfFileSystem(animTexturesPath, &animTexturesArchive, error) &&
                textures->load_library(animTexturesArchive, nullptr) == GR_OK,
                "Anim2D shared fixture loads actual production texture library")) return 1;
    animTexturesArchive->Release();
    U32 heldAnimTexture = 0U;
    if (!Expect(textures->get_texture_id("AnimSharedTile", &heldAnimTexture) == GR_OK && heldAnimTexture,
                "shared animation fixture retains its genuine texture owner across source archetypes")) return 1;
    // Shared texture resolution must produce a real current frame. Required
    // malformed records fail with the original directory restored, no draw.
    for (unsigned animMode = 0U; animMode < 8U; ++animMode)
    {
        const auto animPath = std::filesystem::path(meshPath.string() + ".anim" + std::to_string(animMode));
        IFileSystem *animArchive = nullptr;
        if (!Expect(WriteAnim2DFixture(animPath, animMode) &&
                    FrontierX::OpenNativeUtfFileSystem(animPath, &animArchive, error),
                    "generated required Anim2D archive opens")) return 1;
        const unsigned before = gameMeshDrawCalls + gameStandardDrawCalls;
        bool rejected = false;
        std::string rejection;
        auto *anim = new AnimArchetype;
        try { anim->load(animArchive); }
        catch (const std::runtime_error &failure) { rejected = true; rejection = failure.what(); std::cerr << "Anim2D mode=" << animMode << " rejection: " << rejection << '\n'; }
        if (animMode == 0U || animMode == 6U)
        {
            U32 tile = 0U;
            ITL_TEXTUREFRAME_IRP frame{};
            if (!Expect(!rejected && textures->get_texture_id("AnimSharedTile", &tile) == GR_OK &&
                        textures->get_texture_frame(tile, ITL_FRAME_FIRST, &frame) == GR_OK &&
                        frame.rp_texture_id && anim->frame_cnt == 1U && anim->texCnt == 1U &&
                        anim->frames[0].texture == frame.rp_texture_id &&
                        anim->frames[0].x0 == 0.125F && anim->frames[0].y0 == 0.25F &&
                        anim->frames[0].x1 == 0.875F && anim->frames[0].y1 == 0.75F,
                        "original Anim2D resolves shared texture and exact authored frame UVs")) return 1;
            if (!Expect(textures->release_texture_id(tile) == GR_OK,
                        "Anim2D fixture releases its independently acquired frame lookup")) return 1;
        }
        else
        {
            const char *expectedErrors[6] = {"Native Anim2D required frame texture unavailable",
                "Native Anim2D frame references invalid texture index", "Native Anim2D frame record extent invalid",
                "Native Anim2D texture-name record invalid", "Native Anim2D texture-name count exceeds source array",
                "Native Anim2D flat animation record missing"};
            if (!Expect(rejected && rejection == expectedErrors[animMode == 7U ? 5U : animMode - 1U],
                        "required Anim2D frame, index, extent or names bounds fail at the exact owner check")) return 1;
        }
        delete anim;
        DAFILEDESC frequency("Frequency");
        HANDLE restoredRoot = animArchive->OpenChild(&frequency);
        if (!Expect(restoredRoot != INVALID_HANDLE_VALUE &&
                    before == gameMeshDrawCalls + gameStandardDrawCalls,
                    "original Anim2D failed/loaded owner restores directory and submits no fake draw")) return 1;
        animArchive->CloseHandle(restoredRoot);
        animArchive->Release();
    }
    if (!Expect(textures->release_texture_id(heldAnimTexture) == GR_OK,
                "shared animation fixture releases its original texture ownership")) return 1;
    std::cout << "Original Anim2D shared frames resolved and malformed required frames rejected with clean ownership\n";
    MATMAN = materials;
    vb_mgr = &appBuffers;
    CQFLAGS.b3DEnabled = -1;
    IMeshRender *appRender = CreateMeshRender();
    appRender->AddRef();
    appRender->Init(engineArchetype);
    IMeshInfoTree *appTree = CreateMeshInfoTree(engineInstance);
    appRender->SetupMeshInfo(appTree->GetMeshInfo());
    // The M_COLLECTOR upgrade owner detaches an initialized add/remove tree
    // before MantisBuild reads its renderer vertices. Protect that ownership
    // contract here; actual ObjectExtension transitions require mission runtime.
    {
        std::array<INSTANCE_INDEX, 5> extensionInstances{};
        for (auto &instance : extensionInstances)
            instance = engine->create_instance2(engineArchetype, nullptr);
        JointInfo extensionJoint{};
        extensionJoint.type = JT_FIXED;
        extensionJoint.rel_orientation.set_identity();
        const INSTANCE_INDEX root = extensionInstances[0], add = extensionInstances[1];
        const INSTANCE_INDEX remove = extensionInstances[2], sibling = extensionInstances[3];
        const INSTANCE_INDEX grandchild = extensionInstances[4];
        if (!Expect(engine->create_joint(root, add, &extensionJoint) &&
                    engine->create_joint(root, remove, &extensionJoint) &&
                    engine->create_joint(root, sibling, &extensionJoint) &&
                    engine->create_joint(add, grandchild, &extensionJoint),
                    "extension fixture creates three siblings and an attached grandchild")) return 1;
        IMeshInfoTree *extensionTree = CreateMeshInfoTree(root);
        std::array<MeshInfo *, 5> children{};
        if (!Expect(extensionTree->ListChildren(children.data()) == children.size(),
                    "extension fixture enumerates the complete original mesh tree")) return 1;
        for (MeshInfo *child : children) appRender->SetupMeshInfo(child, false);
        IMeshInfoTree *addTree = nullptr, *removeTree = nullptr;
        extensionTree->GetChildInfo(add, &addTree);
        extensionTree->GetChildInfo(remove, &removeTree);
        MeshInfo *const addInfo = addTree->GetMeshInfo();
        MeshInfo *const removeInfo = removeTree->GetMeshInfo();
        auto contains = [&](IMeshInfoTree *tree, INSTANCE_INDEX expected, unsigned count) {
            const unsigned actualCount = tree->ListChildren(children.data());
            if (actualCount != count) return false;
            for (unsigned i = 0; i < actualCount; ++i)
                if (children[i]->instanceIndex == expected) return true;
            return false;
        };
        for (unsigned cycle = 0; cycle < 2; ++cycle)
        {
            if (!Expect(engine->destroy_joint(root, add), "extension add joint detaches")) return 1;
            extensionTree->DetachChild(add, &addTree);
            if (!Expect(contains(addTree, grandchild, 2) &&
                        contains(extensionTree, sibling, 3) &&
                        addTree->GetMeshInfo() == addInfo && addInfo->mr == appRender &&
                        addInfo->mr->pos_cnt > 0 && addInfo->mr->pos_list &&
                        !addInfo->bOwnsInstance,
                        "detached extension keeps only its subtree and initialized borrowed renderer vertices")) return 1;
            if (!Expect(engine->create_joint(root, add, &extensionJoint), "extension add joint reattaches")) return 1;
            extensionTree->AttachChild(root, addTree);
            if (!Expect(engine->destroy_joint(root, remove), "extension remove joint detaches")) return 1;
            extensionTree->DetachChild(remove, &removeTree);
            if (!Expect(contains(extensionTree, grandchild, 4) &&
                        contains(removeTree, remove, 1) &&
                        removeTree->GetMeshInfo() == removeInfo && removeInfo->mr == appRender,
                        "completed extension transfers the add subtree and isolates the replaced mesh")) return 1;
            if (!Expect(engine->create_joint(root, remove, &extensionJoint), "cancelled extension restores removed joint")) return 1;
            extensionTree->AttachChild(root, removeTree);
            if (!Expect(contains(extensionTree, sibling, 5),
                        "repeated extension transfer restores every sibling exactly once")) return 1;
        }
        if (!Expect(engine->destroy_joint(root, add), "extension teardown detaches its owned add instance")) return 1;
        extensionTree->DetachChild(add, &addTree);
        DestroyMeshInfoTree(addTree);
        DestroyMeshInfoTree(extensionTree);
        bool borrowedInstancesSurvive = true;
        for (const auto instance : extensionInstances)
            borrowedInstancesSurvive &= instanceHasArchetype(instance, engineArchetype);
        if (!Expect(borrowedInstancesSurvive,
                    "extension mesh trees release renderer references without destroying borrowed Engine instances")) return 1;
        engine->destroy_instance(root);
        engine->destroy_instance(add);
    }
    const unsigned appDrawsBefore = gameStandardDrawCalls;
    BATCH = primitive;
    MAINCAM = &camera;
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode)
    {
        const U32 rebuildsBeforeFactor = gpu.renderer.PipelineRebuilds();
        if (!Expect(primitive->set_render_state(
                        D3DRS_TEXTUREFACTOR, 0xFF40A0E0U) == GR_OK &&
                    gpu.renderer.PipelineRebuilds() == rebuildsBeforeFactor,
                    "native App texture factor updates a uniform without pipeline rebuild"))
            return 1;
        transformCapture.submitAppGpu = true;
    }
#endif
    appRender->Render(appTree->GetMeshInfo(), camera.inverse);
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode) transformCapture.submitAppGpu = false;
#endif
    MAINCAM = nullptr;
    BATCH = nullptr;
    std::size_t appMaximumIndex = 0U;
    for (U16 index : gameStandardIndices)
        if (index > appMaximumIndex) appMaximumIndex = index;
    const bool appDrawSubmitted = gameStandardDrawCalls == appDrawsBefore + 1U &&
        gameStandardIndices.size() == 6U &&
        gameStandardVertices.size() == appMaximumIndex + 1U;
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode && !Expect(transformCapture.appStateAtSubmit,
                           "production App draw selects culling and texture factor"))
        return 1;
#endif
    const bool appMeshInitialized = appTree->GetMeshInfo()->mr == appRender &&
        appTree->GetMeshInfo()->faceGroupCnt != 0U &&
        appTree->GetMeshInfo()->faceRenders != nullptr &&
        appRender->pos_cnt != 0U && appRender->face_cnt != 0U;
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode)
    {
        FrontierX::SdlGpuReadback whiteFactorImage;
        FrontierX::SdlGpuReadback oppositeCullImage;
        std::string gpuError;
        const auto & appImage = transformCapture.appImageAtSubmit;
        if (!Expect(transformCapture.appPresentedAtSubmit &&
                    appImage.rgbValid && appImage.changedPixels > 0U &&
                    appImage.checksum != gameGpuChecksum &&
                    primitive->set_render_state(D3DRS_CULLMODE, D3DCULL_CCW) == GR_OK &&
                    gpu.renderer.Readback64(oppositeCullImage, gpuError) &&
                    oppositeCullImage.rgbValid &&
                    oppositeCullImage.checksum != appImage.checksum &&
                    primitive->set_render_state(D3DRS_CULLMODE, D3DCULL_CW) == GR_OK &&
                    primitive->set_render_state(
                        D3DRS_TEXTUREFACTOR, 0xFFFFFFFFU) == GR_OK &&
                    gpu.renderer.Readback64(whiteFactorImage, gpuError) &&
                    whiteFactorImage.rgbValid &&
                    whiteFactorImage.checksum != appImage.checksum &&
                    primitive->set_render_state(D3DRS_CULLMODE, D3DCULL_NONE) == GR_OK,
                    "production App legacy MeshRender reaches visible Vulkan output"))
        {
            std::cerr << gpuError << ' ' << transformCapture.appGpuError
                      << " changed=" << appImage.changedPixels << '\n';
            return 1;
        }
        FrontierX::SdlGpuReadback constantBaseline;
        FrontierX::SdlGpuReadback constantImage;
        FrontierX::SdlGpuReadback constantRestored;
        const float materialConstants[8] = {
            0.25F, 0.0F, 1.0F, 1.0F,
            1.0F, 0.25F, 0.25F, 1.0F};
        const float defaultConstants[8] = {
            0.0F, 0.0F, 1.0F, 1.0F,
            1.0F, 1.0F, 1.0F, 1.0F};
        if (!Expect(primitive->set_texture_stage_state(
                        0U, D3DTSS_COLOROP, D3DTOP_SELECTARG1) == GR_OK &&
                    primitive->set_texture_stage_state(
                        0U, D3DTSS_COLORARG1, D3DTA_DIFFUSE) == GR_OK &&
                    primitive->set_texture_stage_state(
                        0U, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1) == GR_OK &&
                    primitive->set_texture_stage_state(
                        0U, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE) == GR_OK &&
                    gpu.renderer.Readback64(constantBaseline, gpuError) &&
                    constantBaseline.rgbValid &&
                    appPipeline->set_vs_constants(30U, materialConstants, 2U) == GR_OK &&
                    gpu.renderer.Readback64(constantImage, gpuError) &&
                    constantImage.rgbValid &&
                    constantImage.checksum != constantBaseline.checksum &&
                    appPipeline->set_vs_constants(29U, materialConstants, 1U) ==
                        GR_NOT_IMPLEMENTED &&
                    appPipeline->set_vs_constants(30U, defaultConstants, 2U) == GR_OK &&
                    gpu.renderer.Readback64(constantRestored, gpuError) &&
                    constantRestored.rgbValid &&
                    constantRestored.checksum == constantBaseline.checksum,
                    "native pipeline c30/c31 reach the SDL vertex shader with rollback"))
        {
            std::cerr << gpuError << " baseline=" << constantBaseline.checksum
                      << " changed=" << constantImage.checksum
                      << " restored=" << constantRestored.checksum << '\n';
            return 1;
        }
        FrontierX::SdlGpuReadback uvBaseline;
        FrontierX::SdlGpuReadback uvShifted;
        FrontierX::SdlGpuReadback uvRestored;
        const float shiftedUv[4] = {0.5F, 0.0F, 1.0F, 1.0F};
        if (!Expect(gpu.renderer.BindTexture(
                        0U, gpu.renderer.FixtureTextureHandle()) == GR_OK &&
                    primitive->set_texture_stage_state(
                        0U, D3DTSS_COLORARG1, D3DTA_TEXTURE) == GR_OK &&
                    gpu.renderer.Readback64(uvBaseline, gpuError) &&
                    uvBaseline.rgbValid &&
                    appPipeline->set_vs_constants(30U, shiftedUv, 1U) == GR_OK &&
                    gpu.renderer.Readback64(uvShifted, gpuError) &&
                    uvShifted.rgbValid &&
                    uvShifted.checksum != uvBaseline.checksum &&
                    appPipeline->set_vs_constants(30U, defaultConstants, 1U) == GR_OK &&
                    gpu.renderer.Readback64(uvRestored, gpuError) &&
                    uvRestored.rgbValid &&
                    uvRestored.checksum == uvBaseline.checksum,
                    "native pipeline c30 shifts texture coordinates and restores them"))
        {
            std::cerr << gpuError << " uv=" << uvBaseline.checksum
                      << " shifted=" << uvShifted.checksum
                      << " restored=" << uvRestored.checksum << '\n';
            return 1;
        }
        std::cout << "app_gpu checksum=" << appImage.checksum
                  << " opposite_cull=" << oppositeCullImage.checksum
                  << " white_factor=" << whiteFactorImage.checksum
                  << " changed=" << appImage.changedPixels << '\n';
        if (captureRequested && !Expect(WriteP6(
                std::string(argv[8]) + "-app.ppm", appImage),
                "bounded production App P6 capture")) return 1;
    }
#endif
    // The full Terran material adapter must use the already cooked FG1 geometry:
    // changing factory/cook would alter source vertex ordering and RNG draws.
    const int cookedPositions = appRender->pos_cnt;
    const int cookedFaces = appRender->face_cnt;
    IRenderPrimitive *legacyPrimitive = primitive;
    FrontierX::NativeRenderPipeline *legacyPipeline = nullptr;
    if (!gpuMode)
    {
        FrontierX::NativeRenderPrimitiveDesc legacyDescriptor(
            RejectGameDraw, &transformCapture, CaptureNativeLegacyDraw, CaptureLegacyState,
            nullptr, nullptr, CaptureLegacyStage, CaptureLegacyTexture,
            CaptureGameModelview, CaptureGameProjection, nullptr, nullptr,
            CaptureLegacySampler);
        if (!Expect(components->CreateInstance(&legacyDescriptor,
                    reinterpret_cast<void **>(&legacyPrimitive)) == GR_OK,
                    "checked original material CPU backend creation")) return 1;
        legacyPipeline = new FrontierX::NativeRenderPipeline(legacyPrimitive);
        PIPE = legacyPipeline;
    }
    auto &legacyMaterial = appTree->GetMeshInfo()->fgi[0];
    const LightRGB_U8 originalEmission = legacyMaterial.emissive;
    const U8 originalLegacyAlpha = legacyMaterial.a;
    const U32 originalEmissiveTexture = legacyMaterial.emissive_texture_id;
    LIGHT = lights;
    BATCH = legacyPrimitive;
    MAINCAM = &camera;
    NATIVE_USE_ORIGINAL_LEGACY_MATERIALS = true;
    cpuMaterialBackendLive = true;
    U32 textureStages = 0U, textureCoordinates = 0U;
    U32 blendMatrix[6] = {0xABCDEF01U, 0U, 0U, 0U, 0U, 0x12345678U};
    U32 multiplySource = 0U, multiplyDestination = 0U;
    U32 addSource = 0U, addDestination = 0U, alphaSource = 0U, alphaDestination = 0U;
    if (!Expect(PIPE->query_device_ability(RP_A_TEXTURE_STAGES, &textureStages) == GR_OK && textureStages == 2U &&
                PIPE->query_device_ability(RP_A_TEXTURE_COORDINATES, &textureCoordinates) == GR_OK && textureCoordinates == 2U &&
                PIPE->query_device_ability(RP_A_BLEND_MATRIX, &blendMatrix[1]) == GR_OK &&
                blendMatrix[0] == 0xABCDEF01U && blendMatrix[5] == 0x12345678U &&
                blendMatrix[1] == 0xFFFFFFFFU && blendMatrix[2] == 0xFFFFFFFFU &&
                blendMatrix[3] == 0xFFFFFFFFU && blendMatrix[4] == 0xFU &&
                PIPE->query_device_ability(RP_A_BLEND_MUL_SRC, &multiplySource) == GR_OK && multiplySource == D3DBLEND_ZERO &&
                PIPE->query_device_ability(RP_A_BLEND_MUL_DST, &multiplyDestination) == GR_OK && multiplyDestination == D3DBLEND_SRCCOLOR &&
                PIPE->query_device_ability(RP_A_BLEND_ADD_SRC, &addSource) == GR_OK && addSource == D3DBLEND_ONE &&
                PIPE->query_device_ability(RP_A_BLEND_ADD_DST, &addDestination) == GR_OK && addDestination == D3DBLEND_ONE &&
                PIPE->query_device_ability(RP_A_BLEND_TRANSPARENCY_SRC, &alphaSource) == GR_OK && alphaSource == D3DBLEND_SRCALPHA &&
                PIPE->query_device_ability(RP_A_BLEND_TRANSPARENCY_DST, &alphaDestination) == GR_OK && alphaDestination == D3DBLEND_INVSRCALPHA,
                "native selected PolyMesh caps retain four-word blend matrix bounds and implemented blend pairs")) return 1;
    FrontierX::NativeRenderPipeline unboundPipeline;
    U32 unavailableAbility = 99U;
    if (!Expect(unboundPipeline.query_device_ability(RP_A_TEXTURE_STAGES, &unavailableAbility) == GR_NOT_IMPLEMENTED &&
                unavailableAbility == 0U && unboundPipeline.query_device_ability(RP_A_BLEND_MATRIX, &unavailableAbility) == GR_NOT_IMPLEMENTED,
                "unbound native pipeline rejects material capabilities")) return 1;
    legacyMaterial.emissive_texture_id = 0U;
    legacyMaterial.a = 127U;
    legacyMaterial.emissive = {7U, 11U, 13U};
    lights->set_ambient_light(32, 48, 64);
    std::srand(947U);
    const int nextWithoutRender = std::rand();
    std::srand(947U);
    const unsigned nativeDrawsBefore = nativeLegacyDrawCalls;
    const unsigned legacyStateCallsBefore = capturedLegacyStateCalls;
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode) transformCapture.submitNativeLegacyGpu = true;
#endif
    appRender->Render(appTree->GetMeshInfo(), camera.inverse);
    const int nextAfterRender = std::rand();
    const U32 expectedLegacyColor = FrontierX::PackDiffuseBgra(
        static_cast<U8>(((U32(legacyMaterial.diffuse.r) * 32U) >> 8) + 7U),
        static_cast<U8>(((U32(legacyMaterial.diffuse.g) * 48U) >> 8) + 11U),
        static_cast<U8>(((U32(legacyMaterial.diffuse.b) * 64U) >> 8) + 13U),
        legacyMaterial.a);
    bool originalLegacyColors = nativeLegacyDrawCalls == nativeDrawsBefore + 1U &&
        nativeLegacyVertices.size() == 6U;
    for (const auto &vertex : nativeLegacyVertices)
        originalLegacyColors &= vertex.diffuseBgra == expectedLegacyColor &&
            vertex.u2 == vertex.u && vertex.v2 == vertex.v;
    if (!gpuMode && !Expect(capturedLegacyStateCalls > legacyStateCallsBefore &&
                capturedLegacyState.depthEnabled == 1U &&
                capturedLegacyState.depthFunction == D3DCMP_LESSEQUAL &&
                capturedLegacyState.cullMode == D3DCULL_NONE &&
                capturedLegacyState.alphaTestEnabled == 0U &&
                capturedLegacyState.alphaBlendEnabled == 1U &&
                capturedLegacyState.sourceBlend == D3DBLEND_SRCALPHA &&
                capturedLegacyState.destinationBlend == D3DBLEND_INVSRCALPHA &&
                capturedLegacyStage.colorOperation == D3DTOP_SELECTARG2 &&
                capturedLegacyStage.colorArgument2 == D3DTA_DIFFUSE &&
                capturedLegacyStage.alphaOperation == D3DTOP_MODULATE &&
                capturedLegacyStage.second.colorOperation == D3DTOP_DISABLE &&
                capturedLegacyStage.second.alphaOperation == D3DTOP_DISABLE &&
                capturedLegacySampler[0].minFilter == D3DTEXF_LINEAR &&
                capturedLegacySampler[1].mipFilter == D3DTEXF_LINEAR &&
                capturedLegacyTextures[0] == 0U && capturedLegacyTextures[1] == 0U,
                "original legacy material executes checked source depth/blend/texture/sampler states"))
    {
        std::cerr << "legacy state depth=" << capturedLegacyState.depthEnabled
                  << " cull=" << capturedLegacyState.cullMode
                  << " blend=" << capturedLegacyState.alphaBlendEnabled
                  << " color=" << capturedLegacyStage.colorOperation
                  << " alpha=" << capturedLegacyStage.alphaOperation
                  << " textures=" << capturedLegacyTextures[0] << ',' << capturedLegacyTextures[1]
                  << " material alpha=" << unsigned(legacyMaterial.a) << '\n';
        return 1;
    }
    if (!Expect(originalLegacyColors && nextWithoutRender == nextAfterRender &&
                appRender->pos_cnt == cookedPositions &&
                appRender->face_cnt == cookedFaces,
                "original legacy material preserves cooked geometry/RNG and source ambient/diffuse/emission/alpha/UV2"))
        return 1;
    // MassDisruptor reaches the original Puffy virtual on a live target.
    // The old native owner rejected it after the missing-chain crash was fixed.
    {
        const auto source = nativeLegacyVertices;
        std::size_t selected = 0;
        while (selected < 3 && source[selected].x == 0 && source[selected].y == 0 && source[selected].z == 0) ++selected;
        if (!Expect(selected < 3, "Puffy fixture first triangle has a radial vertex")) return 1;
        Vector position(source[selected].x, source[selected].y, source[selected].z);
        if (!Expect(position.magnitude_squared() > 0, "Puffy fixture has a radial vertex")) return 1;
        puffViewPt[0] = position; puffViewPt[0].normalize();
        puffTexture = 0;
        puffPercent = 0.4F;
        const Transform object = engine->get_transform(appTree->GetMeshInfo()->instanceIndex);
        if (!Expect(PIPE->set_modelview(camera.inverse * object) == GR_OK,
                    "Puffy uses the target root model/view")) return 1;
        const unsigned before = nativeLegacyDrawCalls;
        appRender->RenderPuffy(appTree->GetMeshInfo(), camera.inverse, object, Vector(4,4,4));
        Vector radial = position; radial.normalize(); radial *= 4;
        const Vector expected = 0.4F * radial + 0.6F * position;
        const auto &vertex = nativeLegacyVertices[selected];
        U32 puffyDepthWrite = 99, puffyBlend = 99;
        if (!Expect(PIPE->get_render_state(D3DRS_ZWRITEENABLE, &puffyDepthWrite) == GR_OK &&
                    PIPE->get_render_state(D3DRS_ALPHABLENDENABLE, &puffyBlend) == GR_OK,
                    "Puffy blend/depth state is readable at the live receiver")) return 1;
        if (!Expect(nativeLegacyDrawCalls == before + 2 &&
                    std::fabs(vertex.x - expected.x) < 1e-5F &&
                    std::fabs(vertex.y - expected.y) < 1e-5F &&
                    std::fabs(vertex.z - expected.z) < 1e-5F &&
                    (vertex.diffuseBgra & 0x00ffffffU) == 0x00ffffffU &&
                    (vertex.diffuseBgra >> 24) >= 254 &&
                    vertex.u2 == source[selected].u2 && vertex.v2 == source[selected].v2 &&
                    puffyDepthWrite == 0 && puffyBlend == 1,
                    "MassDisruptor source radial deformation and alpha overlay reach native submission")) return 1;
        appRender->Render(appTree->GetMeshInfo(), camera.inverse);
    }
    // TM9 reaches an authored chrome group through MeshRender::Render. The
    // normal legacy fixture cannot detect the former explicit rejection.
    {
        const auto plainVertices = nativeLegacyVertices;
        auto *group = appRender->GetNextFaceGroup(nullptr);
        const auto savedFlags = group->flags;
        const bool suppressed = appTree->GetMeshInfo()->bSuppressEmissive;
        group->flags |= MM_CHROME;
        appTree->GetMeshInfo()->bSuppressEmissive = true;
        bool accepted = false;
        try { appRender->Render(appTree->GetMeshInfo(), camera.inverse); accepted = true; }
        catch (const std::runtime_error &error) { std::cerr << "chrome caller: " << error.what() << '\n'; }
        bool mapped = accepted && nativeLegacyVertices.size() == plainVertices.size() &&
            !appTree->GetMeshInfo()->bSuppressEmissive;
        for (std::size_t i = 0; mapped && i < nativeLegacyVertices.size(); ++i)
        {
            const auto &v = nativeLegacyVertices[i];
            const auto &plain = plainVertices[i];
            mapped &= v.x == plain.x && v.y == plain.y && v.z == plain.z &&
                v.diffuseBgra == plain.diffuseBgra && v.u2 == plain.u2 && v.v2 == plain.v2 &&
                std::fabs(v.u - 0.5F) < 1e-5F && std::fabs(v.v - 0.5F) < 1e-5F;
        }
        if (!Expect(mapped, "production chrome caller preserves lighting/geometry/UV1 and maps normals to UV0")) return 1;
        appRender->Render(appTree->GetMeshInfo(), camera.inverse);
        if (!Expect(nativeLegacyVertices[0].u == 0.5F,
                    "chrome phase does not advance on repeated render")) return 1;
        UpdateRenderEffects(0.5F * PI);
        appRender->Render(appTree->GetMeshInfo(), camera.inverse);
        // The original TRANSFORM J rotation maps this fixture's +Z to -X.
        for (const auto &v : nativeLegacyVertices)
            if (!Expect(std::fabs(v.u) < 1e-5F && std::fabs(v.v - 0.5F) < 1e-5F,
                        "original render-effect update rotates chrome normals by a quarter turn")) return 1;
        UpdateRenderEffects(1.5F * PI);
        appRender->Render(appTree->GetMeshInfo(), camera.inverse);
        if (!Expect(std::fabs(nativeLegacyVertices[0].u - 0.5F) < 1e-5F,
                    "chrome full rotation restores original UV phase")) return 1;
        group->flags = savedFlags;
        appTree->GetMeshInfo()->bSuppressEmissive = suppressed;
        appRender->Render(appTree->GetMeshInfo(), camera.inverse);
    }
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode)
    {
        const auto dimImage = transformCapture.nativeLegacyImage;
        lights->set_ambient_light(255, 255, 255);
        appRender->Render(appTree->GetMeshInfo(), camera.inverse);
        const auto litImage = transformCapture.nativeLegacyImage;
        lights->set_ambient_light(32, 48, 64);
        appRender->Render(appTree->GetMeshInfo(), camera.inverse);
        const auto restoredImage = transformCapture.nativeLegacyImage;
        if (!Expect(dimImage.rgbValid && dimImage.changedPixels > 0U &&
                    litImage.rgbValid && litImage.checksum != dimImage.checksum &&
                    restoredImage.rgbValid && restoredImage.checksum == dimImage.checksum,
                    "original legacy source lighting visibly changes and restores textured pixels"))
            return 1;
        std::cout << "native_legacy_lighting_gpu dim=" << dimImage.checksum
                  << " lit=" << litImage.checksum
                  << " restored=" << restoredImage.checksum << '\n';
    }
#endif
    const auto seamPath = meshPath.parent_path() / "legacy_uv2_seam_fixture.utf";
    IFileSystem *seamArchive = nullptr;
    if (!Expect(FrontierXTest::WriteLegacySeamGameMeshFixture(seamPath, true) &&
                FrontierX::OpenNativeUtfFileSystem(seamPath, &seamArchive, error),
                "authored legacy second-UV seam fixture")) return 1;
    const auto seamArchetype = engine->create_archetype("legacy_seam.3db", seamArchive);
    const auto seamInstance = engine->create_instance2(seamArchetype, nullptr);
    if (!Expect(seamArchetype != INVALID_ARCHETYPE_INDEX &&
                seamInstance != INVALID_INSTANCE_INDEX,
                "production mesh loads independently authored UV2 seam")) return 1;
    Mesh *seamMesh = renderer->get_instance_mesh(seamInstance, nullptr);
    if (!Expect(seamMesh && seamMesh->material_list && seamMesh->material_cnt == 1,
                "production PolyMesh exposes authored emissive material")) return 1;
    auto &polyMaterial = seamMesh->material_list[0];
    if (!Expect(polyMaterial.texture_id != 0U && polyMaterial.emissive_texture_id != 0U,
                "production authored seam resolves both diffuse and emissive texture frames")) return 1;
    acceptPolyMeshCpuDraw = true;
#if defined(FRONTIERX_GAME_GPU)
    transformCapture.capturePolyMeshGpu = gpuMode;
#endif
    const unsigned polyDrawsBefore = gameMeshDrawCalls;
    polyMaterial.num_passes = 0;
    engine->render_instance(&camera, seamInstance, EN_DONT_RECURSE, 1.0F,
                           RF_DONT_CULL_FRUSTUM | RF_DONT_CULL_FACES, nullptr);
    if (!Expect(gameMeshDrawCalls == polyDrawsBefore + 1U && polyMaterial.num_passes == 1,
                "production two-texture PolyMesh validates and submits exactly one combined pass")) return 1;
    const unsigned drawsBeforeReject = gameMeshDrawCalls;
    if (!Expect(primitive->set_texture_stage_state(2U, D3DTSS_COLOROP, D3DTOP_ADD) == GR_NOT_IMPLEMENTED &&
                gameMeshDrawCalls == drawsBeforeReject,
                "unsupported third texture stage rejects before any draw")) return 1;
    if (!gpuMode)
    {
        cpuMaterialBackendLive = false;
        if (!Expect(primitive->verify_state() == GR_NOT_IMPLEMENTED && gameMeshDrawCalls == drawsBeforeReject,
                    "verification rejects a currently unavailable material receiver before drawing")) return 1;
        cpuMaterialBackendLive = true;
        rejectCpuStageCall = 1U;
        if (!Expect(primitive->verify_state() == GR_GENERIC && rejectCpuStageCall == 0U &&
                    gameMeshDrawCalls == drawsBeforeReject,
                    "verification propagates a real current texture-state receiver rejection")) return 1;
        if (!Expect(primitive->verify_state() == GR_OK,
                    "verification revalidates the restored checked material receiver")) return 1;
    }
    const U32 originalSecondDiffuse = polyMaterial.second_diffuse_texture_id;
    polyMaterial.second_diffuse_texture_id = polyMaterial.texture_id;
    polyMaterial.num_passes = 0;
    engine->render_instance(&camera, seamInstance, EN_DONT_RECURSE, 1.0F,
                           RF_DONT_CULL_FRUSTUM | RF_DONT_CULL_FACES, nullptr);
    if (!Expect(gameMeshDrawCalls == drawsBeforeReject,
                "production PolyMesh required third texture stage fails before any draw")) return 1;
    polyMaterial.second_diffuse_texture_id = originalSecondDiffuse;
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode)
    {
        const auto addImage = transformCapture.polyMeshImage;
        const U32 emissiveTexture = polyMaterial.emissive_texture_id;
        polyMaterial.emissive_texture_id = 0U;
        polyMaterial.num_passes = 0;
        engine->render_instance(&camera, seamInstance, EN_DONT_RECURSE, 1.0F,
                               RF_DONT_CULL_FRUSTUM | RF_DONT_CULL_FACES, nullptr);
        const auto baseImage = transformCapture.polyMeshImage;
        polyMaterial.emissive_texture_id = emissiveTexture;
        polyMaterial.num_passes = 0;
        engine->render_instance(&camera, seamInstance, EN_DONT_RECURSE, 1.0F,
                               RF_DONT_CULL_FRUSTUM | RF_DONT_CULL_FACES, nullptr);
        const auto restoredImage = transformCapture.polyMeshImage;
        if (!Expect(addImage.rgbValid && baseImage.rgbValid && restoredImage.rgbValid &&
                    addImage.checksum != baseImage.checksum && restoredImage.checksum == addImage.checksum,
                    "single-pass PolyMesh emissive combination visibly changes and restores authored UV2 pixels")) return 1;
        std::cout << "polymesh_single_pass_gpu add=" << addImage.checksum << " base=" << baseImage.checksum
                  << " restored=" << restoredImage.checksum << '\n';
        // Keep the primitive's accepted binding while destroying its real GPU
        // owner: verify_state must detect the now-stale handle before drawing.
        U32 staleTexture = 0U;
        const std::uint8_t stalePixel[4] = {19U, 31U, 47U, 255U};
        const unsigned drawsBeforeStale = gameMeshDrawCalls;
        if (!Expect(gpu.renderer.CreateTexture(1U, 1U, staleTexture) == GR_OK &&
                    gpu.renderer.ReplaceTexture(staleTexture, {stalePixel, 1U, 1U, 4U}) == GR_OK &&
                    primitive->set_texture_stage_texture(1U, staleTexture) == GR_OK &&
                    primitive->verify_state() == GR_OK &&
                    gpu.renderer.BindTexture(1U, 0U) == GR_OK &&
                    gpu.renderer.DestroyTexture(staleTexture) == GR_OK &&
                    primitive->verify_state() != GR_OK && gameMeshDrawCalls == drawsBeforeStale,
                    "material verification rejects a destroyed real GPU texture binding before drawing")) return 1;
        if (!Expect(primitive->set_texture_stage_texture(1U, 0U) == GR_OK &&
                    primitive->verify_state() == GR_OK,
                    "material verification succeeds after explicitly restoring a valid GPU binding")) return 1;
        std::cout << "Production material verification rejects stale GPU texture bindings\n";
        transformCapture.capturePolyMeshGpu = false;
    }
#endif
    acceptPolyMeshCpuDraw = false;
    ProbeAppVertexBufferManager seamBuffers;
    vb_mgr = &seamBuffers;
    IMeshRender *seamRender = CreateMeshRender();
    seamRender->AddRef();
    seamRender->Init(seamArchetype);
    IMeshInfoTree *seamTree = CreateMeshInfoTree(seamInstance);
    seamRender->SetupMeshInfo(seamTree->GetMeshInfo());
    if (!Expect((seamTree->GetMeshInfo()->fgi[0].texture_flags & TF_F_HAS_ALPHA) == 0U &&
                (seamTree->GetMeshInfo()->fgi[0].emissive_texture_flags & TF_F_HAS_ALPHA) == 0U,
                "original material flags retain authored opaque diffuse and emissive texture classification")) return 1;
    seamRender->Render(seamTree->GetMeshInfo(), camera.inverse);
    bool zeroCornerBase = false;
    bool zeroCornerSeam = false;
    for (const auto &vertex : nativeLegacyVertices)
    {
        if (vertex.x == 0.0F && vertex.y == 0.0F && vertex.z == 0.0F &&
            vertex.u == 0.0F && vertex.v == 0.0F)
        {
            zeroCornerBase |= vertex.u2 == 0.0F && vertex.v2 == 0.0F;
            zeroCornerSeam |= vertex.u2 == 1.0F && vertex.v2 == 1.0F;
        }
    }
    if (!Expect(zeroCornerBase && zeroCornerSeam,
                "legacy FG1 cooking preserves independent per-corner emissive UV seam"))
    {
        for (const auto &vertex : nativeLegacyVertices)
            std::cerr << "legacy corner " << vertex.x << ',' << vertex.y << ',' << vertex.z
                      << " uv=" << vertex.u << ',' << vertex.v
                      << " uv2=" << vertex.u2 << ',' << vertex.v2 << '\n';
        return 1;
    }
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode)
    {
        const auto noEmissiveImage = transformCapture.nativeLegacyImage;
        const auto previousEmissiveEffect = CQEFFECTS.bEmissiveTextures;
        auto &seamMaterial = seamTree->GetMeshInfo()->fgi[0];
        seamMaterial.emissive_texture_id = seamMaterial.texture_id;
        CQEFFECTS.bEmissiveTextures = GlobalEffectsOptions::on;
        seamRender->Render(seamTree->GetMeshInfo(), camera.inverse);
        const auto emissiveImage = transformCapture.nativeLegacyImage;
        seamTree->GetMeshInfo()->bSuppressEmissive = true;
        seamRender->Render(seamTree->GetMeshInfo(), camera.inverse);
        const auto suppressedImage = transformCapture.nativeLegacyImage;
        if (!Expect(noEmissiveImage.rgbValid && noEmissiveImage.changedPixels > 0U &&
                    emissiveImage.rgbValid && emissiveImage.checksum != noEmissiveImage.checksum &&
                    suppressedImage.rgbValid && suppressedImage.checksum == noEmissiveImage.checksum,
                    "original legacy second-UV emissive texture ADD visibly changes and suppression restores pixels"))
            return 1;
        std::cout << "native_legacy_emissive_gpu base=" << noEmissiveImage.checksum
                  << " add=" << emissiveImage.checksum
                  << " suppressed=" << suppressedImage.checksum << '\n';
        seamMaterial.emissive_texture_id = 0U;
        CQEFFECTS.bEmissiveTextures = previousEmissiveEffect;
        auto *chromeGroup = seamRender->GetNextFaceGroup(nullptr);
        const auto savedChromeFlags = chromeGroup->flags;
        // The seam fixture's own diffuse tile is uniform. Use the existing
        // nonuniform GPU fixture to make normal-driven sampling observable.
        const auto chromeTexture = seamMaterial.texture_id;
        seamMaterial.texture_id = transformCapture.gpu->FixtureTextureHandle();
        chromeGroup->flags |= MM_CHROME;
        seamRender->Render(seamTree->GetMeshInfo(), camera.inverse);
        const auto chromeStart = transformCapture.nativeLegacyImage;
        const float pollutedConstants[8] = {0.25F, 0.75F, 0.0F, 0.0F, 0.1F, 0.2F, 0.3F, 0.4F};
        if (!Expect(transformCapture.gpu->SetVertexConstants(30, pollutedConstants, 2) == GR_OK,
                    "chrome fixture interleaves actual App effect constants")) return 1;
        seamRender->Render(seamTree->GetMeshInfo(), camera.inverse);
        const auto chromeAfterEffect = transformCapture.nativeLegacyImage;
        // With a wrapping 2x2 tile, U=0 and U=0.5 both average both columns.
        // An eighth turn makes the normal-derived sampling distinguishable.
        UpdateRenderEffects(0.25F * PI);
        seamRender->Render(seamTree->GetMeshInfo(), camera.inverse);
        const auto chromeQuarter = transformCapture.nativeLegacyImage;
        UpdateRenderEffects(1.75F * PI);
        seamRender->Render(seamTree->GetMeshInfo(), camera.inverse);
        const auto chromeRestored = transformCapture.nativeLegacyImage;
        chromeGroup->flags = savedChromeFlags;
        seamMaterial.texture_id = chromeTexture;
        std::cout << "native_legacy_chrome_gpu start=" << chromeStart.checksum
                  << " quarter=" << chromeQuarter.checksum << " restored=" << chromeRestored.checksum << '\n';
        const float defaultConstants[8] = {0, 0, 1, 1, 1, 1, 1, 1};
        if (!Expect(transformCapture.gpu->SetVertexConstants(30, defaultConstants, 2) == GR_OK &&
                    chromeStart.rgbValid && chromeStart.changedPixels > 0 &&
                    chromeAfterEffect.rgbValid && chromeAfterEffect.checksum == chromeStart.checksum &&
                    chromeQuarter.rgbValid && chromeQuarter.checksum != chromeStart.checksum &&
                    chromeRestored.rgbValid && chromeRestored.checksum == chromeStart.checksum,
                    "chrome ignores App constants, rotates textured pixels and restores a complete cycle")) return 1;
    }
#endif
    // SolarianBuild mutates a private clone. Its clipped corners must carry
    // the original face-local UV2 seam and interpolated normals into rendering.
    {
        const auto original = nativeLegacyVertices;
        IMeshRender *buildRender = CreateMeshRender();
        buildRender->AddRef();
        CopyMesh(seamRender, buildRender, true);
        MeshInfo info;
        CopyMeshInfo(info, *seamTree->GetMeshInfo(), true);
        info.mr = buildRender;
        IRenderMaterial *out = buildRender->GetNextFaceGroup(nullptr);
        const IRenderMaterial *in = seamRender->GetNextFaceGroup(nullptr);
        auto *vertices = static_cast<RPVertex *>(out->src_verts_buffer);
        const U16 first = out->index_list[0], second = out->index_list[1];
        const Vector position = vertices[first].pos;
        vertices[first].pos = 0.5F * (position + vertices[second].pos);
        ResetNativeLegacyBuildCorners(out, in);
        UpdateNativeLegacyBuildFace(out, in, 0, 0);
        buildRender->Render(&info, camera.inverse);
        if (!Expect(std::fabs(nativeLegacyVertices[0].u2 - 0.5F*(original[0].u2+original[1].u2)) < 1e-5F &&
                    std::fabs(nativeLegacyVertices[0].v2 - 0.5F*(original[0].v2+original[1].v2)) < 1e-5F,
                    "Solarian construction interpolates secondary UVs at the actual clipped corner")) return 1;
        vertices[first].pos = position;
        ResetNativeLegacyBuildCorners(out, in);
        buildRender->Render(&info, camera.inverse);
        if (!Expect(nativeLegacyVertices[0].u2 == original[0].u2 &&
                    nativeLegacyVertices[0].v2 == original[0].v2,
                    "Solarian construction resets private corner metadata between reveal frames")) return 1;
    }
    // The living Engine mesh and destruction extent must share team markings.
    MeshChain coloredChain;
    coloredChain.numChildren = 1;
    coloredChain.mi[0] = seamTree->GetMeshInfo();
    IRenderMaterial *coloredGroup = seamRender->GetNextFaceGroup(nullptr);
    const U32 originalFlags = coloredGroup->flags;
    coloredGroup->flags |= MM_MARKINGS;
    SetNativeLegacyTeamColor(engine, renderer, seamInstance, RGB(29,179,47), &coloredChain);
    coloredGroup->flags = originalFlags;
    if (!Expect(seamTree->GetMeshInfo()->fgi[0].diffuse.r == 29 &&
                seamTree->GetMeshInfo()->fgi[0].diffuse.g == 179 &&
                seamTree->GetMeshInfo()->fgi[0].emissive.g == static_cast<U8>(0.3F * 179),
                "original destruction extent receives the living instance's team color")) return 1;
    // MeshExplode uses this exact owner for debris. Check the authored UV seam
    // at a cut edge and after another cut, without changing source split ratios.
    {
        MeshInfo firstHalf, secondHalf;
        if (!Expect(CanSplitMesh(*seamTree->GetMeshInfo()),
                    "original seam mesh is eligible for production explosion split")) return 1;
        SplitMeshInfo(*seamTree->GetMeshInfo(), firstHalf, secondHalf,
                      0.5F, Vector(1.0F, 0.0F, 0.0F));
        firstHalf.instanceIndex = secondHalf.instanceIndex = seamInstance;
        if (!Expect(firstHalf.fgi[0].diffuse.g == 179 && secondHalf.fgi[0].diffuse.r == 29 &&
                    firstHalf.fgi[0].emissive.g == seamTree->GetMeshInfo()->fgi[0].emissive.g,
                    "both original debris halves preserve player markings and emission")) return 1;
        std::vector<FrontierX::RenderVertexP3DiffuseT4> splitVertices;
        auto renderSplit = [&](MeshInfo &part) {
            const unsigned callsBefore = nativeLegacyDrawCalls;
            part.mr->Render(&part, camera.inverse);
            if (nativeLegacyDrawCalls != callsBefore)
                splitVertices.insert(splitVertices.end(), nativeLegacyVertices.begin(),
                                     nativeLegacyVertices.end());
        };
        auto cutSeamPreserved = [&]() {
            bool primaryCorner = false, secondaryCorner = false;
            for (const auto &vertex : splitVertices)
            {
                if (vertex.y == 0.0F && vertex.z == 0.0F)
                {
                    primaryCorner |= vertex.x == 0.5F &&
                        vertex.u2 == vertex.u && vertex.v2 == vertex.v;
                    // Source splitEdge returns (v2.x-d)/(v2.x-v1.x),
                    // hence 1.5 here, not the geometric plane coordinate.
                    // Preserve that source ratio for geometry and UV2 alike.
                    secondaryCorner |= vertex.x == 1.5F &&
                        vertex.u2 == 1.0F && vertex.v2 == 1.0F;
                }
            }
            return primaryCorner && secondaryCorner;
        };
        renderSplit(firstHalf);
        renderSplit(secondHalf);
        if (!Expect(firstHalf.mr->face_cnt + secondHalf.mr->face_cnt == 6 &&
                    cutSeamPreserved(),
                    "original explosion split preserves both authored secondary-UV edge values"))
        {
            std::cerr << "split faces=" << firstHalf.mr->face_cnt << ',' << secondHalf.mr->face_cnt << '\n';
            for (const auto &vertex : splitVertices)
                std::cerr << "split corner " << vertex.x << ',' << vertex.y
                          << " uv=" << vertex.u << ',' << vertex.v
                          << " uv2=" << vertex.u2 << ',' << vertex.v2 << '\n';
            return 1;
        }
        splitVertices.clear();
        for (MeshInfo *half : {&firstHalf, &secondHalf})
        {
            MeshInfo repeated0, repeated1;
            SplitMeshInfo(*half, repeated0, repeated1, 0.5F, Vector(0.0F, 1.0F, 0.0F));
            repeated0.instanceIndex = repeated1.instanceIndex = seamInstance;
            renderSplit(repeated0);
            renderSplit(repeated1);
        }
        if (!Expect(cutSeamPreserved(),
                    "repeated original explosion split retains independently interpolated corner UV2"))
            return 1;
    }
    DestroyMeshInfoTree(seamTree);
    seamRender->Release();
    if (!Expect(seamBuffers.adds == 8U && seamBuffers.deletes == 8U &&
                seamBuffers.owner == nullptr,
                "independent legacy seam mesh releases its cooked vertex owner"))
    {
        std::cerr << "seam buffers adds=" << seamBuffers.adds << " deletes="
                  << seamBuffers.deletes << " owner=" << seamBuffers.owner << '\n';
        return 1;
    }
    vb_mgr = &appBuffers;
    std::cout << "Original legacy source material colors, states, UV2 seam and repeated explosion splits checked\n";
    engine->destroy_instance(seamInstance);
    engine->release_archetype(seamArchetype);
    seamArchive->Release();
    // UnbornMeshList's placement preview needs source fgi colors even when
    // training leaves the global full-mission legacy mode disabled.
    NATIVE_USE_ORIGINAL_LEGACY_MATERIALS = false;
    PreviewCameraPosition previewCamera(camera);
    IBaseCamera *previousCamera = CAMERA;
    CAMERA = &previewCamera;
    MeshChain previewChain;
    previewChain.numChildren = 1;
    previewChain.mi[0] = appTree->GetMeshInfo();
    const auto previewDiffuse = legacyMaterial.diffuse;
    legacyMaterial.emissive = {0U, 0U, 0U};
    legacyMaterial.emissive_texture_id = 0U;
    legacyMaterial.a = 128U;
    lights->set_ambient_light(255, 255, 255);
    auto renderOrdinaryAfterPreview = [&] {
        IRenderPipeline *previewPipeline = PIPE;
        IRenderPrimitive *previewBatch = BATCH;
        PIPE = appPipeline;
        BATCH = primitive;
        appRender->Render(appTree->GetMeshInfo(), camera.inverse);
        PIPE = previewPipeline;
        BATCH = previewBatch;
    };
    for (bool validPlacement : {true, false})
    {
        legacyMaterial.diffuse = validPlacement ? LightRGB_U8{0U, 255U, 0U}
                                               : LightRGB_U8{255U, 0U, 0U};
        const unsigned beforePreview = nativeLegacyDrawCalls;
        TreeRenderNativeLegacyPreview(previewChain);
        const U32 expectedPreview = FrontierX::PackDiffuseBgra(
            validPlacement ? 0U : 254U, validPlacement ? 254U : 0U, 0U, 128U);
        bool exactPreview = nativeLegacyDrawCalls == beforePreview + 1U &&
            !nativeLegacyVertices.empty() && !NATIVE_USE_ORIGINAL_LEGACY_MATERIALS;
        for (const auto &vertex : nativeLegacyVertices)
            exactPreview &= vertex.diffuseBgra == expectedPreview;
        U32 previewBlend = 0U, previewSource = 0U, previewDestination = 0U;
        if (!Expect(exactPreview &&
                    PIPE->get_render_state(D3DRS_ALPHABLENDENABLE, &previewBlend) == GR_OK && previewBlend == TRUE &&
                    PIPE->get_render_state(D3DRS_SRCBLEND, &previewSource) == GR_OK && previewSource == D3DBLEND_SRCALPHA &&
                    PIPE->get_render_state(D3DRS_DESTBLEND, &previewDestination) == GR_OK && previewDestination == D3DBLEND_INVSRCALPHA,
                    "scoped placement preview uses exact green/red source fgi and alpha128 without enabling global legacy mode"))
            return 1;
        const unsigned afterPreview = nativeLegacyDrawCalls;
        renderOrdinaryAfterPreview();
        if (!Expect(nativeLegacyDrawCalls == afterPreview,
                    "successful placement preview restores ordinary material dispatch")) return 1;
    }
    rejectNativeLegacyDraw = true;
    bool previewRejected = false;
    try { TreeRenderNativeLegacyPreview(previewChain); }
    catch (const std::runtime_error &) { previewRejected = true; }
    rejectNativeLegacyDraw = false;
    const unsigned afterRejectedPreview = nativeLegacyDrawCalls;
    renderOrdinaryAfterPreview();
    if (!Expect(previewRejected && !NATIVE_USE_ORIGINAL_LEGACY_MATERIALS &&
                nativeLegacyDrawCalls == afterRejectedPreview,
                "rejected placement preview restores scope before ordinary dispatch")) return 1;
    CAMERA = previousCamera;
    if (!Expect(previewCamera.positionQueries == 3U, "scoped preview queries typed original camera position")) return 1;
    legacyMaterial.diffuse = previewDiffuse;
    std::cout << "Scoped placement preview source colors and exception restoration checked\n";
#if defined(FRONTIERX_GAME_GPU)
    transformCapture.submitNativeLegacyGpu = false;
#endif
    legacyMaterial.emissive = originalEmission;
    legacyMaterial.a = originalLegacyAlpha;
    legacyMaterial.emissive_texture_id = originalEmissiveTexture;
    lights->set_ambient_light(255, 255, 255);
    NATIVE_USE_ORIGINAL_LEGACY_MATERIALS = false;
    cpuMaterialBackendLive = false;
    LIGHT = nullptr;
    BATCH = nullptr;
    MAINCAM = nullptr;
    if (legacyPipeline)
    {
        PIPE = appPipeline;
        legacyPipeline->Release();
        legacyPrimitive->Release();
    }
    DAFILEDESC effectDirectoryDescriptor("shaders");
    IFileSystem * effectDirectory = nullptr;
    if (!Expect(archive->CreateInstance(&effectDirectoryDescriptor,
                    reinterpret_cast<void **>(&effectDirectory)) == GR_OK &&
                    effectDirectory,
                "production shader directory view for native effect")) return 1;
    ID3DXEffect ** nativeEffect = appPipeline->load_effect(
        "VertexColor.fx", effectDirectory);
    UINT nativePassCount = 0U;
    if (!Expect(nativeEffect && *nativeEffect &&
                    appPipeline->load_effect("VertexColor.fx", effectDirectory) ==
                        nativeEffect &&
                    appPipeline->load_effect("Synthetic.fx", effectDirectory) ==
                        nullptr &&
                    (*nativeEffect)->Begin(&nativePassCount, 0U) &&
                    nativePassCount == 1U,
                "native effect accepts the exact repository VertexColor source"))
        return 1;
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode)
    {
        FrontierX::SdlGpuReadback beforeRejectedPass;
        FrontierX::SdlGpuReadback afterRejectedPass;
        std::string rollbackError;
        U32 addressU = 0U;
        U32 addressV = 0U;
        U32 colorOperation = 0U;
        U32 colorArgument = 0U;
        if (!Expect(primitive->set_sampler_state(
                        0U, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP) == GR_OK &&
                    primitive->set_sampler_state(
                        0U, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP) == GR_OK &&
                    primitive->set_texture_stage_state(
                        0U, D3DTSS_COLOROP, D3DTOP_SELECTARG1) == GR_OK &&
                    primitive->set_texture_stage_state(
                        0U, D3DTSS_COLORARG1, D3DTA_DIFFUSE) == GR_OK &&
                    gpu.renderer.Readback64(beforeRejectedPass, rollbackError) &&
                    beforeRejectedPass.rgbValid,
                    "native effect rollback baseline")) return 1;
        transformCapture.rejectStageAfter = 4U;
        if (!Expect(!(*nativeEffect)->BeginPass(0U) &&
                    transformCapture.rejectStageAfter == 0U &&
                    primitive->get_sampler_state(
                        0U, D3DSAMP_ADDRESSU, &addressU) == GR_OK &&
                    addressU == D3DTADDRESS_CLAMP &&
                    primitive->get_sampler_state(
                        0U, D3DSAMP_ADDRESSV, &addressV) == GR_OK &&
                    addressV == D3DTADDRESS_CLAMP &&
                    primitive->get_texture_stage_state(
                        0U, D3DTSS_COLOROP, &colorOperation) == GR_OK &&
                    colorOperation == D3DTOP_SELECTARG1 &&
                    primitive->get_texture_stage_state(
                        0U, D3DTSS_COLORARG1, &colorArgument) == GR_OK &&
                    colorArgument == D3DTA_DIFFUSE &&
                    gpu.renderer.Readback64(afterRejectedPass, rollbackError) &&
                    afterRejectedPass.rgbValid &&
                    afterRejectedPass.checksum == beforeRejectedPass.checksum,
                    "failed VertexColor pass restores sampler and bounded image"))
        {
            std::cerr << rollbackError << '\n';
            return 1;
        }
    }
#endif
    const bool nativePassApplied = (*nativeEffect)->BeginPass(0U);
    U32 nativeColorArgument = 0U;
    if (!Expect(nativePassApplied == gpuMode &&
                    (!gpuMode ||
                     (primitive->get_texture_stage_state(
                          0U, D3DTSS_COLORARG1, &nativeColorArgument) == GR_OK &&
                      nativeColorArgument == D3DTA_TEXTURE)) &&
                    (!nativePassApplied || (*nativeEffect)->EndPass()) &&
                    (*nativeEffect)->End(),
                "native VertexColor pass applies only with a live backend"))
        return 1;
    effectDirectory->Release();
    ProbeOuter effectOuter;
    effectOuter.pipe = appPipeline;
    effectOuter.primitive = primitive;
    AGGDESC effectManagerDescriptor("IMaterialManager");
    effectManagerDescriptor.outer = &effectOuter;
    IDAComponent * effectInner = nullptr;
    effectManagerDescriptor.inner = &effectInner;
    void * rawEffectManager = nullptr;
    if (!Expect(components->CreateInstance(&effectManagerDescriptor,
                    &rawEffectManager) == GR_OK && rawEffectManager && effectInner,
                "production manager creation for VertexColor material")) return 1;
    IMaterialManager * effectMaterials = nullptr;
    if (!Expect(effectInner->QueryInterface(IID_IMaterialManager,
                    reinterpret_cast<void **>(&effectMaterials)) == GR_OK &&
                    effectMaterials,
                "production VertexColor material manager interface")) return 1;
    IMaterialManager::InitInfo effectInit{};
    effectInit.MATDIR = archive;
    effectInit.PIPE = appPipeline;
    effectInit.TMANAGER = gameTextures;
    if (!Expect(effectMaterials->Initialize(effectInit) == GR_OK,
                "production VertexColor material manager initialization")) return 1;
    IMaterial * vertexColorMaterial = effectMaterials->FindMaterial(
        "VertexColor.txt");
    const char * vertexColorTexture = vertexColorMaterial ?
        vertexColorMaterial->GetTextureName("Diffuse + Opacity") : nullptr;
    if (!Expect(vertexColorMaterial &&
                    vertexColorTexture &&
                    std::strcmp(vertexColorTexture, "Tile.tga") == 0,
                "production VertexColor script and saved texture lookup"))
    {
        std::cerr << "material=" << vertexColorMaterial << " texture="
                  << (vertexColorTexture ? vertexColorTexture : "(null)") << '\n';
        return 1;
    }
    vertexColorMaterial->Realize();
    if (!Expect(vertexColorMaterial->IsRealized(),
                "production VertexColor material realization")) return 1;
    IRP_VERTEXBUFFERHANDLE effectBuffer = 0U;
    void * effectBytes = nullptr;
    U32 effectCapacity = 0U;
    std::array<StandardVertex, 3U> effectVertices{};
    effectVertices[0].pos.set(-1.0F, -1.0F, 0.5F);
    effectVertices[1].pos.set(1.0F, -1.0F, 0.5F);
    effectVertices[2].pos.set(0.0F, 1.0F, 0.5F);
    effectVertices[0].u = 0.0F;
    effectVertices[0].v = 0.0F;
    effectVertices[1].u = 1.0F;
    effectVertices[1].v = 0.0F;
    effectVertices[2].u = 0.5F;
    effectVertices[2].v = 1.0F;
    for (StandardVertex & vertex : effectVertices)
    {
        vertex.norm.set(0.0F, 0.0F, 1.0F);
        vertex.r = vertex.g = vertex.b = vertex.a = 255U;
    }
    if (!Expect(appPipeline->create_vertex_buffer(D3DFVF_STANDARD, 3, 0U,
                    &effectBuffer) == GR_OK &&
                    appPipeline->lock_vertex_buffer(effectBuffer, 0U,
                        &effectBytes, &effectCapacity) == GR_OK &&
                    effectCapacity == effectVertices.size(),
                "production VertexColor material buffer ownership")) return 1;
    std::memcpy(effectBytes, effectVertices.data(), sizeof(effectVertices));
    if (!Expect(appPipeline->unlock_vertex_buffer(effectBuffer) == GR_OK,
                "production VertexColor material buffer upload")) return 1;
    IDirect3DVertexBuffer9 effectVertexBuffer{effectBuffer};
    const unsigned vertexColorDrawsBefore = gameStandardDrawCalls;
    appVertexConstants.registerCount = 0U;
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode)
    {
        transformCapture.appPresentedAtSubmit = false;
        transformCapture.appImageAtSubmit = {};
        transformCapture.submitAppGpu = true;
    }
#endif
    vertexColorMaterial->DrawVB(&effectVertexBuffer, nullptr, 0, 3, 0, 0,
                                nullptr, 0.0F);
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode) transformCapture.submitAppGpu = false;
#endif
    if (!Expect(gameStandardDrawCalls == vertexColorDrawsBefore +
                    (gpuMode ? 1U : 0U) &&
                    appVertexConstants.firstRegister == 30U &&
                    appVertexConstants.registerCount == 2U &&
                    appPipeline->destroy_vertex_buffer(effectBuffer) == GR_OK,
                "production VertexColor constants and gated DrawVB submission"))
        return 1;
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode)
    {
        const auto effectImage = transformCapture.appImageAtSubmit;
        const U32 effectTexture = gpu.renderer.CurrentTextureHandle();
        FrontierX::SdlGpuReadback effectWithoutTexture;
        std::string effectGpuError;
        if (!Expect(transformCapture.appPresentedAtSubmit &&
                    effectImage.rgbValid && effectImage.changedPixels > 0U &&
                    effectImage.checksum != gameGpuChecksum &&
                    effectTexture != 0U &&
                    effectTexture == gameTextures->GetFirstTexture() &&
                    gpu.renderer.BindTexture(0U, 0U) == GR_OK &&
                    gpu.renderer.Readback64(effectWithoutTexture,
                                            effectGpuError) &&
                    effectWithoutTexture.rgbValid &&
                    effectWithoutTexture.checksum != effectImage.checksum &&
                    gpu.renderer.BindTexture(0U, effectTexture) == GR_OK,
                    "production VertexColor texture changes visible Vulkan output"))
        {
            std::cerr << effectGpuError << " material=" << effectImage.checksum
                      << " plain=" << effectWithoutTexture.checksum
                      << " texture=" << effectTexture << '\n';
            return 1;
        }
        std::cout << "cq2_material_gpu checksum=" << effectImage.checksum
                  << " untextured=" << effectWithoutTexture.checksum
                  << " changed=" << effectImage.changedPixels
                  << " texture=" << effectTexture << '\n';
        if (captureRequested && !Expect(WriteP6(
                std::string(argv[8]) + "-cq2.ppm", effectImage),
                "bounded production CQ2 Material P6 capture")) return 1;
    }
#endif
    DestroyMeshInfoTree(appTree);
    appRender->Release();
    PIPE = pipeline;
    // The App caller must select and draw through the production IMaterial.
    const ARCHETYPE_INDEX appMaterialArchetype =
        engine->create_archetype("app_material_fixture.3db", meshArchive);
    if (!Expect(appMaterialArchetype != INVALID_ARCHETYPE_INDEX,
                "production App material archetype creation")) return 1;
    const INSTANCE_INDEX appMaterialInstance =
        engine->create_instance2(appMaterialArchetype, nullptr);
    if (!Expect(appMaterialInstance != INVALID_INSTANCE_INDEX,
                "production App material instance creation")) return 1;
    IMeshRender * appMaterialRender = CreateMeshRender();
    appMaterialRender->AddRef();
    appMaterialRender->Init(appMaterialArchetype);
    IMeshInfoTree * appMaterialTree = CreateMeshInfoTree(appMaterialInstance);
    appMaterialRender->SetupMeshInfo(appMaterialTree->GetMeshInfo());
    if (!Expect(appMaterialTree->GetMeshInfo()->faceGroupCnt == 1U &&
                appMaterialTree->GetMeshInfo()->fgi[0].iMat == material,
                "production App face group resolves its CQ2 IMaterial")) return 1;
    const unsigned materialDrawsBefore = gameStandardDrawCalls;
    BATCH = primitive;
    MAINCAM = &camera;
    appMaterialRender->Render(appMaterialTree->GetMeshInfo(), camera.inverse);
    MAINCAM = nullptr;
    BATCH = nullptr;
    const bool appMaterialSubmitted =
        gameStandardDrawCalls == materialDrawsBefore;
    DestroyMeshInfoTree(appMaterialTree);
    appMaterialRender->Release();
    engine->destroy_instance(appMaterialInstance);
    engine->release_archetype(appMaterialArchetype);
    const std::filesystem::path effectMeshPath =
        std::string(argv[1]) + ".vertexcolor.mesh";
    if (!Expect(FrontierXTest::WriteVertexColorGameMeshFixture(effectMeshPath),
                "CQ2 VertexColor game mesh fixture creation")) return 1;
    IFileSystem * effectMeshArchive = nullptr;
    if (!Expect(FrontierX::OpenNativeUtfFileSystem(effectMeshPath,
                    &effectMeshArchive, error) && effectMeshArchive,
                "CQ2 VertexColor game mesh UTF open")) return 1;
    effectOuter.textureLibrary = textures;
    effectOuter.materialManager = effectMaterials;
    IEngine * effectEngine = nullptr;
    if (!Expect(components->CreateInstance(&engineDescriptor,
                    reinterpret_cast<void **>(&effectEngine)) == GR_OK &&
                    effectEngine &&
                    effectEngine->load_engine_components(&effectOuter) == GR_OK,
                "production Engine loads VertexColor material services")) return 1;
    IRenderer * effectRenderer = nullptr;
    if (!Expect(effectEngine->QueryInterface(IID_IRenderer,
                    reinterpret_cast<void **>(&effectRenderer)) == GR_OK &&
                    effectRenderer,
                "production VertexColor Engine renderer lookup")) return 1;
    ENGINE = effectEngine;
    REND = effectRenderer;
    MATMAN = effectMaterials;
    PIPE = appPipeline;
    const ARCHETYPE_INDEX effectArchetype = effectEngine->create_archetype(
        "app_vertexcolor_fixture.3db", effectMeshArchive);
    if (!Expect(effectArchetype != INVALID_ARCHETYPE_INDEX,
                "production App VertexColor archetype creation")) return 1;
    const INSTANCE_INDEX effectInstance = effectEngine->create_instance2(
        effectArchetype, nullptr);
    if (!Expect(effectInstance != INVALID_INSTANCE_INDEX,
                "production App VertexColor instance creation")) return 1;
    IMeshRender * effectRender = CreateMeshRender();
    effectRender->AddRef();
    effectRender->Init(effectArchetype);
    IMeshInfoTree * effectTree = CreateMeshInfoTree(effectInstance);
    effectRender->SetupMeshInfo(effectTree->GetMeshInfo());
    if (!Expect(effectTree->GetMeshInfo()->faceGroupCnt == 1U &&
                    effectTree->GetMeshInfo()->fgi[0].iMat ==
                        vertexColorMaterial,
                "production App resolves its VertexColor IMaterial"))
    {
        std::cerr << "groups=" <<
            static_cast<unsigned>(effectTree->GetMeshInfo()->faceGroupCnt)
                  << " expected=" << vertexColorMaterial
                  << " old=" << material
                  << " other=" << materials->FindMaterial("VertexColor.txt")
                  << " actual=" << effectTree->GetMeshInfo()->fgi[0].iMat << '\n';
        return 1;
    }
    const unsigned effectDrawsBefore = gameStandardDrawCalls;
    BATCH = primitive;
    MAINCAM = &camera;
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode)
    {
        transformCapture.appPresentedAtSubmit = false;
        transformCapture.appImageAtSubmit = {};
        transformCapture.submitAppGpu = true;
    }
#endif
    effectRender->Render(effectTree->GetMeshInfo(), camera.inverse);
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode) transformCapture.submitAppGpu = false;
#endif
    MAINCAM = nullptr;
    BATCH = nullptr;
    const bool appEffectSubmitted =
        gameStandardDrawCalls == effectDrawsBefore + (gpuMode ? 1U : 0U);
#if defined(FRONTIERX_GAME_GPU)
    if (gpuMode)
    {
        const auto & effectAppImage = transformCapture.appImageAtSubmit;
        if (!Expect(transformCapture.appPresentedAtSubmit &&
                    effectAppImage.rgbValid &&
                    effectAppImage.changedPixels > 0U &&
                    effectAppImage.checksum != gameGpuChecksum,
                    "production App CQ2 VertexColor draw reaches visible Vulkan"))
            return 1;
        std::cout << "cq2_app_gpu checksum=" << effectAppImage.checksum
                  << " changed=" << effectAppImage.changedPixels << '\n';
        if (captureRequested && !Expect(WriteP6(
                std::string(argv[8]) + "-cq2-app.ppm", effectAppImage),
                "bounded production CQ2 App P6 capture")) return 1;
    }
#endif
    DestroyMeshInfoTree(effectTree);
    effectRender->Release();
    effectEngine->destroy_instance(effectInstance);
    effectEngine->release_archetype(effectArchetype);
    effectMeshArchive->Release();
    ENGINE = engine;
    REND = renderer;
    MATMAN = materials;
    PIPE = pipeline;
    effectRenderer->Release();
    effectEngine->Release();
    effectMaterials->Release();
    effectInner->Release();
    appPipeline->Release();
    AppStateCapture appState;
    FrontierX::NativeRenderPrimitiveDesc appStateDescriptor(
        CaptureApp2DDraw, &appState, nullptr, CaptureAppRenderState,
        nullptr, nullptr, CaptureAppTextureStage, CaptureAppTextureBind,
        CaptureApp2DModelview, CaptureApp2DProjection, nullptr, nullptr,
        CaptureAppSamplerState, CaptureApp2DViewport);
    IRenderPrimitive *appPrimitive = nullptr;
    if (!Expect(components->CreateInstance(&appStateDescriptor,
                    reinterpret_cast<void **>(&appPrimitive)) == GR_OK && appPrimitive,
                "production App diffuse-state primitive creation")) return 1;
    BATCH = appPrimitive;
    // Deformed/multitexture draws may leave UV1 active. Fighters submit
    // RPVertex with UV0 only; their common setup must restore that channel.
    if (!Expect(BATCH->set_texture_stage_state(0U, D3DTSS_TEXCOORDINDEX, 1U) == GR_OK,
                "prior draw leaves secondary UV channel")) return 1;
    const unsigned diffuseStageCalls = appState.stageCalls;
    SetupDiffuseBlend(64U, true);
    const bool clampedDiffuseState = appState.texture == 64U &&
        appState.stageCalls - diffuseStageCalls == 11U &&
        appState.stage.textureCoordinateIndex == 0U && appState.samplerCalls == 5U &&
        appState.stage.second.colorOperation == D3DTOP_DISABLE &&
        appState.stage.second.alphaOperation == D3DTOP_DISABLE &&
        appState.stage.colorOperation == D3DTOP_MODULATE &&
        appState.stage.colorArgument1 == D3DTA_TEXTURE &&
        appState.stage.colorArgument2 == D3DTA_DIFFUSE &&
        appState.stage.alphaOperation == D3DTOP_MODULATE &&
        appState.stage.alphaArgument1 == D3DTA_TEXTURE &&
        appState.stage.alphaArgument2 == D3DTA_DIFFUSE &&
        appState.sampler.minFilter == D3DTEXF_LINEAR &&
        appState.sampler.magFilter == D3DTEXF_LINEAR &&
        appState.sampler.mipFilter == D3DTEXF_POINT &&
        appState.sampler.addressU == D3DTADDRESS_CLAMP &&
        appState.sampler.addressV == D3DTADDRESS_CLAMP;
    SetupDiffuseBlend(64U, false);
    const bool wrappedDiffuseState = appState.sampler.addressU == D3DTADDRESS_WRAP &&
        appState.sampler.addressV == D3DTADDRESS_WRAP && appState.samplerCalls == 10U;
    DisableTextures();
    const bool untexturedAppState = appState.texture == 0U &&
        appState.stage.colorOperation == D3DTOP_SELECTARG1 &&
        appState.stage.colorArgument1 == D3DTA_DIFFUSE &&
        appState.stage.alphaOperation == D3DTOP_SELECTARG1 &&
        appState.stage.alphaArgument1 == D3DTA_DIFFUSE;
    // The fixed-turret hardpoint is a -Z-forward frame, unlike ship navigation.
    for (const Vector direction : {Vector(1, 1, 0), Vector(-1, 1, 0),
                                   Vector(1, -1, 0), Vector(-1, -1, 0)})
    {
        const float heading = get_angle(direction.x, direction.y);
        TRANSFORM sight;
        sight.rotate_about_i(90 * MUL_DEG_TO_RAD);
        sight.rotate_about_j(heading);
        if (std::fabs(sight.get_yaw_zbased() - heading) > 1.0e-5F ||
            std::fabs(sight.get_pitch_zbased()) > 1.0e-5F) return 1;
        const Vector look = -sight.get_k();
        if (std::fabs(look.x - std::sin(heading)) > 1.0e-5F ||
            std::fabs(look.y - std::cos(heading)) > 1.0e-5F) return 1;
    }
    // FighterWing launches authored -K-forward frames. ObjectControl's exact
    // Fighter specialization must measure that flight ray, while ordinary ship
    // instantiations keep their existing J-navigation contract.
    struct OrdinaryShipSave;
    using FighterFrame = NativeObjectControlFrame<FIGHTER_SAVELOAD>;
    using ShipFrame = NativeObjectControlFrame<OrdinaryShipSave>;
    const auto integrateAngularVelocity = [](TRANSFORM frame, const Vector &angularVelocity)
    {
        // Same world-axis quaternion derivative as ObjectPhysics::physUpdate.
        Quaternion orientation;
        orientation.set(frame);
        const Quaternion omega(angularVelocity);
        const Quaternion derivative = omega * orientation * 0.5F;
        orientation.w += derivative.w * 0.001F;
        orientation.x += derivative.x * 0.001F;
        orientation.y += derivative.y * 0.001F;
        orientation.z += derivative.z * 0.001F;
        orientation.normalize();
        static_cast<Matrix &>(frame) = orientation;
        return frame;
    };
    for (const SINGLE heading : {-2.4F, -0.7F, 0.7F, 2.4F})
    for (const SINGLE elevation : {-0.25F, 0.0F, 0.25F})
    for (const SINGLE bank : {-0.35F, 0.0F, 0.35F})
    {
        TRANSFORM fighter;
        fighter.rotate_about_i(90 * MUL_DEG_TO_RAD);
        fighter.rotate_about_j(heading);
        fighter.rotate_about_i(elevation);
        fighter.rotate_about_k(bank);
        const Vector forward = -fighter.get_k();
        const Vector expected(std::sin(heading) * std::cos(elevation),
                              std::cos(heading) * std::cos(elevation), std::sin(elevation));
        const TRANSFORM pitched = integrateAngularVelocity(fighter, fighter.get_i());
        const TRANSFORM rolled = integrateAngularVelocity(fighter, -FighterFrame::RollAxis(fighter));
        const Vector rollAxisError = FighterFrame::RollAxis(fighter) - fighter.get_k();
        const Vector defaultAxisError = ShipFrame::RollAxis(fighter) - fighter.get_j();
        if (!Expect((forward - expected).magnitude() < 1.0e-5F &&
                    std::fabs(FighterFrame::Yaw(fighter) - heading) < 1.0e-5F &&
                    std::fabs(FighterFrame::Pitch(fighter) - elevation) < 1.0e-5F &&
                    std::fabs(FighterFrame::Roll(fighter) -
                              std::asin(std::sin(bank) * std::cos(elevation))) < 1.0e-5F &&
                    FighterFrame::Pitch(pitched) > FighterFrame::Pitch(fighter) &&
                    FighterFrame::Roll(rolled) > FighterFrame::Roll(fighter) &&
                    rollAxisError.magnitude() < 1.0e-6F &&
                    ShipFrame::Yaw(fighter) == fighter.get_yaw() &&
                    ShipFrame::Pitch(fighter) == fighter.get_pitch() &&
                    ShipFrame::Roll(fighter) == fighter.get_roll() &&
                    defaultAxisError.magnitude() < 1.0e-6F,
                    "Fighter control measures actual -K flight and preserves ordinary ship navigation")) return 1;
        TRANSFORM inverted = fighter;
        inverted.rotate_about_k(PI - bank);
        if (!Expect(std::fabs(FighterFrame::Roll(inverted)) > 3.0F &&
                    std::fabs(FighterFrame::Pitch(inverted) - elevation) < 1.0e-5F,
                    "inverted fighter frame retains authored roll rather than normalizing its axes")) return 1;
    }
    std::cout << "Fighter control frame and original quaternion angular axes checked\n";
    // Projectile/PlasmaBolt retain their original pitch-correction formula.
    // Their turret sight is -K-forward, so the J-forward navigation getter
    // must not redirect a real world target into a nearly vertical shot.
    for (const Vector goal : {Vector(1, 1, 0), Vector(-1, 1, 1),
                             Vector(1, -1, -1), Vector(-1, -1, 0.5F),
                             Vector(-2557, -4436, -389)})
    {
        for (const float initialPitch : {0.0F, 0.23F})
        {
            TRANSFORM sight;
            sight.rotate_about_i(90 * MUL_DEG_TO_RAD);
            sight.rotate_about_j(get_angle(goal.x, goal.y));
            sight.rotate_about_i(initialPitch);
            TRANSFORM oldSight = sight;
            const float desiredPitch = get_angle(goal.z,
                std::sqrt(goal.x * goal.x + goal.y * goal.y));
            oldSight.rotate_about_i(desiredPitch - oldSight.get_pitch());
            sight.rotate_about_i(desiredPitch - FrontierX::NativeWeaponSightPitch(sight));
            const Vector actual = -sight.get_k();
            const Vector old = -oldSight.get_k();
            const float scale = goal.magnitude();
            const float alignment = (actual.x * goal.x + actual.y * goal.y + actual.z * goal.z) / scale;
            const float oldAlignment = (old.x * goal.x + old.y * goal.y + old.z * goal.z) / scale;
            if (!Expect(std::fabs(alignment - 1.0F) < 1.0e-5F && oldAlignment < 0.5F,
                        "original projectile pitch formula follows horizontal/elevated -K sight rather than navigation J"))
                return 1;
        }
    }
    std::cout << "Original projectile sight-axis pitch correction checked\n";
    // Missile starts with Rx90 and later rebuilds that frame around a changed
    // heading in setup_ang_velocity. Its curved-path pitch must keep the -K
    // flight elevation; the J-navigation getter spuriously reports ~90 deg.
    for (const SINGLE heading : {0.0F, 0.73F, -1.21F})
    for (const SINGLE elevation : {0.0F, 0.23F, -0.31F})
    {
        TRANSFORM missile;
        missile.rotate_about_i(90 * MUL_DEG_TO_RAD);
        missile.rotate_about_j(heading);
        missile.rotate_about_i(elevation);
        const SINGLE flightPitch = FrontierX::NativeWeaponSightPitch(missile);
        const Vector goalDirection = -missile.get_k();
        const SINGLE goalPitch = TRANSFORM::get_pitch(goalDirection);
        TRANSFORM steered;
        steered.rotate_about_i(90 * MUL_DEG_TO_RAD);
        steered.rotate_about_j(heading + 0.4F);
        steered.rotate_about_i(flightPitch);
        const Vector ray = -steered.get_k();
        const Vector expected(std::sin(heading+0.4F)*std::cos(elevation),
            std::cos(heading+0.4F)*std::cos(elevation), std::sin(elevation));
        const Vector error = ray-expected;
        if (!Expect(std::fabs(flightPitch-elevation) < 1e-5F &&
                    std::fabs(flightPitch-goalPitch) < 1e-5F &&
                    error.x*error.x + error.y*error.y + error.z*error.z < 1e-9F &&
                    std::fabs(missile.get_pitch()-elevation) > 1.0F,
                    "missile constructor, curved-goal pitch and heading reset preserve actual -K elevation")) return 1;
    }

    // The real TM10 core contains DumbReconProbe, whose translation follows
    // targetPos directly. Its visual nose must follow that same flight ray.
    for (const SINGLE heading : {-2.4F, -0.7F, 0.0F, 0.7F, 2.4F})
    for (const SINGLE elevation : {-0.25F, 0.0F, 0.25F})
    {
        Transform launch;
        launch.rotate_about_i(0.7F); launch.rotate_about_j(-0.4F);
        launch.translation = Vector(17, -31, 23);
        const Vector direction(std::sin(heading)*std::cos(elevation),
                               std::cos(heading)*std::cos(elevation), std::sin(elevation));
        const Transform visual = NativeProbeVisualTransform(launch, launch.translation + 1000*direction);
        const Transform offset = launch.get_inverse() * visual;
        const Transform rendered = launch * offset;
        if (!Expect(dot_product(-rendered.get_k(), direction) > 0.99999F &&
                    rendered.get_j().z > 0.96F &&
                    (rendered.translation-launch.translation).magnitude_squared() < 1e-8F &&
                    std::fabs(dot_product(rendered.get_i(), rendered.get_j())) < 1e-5F,
                    "carrier probe visual is upright, follows actual travel and preserves its saved launch position")) return 1;
        const Transform stopped = NativeProbeVisualTransform(visual, visual.translation);
        if (!Expect(dot_product(-stopped.get_k(), direction) > 0.99999F,
                    "probe arrival retains its heading with no zero-vector normalization")) return 1;
    }
    // ObjectTransform's original save field stores get_yaw(), a -J heading.
    // Weaver remains stationary at its construction heading after load, so
    // it cannot rely on a later movement command to conceal a reversed root.
    for (const SINGLE savedYaw : {-2.7F, -0.8F, 0.6F, 2.3F,
                                 0.0F, -PI/2, PI/2, -PI, PI})
    {
        const Vector heading(std::sin(savedYaw), std::cos(savedYaw), 0);
        TRANSFORM source;
        source.rotate_about_k(savedYaw - PI);
        source.translation = Vector(17, -31, 23);
        SINGLE saved = source.get_yaw();
        TRANSFORM oldLoad;
        oldLoad.rotate_about_k(saved);
        TRANSFORM restored;
        restored.translation = source.translation;
        // A prior animated pose must not accumulate into the saved yaw.
        restored.rotate_about_i(0.4F);
        restored.rotate_about_j(-0.3F);
        for (unsigned reload = 0; reload < 3; ++reload)
        {
            NativeRestoreObjectSavedYaw(restored, saved);
            const Vector front = -restored.get_j();
            const Vector mouth = NativeShipVisualTransform(restored, true).rotate(Vector(0,0,-1));
            if (!Expect(dot_product(front, heading) > 0.99999F &&
                        dot_product(mouth, heading) > 0.99999F &&
                        dot_product(-oldLoad.get_j(), heading) < -0.99999F &&
                        std::fabs(std::remainder(restored.get_yaw() - saved, 2*PI)) < 1e-5F &&
                        std::fabs(restored.get_pitch()) < 1e-5F &&
                        std::fabs(restored.get_roll()) < 1e-5F &&
                        restored.translation.x == 17 && restored.translation.y == -31 &&
                        restored.translation.z == 23,
                        "saved -J yaw restores original ship/Weaver front and position without accumulating animated pitch/roll")) return 1;
            saved = restored.get_yaw();
        }
    }
    std::cout << "Original saved -J heading roundtrip and Weaver model-front quadrants checked\n";
    // Platform::initPlatform installs Rx90 before ObjectTransform::Load.
    // Stripping it turns horizontal model -K weapons into vertical rays.
    for (const SINGLE savedYaw : {-2.7F, -0.8F, 0.0F, 0.6F, 2.3F})
    {
        TRANSFORM platform;
        platform.rotate_about_i(90 * MUL_DEG_TO_RAD);
        platform.translation = Vector(17, -31, 23);
        NativeRestoreObjectSavedYaw(platform, savedYaw, true);
        TRANSFORM sourcePlatform;
        sourcePlatform.rotate_about_i(90 * MUL_DEG_TO_RAD);
        sourcePlatform.rotate_about_k(savedYaw);
        const Vector ray = -platform.get_k();
        if (!Expect((ray + sourcePlatform.get_k()).magnitude() < 1e-5F &&
                    std::fabs(ray.z) < 1e-5F && platform.translation.z == 23,
                    "platform load preserves initialized rest frame and horizontal weapon ray")) return 1;
    }
    std::cout << "Platform saved heading preserves model weapon rest frame checked\n";


    bool app2dSubmitted = false;
    {
        FrontierX::NativeRenderPipeline app2dPipeline(appPrimitive);
        PIPE = &app2dPipeline;
        PB.SetIRenderPrimitive(appPrimitive);
        SCREENRESX = 64U;
        SCREENRESY = 64U;
        DA::RectangleFill(nullptr, 3, 4, 12, 13, RGB(16, 32, 48));
        DA::RectangleHash(nullptr, 5, 6, 14, 15, RGB(64, 80, 96));
        app2dSubmitted = appState.rectangleDraws == 2U &&
            appState.rectangleVertices == 6U &&
            appState.viewport == std::array<int, 4>{{0, 0, 64, 64}} &&
            appState.projection.d[0][0] == 2.0F / 64.0F &&
            appState.projection.d[1][1] == -2.0F / 64.0F &&
            appState.texture == 0U;
        // DrawHighlighted draws two 100-pixel segmented bars per unit. Keep
        // its actual DA caller as the reference for geometry and draw count.
        for (const unsigned units : {1U, 40U})
        {
            const unsigned before = appState.rectangleDraws;
            appState.rectangleGeometry.clear();
            for (unsigned unit = 0; unit < units; ++unit)
            for (unsigned bar = 0; bar < 2U; ++bar)
            for (int i = 0; i < 100; i += 5)
                DA::RectangleFill(nullptr, 3+i, 4+bar*5, 6+i, 6+bar*5,
                                  RGB(16, 32, 48));
            if (!Expect(appState.rectangleDraws-before == units*40U &&
                        appState.rectangleGeometry.size() == units*240U,
                        "selection bars retain all original segmented rectangles")) return 1;
            std::cout << "Selection bar baseline units=" << units
                      << " draws=" << appState.rectangleDraws-before
                      << " vertices=" << appState.rectangleGeometry.size() << '\n';
            const auto reference = appState.rectangleGeometry;
            appState.rectangleGeometry.clear();
            const unsigned optimizedBefore = appState.rectangleDraws;
            for (unsigned unit = 0; unit < units; ++unit)
            for (unsigned bar = 0; bar < 2U; ++bar)
                DA::RectangleFillSegments(nullptr, 3, 4+bar*5, 100,
                                          RGB(16, 32, 48));
            const auto sameVertex = [](const auto &a, const auto &b)
            {
                return a.x == b.x && a.y == b.y && a.z == b.z &&
                    a.diffuseBgra == b.diffuseBgra && a.u == b.u && a.v == b.v;
            };
            if (!Expect(appState.rectangleDraws-optimizedBefore == units*2U &&
                        appState.rectangleGeometry.size() == reference.size() &&
                        std::equal(reference.begin(), reference.end(),
                            appState.rectangleGeometry.begin(), sameVertex),
                        "batched selection bars preserve exact vertices/colors/order with 20 times fewer draws")) return 1;
            std::cout << "Selection bar grouped units=" << units
                      << " draws=" << appState.rectangleDraws-optimizedBefore
                      << " vertices=" << appState.rectangleGeometry.size() << '\n';
        }
        // Partial bars keep the original complete final segment and empty
        // bars issue no geometry, including the one-health-point case.
        for (const S32 length : {0, 1, 5, 6, 99})
        {
            appState.rectangleGeometry.clear();
            for (S32 i = 0; i < length; i += 5)
                DA::RectangleFill(nullptr, -3+i, 4, i, 6, RGB(180, 180, 240));
            const auto reference = appState.rectangleGeometry;
            appState.rectangleGeometry.clear();
            const unsigned before = appState.rectangleDraws;
            DA::RectangleFillSegments(nullptr, -3, 4, length, RGB(180, 180, 240));
            if (!Expect(appState.rectangleDraws-before == (length > 0 ? 1U : 0U) &&
                        appState.rectangleGeometry.size() == reference.size() &&
                        std::equal(reference.begin(), reference.end(),
                            appState.rectangleGeometry.begin(), [](const auto &a, const auto &b) {
                                return a.x == b.x && a.y == b.y && a.z == b.z &&
                                    a.diffuseBgra == b.diffuseBgra && a.u == b.u && a.v == b.v;
                            }), "partial/empty selection bars preserve source segment extent")) return 1;
        }
        // Sector::DrawLinks requests a diagonal smooth line after gate discovery.
        DA::LineDraw(nullptr, 3, 4, 20, 30, RGB(16, 32, 48), true);
        const auto & start = appState.lineVertices[0];
        const auto & end = appState.lineVertices[1];
        if (!Expect(appState.lineDraws == 1U &&
                    start.x == 3.5F && start.y == 4.5F &&
                    end.x == 20.5F && end.y == 30.5F &&
                    start.diffuseBgra == FrontierX::PackDiffuseBgra(16, 32, 48, 255) &&
                    end.diffuseBgra == start.diffuseBgra &&
                    appState.viewport == std::array<int, 4>{{0, 0, 64, 64}},
                    "original Sector map link uses native diagonal line submission")) return 1;
        PB.SetIRenderPrimitive(nullptr);
        SCREENRESX = 0U;
        SCREENRESY = 0U;
        PIPE = nullptr;
    }
    BATCH = nullptr;
    appPrimitive->Release();
    ENGINE = nullptr;
    REND = nullptr;
    PIPE = nullptr;
    TEXLIB = nullptr;
    MATMAN = nullptr;
    vb_mgr = nullptr;
    if (!Expect(appMeshInitialized && appDrawSubmitted && appMaterialSubmitted &&
                appEffectSubmitted &&
                clampedDiffuseState && wrappedDiffuseState && untexturedAppState &&
                app2dSubmitted &&
                appBuffers.adds == 3U &&
                appBuffers.deletes == 3U && appBuffers.owner == nullptr &&
                instanceHasArchetype(engineInstance, engineArchetype),
                "production App mesh ownership and CQ2 effect draw")) return 1;
    // Selected CMP resources live at the owning file's root, outside its part
    // UTFs. Exercise the original MeshManager caller, not an ancestor scanner.
    {
        IFileSystem *objects = nullptr;
        IDAComponent *animationInner = nullptr, *meshManagerInner = nullptr;
        IAggregateComponent *animationAggregate = nullptr, *meshManagerAggregate = nullptr;
        IAnimation *animation = nullptr;
        IMeshManager *meshManager = nullptr;
        ISystemComponent *meshSystem = nullptr;
        IHardpoint *meshHardpoints = nullptr;
        SYSCONSUMERDESC animationDescriptor("IAnimation");
        animationDescriptor.outer = engine;
        animationDescriptor.system = &outer;
        animationDescriptor.inner = &animationInner;
        void *rawAnimation = nullptr;
        AGGDESC meshManagerDescriptor("IMeshManager");
        meshManagerDescriptor.outer = &outer;
        meshManagerDescriptor.inner = &meshManagerInner;
        void *rawMeshManager = nullptr;
        if (!Expect(FrontierX::OpenNativeDirectoryFileSystem(meshPath.parent_path(), &objects, error) &&
                    components->CreateInstance(&animationDescriptor, &rawAnimation) == GR_OK && animationInner &&
                    animationInner->QueryInterface(IID_IAggregateComponent,
                        reinterpret_cast<void **>(&animationAggregate)) == GR_OK &&
                    animationAggregate->Initialize() == GR_OK &&
                    animationInner->QueryInterface(IID_IAnimation, reinterpret_cast<void **>(&animation)) == GR_OK &&
                    components->CreateInstance(&meshManagerDescriptor, &rawMeshManager) == GR_OK && meshManagerInner &&
                    meshManagerInner->QueryInterface(IID_IAggregateComponent,
                        reinterpret_cast<void **>(&meshManagerAggregate)) == GR_OK &&
                    meshManagerAggregate->Initialize() == GR_OK &&
                    meshManagerInner->QueryInterface(IID_IMeshManager, reinterpret_cast<void **>(&meshManager)) == GR_OK &&
                    meshManagerInner->QueryInterface(IID_ISystemComponent, reinterpret_cast<void **>(&meshSystem)) == GR_OK &&
                    engine->QueryInterface(IID_IHardpoint, reinterpret_cast<void **>(&meshHardpoints)) == GR_OK,
                    "original MeshManager and Animation owners initialize with real Engine and source directory")) return 1;
        // Weaver root motion is authored with model +Y up. The native hull
        // maps that to runtime +Z, while child Joint motion remains model-local.
        // Exercise the real IAnimation/FullStream/Engine owner, including the
        // saved yaw-only heading, reverse, a fresh order and instance release.
        {
            IFileSystem *rootArchive = nullptr, *modelArchive = nullptr;
            const auto rootPath = std::filesystem::path(std::string(argv[1]) + ".weaver-root");
            const auto modelPath = std::filesystem::path(std::string(argv[1]) + ".weaver-model");
            if (!Expect(WriteWeaverRootFixture(rootPath) &&
                        FrontierX::OpenNativeDirectoryFileSystem(rootPath, &rootArchive, error) &&
                        FrontierXTest::WriteCompoundGameMeshFixture(modelPath, 0) &&
                        FrontierX::OpenNativeUtfFileSystem(modelPath, &modelArchive, error),
                        "generated Weaver root and child animation fixtures open")) return 1;
            const SCRIPT_SET_ARCH rootArch = animation->create_script_set_arch(rootArchive);
            const auto modelArch = engine->create_archetype("weaver_root_regression.cmp", modelArchive);
            const INSTANCE_INDEX root = engine->create_instance2(modelArch, nullptr);
            const INSTANCE_INDEX child = engine->get_instance_child_next(root, EN_DONT_RECURSE, INVALID_INSTANCE_INDEX);
            JointInfo joint(JT_LOOSE);
            joint.parent_point.zero(); joint.child_point.zero(); joint.rel_position.zero();
            joint.rel_orientation.set_identity();
            if (!Expect(rootArch != INVALID_SCRIPT_SET_ARCH && root != INVALID_INSTANCE_INDEX &&
                        child != INVALID_INSTANCE_INDEX && engine->destroy_joint(root, child) &&
                        engine->create_joint(root, child, &joint),
                        "real Engine retains named root and original animated child")) return 1;
            const Transform basis = NativeShipModelAxisCorrection();
            const Transform motion{Matrix(Quaternion(Vector(1,0,0), 65*MUL_DEG_TO_RAD))};
            const Vector frontAnchor(0,-1,-4); // independent asymmetric model nose
            const auto samePose = [](const Transform &a, const Transform &b) {
                for (unsigned row = 0; row < 3; ++row)
                for (unsigned column = 0; column < 3; ++column)
                    if (std::fabs(a.d[row][column]-b.d[row][column]) > 1e-5F) return false;
                return (a.translation-b.translation).magnitude() < 1e-4F;
            };
            const auto childUnchanged = [&]() {
                float pose[7];
                if (!engine->get_joint_state(child, IE_JST_BASIC, pose)) return false;
                const Quaternion q(pose[3],pose[4],pose[5],pose[6]);
                return samePose(Transform(Matrix(q), Vector(pose[0],pose[1],pose[2])), motion);
            };
            for (const float yaw : {0.0F, 0.7F, -1.2F, 2.4F})
            {
                Transform heading;
                heading.rotate_about_k(yaw);
                heading.translation = Vector(13, 29, 20);
                const Vector towardPlanet = heading.rotate(Vector(0,-1,0));
                engine->set_transform(root, heading);
                SCRIPT_INST script = animation->create_script_inst(rootArch, root, "Sc_build");
                if (!Expect(script != INVALID_SCRIPT_INST &&
                            animation->script_start(script, Animation::FORWARD, Animation::END) == 0,
                            "original unanchored END restore starts")) return 1;
                animation->update_instance(root, 0.01F);
                if (!Expect(samePose(engine->get_transform(root), heading),
                            "old END restore incorrectly subtracts the required root build motion")) return 1;
                animation->script_start(script, Animation::BACKWARDS, Animation::END);
                animation->update_instance(root, 1.1F);
                if (!Expect(samePose(engine->get_transform(root), heading*motion.get_inverse()),
                            "old reverse after yaw-only load reproduces incorrect root pose")) return 1;
                animation->release_script_inst(script);
                for (unsigned reload = 0; reload < 2; ++reload)
                {
                    engine->set_transform(root, heading);
                    script = animation->create_script_inst(rootArch, root, "Sc_build");
                    if (!Expect(script != INVALID_SCRIPT_INST &&
                                FrontierX::ConfigureNativeAnimationRootBasis(animation, script, root,
                                    basis.get_orientation()) &&
                                !FrontierX::ConfigureNativeAnimationRootBasis(animation, script, child,
                                    basis.get_orientation()) &&
                                animation->script_start(script, Animation::FORWARD, Animation::END,
                                    1, 0, 1, 1, &heading) == 0,
                                "explicit loaded heading and model basis restore root motion only")) return 1;
                    animation->update_instance(root, 0.01F);
                    const Transform expected = heading*basis*motion;
                    const Transform actual = NativeShipVisualTransform(engine->get_transform(root), true);
                    Vector expectedFront = expected.rotate(frontAnchor), actualFront = actual.rotate(frontAnchor);
                    expectedFront.normalize(); actualFront.normalize();
                    if (!Expect(samePose(actual, expected) && childUnchanged() &&
                                dot_product(actualFront, towardPlanet) > 0.6F &&
                                dot_product(actualFront, expectedFront) > 0.99999F,
                                "actual Weaver model front faces planet with authored 65-degree root motion and unchanged child pose")) return 1;
                    animation->script_start(script, Animation::BACKWARDS, Animation::END);
                    animation->update_instance(root, 1.1F);
                    if (!Expect(samePose(engine->get_transform(root), heading),
                                "original reverse returns Weaver to level travelling pose")) return 1;
                    animation->script_stop(script);
                    animation->release_script_inst(script);
                    script = animation->create_script_inst(rootArch, root, "Sc_build");
                    if (!Expect(script != INVALID_SCRIPT_INST &&
                                FrontierX::ConfigureNativeAnimationRootBasis(animation, script, root,
                                    basis.get_orientation()),
                                "new Weaver order owns fresh per-instance root basis")) return 1;
                    animation->script_start(script, Animation::FORWARD, Animation::BEGIN);
                    animation->update_instance(root, 1.1F);
                    if (!Expect(samePose(NativeShipVisualTransform(engine->get_transform(root), true), expected) &&
                                childUnchanged(), "fresh build retains original heading, position and child animation")) return 1;
                    animation->release_script_inst(script);
                }
                // A sibling script from the same archive must retain authored
                // behavior after configured instances have been released.
                engine->set_transform(root, heading);
                script = animation->create_script_inst(rootArch, root, "Sc_build");
                animation->script_start(script, Animation::FORWARD, Animation::BEGIN);
                animation->update_instance(root, 1.1F);
                if (!Expect(samePose(engine->get_transform(root), heading*motion) && childUnchanged(),
                            "unconfigured sibling retains original root and child motion after instance release")) return 1;
                animation->release_script_inst(script);

                engine->set_transform(root, heading);
                script = animation->create_script_inst(rootArch, root, "Sc_move");
                if (!Expect(FrontierX::ConfigureNativeAnimationRootBasis(animation, script, root,
                                basis.get_orientation()), "root translation uses the same explicit model basis")) return 1;
                animation->script_start(script, Animation::FORWARD, Animation::BEGIN);
                animation->update_instance(root, 1.1F);
                Vector expectedPosition = heading * basis.rotate(Vector(2,3,4));
                Vector absolute, offset;
                if (!Expect((engine->get_position(root)-expectedPosition).magnitude() < 1e-4F &&
                            animation->get_root_data(script, absolute, offset) &&
                            (offset-basis.rotate(Vector(2,3,4))).magnitude() < 1e-4F,
                            "full-stream translated root and relative offset share model-to-runtime basis")) return 1;
                const Vector movedPosition(71,43,20);
                animation->change_script_position(script, movedPosition);
                animation->change_script_orientation(script, heading.get_orientation());
                animation->get_root_data(script, absolute, offset);
                if (!Expect((absolute-movedPosition).magnitude() < 1e-4F,
                            "root position/orientation changes retain converted relative offsets")) return 1;
                animation->release_script_inst(script);
            }
            if (!Expect(!FrontierX::ConfigureNativeAnimationRootBasis(animation, INVALID_SCRIPT_INST,
                            root, basis.get_orientation()), "invalid script has no root basis owner")) return 1;
            engine->destroy_instance(root);
            engine->release_archetype(modelArch);
            animation->release_script_set_arch(rootArch);
            rootArchive->Release(); modelArchive->Release();
            std::cout << "Original FullStream Weaver model-basis root/child/front/reverse/reload ownership checked\n";
        }
        IMeshManager::InitInfo meshInit{};
        meshInit.ENGINE = engine;
        meshInit.ANIM = animation;
        meshInit.OBJDIR = objects;
        meshInit.REND = renderer;
        meshInit.PIPE = pipeline;
        meshInit.CAMERA = &camera;
        meshInit.HARDPOINT = meshHardpoints;
        meshInit.MATMAN = materials;
        meshManager->InitManager(meshInit);
        for (const unsigned mode : {0U, 6U, 7U, 8U})
        {
            const std::string name = "shared_compound_" + std::to_string(mode) + ".cmp";
            if (!Expect(FrontierXTest::WriteCompoundGameMeshFixture(meshPath.parent_path() / name, mode),
                        "owning compound shared-library fixture created independently of child libraries")) return 1;
            const unsigned before = gameMeshDrawCalls;
            IMeshArchetype *arch = meshManager->CreateMeshArch(name.c_str());
            if (mode == 0U || mode == 6U || mode == 7U)
            {
                const U32 expectedGroups = mode == 6U ? 1U : 2U;
                if (!Expect(arch && arch->GetNumFaceGroups() == expectedGroups &&
                            (mode != 7U || textures->has_texture_id("CompoundSharedTile") == GR_OK),
                            "actual MeshManager preloads the owning CMP library before required part materials")) return 1;
                std::vector<ARCHETYPE_INDEX> parts;
                const auto collect = [](ARCHETYPE_INDEX, ARCHETYPE_INDEX child, void *context) {
                    static_cast<std::vector<ARCHETYPE_INDEX> *>(context)->push_back(child);
                    return true;
                };
                const auto compound = arch->GetEngineArchtype();
                if (!Expect(engine->enumerate_archetype_parts(compound, collect, &parts) == GR_OK && parts.size() == 2U,
                            "actual MeshManager retains both required compound part archetypes")) return 1;
                if (mode == 6U && !Expect(renderer->get_archetype_mesh(compound, 0U) == nullptr &&
                        renderer->get_archetype_mesh(parts[0], 0U) == nullptr &&
                        renderer->get_archetype_mesh(parts[1], 0U) != nullptr,
                        "genuine hardpoint-only root has no local mesh while full compound retains child geometry")) return 1;
                if (mode == 7U) for (const auto part : parts)
                {
                    const Mesh *mesh = renderer->get_archetype_mesh(part, 0U);
                    PixelFormat format;
                    if (!Expect(mesh && mesh->material_list && mesh->material_list[0].texture_id != 0U &&
                                pipeline->get_texture_format(mesh->material_list[0].texture_id, &format) == GR_OK &&
                                !format.has_alpha_channel(),
                                "both original part materials resolve actual opaque texture frames from the outer library")) return 1;
                }
                const auto instance = engine->create_instance2(compound, nullptr);
                if (!Expect(instance != INVALID_INSTANCE_INDEX,
                            "shared-resource compound creates its original connected instances")) return 1;
                cpuMaterialBackendLive = true;
                acceptPolyMeshCpuDraw = true;
                engine->render_instance(&camera, instance, static_cast<EngineFlags>(0), 1.0F,
                                       RF_DONT_CULL_FRUSTUM | RF_DONT_CULL_FACES, nullptr);
                acceptPolyMeshCpuDraw = false;
                cpuMaterialBackendLive = false;
                if (!Expect(gameMeshDrawCalls == before + expectedGroups,
                            "outer-library textures reach both original production part draws")) return 1;
                engine->destroy_instance(instance);
                meshManager->ReleaseArch(arch);
            }
            else if (!Expect(!arch && !meshManager->FindMeshArch(name.c_str()) &&
                             textures->has_texture_id("CompoundMissingTile") != GR_OK && gameMeshDrawCalls == before,
                             "missing required outer map rejects actual MeshManager load without cached archetype or draw")) return 1;
            if (!Expect(absentArchetype(name.c_str()) && absentArchetype("CompoundRoot.3db") &&
                        absentArchetype("CompoundChild.3db"),
                        "actual MeshManager compound release/failure retains no acquired Engine archetype")) return 1;
        }
        if (!Expect(meshSystem->ShutdownAggregate() == GR_OK,
                    "actual MeshManager releases its Engine and Animation references")) return 1;
        meshManager->Release();
        meshSystem->Release();
        meshManagerAggregate->Release();
        meshManagerInner->Release();
        animation->Release();
        animationAggregate->Release();
        animationInner->Release();
        meshHardpoints->Release();
        objects->Release();
        std::cout << "Original MeshManager owning compound shared textures loaded and required missing maps rejected\n";
    }
    renderer->Release();
    engine->set_instance_handler(engineInstance, nullptr);
    if (!Expect(instanceHasArchetype(engineInstance, engineArchetype),
                "production engine replaces its owned instance handler")) return 1;
    engine->destroy_instance(engineInstance);
    engine->release_archetype(engineArchetype);
    legacyMeshArchive->Release();
    static_cast<FrontierX::NativeComponentManager *>(components)->SetProfileParser(nullptr);
    primitive->Release();
    engine->Release();
    outer.lightManager = nullptr;
    lightAggregate->Release();
    lights->Release();
    const auto indexedPath = std::filesystem::path(meshPath.string() + ".indexed");
    IFileSystem *indexedArchive = nullptr;
    IFileSystem *indexedDirectory = nullptr;
    DAFILEDESC indexedDescriptor("openFLAME 3D N-mesh");
    const std::array<std::uint8_t, 16> indexedExpected{{
        255, 0, 0, 255, 0, 255, 0, 255,
        0, 0, 255, 255, 15, 31, 63, 255}};
    if (!Expect(FrontierXTest::WriteLegacyGameMeshFixture(indexedPath, true, true) &&
        FrontierX::OpenNativeUtfFileSystem(indexedPath, &indexedArchive, error) &&
        indexedArchive->CreateInstance(&indexedDescriptor,
            reinterpret_cast<void **>(&indexedDirectory)) == GR_OK &&
        textures->load_texture(indexedDirectory, "Tile8") == GR_OK &&
        std::equal(indexedExpected.begin(), indexedExpected.end(), textureCapture.pixels.begin()),
        "Original TextureLibrary indexed palette reaches real native upload owner")) return 1;
    indexedDirectory->Release();
    indexedArchive->Release();
    // Real external IonCannonFX reached normal_1_6, whose raw MIP0 TGA buffer
    // used new[]/delete. Existing indexed fixtures above use another loader.
    const auto rawTgaPath = std::filesystem::path(meshPath.string() + ".raw-tga");
    IFileSystem *rawTgaArchive = nullptr;
    IFileSystem *rawTgaDirectory = nullptr;
    // Pin the original 1.6 loader's row orientation as well as palette colors.
    const std::array<std::uint8_t, 16> rawTgaExpected{{
        0, 0, 255, 255, 255, 255, 255, 255,
        255, 0, 0, 255, 0, 255, 0, 255}};
    if (!Expect(FrontierXTest::WriteRawTgaTextureFixture(rawTgaPath) &&
        FrontierX::OpenNativeUtfFileSystem(rawTgaPath, &rawTgaArchive, error) &&
        rawTgaArchive->CreateInstance(&indexedDescriptor,
            reinterpret_cast<void **>(&rawTgaDirectory)) == GR_OK,
        "raw MIP0 indexed TGA fixture opens through original texture directory")) return 1;
    for (unsigned cycle = 0; cycle != 3; ++cycle)
    {
        ITL_TEXTURE_ID texture = ITL_INVALID_ID;
        const unsigned destroyedBefore = textureCapture.destroys;
        const unsigned uploadedBefore = textureCapture.uploads;
        const GENRESULT rawLoaded = textures->load_texture(rawTgaDirectory, "RawTgaTile");
        const GENRESULT rawId = textures->get_texture_id("RawTgaTile", &texture);
        if (!Expect(rawLoaded == GR_OK && rawId == GR_OK &&
            texture != ITL_INVALID_ID && textureCapture.uploads == uploadedBefore + 1 &&
            textureCapture.lastUploadWidth == 2 && textureCapture.lastUploadHeight == 2 &&
            textureCapture.lastUploadBytes == rawTgaExpected.size() &&
            std::equal(rawTgaExpected.begin(), rawTgaExpected.end(), textureCapture.pixels.begin()),
            "original 1.6 raw TGA loader uploads all indexed colors without allocation mismatch") ||
            !Expect(textures->release_texture_id(texture) == GR_OK &&
                textureCapture.destroys == destroyedBefore + 1 &&
                textures->has_texture_id("RawTgaTile") != GR_OK,
                "raw TGA texture releases and reloads without retained cache ownership")) return 1;
    }
    rawTgaDirectory->Release();
    rawTgaArchive->Release();
    // The real TM4 BlackHole ring is RGB888 with an independent alpha plane;
    // both the library admission and native upload used to reject this format.
    const auto rgb888Path = std::filesystem::path(meshPath.string() + ".rgb888");
    for (const bool alpha : {true, false, true})
    {
        IFileSystem *archive = nullptr;
        ITL_TEXTURE_ID id = ITL_INVALID_ID;
        ITL_TEXTUREFRAME_IRP frame{};
        PixelFormat format;
        const unsigned uploadsBefore = textureCapture.uploads;
        const unsigned destroysBefore = textureCapture.destroys;
        std::array<std::uint8_t, 16> expected{{17,34,51,0, 68,85,102,96,
                                            119,136,153,255, 170,187,204,0}};
        if (!alpha)
            for (std::size_t i = 3; i < expected.size(); i += 4) expected[i] = 255;
        if (!Expect(FrontierXTest::WriteMipTextureFixture(rgb888Path, alpha, 0, true) &&
            FrontierX::OpenNativeUtfFileSystem(rgb888Path, &archive, error) &&
            textures->load_library(archive, nullptr) == GR_OK &&
            textures->get_texture_id("Rgb888Tile", &id) == GR_OK &&
            textures->get_texture_frame(id, 0, &frame) == GR_OK &&
            pipeline->get_texture_format(frame.rp_texture_id, &format) == GR_OK &&
            format.has_alpha_channel() == alpha &&
            textureCapture.uploads == uploadsBefore + 1 &&
            std::equal(expected.begin(), expected.end(), textureCapture.pixels.begin()),
            "original RGB888 UTF texture preserves two-row RGB order and authored alpha")) return 1;
        const auto retainedPixels = textureCapture.pixels;
        PixelFormat rgbFormat(PF_RGB);
        const unsigned char tooShort[12]{};
        if (!Expect(pipeline->set_texture_level_data(frame.rp_texture_id, 0, 2, 2, 5,
                rgbFormat, tooShort, nullptr, nullptr) != GR_OK &&
            textureCapture.uploads == uploadsBefore + 1 && textureCapture.pixels == retainedPixels,
            "RGB888 rejects a short source stride without changing retained output")) return 1;
        if (!Expect(textures->release_texture_id(id) == GR_OK &&
            textureCapture.destroys == destroysBefore + 1 &&
            textures->has_texture_id("Rgb888Tile") != GR_OK &&
            pipeline->get_texture_format(frame.rp_texture_id, &format) != GR_OK,
            "RGB888 texture owner releases and permits repeated alpha/opaque reload")) return 1;
        archive->Release();
    }
    for (const int delta : {-4, -1, 1})
    {
        IFileSystem *archive = nullptr;
        const unsigned uploadsBefore = textureCapture.uploads;
        if (!Expect(FrontierXTest::WriteMipTextureFixture(rgb888Path, true, delta, true) &&
            FrontierX::OpenNativeUtfFileSystem(rgb888Path, &archive, error) &&
            textures->load_library(archive, nullptr) == GR_OK &&
            textures->has_texture_id("Rgb888Tile") != GR_OK &&
            textureCapture.uploads == uploadsBefore,
            "RGB888 rejects empty/truncated/oversized authored alpha without publishing a texture")) return 1;
        archive->Release();
    }
    textures->Release();
    textureAggregate->Release();
    textureInner->Release();
    meshDirectory->Release();
    meshArchive->Release();
    materials->Release();
    gameTextures->Release();
    gameTextureInner->Release();
    inner->Release();
    materialDirectory->Release();
    archive->Release();
    pipeline->Release();
    materialPrimitive->Release();
    std::cout << "production game material load passed\n";
    return 0;
}
