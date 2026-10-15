#ifndef FRONTIERX_MATERIAL_TEXTURE_FIXTURE_H
#define FRONTIERX_MATERIAL_TEXTURE_FIXTURE_H

#include <filesystem>
#include <string>

namespace FrontierXTest
{
// Generates repository-owned UTF material and RGB565 texture input for native checks.
// Alpha mode protects selected Terran platform Palette/MIPn/Alpha 8 bit data.
bool WriteMipTextureFixture(const std::filesystem::path &path, bool alphaPlanes = false,
                            int levelTwoAlphaExtentDelta = 0);
bool WriteMaterialFixture(const std::filesystem::path & path,
                          const std::string & simpleType = "DcDtOc",
                          const std::string & mipDirectory = "MIP0");
}

#endif
