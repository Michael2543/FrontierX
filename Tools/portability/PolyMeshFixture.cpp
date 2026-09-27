#include "PolyMeshFixture.h"

#include "FrontierX/NativeUtfFileSystem.h"
#include "ITextureLibrary.h"
#include "xmesh.h"

#include <cstdint>
#include <cstring>
#include <fstream>
#include <initializer_list>
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
                          bool embeddedTexture = false)
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
    const U32 meshRoot = wrapForComponent ? directory(0U, "openFLAME 3D N-mesh") : 0U;
    auto words = [](std::initializer_list<U32> values) {
        std::vector<unsigned char> bytes;
        for (U32 value : values) Word(bytes, value);
        return bytes;
    };
    const U32 groups = directory(meshRoot, "Face groups");
    file(groups, "Count", words({1U}));
    const U32 group = directory(groups, "Group0");
    file(group, "Material", words({0U}));
    file(group, "Face count", words({2U}));
    file(group, "Face vertex chain", words({0U, 1U, 2U, 0U, 3U, 4U}));
    file(group, "Face normal", words({0U, 0U}));
    file(group, "Face D-coefficient", words({0U, 0U}));
    file(group, "Face property", words({4U, 4U}));
    const U32 vertices = directory(meshRoot, "Vertices");
    file(vertices, "Vertex batch count", words({5U}));
    file(vertices, "Vertex batch list", words({0U, 1U, 2U, 3U, 4U}));
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
    file(vertices, "Texture batch list", words({0U, 1U, 2U, 3U, 4U}));
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
        file(base, "Diffuse", std::move(diffuse));
        std::vector<unsigned char> transparency;
        Float(transparency, 0.5F);
        file(base, "Transparency", std::move(transparency));
        file(base, "Texture name", std::vector<unsigned char>(
            textureName.begin(), textureName.end()));
    }
    const U32 textureLibrary = directory(meshRoot, "Texture library");
    if (embeddedTexture)
    {
        const U32 tile = directory(textureLibrary, "Tile");
        file(tile, "Image X size", words({2U}));
        file(tile, "Image Y size", words({2U}));
        const U32 rgb = directory(tile, "True RGB 565");
        const U32 mip = directory(rgb, "MIP0");
        // Distinct primaries make a sampled draw differ from white fallback.
        file(mip, "Image colors", std::vector<unsigned char>{
            0x00U, 0xf8U, 0xe0U, 0x07U, 0x1fU, 0x00U, 0xffU, 0xffU});
    }

    constexpr U32 headerSize = 56U;
    constexpr U32 recordSize = 44U;
    const U32 recordsSize = static_cast<U32>(entries.size()) * recordSize;
    std::vector<unsigned char> names;
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
                                bool textured)
{
    return WritePolyMeshFixture(path, textured ? "Tile" : "", false,
                                1U, "Base", true, textured);
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
        loaded = mesh.read(archive, textures, nullptr, 0U) &&
            mesh.material_cnt == 1 && mesh.material_list &&
            mesh.material_list[0].diffuse_texture_ref != ITL_INVALID_REF_ID &&
            mesh.material_list[0].texture_id == 0U &&
            (mesh.material_list[0].texture_flags & TF_F_HAS_ALPHA) == 0U;
    }
    archive->Release();
    loaded = loaded && textures->has_texture_id("Missing") != GR_OK;
    if (!loaded)
    {
        error = "missing legacy material texture has no frame or retained ID";
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
        loaded = mesh.read(archive, textures, &manager, 0U) &&
            mesh.imaterial_cnt == 1 && mesh.imaterial_list &&
            mesh.imaterial_list[0] == nullptr &&
            manager.lookups == 1U && manager.lastName == "Base" &&
            archive->SetCurrentDirectory("CQ2_Material_Library") &&
            archive->SetCurrentDirectory("..");
    }
    archive->Release();
    if (!loaded)
    {
        error = "CQ2 material directory did not reach IMaterialManager lookup";
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
