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
bool RunPolyMeshFixture(const std::filesystem::path & path,
                        ITextureLibrary * textures, std::string & error);
}

#endif
