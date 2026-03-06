#include "parse/MapParser.h"

// This file implements a minimal, permissive parser for Valve 220 map files.
// It prioritises robustness and data preservation over strict validation.
// Semantic interpretation is deferred to later pipeline stages.

#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace culprit::tools {

namespace {

// Trim leading whitespace (spaces and tabs only).
// Map files are not expected to contain exotic whitespace.
inline void ltrim(std::string& s) noexcept {
  size_t i = 0;
  while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) ++i;
  if (i) s.erase(0, i);
}

// Trim trailing whitespace and carriage returns.
// Handles Windows-style line endings.
inline void rtrim(std::string& s) noexcept {
  while (!s.empty() &&
         (s.back() == ' ' || s.back() == '\t' || s.back() == '\r'))
    s.pop_back();
}

// Simple prefix test used for map syntax parsing.
constexpr bool starts_with(const std::string& s, char c) noexcept {
  return !s.empty() && s.front() == c;
}

// Returns true if the character is part of a token.
// Valve 220 syntax uses (), [], and whitespace as delimiters.
constexpr bool is_token_char(char c) noexcept {
  return !(c == '[' || c == ']' || c == '(' || c == ')' || c == ' ' ||
           c == '\t');
}

// Parses a key/value line of the form:
//   "key" "value"
//
// This parser is intentionally permissive and ignores malformed lines
// rather than rejecting the entire file.
[[nodiscard]] bool parse_kv_line(const std::string& line, std::string& outKey,
                                 std::string& outVal) {
  outKey.clear();
  outVal.clear();

  int q = 0;
  std::string cur;
  std::array<std::string, 2> fields{};

  for (char ch : line) {
    if (ch == '"') {
      if (q & 1) {
        if (q / 2 < 2) fields[q / 2] = cur;
        cur.clear();
      }
      ++q;
      continue;
    }

    if (q & 1) cur.push_back(ch);
  }

  if (!fields[0].empty()) outKey = std::move(fields[0]);
  if (!fields[1].empty()) outVal = std::move(fields[1]);

  return !outKey.empty();
}

// Tokenizes a face definition line according to Valve 220 rules.
// Faces are expected to contain at least 21 tokens.
[[nodiscard]] bool split_face_line(const std::string& line,
                                   std::vector<std::string>& out) {
  out.clear();

  std::string tmp;
  tmp.reserve(32);
  bool inTok = false;

  for (char ch : line) {
    if (is_token_char(ch)) {
      inTok = true;
      tmp.push_back(ch);
    } else if (inTok) {
      out.push_back(tmp);
      tmp.clear();
      inTok = false;
    }
  }

  if (inTok && !tmp.empty()) out.push_back(tmp);

  return out.size() >= 21;
}

// Float parsing is centralized to allow later replacement
// with from_chars or a custom fast parser.
inline float parse_float(const std::string& s) { return std::stof(s); }

}  // namespace

// ------------------------------------------------------------
// Parses a Valve 220 .map file into raw Entity structures.
// Parsing is line-based and state-driven.
// Errors are reported but do not abort parsing.
std::vector<Entity> ParseToEntities(const std::filesystem::path& path) {
  std::vector<Entity> entities;

  std::ifstream mapFile(path);
  if (!mapFile) {
    std::cerr << "[MapParser] Could not open map file: " << path << "\n";
    return entities;
  }

  // The parser maintains a simple state machine:
  // - parsingEntity: inside an entity block
  // - parsingBrush:  inside a brush block
  bool parsingEntity = false;
  bool parsingBrush = false;

  std::string line;
  uint32_t lineNo = 0;

  std::vector<std::string> toks;
  toks.reserve(32);

  while (std::getline(mapFile, line)) {
    ++lineNo;
    rtrim(line);
    ltrim(line);

    // Skip empty lines and comments
    if (line.empty()) continue;
    if (line.size() >= 2 && line[0] == '/' && line[1] == '/') continue;

    // Open entity
    if (starts_with(line, '{') && !parsingEntity) {
      entities.emplace_back(Entity{});
      parsingEntity = true;
      parsingBrush = false;
      continue;
    }

    if (!parsingEntity) continue;

    // Close entity or brush
    if (starts_with(line, '}')) {
      if (parsingBrush) {
        parsingBrush = false;
      } else {
        parsingEntity = false;
      }
      continue;
    }

    // Open brush
    if (starts_with(line, '{')) {
      parsingBrush = true;
      entities.back().brushes.emplace_back(Brush{});
      continue;
    }

    // Entity properties are stored verbatim.
    // No interpretation is performed at this stage.
    if (starts_with(line, '"')) {
      std::string k, v;
      if (!parse_kv_line(line, k, v)) {
        std::cerr << "[MapParser] KV parse error at line " << lineNo << "\n";
        continue;
      }

      if (k == "classname") {
        entities.back().classname = v;
      }
      entities.back().properties.emplace(std::move(k), std::move(v));
      continue;
    }

    // Face definition inside a brush.
    // Valve 220 format encodes plane points, material name,
    // texture axes, rotation, and scale in a single line.
    if (parsingBrush) {
      // Parsing errors are reported but do not stop parsing.
      // This allows partially valid maps to be inspected and debugged.
      if (!split_face_line(line, toks)) {
        std::cerr << "[MapParser] Face parse error at line " << lineNo << "\n";
        continue;
      }

      int p = 0;
      Face f{};

      // Plane points are swizzled from Valve's coordinate order
      // into the engine's coordinate system.
      f.points[0][0] = parse_float(toks[p++]);
      f.points[0][1] = parse_float(toks[p++]);
      f.points[0][2] = parse_float(toks[p++]);

      f.points[1][0] = parse_float(toks[p++]);
      f.points[1][1] = parse_float(toks[p++]);
      f.points[1][2] = parse_float(toks[p++]);

      f.points[2][0] = parse_float(toks[p++]);
      f.points[2][1] = parse_float(toks[p++]);
      f.points[2][2] = parse_float(toks[p++]);

      f.materialName = toks[p++];

      // Texture U axis
      f.textureUAxis[0] = parse_float(toks[p++]);
      f.textureUAxis[1] = parse_float(toks[p++]);
      f.textureUAxis[2] = parse_float(toks[p++]);
      f.textureUAxis[3] = parse_float(toks[p++]);

      // Texture V axis
      f.textureVAxis[0] = parse_float(toks[p++]);
      f.textureVAxis[1] = parse_float(toks[p++]);
      f.textureVAxis[2] = parse_float(toks[p++]);
      f.textureVAxis[3] = parse_float(toks[p++]);

      f.textureRotation = parse_float(toks[p++]);
      f.uScale = parse_float(toks[p++]);
      f.vScale = parse_float(toks[p++]);

      entities.back().brushes.back().faces.push_back(std::move(f));
    }
  }

  return entities;
}

}  // namespace culprit::tools
