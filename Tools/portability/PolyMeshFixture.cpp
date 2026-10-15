#include "PolyMeshFixture.h"

#include "FrontierX/NativeUtfFileSystem.h"
#include "ITextureLibrary.h"
#include "xmesh.h"
#include "TextureCoord.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <initializer_list>
#include <iterator>
#include <utility>
#include <vector>

namespace FrontierXTest
{
namespace
{
struct Entry
{
    std::string name;
    std::vector<unsigned char> data;
    std::vector<U32> children;
    bool directory;
};

struct CountingMaterialManager : IMaterialManager
{
    unsigned lookups = 0;
    std::string lastName;

    GENRESULT COMAPI QueryInterface(const C8 *, void **) override { return GR_GENERIC; }
    U32 COMAPI AddRef() override { return 1U; }
    U32 COMAPI Release() override { return 1U; }
    GENRESULT COMAPI Initialize(InitInfo &) override { return GR_GENERIC; }
    GENRESULT COMAPI OpenEditWindow(IMaterialCallback *) override { return GR_GENERIC; }
    GENRESULT COMAPI IsEditWindowOpen() override { return GR_GENERIC; }
    GENRESULT COMAPI FindFirstMaterial(IMaterial **) override { return GR_GENERIC; }
    GENRESULT COMAPI FindNextMaterial(IMaterial *, IMaterial **) override { return GR_GENERIC; }
    IMaterial * FindMaterial(const char * name) override
    {
        ++lookups;
        lastName = name ? name : "";
        return nullptr;
    }
};

void Word(std::vector<unsigned char> & bytes, U32 value)
{
    for (unsigned shift = 0; shift != 32; shift += 8)
        bytes.push_back(static_cast<unsigned char>(value >> shift));
}

void Float(std::vector<unsigned char> & bytes, float value)
{
    U32 bits = 0;
    static_assert(sizeof(bits) == sizeof(value));
    std::memcpy(&bits, &value, sizeof(bits));
    Word(bytes, bits);
}

void Vector3(std::vector<unsigned char> & bytes, float x, float y, float z)
{
    Float(bytes, x);
    Float(bytes, y);
    Float(bytes, z);
}

bool WritePolyMeshFixture(const std::filesystem::path & path,
                          const std::string & textureName,
                          bool cq2 = false, U32 materialCount = 1U,
                          const char * cq2MaterialName = "Base",
                          bool wrapForComponent = false,
                          bool embeddedTexture = false, bool threeSortFaces = false,
                          bool indexedTexture = false, bool secondUvSeam = false,
                          int compoundMode = -1, unsigned hardpointClaimMode = 0U)
{
    std::vector<Entry> entries;
    entries.push_back({"/", {}, {}, true});
    auto directory = [&](U32 parent, const char * name) {
        const U32 index = static_cast<U32>(entries.size());
        entries.push_back({name, {}, {}, true});
        entries[parent].children.push_back(index);
        return index;
    };
    auto file = [&](U32 parent, const char * name, std::vector<unsigned char> data) {
        const U32 index = static_cast<U32>(entries.size());
        entries.push_back({name, std::move(data), {}, false});
        entries[parent].children.push_back(index);
    };
    auto words = [](std::initializer_list<U32> values) {
        std::vector<unsigned char> bytes;
        for (U32 value : values) Word(bytes, value);
        return bytes;
    };
    if (hardpointClaimMode != 1U)
    {
    const U32 meshRoot = wrapForComponent ? directory(0U, "openFLAME 3D N-mesh") : 0U;
    const U32 groups = directory(meshRoot, "Face groups");
    file(groups, "Count", words({1U}));
    const U32 group = directory(groups, "Group0");
    file(group, "Material", words({0U}));
    file(group, "Face count", words({threeSortFaces ? 3U : 2U}));
    file(group, "Face vertex chain", threeSortFaces
        ? words({0U, 1U, 2U, 0U, 3U, 4U, 0U, 3U, 2U})
        : words({0U, 1U, 2U, secondUvSeam ? 5U : 0U, 3U, 4U}));
    file(group, "Face normal", threeSortFaces ? words({0U, 0U, 0U}) : words({0U, 0U}));
    file(group, "Face D-coefficient", threeSortFaces ? words({0U, 0U, 0U}) : words({0U, 0U}));
    file(group, "Face property", threeSortFaces ? words({4U, 4U, 4U}) : words({4U, 4U}));
    const U32 vertices = directory(meshRoot, "Vertices");
    file(vertices, "Vertex batch count", words({secondUvSeam ? 6U : 5U}));
    file(vertices, "Vertex batch list", secondUvSeam
        ? words({0U, 1U, 2U, 3U, 4U, 0U}) : words({0U, 1U, 2U, 3U, 4U}));
    file(vertices, "Object vertex count", words({5U}));
    std::vector<unsigned char> positions;
    Vector3(positions, 0.0F, 0.0F, 0.0F);
    Vector3(positions, 1.0F, 0.0F, 0.0F);
    Vector3(positions, 0.0F, 1.0F, 0.0F);
    Vector3(positions, 2.0F, 0.0F, 0.0F);
    Vector3(positions, 0.0F, 2.0F, 0.0F);
    file(vertices, "Object vertex list", std::move(positions));
    file(vertices, "Vertex normal", words({0U, 0U, 0U, 0U, 0U}));
    file(vertices, "Vertex D-coefficient", words({0U, 0U, 0U, 0U, 0U}));
    file(vertices, "Texture vertex count", words({5U}));
    std::vector<unsigned char> uv;
    for (const auto & coordinate : {std::pair{0.0F, 0.0F},
                                    std::pair{1.0F, 0.0F},
                                    std::pair{0.0F, 1.0F},
                                    std::pair{1.0F, 1.0F},
                                    std::pair{0.5F, 0.5F}})
    {
        Float(uv, coordinate.first);
        Float(uv, coordinate.second);
    }
    file(vertices, "Texture vertex list", std::move(uv));
    file(vertices, "Texture batch list", secondUvSeam
        ? words({0U, 1U, 2U, 3U, 4U, 0U}) : words({0U, 1U, 2U, 3U, 4U}));
    if (secondUvSeam)
        file(vertices, "Texture batch list2", words({0U, 1U, 2U, 3U, 4U, 3U}));
    const U32 normals = directory(meshRoot, "Normals");
    file(normals, "Surface normal count", words({1U}));
    std::vector<unsigned char> normal;
    Vector3(normal, 0.0F, 0.0F, 1.0F);
    file(normals, "Surface normal list", std::move(normal));
    const U32 materials = directory(meshRoot,
        cq2 ? "CQ2_Material_Library" : "Material library");
    file(materials, "Material count", words({materialCount}));
    const U32 base = directory(materials, cq2 ? cq2MaterialName : "Base");
    if (!cq2)
    {
        file(base, "Material identifier", words({0U}));
        std::vector<unsigned char> diffuse;
        Float(diffuse, 0.25F);
        Float(diffuse, 0.5F);
        Float(diffuse, 1.0F);
        if (secondUvSeam)
        {
            // The source selects secondary coordinates only for the nested
            // emission material format with TC_WRAP_UV_1. An old flat material
            // explicitly has no emissive map, even when a UV2 batch exists.
            directory(base, "Ambient");
            const U32 diffuseProperty = directory(base, "Diffuse");
            file(diffuseProperty, "Constant", std::move(diffuse));
            const U32 diffuseMap = directory(diffuseProperty, "Map");
            file(diffuseMap, "Name", std::vector<unsigned char>(textureName.begin(), textureName.end()));
            file(diffuseMap, "Flags", words({0U}));
            const U32 emissionProperty = directory(base, "Emission");
            const U32 emissionMap = directory(emissionProperty, "Map");
            file(emissionMap, "Name", std::vector<unsigned char>(textureName.begin(), textureName.end()));
            U32 uv2Flags = 0U;
            SET_TC_WRAP_MODE(uv2Flags, TC_WRAP_UV_1);
            file(emissionMap, "Flags", words({uv2Flags}));
        }
        else file(base, "Diffuse", std::move(diffuse));
        std::vector<unsigned char> transparency;
        Float(transparency, 0.5F);
        file(base, "Transparency", std::move(transparency));
        file(base, "Texture name", std::vector<unsigned char>(
            textureName.begin(), textureName.end()));
    }
    const U32 textureLibrary = directory(meshRoot, "Texture library");
    if (embeddedTexture)
    {
        const U32 tile = directory(textureLibrary, textureName.c_str());
        file(tile, "Image X size", words({2U}));
        file(tile, "Image Y size", words({2U}));
        const U32 rgb = directory(tile, indexedTexture ? "Palette 8 Bit" : "True RGB 565");
        const U32 mip = directory(rgb, "MIP0");
        // Distinct primaries make a sampled draw differ from white fallback.
        if (indexedTexture)
        {
            file(mip, "Image indices", {0, 1, 2, 3});
            file(rgb, "Palette color count", words({4U}));
            file(rgb, "Palette RGB 888", {255, 0, 0, 0, 255, 0, 0, 0, 255, 15, 31, 63});
        }
        else
            file(mip, "Image colors", std::vector<unsigned char>{
                0x00U, 0xf8U, 0xe0U, 0x07U, 0x1fU, 0x00U, 0xffU, 0xffU});
    }

    }
    if (hardpointClaimMode != 0U)
    {
        const U32 hardpoints = directory(0U, "Hardpoints");
        const U32 fixed = directory(hardpoints, "Fixed");
        const U32 locator = directory(fixed, "Locator");
        std::vector<unsigned char> position;
        Vector3(position, 0.25F, 0.5F, 0.75F);
        file(locator, "Position", std::move(position));
        std::vector<unsigned char> orientation;
        for (unsigned row = 0U; row < 3U; ++row)
            for (unsigned column = 0U; column < 3U; ++column)
                Float(orientation, row == column ? 1.0F : 0.0F);
        file(locator, "Orientation", std::move(orientation));
    }
    constexpr U32 headerSize = 56U;
    constexpr U32 recordSize = 44U;
    std::vector<unsigned char> names;
    if (compoundMode >= 0)
    {
        // A valid simple mesh remains present to expose forbidden fallback
        // after any required part of this declared compound fails.
        const auto partPath = std::filesystem::path(path.string() + ".part");
        if (!WritePolyMeshFixture(partPath, "", false, 1U, "Base", true)) return false;
        std::ifstream input(partPath, std::ios::binary);
        const std::vector<unsigned char> partBytes{std::istreambuf_iterator<char>(input),
                                                  std::istreambuf_iterator<char>()};
        if (partBytes.empty()) return false;
        auto rendererClaimPart = [&](bool declared) {
            const auto claimPath = std::filesystem::path(path.string() + ".claim-part");
            if (!WritePolyMeshFixture(claimPath, declared ? "RequiredMissingTile" : "",
                    false, 1U, "Base", true, false, false, false, false, -1, declared ? 2U : 1U))
                return std::vector<unsigned char>{};
            std::ifstream claimInput(claimPath, std::ios::binary);
            return std::vector<unsigned char>{std::istreambuf_iterator<char>(claimInput),
                                              std::istreambuf_iterator<char>()};
        };
        auto sharedPart = [&]() {
            const auto sharedPath = std::filesystem::path(path.string() + ".shared-part");
            if (!WritePolyMeshFixture(sharedPath, compoundMode == 7 ? "CompoundSharedTile" : "CompoundMissingTile",
                    false, 1U, "Base", true)) return std::vector<unsigned char>{};
            std::ifstream sharedInput(sharedPath, std::ios::binary);
            return std::vector<unsigned char>{std::istreambuf_iterator<char>(sharedInput),
                                              std::istreambuf_iterator<char>()};
        };
        const auto sharedBytes = compoundMode >= 7 ? sharedPart() : partBytes;
        const auto rootBytes = compoundMode >= 7 ? sharedBytes : compoundMode == 4 || compoundMode == 6
            ? rendererClaimPart(compoundMode == 4) : partBytes;
        const auto childBytes = compoundMode >= 7 ? sharedBytes : compoundMode == 5 ? rendererClaimPart(true) : partBytes;
        if (rootBytes.empty() || childBytes.empty()) return false;
        file(0U, "CompoundRoot.3db", rootBytes);
        file(0U, "CompoundChild.3db", childBytes);
        if (compoundMode >= 7)
        {
            const U32 sharedLibrary = directory(0U, "Texture library");
            if (compoundMode == 7)
            {
                const U32 tile = directory(sharedLibrary, "CompoundSharedTile");
                file(tile, "Image X size", words({2U}));
                file(tile, "Image Y size", words({2U}));
                const U32 format = directory(tile, "True RGB 565");
                const U32 mip = directory(format, "MIP0");
                file(mip, "Image colors", {0x00U,0xf8U, 0xe0U,0x07U, 0x1fU,0x00U, 0xffU,0xffU});
            }
        }
        const U32 compound = directory(0U, "Cmpnd");
        auto part = [&](const char *directoryName, const char *objectName,
                        const char *fileName, U32 index) {
            const U32 entry = directory(compound, directoryName);
            file(entry, "Object name", std::vector<unsigned char>(objectName, objectName + std::strlen(objectName)));
            file(entry, "File name", std::vector<unsigned char>(fileName, fileName + std::strlen(fileName)));
            file(entry, "Index", words({index}));
        };
        part("Root", "Root", compoundMode == 1 ? "MissingRoot.3db" : "CompoundRoot.3db", 0U);
        part("Part0", "Child", compoundMode == 2 ? "MissingChild.3db" : "CompoundChild.3db", 1U);
        if (compoundMode == 3) part("Part1", "Missing", "MissingLast.3db", 2U);
        const U32 connections = directory(compound, "Cons");
        // Original Fix record: two 64-byte names, float3 position, float3x3.
        std::vector<unsigned char> joint(128U, 0U);
        std::memcpy(joint.data(), "Root", 4U);
        std::memcpy(joint.data() + 64U, "Child", 5U);
        Vector3(joint, 0.0F, 0.0F, 0.0F);
        for (unsigned row = 0U; row < 3U; ++row)
            for (unsigned column = 0U; column < 3U; ++column)
                Float(joint, row == column ? 1.0F : 0.0F);
        file(connections, "Fix", std::move(joint));
    }
    const U32 recordsSize = static_cast<U32>(entries.size()) * recordSize;
    std::vector<U32> nameOffsets;
    for (const Entry & entry : entries)
    {
        nameOffsets.push_back(static_cast<U32>(names.size()));
        names.insert(names.end(), entry.name.begin(), entry.name.end());
        names.push_back(0);
    }
    std::vector<unsigned char> data;
    std::vector<U32> dataOffsets(entries.size(), 0U);
    for (U32 index = 0; index < entries.size(); ++index)
    {
        if (!entries[index].directory)
        {
            dataOffsets[index] = static_cast<U32>(data.size());
            data.insert(data.end(), entries[index].data.begin(), entries[index].data.end());
        }
    }
    std::vector<U32> next(entries.size(), 0U);
    for (const Entry & entry : entries)
        for (std::size_t child = 1; child < entry.children.size(); ++child)
            next[entry.children[child - 1U]] = entry.children[child] * recordSize;
    std::vector<unsigned char> bytes;
    for (U32 value : {0x20465455U, 0x101U, headerSize, recordsSize,
                      0U, recordSize, headerSize + recordsSize,
                      static_cast<U32>(names.size()), static_cast<U32>(names.size()),
                      headerSize + recordsSize + static_cast<U32>(names.size()),
                      0U, 0U, 0U, 0U}) Word(bytes, value);
    for (U32 index = 0; index < entries.size(); ++index)
    {
        const Entry & entry = entries[index];
        const U32 child = entry.directory ?
            (entry.children.empty() ? 0U : entry.children[0] * recordSize) :
            dataOffsets[index];
        const U32 size = static_cast<U32>(entry.data.size());
        for (U32 value : {next[index], nameOffsets[index],
                          entry.directory ? 0x10U : 0U, 0U, child, size,
                          size, size, 0U, 0U, 0U}) Word(bytes, value);
    }
    bytes.insert(bytes.end(), names.begin(), names.end());
    bytes.insert(bytes.end(), data.begin(), data.end());
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    output.write(reinterpret_cast<const char *>(bytes.data()),
                 static_cast<std::streamsize>(bytes.size()));
    return output.good();
}
}

bool WriteGameMeshFixture(const std::filesystem::path & path)
{
    return WritePolyMeshFixture(path, "", true, 1U, "Base.txt", true);
}

bool WriteVertexColorGameMeshFixture(const std::filesystem::path & path)
{
    return WritePolyMeshFixture(path, "", true, 1U,
                                "VertexColor.txt", true);
}

bool WriteLegacyGameMeshFixture(const std::filesystem::path & path,
                                bool textured, bool indexed)
{
    return WritePolyMeshFixture(path, textured ? (indexed ? "Tile8" : "Tile") : "", false,
                                1U, "Base", true, textured, false, indexed);
}

bool WriteAnim2DSharedTextureFixture(const std::filesystem::path &path)
{
    return WritePolyMeshFixture(path, "AnimSharedTile", false, 1U, "Base", false, true);
}

bool WriteLegacySeamGameMeshFixture(const std::filesystem::path &path, bool textured)
{
    return WritePolyMeshFixture(path, textured ? "Tile" : "", false,
                               1U, "Base", true, textured, false, false, true);
}

bool WriteRequiredMaterialFailureFixture(const std::filesystem::path &path, unsigned mode)
{
    // Protect original XMesh/material callers against unresolved named assets
    // and a material table that ends before its declared persistent count.
    if (mode > 2U) return false;
    return WritePolyMeshFixture(path, mode == 2U ? "" : "RequiredMissingTile", false,
                               mode == 2U ? 2U : 1U, "Base", true, false,
                               false, false, mode == 1U);
}

bool WriteCompoundGameMeshFixture(const std::filesystem::path &path, unsigned mode)
{
    return mode <= 8U && WritePolyMeshFixture(path, "", false, 1U, "Base", true,
        false, false, false, false, static_cast<int>(mode));
}

bool WriteRendererClaimGameMeshFixture(const std::filesystem::path &path, bool declaredMesh)
{
    return WritePolyMeshFixture(path, declaredMesh ? "RequiredMissingTile" : "", false,
        1U, "Base", true, false, false, false, false, -1, declaredMesh ? 2U : 1U);
}

bool RunPolyMeshFixture(const std::filesystem::path & path,
                        ITextureLibrary * textures, std::string & error)
{
    if (!textures || !WritePolyMeshFixture(path, "Tile"))
    {
        error = "production mesh UTF fixture creation";
        return false;
    }
    IFileSystem * archive = nullptr;
    if (!FrontierX::OpenNativeUtfFileSystem(path, &archive, error) || !archive)
        return false;
    bool loaded = false;
    {
        XMesh mesh;
        loaded = mesh.read(archive, textures, nullptr, 0U) &&
            mesh.object_vertex_cnt == 5 && mesh.vertex_batch_cnt == 5 &&
            mesh.face_group_cnt == 1 && mesh.face_cnt == 2 &&
            mesh.sorted_faces == 1U && mesh.face_groups &&
            mesh.face_groups[0].face_cnt == 2 &&
            mesh.face_groups[0].material == 0 &&
            mesh.material_cnt == 1 && mesh.material_list &&
            mesh.material_list[0].name &&
            std::strcmp(mesh.material_list[0].name, "Base") == 0 &&
            mesh.material_list[0].diffuse.r == 63U &&
            mesh.material_list[0].diffuse.g == 127U &&
            mesh.material_list[0].diffuse.b == 255U &&
            mesh.material_list[0].transparency == 127U &&
            mesh.material_list[0].diffuse_texture_ref != ITL_INVALID_REF_ID &&
            mesh.material_list[0].texture_id == 64U &&
            mesh.face_groups[0].face_area[0] == 2.0F &&
            mesh.face_groups[0].face_area[1] == 0.5F &&
            mesh.face_groups[0].face_vertex_chain[0] == 0 &&
            mesh.face_groups[0].face_vertex_chain[1] == 3 &&
            mesh.face_groups[0].face_vertex_chain[2] == 4 &&
            mesh.centroid.x == 0.6F && mesh.centroid.y == 0.6F &&
            mesh.radius > 0.0F;
    }
    archive->Release();
    archive = nullptr;
    if (!loaded)
    {
        error = "production XMesh UTF load and face-area reorder";
        return false;
    }
    // Two faces never multiply rand() by more than one. Three faces expose
    // Linux's larger RAND_MAX while also verifying the original draw count.
    const std::filesystem::path sortPath = path.string() + ".sort";
    if (!WritePolyMeshFixture(sortPath, "Tile", false, 1U, "Base", false, false, true) ||
        !FrontierX::OpenNativeUtfFileSystem(sortPath, &archive, error) || !archive)
    {
        error = "three-face sorting fixture creation";
        return false;
    }
    std::srand(1);
    for (int face = 0; face < 3; ++face) (void) std::rand();
    const int expectedNextRandom = std::rand();
    std::srand(1);
    {
        XMesh mesh;
        loaded = mesh.read(archive, textures, nullptr, 0U) &&
            mesh.face_cnt == 3 && mesh.sorted_faces == 1U &&
            mesh.face_groups[0].face_area[0] == 2.0F &&
            mesh.face_groups[0].face_area[1] == 1.0F &&
            mesh.face_groups[0].face_area[2] == 0.5F &&
            std::rand() == expectedNextRandom;
    }
    archive->Release();
    archive = nullptr;
    if (!loaded)
    {
        error = "three-face order or original RNG draw count";
        return false;
    }
    const std::filesystem::path missingPath = path.string() + ".missing";
    if (!WritePolyMeshFixture(missingPath, "Missing") ||
        !FrontierX::OpenNativeUtfFileSystem(missingPath, &archive, error) ||
        !archive)
    {
        error = "missing-texture mesh UTF fixture creation";
        return false;
    }
    {
        XMesh mesh;
        loaded = !mesh.read(archive, textures, nullptr, 0U);
    }
    archive->Release();
    loaded = loaded && textures->has_texture_id("Missing") != GR_OK;
    if (!loaded)
    {
        error = "missing required legacy texture rejects mesh and retains no texture ID";
        return false;
    }
    const std::filesystem::path cq2Path = path.string() + ".cq2";
    if (!WritePolyMeshFixture(cq2Path, "", true) ||
        !FrontierX::OpenNativeUtfFileSystem(cq2Path, &archive, error) ||
        !archive)
    {
        error = "CQ2 material UTF fixture creation";
        return false;
    }
    {
        XMesh mesh;
        loaded = !mesh.read(archive, textures, nullptr, 0U) &&
            archive->SetCurrentDirectory("CQ2_Material_Library") &&
            archive->SetCurrentDirectory("..");
    }
    archive->Release();
    if (!loaded)
    {
        error = "CQ2 material path requires a material manager";
        return false;
    }
    if (!FrontierX::OpenNativeUtfFileSystem(cq2Path, &archive, error) ||
        !archive)
    {
        error = "CQ2 material UTF fixture reopening";
        return false;
    }
    CountingMaterialManager manager;
    {
        XMesh mesh;
        loaded = !mesh.read(archive, textures, &manager, 0U) &&
            mesh.imaterial_cnt == 1 && mesh.imaterial_list &&
            mesh.imaterial_list[0] == nullptr &&
            manager.lookups == 1U && manager.lastName == "Base" &&
            archive->SetCurrentDirectory("CQ2_Material_Library") &&
            archive->SetCurrentDirectory("..");
    }
    archive->Release();
    if (!loaded)
    {
        error = "unresolved CQ2 material must reach real lookup and reject the mesh";
        return false;
    }
    const std::filesystem::path countPath = path.string() + ".count";
    if (!WritePolyMeshFixture(countPath, "Tile", false, 0U) ||
        !FrontierX::OpenNativeUtfFileSystem(countPath, &archive, error) ||
        !archive)
    {
        error = "mismatched material count UTF fixture creation";
        return false;
    }
    {
        XMesh mesh;
        loaded = !mesh.read(archive, textures, nullptr, 0U);
    }
    archive->Release();
    if (!loaded) error = "mesh material count must bound directory entries";
    return loaded;
}
}
