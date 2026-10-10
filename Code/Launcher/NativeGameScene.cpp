#include "CQTrace.h"
#include "NativeGameScene.h"
#include "NativeGameSystem.h"

#include "FrontierX/GameMaterialManager.h"
#include "FrontierX/GameTextureManager.h"
#include "FrontierX/NativeComponentManager.h"
#include "FrontierX/NativeMathEngine.h"
#include "FrontierX/NativePolyMesh.h"
#include "FrontierX/NativeRenderManager.h"
#include "FrontierX/NativeRenderPipeline.h"
#include "FrontierX/NativeRenderPrimitive.h"
#include "FrontierX/NativeSystemContainer.h"
#include "FrontierX/NativeUtfFileSystem.h"
#include "FrontierX/RenderSmokeScene.h"
#include "FrontierX/SdlGpuRenderer.h"
#include "Include/NativeAppGlobals.h"
#include "ICamera.h"
#include "ENGINE.H"
#include "IMaterialManager.h"
#include "IProfileParser.h"
#include "ITextureLibrary.h"
#include "IVertexBuffer.h"
#include "IVertexBufferManager.h"
#include "Mesh.h"
#include "MeshRender.h"
#include "TManager.h"
#include "View2d.h"
#include "matrix4.h"
#include "renderer.h"

#include <cstring>
#include <new>

extern bool RegisterNativeTextureLibrary(ICOManager * manager);

namespace FrontierX
{
namespace
{
template <class Interface>
class Owner
{
public:
    ~Owner() { Reset(); }
    Interface ** Receive() { return &value_; }
    Interface * Get() const { return value_; }
    void Reset()
    {
        if (value_) value_->Release();
        value_ = nullptr;
    }
private:
    Interface * value_ = nullptr;
};

struct SceneServices final : IDAComponent
{
    IRenderPipeline * pipeline = nullptr;
    IRenderPrimitive * primitive = nullptr;

    GENRESULT COMAPI QueryInterface(const C8 * name, void ** result) override
    {
        if (!result) return GR_INVALID_PARMS;
        *result = nullptr;
        if (!name) return GR_INTERFACE_UNSUPPORTED;
        if (std::strcmp(name, IID_IRenderPipeline) == 0 && pipeline)
        {
            *result = pipeline;
            pipeline->AddRef();
        }
        else if (std::strcmp(name, IID_IRenderPrimitive) == 0 && primitive)
        {
            *result = primitive;
            primitive->AddRef();
        }
        return *result ? GR_OK : GR_INTERFACE_UNSUPPORTED;
    }
    U32 COMAPI AddRef() override { return 1U; }
    U32 COMAPI Release() override { return 1U; }
};

struct SceneSystemServices : IDAComponent
{
    struct Inner final : IDAComponent
    {
        explicit Inner(SceneSystemServices * owner) : owner(owner) {}

        GENRESULT COMAPI QueryInterface(const C8 * name, void ** result) override
        {
            return owner->QueryService(name, result);
        }
        U32 COMAPI AddRef() override { return ++references; }
        U32 COMAPI Release() override
        {
            const U32 remaining = --references;
            if (!remaining) delete owner;
            return remaining;
        }

        SceneSystemServices * owner;
        U32 references = 1U;
    } inner;

    SceneSystemServices(AGGDESC * descriptor, const SceneServices & services)
        : inner(this), outer(descriptor->outer ? descriptor->outer : &inner),
          pipeline(services.pipeline), primitive(services.primitive)
    {
        if (descriptor->outer) *descriptor->inner = &inner;
    }
    virtual ~SceneSystemServices() = default;

    GENRESULT COMAPI QueryInterface(const C8 * name, void ** result) override
    {
        return outer->QueryInterface(name, result);
    }
    U32 COMAPI AddRef() override { return outer->AddRef(); }
    U32 COMAPI Release() override { return outer->Release(); }

    GENRESULT QueryService(const C8 * name, void ** result)
    {
        if (!result) return GR_INVALID_PARMS;
        *result = nullptr;
        if (!name) return GR_INTERFACE_UNSUPPORTED;
        if (std::strcmp(name, IID_IRenderPipeline) == 0 && pipeline)
        {
            *result = pipeline;
            pipeline->AddRef();
        }
        else if (std::strcmp(name, IID_IRenderPrimitive) == 0 && primitive)
        {
            *result = primitive;
            primitive->AddRef();
        }
        return *result ? GR_OK : GR_INTERFACE_UNSUPPORTED;
    }

    void Detach()
    {
        pipeline = nullptr;
        primitive = nullptr;
    }

    IDAComponent * outer;
    IRenderPipeline * pipeline;
    IRenderPrimitive * primitive;
};

constexpr char SceneSystemServiceName[] = "FrontierXSceneServices";

class SceneSystemServiceFactory final : public IComponentFactory
{
public:
    SceneSystemServiceFactory(const SceneServices & services,
                              SceneSystemServices * & created)
        : services_(&services), created_(created) {}

    GENRESULT COMAPI QueryInterface(const C8 * name, void ** result) override
    {
        if (!result) return GR_INVALID_PARMS;
        *result = nullptr;
        if (!name || std::strcmp(name, IID_IComponentFactory) != 0)
            return GR_INTERFACE_UNSUPPORTED;
        *result = static_cast<IComponentFactory *>(this);
        AddRef();
        return GR_OK;
    }

    U32 COMAPI AddRef() override { return ++references_; }
    U32 COMAPI Release() override
    {
        const U32 remaining = --references_;
        if (!remaining) delete this;
        return remaining;
    }

    GENRESULT COMAPI CreateInstance(DACOMDESC * descriptor, void ** result) override
    {
        if (!result) return GR_INVALID_PARMS;
        *result = nullptr;
        if (!services_ || !descriptor || descriptor->size != sizeof(AGGDESC) ||
            !descriptor->interface_name ||
            std::strcmp(descriptor->interface_name, SceneSystemServiceName) != 0)
            return GR_INTERFACE_UNSUPPORTED;
        auto * aggregate = static_cast<AGGDESC *>(descriptor);
        if (aggregate->outer && !aggregate->inner) return GR_INVALID_PARMS;
        auto * component = new (std::nothrow)
            SceneSystemServices(aggregate, *services_);
        if (!component) return GR_OUT_OF_MEMORY;
        created_ = component;
        *result = static_cast<IDAComponent *>(component);
        return GR_OK;
    }

    void Disable() { services_ = nullptr; }

private:
    U32 references_ = 1U;
    const SceneServices * services_;
    SceneSystemServices * & created_;
};

bool AttachSceneSystemServices(ICOManager * manager, ISystemContainer * container,
                               const SceneServices & services,
                               SceneSystemServices * & attached)
{
    attached = nullptr;
    auto * factory = new (std::nothrow) SceneSystemServiceFactory(services, attached);
    if (!factory) return false;
    const GENRESULT registered = manager->RegisterComponent(
        factory, SceneSystemServiceName);
    GENRESULT added = GR_GENERIC;
    GENRESULT unregistered = GR_GENERIC;
    if (registered == GR_OK)
    {
        const AGGDESC descriptor(SceneSystemServiceName);
        added = container->AddComponent(&descriptor);
        unregistered = manager->UnregisterComponent(factory, SceneSystemServiceName);
    }
    factory->Disable();
    factory->Release();
    if (registered == GR_OK && added == GR_OK && unregistered == GR_OK && attached)
        return true;
    if (added == GR_OK && attached)
    {
        attached->Detach();
        RemoveNativeSystemComponent(container, &attached->inner);
    }
    attached = nullptr;
    return false;
}

struct SceneSystemServiceDetach
{
    ISystemContainer * container;
    SceneSystemServices * component;
    ~SceneSystemServiceDetach()
    {
        if (component)
        {
            component->Detach();
            RemoveNativeSystemComponent(container, &component->inner);
        }
    }
};

struct SceneCamera final : ICamera
{
    U32 references = 1U;
    Transform transform;
    Transform inverse;
    Vector position{0.0F, 0.0F, 5.0F};
    Vector look{0.0F, 0.0F, 0.0F};
    ViewRect pane{0, 0, 63, 63};

    SceneCamera()
    {
        transform.translation.z = 5.0F;
        inverse.translation.z = -5.0F;
    }
    GENRESULT COMAPI QueryInterface(const C8 * name, void ** result) override
    {
        if (!result) return GR_INVALID_PARMS;
        *result = nullptr;
        if (!name || std::strcmp(name, IID_ICamera) != 0)
            return GR_INTERFACE_UNSUPPORTED;
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
    bool COMAPI point_to_screen(float &, float &, float &, const Vector &) const override
    {
        return false;
    }
    void COMAPI screen_to_point(Vector & point, float, float) const override
    {
        point = look;
    }
    vis_state COMAPI object_visibility(const Vector &, float) const override
    {
        return VS_FULLY_VISIBLE;
    }
    Transform COMAPI get_inverse_transform() const override { return inverse; }
};

bool Fail(std::string & error, const char * message)
{
    error = message;
    return false;
}

struct ProfileReset
{
    NativeComponentManager * manager;
    ~ProfileReset() { manager->SetProfileParser(nullptr); }
};

struct SceneGpuContext
{
    SdlGpuRenderer * gpu = nullptr;
    SdlGpuReadback * appImage = nullptr;
    std::string * error = nullptr;
    bool appPresented = false;
    bool appTextureInfluenced = false;
    unsigned appDraws = 0U;
    U32 appTexture = 0U;
    UINT firstVertexRegister = 0U;
    UINT vertexRegisterCount = 0U;
};

GENRESULT DACOM_CDECL SubmitScene(void * context,
                                  const RenderSubmissionView & view)
{
    return static_cast<SceneGpuContext *>(context)->gpu->Submit(view);
}

GENRESULT DACOM_CDECL SubmitSceneMesh(void * context,
                                      const RenderSubmissionView3 & view)
{
    return static_cast<SceneGpuContext *>(context)->gpu->Submit(view);
}

GENRESULT DACOM_CDECL SubmitSceneApp(void * context,
                                     const RenderSubmissionView4 & view)
{
    auto & scene = *static_cast<SceneGpuContext *>(context);
    const GENRESULT result = scene.gpu->Submit(view);
    if (result != GR_OK) return result;
    ++scene.appDraws;
    scene.appTexture = scene.gpu->CurrentTextureHandle();
    if (!scene.appImage || !scene.error) return GR_OK;
    SdlGpuReadback untextured;
    if (!scene.gpu->Readback64(*scene.appImage, *scene.error) ||
        !scene.appImage->rgbValid || !scene.appTexture ||
        scene.gpu->BindTexture(0U, 0U) != GR_OK ||
        !scene.gpu->Readback64(untextured, *scene.error))
        return GR_GENERIC;
    scene.appTextureInfluenced = untextured.rgbValid &&
        untextured.checksum != scene.appImage->checksum;
    if (scene.gpu->BindTexture(0U, scene.appTexture) != GR_OK ||
        !scene.gpu->Present(scene.appPresented, *scene.error))
        return GR_GENERIC;
    return GR_OK;
}

GENRESULT DACOM_CDECL SetSceneState(void * context,
                                    const RenderStateSnapshot & state)
{
    return static_cast<SceneGpuContext *>(context)->gpu->SetRenderState(state);
}

GENRESULT DACOM_CDECL SetSceneStage(void * context,
                                    const TextureStageSnapshot & state)
{
    return static_cast<SceneGpuContext *>(context)->gpu->SetTextureStageState(state);
}

GENRESULT DACOM_CDECL BindSceneTexture(void * context, U32 stage, U32 texture)
{
    return static_cast<SceneGpuContext *>(context)->gpu->BindTexture(stage, texture);
}

GENRESULT DACOM_CDECL SetSceneModelview(void * context,
                                        const Transform & modelview)
{
    return static_cast<SceneGpuContext *>(context)->gpu->SetModelview(modelview);
}

GENRESULT DACOM_CDECL SetSceneProjection(void * context,
                                         const Matrix4 & projection)
{
    return static_cast<SceneGpuContext *>(context)->gpu->SetProjection(projection);
}

GENRESULT DACOM_CDECL SetSceneViewport(void * context, int x, int y,
                                       int width, int height)
{
    return static_cast<SceneGpuContext *>(context)->gpu->SetViewport(
        x, y, width, height);
}

GENRESULT DACOM_CDECL SetSceneSampler(void * context,
                                      const SamplerStateSnapshot & state)
{
    return static_cast<SceneGpuContext *>(context)->gpu->SetSamplerState(state);
}

GENRESULT DACOM_CDECL SetSceneVertexConstants(void * context,
                                               UINT firstRegister,
                                               const FLOAT * values,
                                               UINT registerCount)
{
    auto & scene = *static_cast<SceneGpuContext *>(context);
    const GENRESULT result = scene.gpu->SetVertexConstants(
        firstRegister, values, registerCount);
    if (result == GR_OK)
    {
        scene.firstVertexRegister = firstRegister;
        scene.vertexRegisterCount = registerCount;
    }
    return result;
}

struct SceneVertexBufferRegistry final : ICQ_VB_Manager
{
    IVertexBufferOwner * owner = nullptr;
    unsigned additions = 0U;
    unsigned removals = 0U;
    void Add(IVertexBufferOwner * value) override
    {
        owner = value;
        ++additions;
    }
    void Delete(IVertexBufferOwner * value) override
    {
        if (owner == value) owner = nullptr;
        ++removals;
    }
};

struct SceneAppGlobals
{
    SceneAppGlobals(IEngine * engine, IRenderer * renderer,
                    IRenderPipeline * pipeline, IRenderPrimitive * primitive,
                    ITextureLibrary * textures, IMaterialManager * materials,
                    ICamera * camera, ICQ_VB_Manager * buffers)
    {
        ENGINE = engine;
        REND = renderer;
        PIPE = pipeline;
        BATCH = primitive;
        TEXLIB = textures;
        MATMAN = materials;
        MAINCAM = camera;
        vb_mgr = buffers;
        CQFLAGS.b3DEnabled = -1;
    }
    ~SceneAppGlobals()
    {
        MAINCAM = nullptr;
        BATCH = nullptr;
        ENGINE = nullptr;
        REND = nullptr;
        PIPE = nullptr;
        TEXLIB = nullptr;
        MATMAN = nullptr;
        vb_mgr = nullptr;
    }
};

struct SceneMeshInfoOwner
{
    IMeshInfoTree * tree = nullptr;
    ~SceneMeshInfoOwner()
    {
        if (tree) DestroyMeshInfoTree(tree);
    }
};

struct SceneEngineRelease
{
    NativeGameSystem * system;
    ~SceneEngineRelease() { ReleaseNativeGameSystemEngine(system); }
};
} // namespace

bool RenderNativeGameScene(ICOManager * components, NativeGameSystem * gameSystem,
                           std::uint32_t windowToken,
                           const std::filesystem::path & materialPath,
                           const std::filesystem::path & meshPath,
                           const char * vertexShader,
                           const char * fragmentShader,
                           const char * extendedVertexShader,
                           const char * gameVertexShader,
                           NativeSceneContinue continueScene, void * loopContext,
                           std::uint32_t & presentedFrames,
                           SdlGpuReadback & image, std::string & error)
{
    presentedFrames = 0U;
    if (!components || !gameSystem || !NativeGameSystemEngine(gameSystem) ||
        !windowToken || !vertexShader || !fragmentShader ||
        !extendedVertexShader || !gameVertexShader || !continueScene)
        return Fail(error, "invalid game-scene startup input");
    // NativeShell registers the system-service factories before GameSystem startup.
    if (!RegisterNativeMathEngine(components) ||
        !RegisterNativeTextureLibrary(components))
        return Fail(error, "game-scene owner registration failed");

    SdlGpuRenderer gpu;
    const SdlGpuTextureView initialTexture{
        RenderSmokeTextureRgba.data(), RenderSmokeTextureWidth,
        RenderSmokeTextureHeight, RenderSmokeTextureRgba.size()};
    if (!gpu.Initialize(windowToken, vertexShader, fragmentShader,
                        initialTexture, error, extendedVertexShader,
                        gameVertexShader)) return false;

    Owner<IFileSystem> materialArchive;
    Owner<IFileSystem> meshArchive;
    std::error_code materialStatus;
    const bool physicalMaterial = std::filesystem::is_directory(materialPath, materialStatus);
    const bool materialOpened = physicalMaterial ?
        OpenNativeDirectoryFileSystem(materialPath, materialArchive.Receive(), error) :
        OpenNativeUtfFileSystem(materialPath, materialArchive.Receive(), error);
    if (!materialOpened ||
        !OpenNativeUtfFileSystem(meshPath, meshArchive.Receive(), error))
        return false;

    SceneGpuContext gpuContext{&gpu};
    NativeRenderPrimitiveDesc primitiveDescriptor(
        SubmitScene, &gpuContext, nullptr, SetSceneState, nullptr, nullptr,
        SetSceneStage, BindSceneTexture, SetSceneModelview,
        SetSceneProjection, SubmitSceneMesh, SubmitSceneApp, SetSceneSampler,
        SetSceneViewport);
    ISystemContainer * system = NativeGameSystemContainer(gameSystem);
    Owner<IRenderPrimitive> primitive;
    bool profilePrimitive = system &&
        system->QueryInterface(IID_IRenderPrimitive,
            reinterpret_cast<void **>(primitive.Receive())) == GR_OK &&
        primitive.Get();
    if (profilePrimitive)
    {
        if (BindNativeRenderPrimitive(primitive.Get(), primitiveDescriptor) != GR_OK)
            return Fail(error, "game-scene profile primitive binding failed");
    }
    else
    {
        primitive.Reset();
        if (components->CreateInstance(&primitiveDescriptor,
                reinterpret_cast<void **>(primitive.Receive())) != GR_OK ||
            !primitive.Get())
            return Fail(error, "game-scene primitive creation failed");
    }
    struct PrimitiveBindingRelease
    {
        IRenderPrimitive * primitive;
        ~PrimitiveBindingRelease()
        {
            if (primitive) UnbindNativeRenderPrimitive(primitive);
        }
    } primitiveBinding{profilePrimitive ? primitive.Get() : nullptr};
    const NativeTextureOperations textureOperations{
        &gpu, SdlGpuRenderer::TextureLookupCallback,
        SdlGpuRenderer::TextureCreateCallback,
        SdlGpuRenderer::TextureUploadCallback,
        SdlGpuRenderer::TextureDestroyCallback};
    NativeRenderPipeline localPipeline(primitive.Get(), textureOperations,
                                       SetSceneVertexConstants, &gpuContext);
    Owner<IRenderPipeline> profilePipeline;
    const bool hasProfilePipeline = system &&
        system->QueryInterface(IID_IRenderPipeline,
            reinterpret_cast<void **>(profilePipeline.Receive())) == GR_OK &&
        profilePipeline.Get();
    if (hasProfilePipeline &&
        BindNativeRenderPipeline(profilePipeline.Get(), primitive.Get(),
                                 textureOperations, SetSceneVertexConstants,
                                 &gpuContext) != GR_OK)
        return Fail(error, "game-scene profile pipeline binding failed");
    struct PipelineBindingRelease
    {
        IRenderPipeline * pipeline;
        ~PipelineBindingRelease()
        {
            if (pipeline) UnbindNativeRenderPipeline(pipeline);
        }
    } pipelineBinding{hasProfilePipeline ? profilePipeline.Get() : nullptr};
    IRenderPipeline * pipeline = hasProfilePipeline
        ? profilePipeline.Get() : static_cast<IRenderPipeline *>(&localPipeline);
    Owner<IVertexBufferManager> profileVertexBuffers;
    const bool hasProfileVertexBuffers = system &&
        system->QueryInterface(IID_IVertexBufferManager,
            reinterpret_cast<void **>(profileVertexBuffers.Receive())) == GR_OK &&
        profileVertexBuffers.Get();
    if (!hasProfileVertexBuffers) profileVertexBuffers.Reset();
    if (hasProfileVertexBuffers &&
        profileVertexBuffers.Get()->initialize(nullptr) != GR_OK)
        return Fail(error, "game-scene vertex-buffer manager initialization failed");
    struct VertexBufferCleanup
    {
        IVertexBufferManager * manager;
        ~VertexBufferCleanup() { if (manager) manager->cleanup(); }
    } vertexBufferCleanup{profileVertexBuffers.Get()};
    SceneServices services;
    services.pipeline = pipeline;
    services.primitive = primitive.Get();

    SceneSystemServices * attachedServices = nullptr;
    if (!system || !AttachSceneSystemServices(
            components, system, services, attachedServices))
        return Fail(error, "game-scene system-service aggregation failed");
    SceneSystemServiceDetach serviceDetach{system, attachedServices};
    struct AddedSystemComponentDetach
    {
        ISystemContainer * container;
        IDAComponent * inner;
        ~AddedSystemComponentDetach()
        {
            if (inner) RemoveNativeSystemComponent(container, inner);
        }
    };

    const AGGDESC textureManagerDescriptor("ITManager");
    Owner<IDAComponent> textureManagerInner;
    AddedSystemComponentDetach textureManagerDetach{system, nullptr};
    Owner<ITManager> textureManager;
    const bool profileTextureManager =
        system->QueryInterface(IID_ITManager,
            reinterpret_cast<void **>(textureManager.Receive())) == GR_OK &&
        textureManager.Get();
    if (!profileTextureManager)
    {
        textureManager.Reset();
        if (AddNativeSystemComponent(system, &textureManagerDescriptor,
            textureManagerInner.Receive()) != GR_OK ||
            !textureManagerInner.Get())
            return Fail(error, "game-scene texture manager aggregation failed");
        textureManagerDetach.inner = textureManagerInner.Get();
        if (textureManagerInner.Get()->QueryInterface(IID_ITManager,
            reinterpret_cast<void **>(textureManager.Receive())) != GR_OK ||
            !textureManager.Get())
            return Fail(error, "game-scene texture manager interface failed");
    }
    struct TextureManagerCleanup
    {
        ITManager * manager;
        ~TextureManagerCleanup()
        {
            manager->Flush();
            ITManager::InitInfo detached{};
            manager->Initialize(detached);
        }
    } textureCleanup{textureManager.Get()};
    ITManager::InitInfo textureInit{};
    textureInit.PIPE = pipeline;
    textureManager.Get()->Initialize(textureInit);

    const AGGDESC materialDescriptor("IMaterialManager");
    Owner<IDAComponent> materialInner;
    AddedSystemComponentDetach materialDetach{system, nullptr};
    Owner<IMaterialManager> materials;
    const bool profileMaterials =
        system->QueryInterface(IID_IMaterialManager,
            reinterpret_cast<void **>(materials.Receive())) == GR_OK &&
        materials.Get();
    if (!profileMaterials)
    {
        materials.Reset();
        if (AddNativeSystemComponent(system, &materialDescriptor,
            materialInner.Receive()) != GR_OK || !materialInner.Get())
            return Fail(error, "game-scene material manager aggregation failed");
        materialDetach.inner = materialInner.Get();
        if (materialInner.Get()->QueryInterface(IID_IMaterialManager,
            reinterpret_cast<void **>(materials.Receive())) != GR_OK ||
            !materials.Get())
            return Fail(error, "game-scene material manager interface failed");
    }
    if (!ResetNativeGameMaterialManager(materials.Get()))
        return Fail(error, "game-scene material manager owner is unsupported");
    struct MaterialManagerCleanup
    {
        IMaterialManager * manager;
        ~MaterialManagerCleanup() { ResetNativeGameMaterialManager(manager); }
    } materialCleanup{materials.Get()};
    IMaterialManager::InitInfo materialInit{};
    materialInit.MATDIR = materialArchive.Get();
    materialInit.PIPE = pipeline;
    materialInit.TMANAGER = textureManager.Get();
    if (materials.Get()->Initialize(materialInit) != GR_OK)
        return Fail(error, "game-scene material manager initialization failed");

    const AGGDESC textureLibraryDescriptor("TextureLibrary");
    Owner<IDAComponent> textureLibraryInner;
    AddedSystemComponentDetach textureLibraryDetach{system, nullptr};
    Owner<IAggregateComponent> textureLibraryAggregate;
    Owner<ITextureLibrary> textureLibrary;
    const bool profileTextureLibrary =
        system->QueryInterface(IID_ITextureLibrary,
            reinterpret_cast<void **>(textureLibrary.Receive())) == GR_OK &&
        textureLibrary.Get();
    if (!profileTextureLibrary)
    {
        textureLibrary.Reset();
        if (AddNativeSystemComponent(system, &textureLibraryDescriptor,
                textureLibraryInner.Receive()) != GR_OK ||
            !textureLibraryInner.Get())
            return Fail(error, "game-scene texture library aggregation failed");
        textureLibraryDetach.inner = textureLibraryInner.Get();
        if (textureLibraryInner.Get()->QueryInterface(IID_IAggregateComponent,
                reinterpret_cast<void **>(textureLibraryAggregate.Receive())) != GR_OK ||
            !textureLibraryAggregate.Get() ||
            textureLibraryAggregate.Get()->Initialize() != GR_OK ||
            textureLibraryInner.Get()->QueryInterface(IID_ITextureLibrary,
                reinterpret_cast<void **>(textureLibrary.Receive())) != GR_OK ||
            !textureLibrary.Get())
            return Fail(error, "game-scene texture library initialization failed");
    }
    Owner<IRenderPipeline> systemPipeline;
    Owner<IRenderPrimitive> systemPrimitive;
    Owner<ITManager> systemTextureManager;
    Owner<ITextureLibrary> systemTextures;
    Owner<IMaterialManager> systemMaterials;
    if (system->QueryInterface(IID_IRenderPipeline,
            reinterpret_cast<void **>(systemPipeline.Receive())) != GR_OK ||
        systemPipeline.Get() != pipeline ||
        system->QueryInterface(IID_IRenderPrimitive,
            reinterpret_cast<void **>(systemPrimitive.Receive())) != GR_OK ||
        systemPrimitive.Get() != primitive.Get() ||
        system->QueryInterface(IID_ITManager,
            reinterpret_cast<void **>(systemTextureManager.Receive())) != GR_OK ||
        systemTextureManager.Get() != textureManager.Get() ||
        system->QueryInterface(IID_ITextureLibrary,
            reinterpret_cast<void **>(systemTextures.Receive())) != GR_OK ||
        systemTextures.Get() != textureLibrary.Get() ||
        system->QueryInterface(IID_IMaterialManager,
            reinterpret_cast<void **>(systemMaterials.Receive())) != GR_OK ||
        systemMaterials.Get() != materials.Get())
        return Fail(error, "game-scene system-service forwarding failed");

    PROFPARSEDESC2 profileDescriptor;
    Owner<IAggregateComponent> profileAggregate;
    if (components->CreateInstance(&profileDescriptor,
            reinterpret_cast<void **>(profileAggregate.Receive())) != GR_OK ||
        !profileAggregate.Get())
        return Fail(error, "game-scene profile creation failed");
    Owner<IProfileParser2> profile2;
    Owner<IProfileParser> profile;
    constexpr char profileText[] = "[Engine]\nIRenderer\n[RenderManager]\n  PolyMesh\n";
    if (profileAggregate.Get()->QueryInterface(IID_IProfileParser2,
            reinterpret_cast<void **>(profile2.Receive())) != GR_OK ||
        !profile2.Get() ||
        profile2.Get()->Initialize2(profileText, sizeof(profileText) - 1U) != GR_OK ||
        profileAggregate.Get()->QueryInterface(IID_IProfileParser,
            reinterpret_cast<void **>(profile.Receive())) != GR_OK ||
        !profile.Get())
        return Fail(error, "game-scene profile initialization failed");
    static_cast<NativeComponentManager *>(components)->SetProfileParser(profile.Get());
    ProfileReset profileReset{static_cast<NativeComponentManager *>(components)};

    Owner<IEngine> engine;
    *engine.Receive() = NativeGameSystemEngine(gameSystem);
    engine.Get()->AddRef();
    SceneEngineRelease engineRelease{gameSystem};
    if (engine.Get()->load_engine_components(system) != GR_OK)
        return Fail(error, "game-scene RenderMgr/PolyMesh composition failed");
    const ARCHETYPE_INDEX archetype = engine.Get()->create_archetype(
        "native_game_scene.3db", meshArchive.Get());
    if (archetype == INVALID_ARCHETYPE_INDEX)
        return Fail(error, "game-scene mesh archetype creation failed");
    const INSTANCE_INDEX instance = engine.Get()->create_instance2(archetype, nullptr);
    if (instance == INVALID_INSTANCE_INDEX)
    {
        engine.Get()->release_archetype(archetype);
        return Fail(error, "game-scene mesh instance creation failed");
    }
    Owner<IRenderer> gameRenderer;
    Mesh * gameMesh = nullptr;
    if (engine.Get()->QueryInterface(IID_IRenderer,
            reinterpret_cast<void **>(gameRenderer.Receive())) == GR_OK &&
        gameRenderer.Get())
        gameMesh = gameRenderer.Get()->get_instance_mesh(instance, nullptr);
    Matrix4 projection(true);
    projection.d[0][0] = 0.5F;
    projection.d[1][1] = 0.5F;
    projection.d[2][2] = -0.1F;
    SceneCamera camera;
    const bool ready = gameMesh &&
        primitive.Get()->set_projection(projection) == GR_OK &&
        primitive.Get()->set_render_state(D3DRS_CULLMODE, D3DCULL_NONE) == GR_OK;
    bool drawn = false;
    if (ready && gameMesh->imaterial_list && gameMesh->imaterial_cnt > 0)
    {
        SceneVertexBufferRegistry buffers;
        {
            SceneAppGlobals appGlobals(engine.Get(), gameRenderer.Get(),
                pipeline, primitive.Get(), textureLibrary.Get(),
                materials.Get(), &camera, &buffers);
            Owner<IMeshRender> appRender;
            *appRender.Receive() = CreateMeshRender();
            if (appRender.Get()) appRender.Get()->AddRef();
            SceneMeshInfoOwner appTree;
            if (appRender.Get() && gameMesh->imaterial_list[0])
            {
                appRender.Get()->Init(archetype);
                appTree.tree = CreateMeshInfoTree(instance);
                if (appTree.tree && appTree.tree->GetMeshInfo())
                {
                    appRender.Get()->SetupMeshInfo(appTree.tree->GetMeshInfo());
                    gameMesh->imaterial_list[0]->Realize();
                    gpuContext.appImage = &image;
                    gpuContext.error = &error;
                    appRender.Get()->Render(appTree.tree->GetMeshInfo(), camera.inverse);
                    drawn = gpuContext.appDraws != 0U &&
                        gpuContext.appPresented &&
                        gpuContext.appTextureInfluenced && image.rgbValid &&
                        image.changedPixels > 0U &&
                        gpuContext.firstVertexRegister == 30U &&
                        gpuContext.vertexRegisterCount == 2U &&
                        gpuContext.appTexture != 0U &&
                        gpu.IsTexture(gpuContext.appTexture) &&
                        gpuContext.appTexture == textureManager.Get()->GetFirstTexture();
                    if (drawn)
                    {
                        ++presentedFrames;
                        const std::uint64_t firstChecksum = image.checksum;
                        while (continueScene(loopContext))
                        {
                            gpuContext.appPresented = false;
                            gpuContext.appTextureInfluenced = false;
                            const unsigned previousDraws = gpuContext.appDraws;
                            appRender.Get()->Render(appTree.tree->GetMeshInfo(),
                                                    camera.inverse);
                            if (gpuContext.appDraws != previousDraws + 1U ||
                                !gpuContext.appPresented ||
                                !gpuContext.appTextureInfluenced ||
                                !image.rgbValid || image.checksum != firstChecksum)
                            {
                                drawn = Fail(error,
                                    "game-scene repeated App frame differed");
                                break;
                            }
                            ++presentedFrames;
                        }
                    }
                }
            }
        }
        drawn = drawn && buffers.owner == nullptr &&
            buffers.additions == buffers.removals;
    }
    else if (ready)
    {
        engine.Get()->update(0.0F);
        engine.Get()->render_instance(&camera, instance, EN_DONT_RECURSE, 1.0F,
            RF_DONT_CULL_FRUSTUM | RF_DONT_CULL_FACES, nullptr);
        bool presented = false;
        const U32 textureHandle = gpu.CurrentTextureHandle();
        if (gpu.VertexCount() == 0U || gpu.IndexCount() == 0U)
            Fail(error, "game-scene PolyMesh submitted no geometry");
        else if (textureHandle == 0U)
            Fail(error, "game-scene PolyMesh did not bind a texture");
        else if (!gpu.IsTexture(textureHandle))
            Fail(error, "game-scene PolyMesh texture handle is invalid");
        else if (!gpu.Readback64(image, error) || !image.rgbValid)
            Fail(error, "game-scene PolyMesh readback failed");
        else if (image.changedPixels == 0U)
            Fail(error, "game-scene PolyMesh image is unchanged");
        else
        {
            drawn = gpu.Present(presented, error) && presented;
            if (!drawn && error.empty())
                Fail(error, "game-scene PolyMesh presentation failed");
        }
        if (drawn)
        {
            ++presentedFrames;
            const std::uint64_t firstChecksum = image.checksum;
            while (continueScene(loopContext))
            {
                engine.Get()->render_instance(&camera, instance, EN_DONT_RECURSE,
                    1.0F, RF_DONT_CULL_FRUSTUM | RF_DONT_CULL_FACES, nullptr);
                presented = false;
                if (!gpu.Readback64(image, error) || !image.rgbValid ||
                    image.checksum != firstChecksum ||
                    !gpu.Present(presented, error) || !presented)
                {
                    drawn = Fail(error, "game-scene repeated mesh frame differed");
                    break;
                }
                ++presentedFrames;
            }
        }
    }
    engine.Get()->destroy_instance(instance);
    engine.Get()->release_archetype(archetype);
    if (!drawn && error.empty())
        return Fail(error, "game-scene draw or bounded readback failed");
    return drawn;
}
} // namespace FrontierX
