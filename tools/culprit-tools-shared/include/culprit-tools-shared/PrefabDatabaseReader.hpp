#pragma once

#include <cstdint>
#include <filesystem>
#include <string_view>
#include <vector>
#include <fstream>

#include "PrefabDatabaseFormat.hpp"

namespace culprit::formats {

class PrefabDatabaseReader {
 public:
  // ============================================================
  // File lifetime
  // ============================================================

  bool Open(const std::filesystem::path& path) {
    Close();

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
      return false;
    }

    size_ = static_cast<size_t>(file.tellg());
    file.seekg(0);

    buffer_.resize(size_);
    file.read(reinterpret_cast<char*>(buffer_.data()), size_);

    if (!file) {
      Close();
      return false;
    }

    base_   = buffer_.data();
    header_ = reinterpret_cast<const PrefabFileHeader*>(base_);

    if (!ValidateHeader()) {
      Close();
      return false;
    }

    return true;
  }

  void Close() {
    buffer_.clear();
    base_   = nullptr;
    header_ = nullptr;
    size_   = 0;
  }

  bool IsValid() const { return header_ != nullptr; }

  // ============================================================
  // Header access
  // ============================================================

  const PrefabFileHeader& Header() const { return *header_; }

  // ============================================================
  // Core tables
  // ============================================================

  const PrefabRecord* Prefabs() const {
    return TableAt<PrefabRecord>(header_->prefabsOffset,
                                 header_->prefabCount);
  }

  const RenderPrefabRecord* RenderPrefabs() const {
    return TableAt<RenderPrefabRecord>(header_->renderPrefabsOffset,
                                       header_->renderPrefabCount);
  }

  const RenderMaterialRef* Materials() const {
    return TableAt<RenderMaterialRef>(header_->materialsOffset,
                                      header_->materialCount);
  }

  const PropertyOverride* Overrides() const {
    return TableAt<PropertyOverride>(header_->overridesOffset,
                                     header_->overrideCount);
  }

  // ============================================================
  // String table
  // ============================================================

  const char* StringTable() const {
    return reinterpret_cast<const char*>(base_ + header_->stringTableOffset);
  }

  std::string_view GetStringView(uint32_t offset,
                                 uint32_t length) const {
    const char* base = StringTable();
    return std::string_view(base + offset, length);
  }

  // ============================================================
  // Prefab lookup
  // ============================================================

  const PrefabRecord* FindPrefabByClassname(
      std::string_view classname) const {
    const PrefabRecord* prefabs = Prefabs();
    const char* strings = StringTable();

    for (uint32_t i = 0; i < header_->prefabCount; ++i) {
      const PrefabRecord& p = prefabs[i];
      std::string_view name(strings + p.classnameOffset,
                            p.classnameLength);
      if (name == classname) {
        return &p;
      }
    }

    return nullptr;
  }

  // ============================================================
  // Render helpers
  // ============================================================

  const RenderPrefabRecord* GetRenderPrefab(
      const PrefabRecord& prefab) const {
    if (prefab.renderPrefabIndex == UINT32_MAX) {
      return nullptr;
    }

    if (prefab.renderPrefabIndex >= header_->renderPrefabCount) {
      return nullptr;
    }

    return &RenderPrefabs()[prefab.renderPrefabIndex];
  }

  std::string_view GetRenderModelPath(
      const RenderPrefabRecord& render) const {
    return GetStringView(render.modelPathOffset,
                         render.modelPathLength);
  }

  std::string_view GetRenderMaterialPath(
      const RenderPrefabRecord& render,
      uint32_t slot) const {
    if (slot >= render.materialCount) {
      return {};
    }

    const uint32_t index = render.materialIndex + slot;
    if (index >= header_->materialCount) {
      return {};
    }

    const RenderMaterialRef& mat = Materials()[index];
    return GetStringView(mat.pathOffset, mat.pathLength);
  }

  // ============================================================
  // Override helpers
  // ============================================================

  const PropertyOverride* GetOverrides(
      const PrefabRecord& prefab) const {
    if (prefab.overrideCount == 0) {
      return nullptr;
    }

    if (prefab.overrideIndex + prefab.overrideCount >
        header_->overrideCount) {
      return nullptr;
    }

    return Overrides() + prefab.overrideIndex;
  }

 private:
  // ============================================================
  // Validation
  // ============================================================

  bool ValidateHeader() const {
    if (!header_) return false;

    if (header_->magic != kPrefabMagic ||
        header_->version != kPrefabVersion) {
      return false;
    }

    return ValidateTable(header_->prefabsOffset,
                         header_->prefabCount,
                         sizeof(PrefabRecord)) &&
           ValidateTable(header_->renderPrefabsOffset,
                         header_->renderPrefabCount,
                         sizeof(RenderPrefabRecord)) &&
           ValidateTable(header_->materialsOffset,
                         header_->materialCount,
                         sizeof(RenderMaterialRef)) &&
           ValidateTable(header_->overridesOffset,
                         header_->overrideCount,
                         sizeof(PropertyOverride)) &&
           header_->stringTableOffset < size_;
  }

  bool ValidateTable(uint64_t offset,
                     uint64_t count,
                     uint64_t elemSize) const {
    if (count == 0) {
      return true;
    }

    const uint64_t end = offset + count * elemSize;
    return offset < size_ && end <= size_;
  }

  template <typename T>
  const T* TableAt(uint64_t offset, uint64_t count) const {
    if (count == 0) {
      return nullptr;
    }
    return reinterpret_cast<const T*>(base_ + offset);
  }

 private:
  std::vector<uint8_t> buffer_;
  const uint8_t* base_ = nullptr;
  const PrefabFileHeader* header_ = nullptr;
  size_t size_ = 0;
};

} // namespace culprit::formats
