#pragma once

#include "mesh/MeshTypes.h"

namespace culprit::tools {

struct CompiledEntity {
    std::string classname;
    std::unordered_map<std::string, std::string> properties;

    uint32_t meshIndex;
    uint32_t materialSetIndex;
    maths::Mat4f localTransform;
};

struct CompiledMap {
    std::vector<MeshPrototype> meshes;
    std::vector<std::vector<std::string>> materialSets;
    std::vector<CompiledEntity> compiledEntities;
};

}  // namespace culprit::tools
