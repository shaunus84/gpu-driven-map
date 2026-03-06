#pragma once

#include "resolve/ResolveMaterials.h"

#include <filesystem>
#include <iostream>

namespace culprit::tools {

void ResolveMaterials(ParsedMap& map, const std::vector<std::string>& activeMods,
                      const std::filesystem::path& workingFilesRoot) {
    for (MaterialSet& ms : map.materialSets) {
        for (std::string& mat : ms.materials) {
            bool found = false;

            std::filesystem::path materialPath(mat);
            std::string materialName = materialPath.parent_path().generic_string();

            for (const std::string& mod : activeMods) {
                std::filesystem::path toml =
                    workingFilesRoot / mod / "textures" / materialName / (materialName + ".toml");

                if (std::filesystem::exists(toml)) {
                    mat = std::filesystem::relative(toml, workingFilesRoot).generic_string();

                    found = true;
                    break;
                }
            }

            if (!found) {
                // Optional: warn or fallback
                // std::cout << "Material not found: "
                //           << mat << "\n";
            }
        }
    }
}

}  // namespace culprit::tools
