#pragma once

#include <array>
#include <cstdint>

namespace objc3c::config {

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

}  // namespace objc3c::config
