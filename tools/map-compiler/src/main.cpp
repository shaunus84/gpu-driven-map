#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "parse/MapParser.h"
#include "pipeline/MapBuildContext.h"
#include "pipeline/MapIntermediateWriter.h"
#include "pipeline/MapParsePipeline.h"
#include "resolve/ResolveMaterials.h"

constexpr float kTBToEngineScale = 1.0f / 32.0f;

namespace {

inline void PrintBanner() {
  std::cout <<
      R"(=====================================================================================================================================
                        MAP PARSER
=====================================================================================================================================)"
            << "\n";
}

std::vector<std::string> ExtractActiveMods(const std::vector<culprit::tools::Entity>& entities) {
  std::vector<std::string> activeMods;

  for (const auto& e : entities) {
    auto it = e.properties.find("classname");
    if (it != e.properties.end() && it->second == "worldspawn") {
      auto modIt = e.properties.find("_tb_mod");
      if (modIt == e.properties.end())
        break;

      std::string modList = modIt->second;
      size_t pos = 0;

      while ((pos = modList.find(';')) != std::string::npos) {
        activeMods.push_back(modList.substr(0, pos));
        modList.erase(0, pos + 1);
      }

      if (!modList.empty())
        activeMods.push_back(modList);

      break;
    }
  }

  return activeMods;
}
}  // namespace

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "Usage: map-parser --working-dir <working-dir> --out-dir <dir>"
                 "<map1.map> [map2.map ...]\n";
    return 1;
  }

  std::filesystem::path outDir;
  std::filesystem::path workingDir;
  std::vector<std::filesystem::path> inputs;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    if (arg == "--out-dir" && i + 1 < argc) {
      outDir = argv[++i];
    } else if (arg == "--working-dir" && i + 1 < argc) {
      workingDir = argv[++i];
    } else if (!arg.empty() && arg[0] == '-') {
      std::cerr << "Unknown argument: " << arg << "\n";
      return 1;
    } else {
      inputs.emplace_back(arg);
    }
  }

  if (workingDir.empty()) {
    std::cerr << "--working-dir is required\n";
    return 1;
  }

  if (outDir.empty()) {
    std::cerr << "--out-dir is required\n";
    return 1;
  }

  if (inputs.empty()) {
    std::cerr << "No input maps provided\n";
    return 1;
  }

  PrintBanner();

  std::filesystem::create_directories(outDir);

  for (const auto& mapPath : inputs) {
    const std::string mapName = mapPath.stem().string();
    const auto outPmap = outDir / (mapName + ".pmap");

    std::cout << "[map-parser] Loading: " << mapPath << "\n";

    auto entities = culprit::tools::ParseToEntities(mapPath.string());

    culprit::tools::MapBuildContext context{workingDir, entities, kTBToEngineScale};

    auto activeMods = ExtractActiveMods(entities);
    auto parsedMap = ParseMapToIntermediate(context);
    culprit::tools::ResolveMaterials(parsedMap, activeMods, workingDir);

    if (!culprit::tools::WriteIntermediateMap(outPmap.string(), parsedMap)) {
      std::cerr << "[map-parser] Failed to write " << outPmap << "\n";
      return 3;
    }

    std::cout << "[map-parser] Wrote: " << outPmap << "\n";
  }

  std::cout << "[map-parser] OK\n";
  return 0;
}
