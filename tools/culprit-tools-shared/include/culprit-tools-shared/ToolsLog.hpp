#pragma once
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string_view>

namespace culprit::tools {

enum class LogLevel : uint8_t { Trace, Debug, Info, Warn, Error };

namespace detail {

inline const char* ToString(LogLevel lvl) {
  switch (lvl) {
    case LogLevel::Trace:
      return "TRACE";
    case LogLevel::Debug:
      return "DEBUG";
    case LogLevel::Info:
      return "INFO";
    case LogLevel::Warn:
      return "WARN";
    case LogLevel::Error:
      return "ERROR";
  }
  return "INFO";
}

inline std::ostream& StreamFor(LogLevel lvl) {
  return (lvl == LogLevel::Error) ? std::cerr : std::cout;
}

}  // namespace detail

struct LogConfig {
  std::string_view toolName = "tool";
  std::atomic<LogLevel> minLevel{LogLevel::Info};
  bool showLevel = true;  // [scene-compiler] INFO: ...
  bool showTool = true;   // [scene-compiler] ...
};

// Global config (simple + good enough for CLI tools).
inline LogConfig& GetLogConfig() {
  static LogConfig cfg{};
  return cfg;
}

inline void SetToolName(std::string_view name) {
  GetLogConfig().toolName = name;
}

inline void SetMinLogLevel(LogLevel lvl) {
  GetLogConfig().minLevel.store(lvl, std::memory_order_relaxed);
}

inline bool IsEnabled(LogLevel lvl) {
  return static_cast<uint8_t>(lvl) >=
         static_cast<uint8_t>(GetLogConfig().minLevel.load(std::memory_order_relaxed));
}

inline void PrintBanner(std::string_view title) {
  std::cout <<
      R"(=====================================================================================================================================
)" << "                        "
            << title <<
      R"(
=====================================================================================================================================)"
            << "\n";
}

inline void LogRaw(LogLevel lvl, std::string_view msg) {
  if (!IsEnabled(lvl))
    return;

  auto& cfg = GetLogConfig();
  auto& os = detail::StreamFor(lvl);

  if (cfg.showTool) {
    os << "[" << cfg.toolName << "] ";
  }
  if (cfg.showLevel) {
    os << detail::ToString(lvl) << ": ";
  }
  os << msg << "\n";
}

// Stream-building front-end (keeps your existing call style).
template <typename... Args>
inline void Log(LogLevel lvl, Args&&... args) {
  if (!IsEnabled(lvl))
    return;

  std::ostringstream oss;
  (oss << ... << args);
  LogRaw(lvl, oss.str());
}

template <typename... Args>
inline void LogTrace(Args&&... args) {
  Log(LogLevel::Trace, std::forward<Args>(args)...);
}
template <typename... Args>
inline void LogDebug(Args&&... args) {
  Log(LogLevel::Debug, std::forward<Args>(args)...);
}
template <typename... Args>
inline void LogInfo(Args&&... args) {
  Log(LogLevel::Info, std::forward<Args>(args)...);
}
template <typename... Args>
inline void LogWarn(Args&&... args) {
  Log(LogLevel::Warn, std::forward<Args>(args)...);
}
template <typename... Args>
inline void LogError(Args&&... args) {
  Log(LogLevel::Error, std::forward<Args>(args)...);
}

}  // namespace culprit::tools