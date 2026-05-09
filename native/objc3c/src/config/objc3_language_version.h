#pragma once

#include <cstdint>
#include <string>

namespace objc3c::config {

inline constexpr std::uint8_t kCanonicalLanguageVersion = 3u;
inline constexpr const char *kCanonicalLanguageProfileName = "canonical";

inline constexpr bool IsCanonicalLanguageVersion(std::uint32_t version) {
  return version == kCanonicalLanguageVersion;
}

inline std::string UnsupportedLanguageVersionDiagnostic(std::uint32_t version) {
  return "unsupported Objective-C language version for native frontend (expected " +
         std::to_string(static_cast<unsigned>(kCanonicalLanguageVersion)) +
         "): " + std::to_string(version);
}

}  // namespace objc3c::config
