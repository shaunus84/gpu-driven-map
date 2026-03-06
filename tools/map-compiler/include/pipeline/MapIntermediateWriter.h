#pragma once

#include <string>

#include "mesh/MeshTypes.h"

namespace culprit::tools {

// =============================================================================
// MapIntermediateWriter
//
// Serializes ParsedMapGeometry into a shared, POD-only intermediate map format.
// This file format is intended for consumption by later build steps,
// NOT for direct runtime loading.
// =============================================================================

// Writes ParsedMap into the shared intermediate map file format.
// Returns true on success, false on failure.
bool WriteIntermediateMap(const std::string& outputPath, const ParsedMap& map);

}  // namespace culprit::tools
