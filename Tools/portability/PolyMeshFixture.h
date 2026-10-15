#ifndef FRONTIERX_POLY_MESH_FIXTURE_H
#define FRONTIERX_POLY_MESH_FIXTURE_H

#include <filesystem>
#include <string>

struct ITextureLibrary;

namespace FrontierXTest
{
bool WriteGameMeshFixture(const std::filesystem::path & path);
bool WriteVertexColorGameMeshFixture(const std::filesystem::path & path);
bool WriteLegacyGameMeshFixture(const std::filesystem::path & path,
                                bool textured = false, bool indexed = false);
bool WriteAnim2DSharedTextureFixture(const std::filesystem::path &path);
bool WriteRawTgaTextureFixture(const std::filesystem::path &path);
bool WriteLegacySeamGameMeshFixture(const std::filesystem::path &path, bool textured);
bool WriteRequiredMaterialFailureFixture(const std::filesystem::path &path, unsigned mode);
bool WriteCompoundGameMeshFixture(const std::filesystem::path &path, unsigned mode);
bool WriteRendererClaimGameMeshFixture(const std::filesystem::path &path, bool declaredMesh);
bool RunPolyMeshFixture(const std::filesystem::path & path,
                        ITextureLibrary * textures, std::string & error);
}

#endif
