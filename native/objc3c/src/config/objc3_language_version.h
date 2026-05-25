#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include "config/objc3_config_validation.h"

namespace objc3c::config {

inline constexpr std::uint8_t kCanonicalLanguageVersion = 3u;
inline constexpr const char *kCanonicalLanguageProfileName = "canonical";
inline constexpr const char *kStrictLanguageProfileName = "strict";
inline constexpr const char *kStrictConcurrencyLanguageProfileName =
    "strict-concurrency";

inline constexpr bool IsCanonicalLanguageVersion(std::uint32_t version) {
  return version == kCanonicalLanguageVersion;
}

std::string UnsupportedLanguageVersionDiagnostic(std::uint32_t version);
ConfigValidationResult ValidateCanonicalLanguageVersion(std::uint32_t version);
bool ParseLanguageVersionText(std::string_view text, std::uint32_t &version);
ConfigValidationResult ValidateCanonicalLanguageVersionText(
    std::string_view text);

}  // namespace objc3c::config
