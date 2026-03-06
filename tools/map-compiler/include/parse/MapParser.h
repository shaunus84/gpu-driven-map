#pragma once

#include <filesystem>
#include <vector>

#include "parse/MapAuthoredTypes.h"

namespace culprit::tools {

// =============================================================================
// MapParser
//
// Parses a Valve 220 .map file into raw entities, brushes, and faces.
//
// This stage:
// - Preserves authored data verbatim
// - Performs no geometry processing
// - Performs no validation beyond basic syntax
//
// The output is structural "truth data" consumed by later stages.
// =============================================================================

std::vector<Entity> ParseToEntities(const std::filesystem::path& path);

}  // namespace culprit::tools
