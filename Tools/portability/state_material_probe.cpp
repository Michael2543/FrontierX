#include "DACOM.H"
#include "FDump.h"
#include "FrontierX/LegacyRenderSubmission.h"
#include "FrontierX/NativeRenderPipeline.h"
#include "FrontierX/NativeRenderPrimitive.h"
#include "MaterialTextureFixture.h"
#include "PolyMeshFixture.h"
#include "FrontierX/NativeUtfFileSystem.h"
#include "FrontierX/UtfMetadata.h"
#include "FrontierX/RenderState.h"
#include "FrontierX/StateMaterialRegistration.h"
#include "FrontierX/VertexBufferManagerRegistration.h"
#include "IMaterial.h"
#include "IMaterialLibrary.h"
#include "IMaterialProperties.h"
#include "IStateMaterial.h"
#include "ITextureLibrary.h"
#include "IVertexBufferManager.h"

#include <array>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace
{
struct Capture
{
    unsigned draws = 0U;
    U32 texture = 0U;
    FrontierX::TextureStageSnapshot stage{};
    FrontierX::RenderStateSnapshot render{};
};

struct TextureCapture
{
    std::array<std::uint8_t, 16> pixels{};
    unsigned creates = 0U;
    unsigned uploads = 0U;
    unsigned destroys = 0U;
    bool failUpload = false;
};

GENRESULT CreateTexture(void * context, U32 width, U32 height, U32 & handle)
{
    if (width != 2U || height != 2U) return GR_INVALID_PARMS;
    ++static_cast<TextureCapture *>(context)->creates;
    handle = 64U;
    return GR_OK;
}

GENRESULT UploadTexture(void * context, U32 handle, const std::uint8_t * pixels,
                        U32 width, U32 height, std::size_t byteCount)
{
    if (handle != 64U || width != 2U || height != 2U || byteCount != 16U)
        return GR_INVALID_PARMS;
    auto & capture = *static_cast<TextureCapture *>(context);
    ++capture.uploads;
    if (capture.failUpload) return GR_GENERIC;
    std::memcpy(capture.pixels.data(), pixels, byteCount);
    return GR_OK;
}

GENRESULT DestroyTexture(void * context, U32 handle)
{
    if (handle != 64U) return GR_INVALID_PARMS;
    ++static_cast<TextureCapture *>(context)->destroys;
    return GR_OK;
}

int DACOM_CDECL IgnoreDiagnostic(ErrorCode, const C8 *, ...) { return 0; }

GENRESULT DACOM_CDECL CaptureDraw(void * context,
                                   const FrontierX::RenderSubmissionView & view)
{
    auto & capture = *static_cast<Capture *>(context);
    if (!view.IsValid() || view.vertexCount != 3U || view.indexCount != 3U ||
        view.PrimitiveCount() != 1U) return GR_INVALID_PARMS;
    ++capture.draws;
    return GR_OK;
}

GENRESULT DACOM_CDECL CaptureRenderState(
    void * context, const FrontierX::RenderStateSnapshot & state)
{
    static_cast<Capture *>(context)->render = state;
    return GR_OK;
}

GENRESULT DACOM_CDECL CaptureTextureStage(
    void * context, const FrontierX::TextureStageSnapshot & state)
{
    static_cast<Capture *>(context)->stage = state;
    return GR_OK;
}

GENRESULT DACOM_CDECL CaptureTexture(void * context, U32 stage, U32 handle)
{
    if (stage != 0U || handle != 42U) return GR_NOT_IMPLEMENTED;
    static_cast<Capture *>(context)->texture = handle;
    return GR_OK;
}

class MaterialServices final : public IDAComponent
{
public:
    MaterialServices(IRenderPrimitive * primitive, IRenderPipeline * pipeline,
                     IVertexBufferManager * manager, ITextureLibrary * textures) :
        primitive_(primitive), pipeline_(pipeline), manager_(manager), textures_(textures) {}

    void SetTextureLibrary(ITextureLibrary * textures) { textures_ = textures; }

    GENRESULT COMAPI QueryInterface(const C8 * name, void ** output) override
    {
        if (!name || !output) return GR_INVALID_PARMS;
        *output = nullptr;
        if (std::strcmp(name, IID_IRenderPrimitive) == 0)
            *output = primitive_;
        else if (std::strcmp(name, IID_IRenderPipeline) == 0)
            *output = pipeline_;
        else if (std::strcmp(name, IID_IVertexBufferManager) == 0)
            *output = manager_;
        else if (std::strcmp(name, IID_ITextureLibrary) == 0)
            *output = textures_;
        else
            return GR_INTERFACE_UNSUPPORTED;
        if (!*output) return GR_INTERFACE_UNSUPPORTED;
        static_cast<IDAComponent *>(*output)->AddRef();
        return GR_OK;
    }
    U32 COMAPI AddRef() override { return ++references_; }
    U32 COMAPI Release() override { return --references_; }

private:
    IRenderPrimitive * primitive_;
    IRenderPipeline * pipeline_;
    IVertexBufferManager * manager_;
    ITextureLibrary * textures_;
    U32 references_ = 1U;
};

bool Expect(bool condition, const char * message)
{
    if (!condition) std::cerr << "state material probe failed: " << message << '\n';
    return condition;
}

using FrontierXTest::WriteMaterialFixture;

}

extern "C"
{
DA_ERROR_HANDLER FDUMP = IgnoreDiagnostic;
}

extern bool Register_MaterialLibrary(void);
extern bool Register_DcDtOcMaterial(void);
extern bool RegisterNativeTextureLibrary(ICOManager * manager);

int main(int argc, char ** argv)
{
    if (!Expect(argc == 2 && WriteMaterialFixture(argv[1]),
                "repository material UTF fixture creation")) return 1;
    std::vector<FrontierX::UtfDirectoryEntry> listed{{"retained", false}};
    std::string listError;
    if (!Expect(!FrontierX::ListUtfDirectory(argv[1], "material library", listed,
                    listError) && listed.size() == 1U &&
                    listed[0].name == "retained" &&
                    FrontierX::ListUtfDirectory(argv[1], "Material library",
                        listed, listError) && listed.size() == 2U,
                "UTF directory listing commits only an exact-case valid tree")) return 1;
    ICOManager * components = DACOM_Acquire();
    if (!Expect(components && FrontierX::RegisterNativeStateMaterial(components) &&
                    Register_MaterialLibrary() && Register_DcDtOcMaterial() &&
                    RegisterNativeTextureLibrary(components),
                "production material registration")) return 1;
    RegisterTheVertexBufferManager(components);
    FrontierX::RegisterNativeRenderPrimitive(components);

    FrontierX::NativeRenderPipeline vertexPipeline(nullptr);
    Capture capture;
    FrontierX::NativeRenderPrimitiveDesc primitiveDescriptor(
        CaptureDraw, &capture, nullptr, CaptureRenderState,
        FrontierX::NativeRenderPipeline::OwnedVertexBufferLookup,
        &vertexPipeline, CaptureTextureStage, CaptureTexture);
    void * rawPrimitive = nullptr;
    if (!Expect(components->CreateInstance(&primitiveDescriptor, &rawPrimitive) == GR_OK &&
                    rawPrimitive, "native primitive creation")) return 1;
    auto * primitive = static_cast<IRenderPrimitive *>(rawPrimitive);
    TextureCapture textureCapture;
    FrontierX::NativeTextureOperations textureOperations{};
    textureOperations.context = &textureCapture;
    textureOperations.create = CreateTexture;
    textureOperations.upload = UploadTexture;
    textureOperations.destroy = DestroyTexture;
    FrontierX::NativeRenderPipeline pipeline(
        FrontierX::NativeRenderPipeline::OwnedVertexBufferLookup,
        &vertexPipeline, primitive, textureOperations);

    AGGDESC managerDescriptor("VertexBufferManager");
    managerDescriptor.outer = &vertexPipeline;
    IDAComponent * managerComponent = nullptr;
    managerDescriptor.inner = &managerComponent;
    void * rawManager = nullptr;
    if (!Expect(components->CreateInstance(&managerDescriptor, &rawManager) == GR_OK &&
                    rawManager && managerComponent,
                "production vertex manager creation")) return 1;
    IVertexBufferManager * manager = nullptr;
    if (!Expect(managerComponent->QueryInterface(IID_IVertexBufferManager,
                    reinterpret_cast<void **>(&manager)) == GR_OK && manager &&
                    manager->initialize(nullptr) == GR_OK,
                "production vertex manager initialization")) return 1;

    MaterialServices services(primitive, &pipeline, manager, nullptr);
    AGGDESC textureDescriptor("TextureLibrary");
    textureDescriptor.outer = &services;
    IDAComponent * textureInner = nullptr;
    textureDescriptor.inner = &textureInner;
    void * rawTextures = nullptr;
    if (!Expect(components->CreateInstance(&textureDescriptor, &rawTextures) == GR_OK &&
                    rawTextures && textureInner,
                "production texture library creation")) return 1;
    IAggregateComponent * textureAggregate = nullptr;
    ITextureLibrary * textures = nullptr;
    if (!Expect(textureInner->QueryInterface(IID_IAggregateComponent,
                    reinterpret_cast<void **>(&textureAggregate)) == GR_OK &&
                    textureAggregate && textureAggregate->Initialize() == GR_OK &&
                    textureInner->QueryInterface(IID_ITextureLibrary,
                    reinterpret_cast<void **>(&textures)) == GR_OK && textures,
                "production texture library initialization")) return 1;
    services.SetTextureLibrary(textures);
    U32 textureLoadMode = ITL_STATE_MAX;
    U32 textureLodLoad = 0U;
    ITL_TEXTURE_ID syntheticTexture = ITL_INVALID_ID;
    U32 textureCount = 0U;
    if (!Expect(textures->get_library_state(ITL_STATE_TEXTURE_LOAD_MODE,
                    &textureLoadMode) == GR_OK &&
                    textureLoadMode == ITL_LM_USE_FIRST &&
                    textures->get_library_state(ITL_STATE_TEXTURE_LOD_LOAD,
                        &textureLodLoad) == GR_OK && textureLodLoad == 1U &&
                    textures->get_texture_id("synthetic", &syntheticTexture) == GR_OK &&
                    syntheticTexture != ITL_INVALID_ID &&
                    textures->has_texture_id("synthetic") == GR_OK &&
                    textures->get_texture_count(&textureCount) == GR_OK &&
                    textureCount == 1U &&
                    textures->release_texture_id(syntheticTexture) == GR_OK &&
                    textures->has_texture_id("synthetic") != GR_OK &&
                    textures->get_texture_count(&textureCount) == GR_OK &&
                    textureCount == 0U,
                "production texture library default state and reference lifetime"))
        return 1;
    AGGDESC libraryDescriptor("MaterialLibrary");
    libraryDescriptor.outer = &services;
    IDAComponent * libraryInner = nullptr;
    libraryDescriptor.inner = &libraryInner;
    void * rawLibrary = nullptr;
    if (!Expect(components->CreateInstance(&libraryDescriptor, &rawLibrary) == GR_OK &&
                    rawLibrary && libraryInner, "production material library creation")) return 1;
    IAggregateComponent * libraryAggregate = nullptr;
    IMaterialLibrary * library = nullptr;
    if (!Expect(libraryInner->QueryInterface(IID_IAggregateComponent,
                    reinterpret_cast<void **>(&libraryAggregate)) == GR_OK &&
                    libraryAggregate && libraryAggregate->Initialize() == GR_OK &&
                    libraryInner->QueryInterface(IID_IMaterialLibrary,
                    reinterpret_cast<void **>(&library)) == GR_OK && library,
                "production material library initialization")) return 1;
    IFileSystem * fixture = nullptr;
    std::string fixtureError;
    if (!Expect(FrontierX::OpenNativeUtfFileSystem(argv[1], &fixture,
                    fixtureError) && fixture, "native material UTF opening")) return 1;
    std::vector<FrontierX::UtfDirectoryEntry> textureEntries;
    if (!Expect(FrontierX::ListUtfDirectory(argv[1],
                    "Texture library/Tile/True RGB 565", textureEntries,
                    fixtureError) && textureEntries.size() == 1U &&
                    textureEntries[0].name == "MIP0",
                "generated UTF texture directory reaches MIP0")) return 1;
    if (!Expect(textures->load_library(fixture, nullptr) == GR_OK &&
                    textures->has_texture_id("Tile") == GR_OK &&
                    textureCapture.creates == 1U && textureCapture.uploads == 1U &&
                    textureCapture.pixels ==
                        std::array<std::uint8_t, 16>{{255, 0, 0, 255, 0, 255, 0, 255,
                                                      0, 0, 255, 255, 255, 255, 255, 255}},
                "production UTF RGB565 texture reaches native RGBA8 upload")) return 1;
    ITL_TEXTURE_ID tile = ITL_INVALID_ID;
    ITL_TEXTUREFRAME_IRP tileFrame{};
    if (!Expect(textures->get_texture_id("Tile", &tile) == GR_OK &&
                    textures->get_texture_frame(tile, ITL_FRAME_FIRST, &tileFrame) == GR_OK &&
                    tileFrame.rp_texture_id == 64U,
                "loaded production texture exposes its native frame")) return 1;
    std::string meshError;
    if (!FrontierXTest::RunPolyMeshFixture(std::string(argv[1]) + ".mesh",
                                          textures, meshError))
    {
        std::cerr << "state material probe failed: " << meshError << '\n';
        return 1;
    }
    WIN32_FIND_DATA found{};
    HANDLE search = fixture->FindFirstFile("*.*", &found);
    if (!Expect(search != INVALID_HANDLE_VALUE &&
                    std::strcmp(found.cFileName, "Material library") == 0 &&
                    (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                    fixture->FindNextFile(search, &found) == TRUE &&
                    std::strcmp(found.cFileName, "Texture library") == 0 &&
                    fixture->FindNextFile(search, &found) == FALSE &&
                    fixture->GetLastError() == ERROR_NO_MORE_FILES &&
                    fixture->FindClose(search) == TRUE &&
                    fixture->SetCurrentDirectory("material library") == TRUE &&
                    fixture->SetCurrentDirectory("\\") == TRUE &&
                    fixture->SetCurrentDirectory("Material library") == TRUE,
                "native UTF enumeration and Windows-style case-insensitive navigation")) return 1;
    search = fixture->FindFirstFile("*.*", &found);
    if (!Expect(search != INVALID_HANDLE_VALUE &&
                    std::strcmp(found.cFileName, "Loaded") == 0 &&
                    fixture->FindNextFile(search, &found) == TRUE &&
                    std::strcmp(found.cFileName, "Other") == 0 &&
                    fixture->FindNextFile(search, &found) == FALSE &&
                    fixture->GetLastError() == ERROR_NO_MORE_FILES &&
                    fixture->FindClose(search) == TRUE &&
                    fixture->FindClose(search) == FALSE &&
                    fixture->SetCurrentDirectory("..") == TRUE,
                "native UTF sibling enumeration and search close")) return 1;
    const HANDLE closedSearch = search;
    search = fixture->FindFirstFile("*.*", &found);
    if (!Expect(search != INVALID_HANDLE_VALUE && search != closedSearch &&
                    fixture->FindNextFile(closedSearch, &found) == FALSE &&
                    fixture->FindClose(search) == TRUE,
                "native UTF reused slot does not revive a closed search")) return 1;
    // Mission::LoadParseData uses a root-absolute Win32 UTF child path.
    DAFILEDESC absoluteType("\\Material library\\Other\\Type");
    const HANDLE absoluteHandle = fixture->OpenChild(&absoluteType);
    char absoluteText[7]{};
    DWORD absoluteRead = 0;
    DAFILEDESC escapeType("\\Material library\\..\\Other\\Type");
    if (!Expect(absoluteHandle != INVALID_HANDLE_VALUE &&
                    fixture->ReadFile(absoluteHandle, absoluteText, 6U,
                        &absoluteRead, nullptr) && absoluteRead == 6U &&
                    std::strcmp(absoluteText, "DcDtOc") == 0 &&
                    fixture->CloseHandle(absoluteHandle) &&
                    fixture->OpenChild(&escapeType) == INVALID_HANDLE_VALUE &&
                    fixture->SetCurrentDirectory("\\Material library\\Other") &&
                    fixture->SetCurrentDirectory("\\"),
                "root-absolute UTF mission paths preserve case and reject traversal")) return 1;
    if (!Expect(fixture->SetCurrentDirectory("Material library/Other") &&
                    (search = fixture->FindFirstFile("*.*", &found)) != INVALID_HANDLE_VALUE &&
                    std::strcmp(found.cFileName, "Type") == 0 &&
                    found.nFileSizeLow == 6U && found.nFileSizeHigh == 0U &&
                    fixture->FindClose(search) &&
                    fixture->FindFirstFile("missing.*", &found) == INVALID_HANDLE_VALUE &&
                    fixture->GetLastError() == ERROR_FILE_NOT_FOUND &&
                    fixture->SetCurrentDirectory("\\"),
                "native UTF enumeration supplies file size for GenData traversal")) return 1;
    DAFILEDESC childDescriptor("Material library");
    IFileSystem * materialDirectory = nullptr;
    if (!Expect(fixture->CreateInstance(&childDescriptor,
                    reinterpret_cast<void **>(&materialDirectory)) == GR_OK &&
                    materialDirectory &&
                    fixture->GetCurrentDirectory(0U, nullptr) == 2U,
                "UTF factory opens a child directory without moving its parent")) return 1;
    DAFILEDESC nestedDescriptor("Other");
    IFileSystem * otherDirectory = nullptr;
    if (!Expect(materialDirectory->CreateInstance(&nestedDescriptor,
                    reinterpret_cast<void **>(&otherDirectory)) == GR_OK &&
                    otherDirectory,
                "UTF factory opens a nested material directory")) return 1;
    DAFILEDESC typeDescriptor("Type");
    const HANDLE typeHandle = otherDirectory->OpenChild(&typeDescriptor);
    char nestedType[IM_MAX_TYPE_LEN]{};
    DWORD typeRead = 0;
    if (!Expect(typeHandle != INVALID_HANDLE_VALUE &&
                    otherDirectory->ReadFile(typeHandle, nestedType, 6U,
                        &typeRead, nullptr) == TRUE && typeRead == 6U &&
                    std::strcmp(nestedType, "DcDtOc") == 0 &&
                    otherDirectory->CloseHandle(typeHandle) == TRUE,
                "nested UTF view reads its direct child")) return 1;
    DAFILEDESC wrongCase("other");
    DAFILEDESC traversal("../Other");
    IFileSystem * rejected = nullptr;
    const bool caseResolved = materialDirectory->CreateInstance(&wrongCase,
                    reinterpret_cast<void **>(&rejected)) == GR_OK && rejected;
    if (rejected) rejected->Release();
    rejected = nullptr;
    if (!Expect(caseResolved && materialDirectory->CreateInstance(&traversal,
                    reinterpret_cast<void **>(&rejected)) == GR_INVALID_PARMS &&
                    !rejected,
                "UTF child factory resolves Windows case and rejects traversal")) return 1;
    IFileSystem * typeView = nullptr;
    char implicitType[IM_MAX_TYPE_LEN]{};
    DWORD implicitRead = 0;
    if (!Expect(otherDirectory->CreateInstance(&typeDescriptor,
                    reinterpret_cast<void **>(&typeView)) == GR_OK && typeView &&
                    typeView->GetFileSize(nullptr, nullptr) == 6U &&
                    typeView->ReadFile(nullptr, implicitType, 6U,
                        &implicitRead, nullptr) == TRUE && implicitRead == 6U &&
                    std::strcmp(implicitType, "DcDtOc") == 0,
                "UTF file view serves an implicit production file stream")) return 1;
    const HANDLE typeMapping = typeView->CreateFileMapping();
    const void *mappedType = typeView->MapViewOfFile(typeMapping);
    if (!Expect(typeMapping != INVALID_HANDLE_VALUE && mappedType &&
                    std::memcmp(mappedType, "DcDtOc", 6U) == 0 &&
                    typeView->MapViewOfFile(typeMapping, FILE_MAP_READ, 0, 1, 0) == nullptr &&
                    typeView->MapViewOfFile(typeMapping, FILE_MAP_READ, 0, 0, 7) == nullptr &&
                    typeView->UnmapViewOfFile(mappedType) == TRUE &&
                    typeView->UnmapViewOfFile(mappedType) == FALSE &&
                    typeView->CloseHandle(typeMapping) == TRUE &&
                    typeView->MapViewOfFile(typeMapping) == nullptr,
                "UTF file view supplies the texture-manager read-only mapping contract")) return 1;
    if (!Expect(typeView->CreateFileMapping(nullptr, nullptr, PAGE_READWRITE) ==
                    INVALID_HANDLE_VALUE,
                "UTF file view rejects a writable texture mapping")) return 1;
    typeView->Release();
    otherDirectory->Release();
    materialDirectory->Release();
    if (!Expect(library->load_library(fixture) == GR_OK &&
                    library->load_library(fixture) == GR_OK &&
                    library->find_material("Loaded", nullptr) == GR_OK &&
                    library->find_material("Other", nullptr) == GR_OK,
                "production library loads a material class from UTF")) return 1;
    U32 loadedCount = 0U;
    if (!Expect(library->get_material_count(&loadedCount) == GR_OK &&
                    loadedCount == 2U,
                "repeated material-library load keeps one owner per name")) return 1;
    IMaterial * simple = nullptr;
    IMaterialProperties * properties = nullptr;
    char simpleType[IM_MAX_TYPE_LEN]{};
    float diffuse[3]{};
    float opacity[1]{};
    if (!Expect(library->find_material("Other", &simple) == GR_OK && simple &&
                    simple->QueryInterface(IID_IMaterialProperties,
                        reinterpret_cast<void **>(&properties)) == GR_OK &&
                    properties && simple->get_type(simpleType,
                        sizeof(simpleType)) == GR_OK &&
                    std::strcmp(simpleType, "DcDtOc") == 0 &&
                    properties->get_constant(0U, 3U, diffuse) == GR_OK &&
                    diffuse[0] == 0.25F && diffuse[1] == 0.5F &&
                    diffuse[2] == 0.75F &&
                    properties->get_constant(1U, 1U, opacity) == GR_OK &&
                    opacity[0] == 0.5F &&
                    simple->apply() == GR_NOT_IMPLEMENTED,
                "production SimpleMaterial loads diffuse and opacity fields")) return 1;
    ITL_TEXTURE_REF_ID materialTexture = ITL_INVALID_REF_ID;
    ITL_TEXTUREFRAME_IRP materialFrame{};
    if (!Expect(simple->verify(1U, 1.0F) == GR_OK &&
                    simple->update(0.0F) == GR_OK &&
                    properties->get_texture(0U, &materialTexture) == GR_OK &&
                    materialTexture != ITL_INVALID_REF_ID &&
                    textures->get_texture_ref_frame(materialTexture,
                        ITL_FRAME_CURRENT, &materialFrame) == GR_OK &&
                    materialFrame.rp_texture_id == 64U &&
                    textures->release_texture_ref(materialTexture) == GR_OK &&
                    textures->release_texture_id(tile) == GR_OK,
                "production SimpleMaterial resolves its named UTF texture frame")) return 1;
    const std::filesystem::path invalidPath = std::string(argv[1]) + ".invalid";
    IFileSystem * invalidFixture = nullptr;
    if (!Expect(WriteMaterialFixture(invalidPath, "Unknown") &&
                    FrontierX::OpenNativeUtfFileSystem(invalidPath, &invalidFixture,
                        fixtureError) && invalidFixture &&
                    invalidFixture->SetCurrentDirectory("Material library") == TRUE &&
                    invalidFixture->SetCurrentDirectory("Other") == TRUE &&
                    simple->load_from_filesystem(invalidFixture) == GR_INVALID_PARMS &&
                    properties->get_constant(0U, 3U, diffuse) == GR_OK &&
                    diffuse[0] == 0.25F,
                "unsupported material type does not replace loaded fields")) return 1;
    invalidFixture->Release();
    properties->Release();
    fixture->Release();
    char smallClassId[4] = {};
    if (!Expect(library->add_material_map("Fixture", "StateMaterial") == GR_OK &&
                    library->find_material_map("Fixture", sizeof(smallClassId),
                        smallClassId) == GR_INVALID_PARMS &&
                    library->find_material_map("Fixture", 0U, smallClassId) ==
                        GR_INVALID_PARMS,
                "material class mapping rejects undersized output")) return 1;
    IMaterial * material = nullptr;
    if (!Expect(library->create_material("probe", "Fixture", &material) == GR_OK &&
                    material && library->find_material("PROBE", nullptr) == GR_OK,
                "production material library creates and indexes StateMaterial")) return 1;
    IStateMaterial * states = nullptr;
    if (!Expect(material->QueryInterface(IID_IStateMaterial,
                    reinterpret_cast<void **>(&states)) == GR_OK && states,
                "material interfaces and library initialization")) return 1;
    const std::string longName(IM_MAX_NAME_LEN, 'x');
    char materialName[IM_MAX_NAME_LEN]{};
    if (!Expect(material->set_name("probe") == GR_OK &&
                    material->set_name(longName.c_str()) == GR_INVALID_PARMS &&
                    material->get_name(materialName, 5U) == GR_INVALID_PARMS &&
                    material->get_name(materialName, sizeof(materialName)) == GR_OK &&
                    std::strcmp(materialName, "probe") == 0 &&
                    material->get_type(materialName, 0U) == GR_INVALID_PARMS,
                "material name and type buffers remain bounded")) return 1;

    if (!Expect(states->set_render_state(D3DRS_ZENABLE, 0U) == GR_OK &&
                    states->set_texture_stage_state(0U, D3DTSS_COLOROP,
                        D3DTOP_SELECTARG1) == GR_OK &&
                    states->set_texture_stage_texture(0U, 42U) == GR_OK &&
                    material->apply() == GR_OK && capture.render.depthEnabled == 0U &&
                    capture.stage.colorOperation == D3DTOP_SELECTARG1 &&
                    capture.texture == 42U,
                "production material applies saved depth, combine and texture states"))
        return 1;
    if (!Expect(states->set_texture_stage_texture(0U, 43U) == GR_OK &&
                    material->apply() == GR_NOT_IMPLEMENTED &&
                    capture.texture == 42U &&
                    states->set_texture_stage_texture(0U, 42U) == GR_OK &&
                    material->apply() == GR_OK,
                "material reports rejected backend texture and recovers")) return 1;

    constexpr U32 format = FrontierX::LegacyD3d9::kRpVertexFvf;
    IRP_VERTEXBUFFERHANDLE buffer = 0U;
    if (!Expect(vertexPipeline.create_vertex_buffer(format, 3, 0U, &buffer) == GR_OK &&
                    buffer, "cached vertex buffer creation")) return 1;
    void * vertexData = nullptr;
    U32 vertexCount = 0U;
    const std::array<FrontierX::RenderVertexP3DiffuseT2, 3> vertices{{
        {-0.5F, -0.5F, 0.5F, 0xffffffffU, 0.0F, 1.0F},
        {0.5F, -0.5F, 0.5F, 0xffffffffU, 1.0F, 1.0F},
        {0.0F, 0.5F, 0.5F, 0xffffffffU, 0.5F, 0.0F},
    }};
    if (!Expect(vertexPipeline.lock_vertex_buffer(buffer, 0U, &vertexData,
                    &vertexCount) == GR_OK && vertexCount == vertices.size() &&
                    vertexData, "cached buffer lock")) return 1;
    std::memcpy(vertexData, vertices.data(), sizeof(vertices));
    if (!Expect(vertexPipeline.unlock_vertex_buffer(buffer) == GR_OK,
                "cached buffer unlock")) return 1;
    MaterialContext context{};
    context.vertex_buffer = buffer;
    U16 indices[3] = {0U, 1U, 2U};
    if (!Expect(material->render(&context, D3DPT_TRIANGLELIST, nullptr, 0U, 3U,
                    indices, 3U, 0U) == GR_OK && capture.draws == 1U,
                "production material indexed draw reaches native primitive")) return 1;
    indices[0] = 3U;
    if (!Expect(material->render(&context, D3DPT_TRIANGLELIST, nullptr, 0U, 3U,
                    indices, 3U, 0U) == GR_INVALID_PARMS && capture.draws == 1U &&
                    material->render(nullptr, D3DPT_TRIANGLELIST, nullptr, 0U, 3U,
                        indices, 3U, 0U) == GR_INVALID_PARMS,
                "material propagates rejected draw and null context")) return 1;
    indices[0] = 0U;
    if (!Expect(simple->render(&context, D3DPT_TRIANGLELIST, nullptr, 0U, 3U,
                    indices, 3U, 0U) == GR_OK && capture.draws == 2U,
                "library-loaded SimpleMaterial submits cached indexed draw")) return 1;
    indices[0] = 3U;
    MaterialContext uncached{};
    if (!Expect(simple->render(&context, D3DPT_TRIANGLELIST, nullptr, 0U, 3U,
                    indices, 3U, 0U) == GR_INVALID_PARMS &&
                    simple->render(nullptr, D3DPT_TRIANGLELIST, nullptr, 0U, 3U,
                        indices, 3U, 0U) == GR_INVALID_PARMS &&
                    simple->render(&uncached, D3DPT_TRIANGLELIST, nullptr, 0U, 3U,
                        indices, 3U, 0U) == GR_INVALID_PARMS && capture.draws == 2U,
                "SimpleMaterial propagates failed draw and null input")) return 1;

    if (!Expect(vertexPipeline.destroy_vertex_buffer(buffer) == GR_OK &&
                    manager->cleanup() == GR_OK,
                "material draw resources destroyed")) return 1;
    simple->Release();
    states->Release();
    material->Release();
    U32 materialCount = 1U;
    if (!Expect(library->free_library() == GR_OK &&
                    library->get_material_count(&materialCount) == GR_OK &&
                    materialCount == 0U, "material library releases indexed owner")) return 1;
    if (!Expect(textures->free_library(TRUE) == GR_OK &&
                    textureCapture.destroys == 1U,
                "loaded UTF texture releases its native owner")) return 1;
    textureCapture.failUpload = true;
    IFileSystem * rejectedTextureFixture = nullptr;
    if (!Expect(FrontierX::OpenNativeUtfFileSystem(argv[1],
                    &rejectedTextureFixture, fixtureError) &&
                    rejectedTextureFixture &&
                    textures->load_library(rejectedTextureFixture, nullptr) == GR_OK &&
                    textures->has_texture_id("Tile") != GR_OK &&
                    textureCapture.creates == 2U &&
                    textureCapture.uploads == 2U &&
                    textureCapture.destroys == 2U,
                "failed production upload rolls back the native texture")) return 1;
    rejectedTextureFixture->Release();
    const std::filesystem::path missingMipPath =
        std::string(argv[1]) + ".missing_mip";
    IFileSystem * missingMipFixture = nullptr;
    textureCapture.failUpload = false;
    if (!Expect(WriteMaterialFixture(missingMipPath, "DcDtOc", "MIP1") &&
                    FrontierX::OpenNativeUtfFileSystem(missingMipPath,
                        &missingMipFixture, fixtureError) && missingMipFixture &&
                    textures->load_library(missingMipFixture, nullptr) == GR_OK &&
                    textures->has_texture_id("Tile") != GR_OK &&
                    textureCapture.creates == 2U,
                "missing MIP0 terminates without publishing a texture")) return 1;
    missingMipFixture->Release();
    library->Release();
    libraryAggregate->Release();
    libraryInner->Release();
    textures->Release();
    textureAggregate->Release();
    textureInner->Release();
    manager->Release();
    managerComponent->Release();
    primitive->Release();
    std::cout << "production state material draw passed\n";
    return 0;
}
