#pragma once

#include <array>
#include <cstdint>
#include <string>

namespace objc3c::config {

inline constexpr std::uint8_t kCanonicalLanguageVersion = 3u;
inline constexpr const char *kCanonicalLanguageProfileName = "canonical";

enum class FeatureState : std::uint8_t {
  Implemented,
  Rejected,
  Reserved,
  Internal,
};

struct LanguageFeatureState {
  const char *feature;
  FeatureState state;
  const char *diagnostic_code;
  const char *summary;
};

struct CommandOptionState {
  const char *spelling;
  FeatureState state;
  const char *diagnostic_code;
  const char *summary;
};

inline constexpr std::array<LanguageFeatureState, 7> kCanonicalFeatureStates = {{
    {"objc3-language-version", FeatureState::Implemented, "",
     "Objective-C 3.0 is the only accepted native frontend language version."},
    {"legacy-objective-c-mode", FeatureState::Rejected, "O3C001",
     "Legacy Objective-C compatibility mode is removed from active surfaces."},
    {"legacy-literal-aliases", FeatureState::Rejected, "O3C002",
     "YES, NO, and NULL are rejected as legacy aliases on the canonical path."},
    {"migration-assist", FeatureState::Rejected, "O3C003",
     "Migration-assist behavior is not a supported hard-cutover mode."},
    {"optional-template-alias", FeatureState::Rejected, "O3C004",
     "optional<T> aliases are rejected in favor of canonical Optional<T> spelling."},
    {"runtime-shim-dispatch", FeatureState::Rejected, "O3R001",
     "Runtime dispatch must resolve or return a structured error."},
    {"feature-reservation", FeatureState::Reserved, "",
     "Reserved syntax must remain explicit and fail closed until implemented."},
}};

inline constexpr std::array<CommandOptionState, 3> kRemovedCommandOptions = {{
    {"--objc3-compat-mode", FeatureState::Rejected, "O3C001",
     "Objective-C 3.0 is canonical-only; compatibility mode was removed."},
    {"--objc3-migration-assist", FeatureState::Rejected, "O3C003",
     "Migration-assist mode was removed from the hard-cutover command surface."},
    {"--objc3-canonical-rejection-diagnostics", FeatureState::Rejected, "O3C003",
     "Report-only canonical rejection diagnostics were removed from the active command surface."},
}};

inline constexpr bool IsCanonicalLanguageVersion(std::uint32_t version) {
  return version == kCanonicalLanguageVersion;
}

inline std::string UnsupportedLanguageVersionDiagnostic(std::uint32_t version) {
  return "unsupported Objective-C language version for native frontend (expected " +
         std::to_string(static_cast<unsigned>(kCanonicalLanguageVersion)) +
         "): " + std::to_string(version);
}

inline const CommandOptionState *FindRemovedCommandOption(const std::string &flag) {
  for (const auto &option : kRemovedCommandOptions) {
    if (flag == option.spelling) {
      return &option;
    }
  }
  return nullptr;
}

}  // namespace objc3c::config
