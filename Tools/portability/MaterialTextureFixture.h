#ifndef FRONTIERX_MATERIAL_TEXTURE_FIXTURE_H
#define FRONTIERX_MATERIAL_TEXTURE_FIXTURE_H

#include <filesystem>
#include <string>

namespace FrontierXTest
{
// Generates repository-owned UTF material and RGB565 texture input for native checks.
bool WriteMaterialFixture(const std::filesystem::path & path,
                          const std::string & simpleType = "DcDtOc",
                          const std::string & mipDirectory = "MIP0");
}

#endif
