#pragma once

#include <filesystem>

#include "mesh/MeshTypes.h"

namespace culprit::tools {

void ResolveMaterials(ParsedMap& map, const std::vector<std::string>& activeMods,
                      const std::filesystem::path& workingFilesRoot);
}
