#include "config/objc3_feature_state_catalog.h"

#include <array>

namespace objc3c::config {

namespace {

constexpr std::array<LanguageFeatureState, 11> kCanonicalFeatureStates = {{
    {"objc3-language-version", FeatureState::Implemented, "",
     "Objective-C 3.0 is the only accepted native frontend language version."},
    {"legacy-objective-c-mode", FeatureState::Rejected, "O3C001",
     "Retired legacy Objective-C mode is rejected by active surfaces."},
    {"legacy-literal-aliases", FeatureState::Rejected, "O3C002",
     "YES, NO, and NULL are rejected as legacy aliases on the canonical path."},
    {"migration-assist", FeatureState::Rejected, "O3C003",
     "Migration-assist behavior is not a supported hard-cutover mode."},
    {"optional-template-alias", FeatureState::Rejected, "O3C004",
     "optional<T> aliases are rejected in favor of canonical Optional<T> spelling."},
    {"runtime-shim-dispatch", FeatureState::Rejected, "O3R001",
     "Retired runtime shim dispatch switches are rejected."},
    {"runtime-fallback-dispatch", FeatureState::Rejected, "O3R001",
     "Retired runtime fallback dispatch switches are rejected."},
    {"report-only-canonical-diagnostics", FeatureState::Rejected, "O3C003",
     "Report-only canonical rejection diagnostics are not an active mode."},
    {"retired-shim-options", FeatureState::Rejected, "O3C001",
     "Retired shim switches are rejected by the native command surface."},
    {"old-mode-option-aliases", FeatureState::Rejected, "O3C001",
     "Old-mode option aliases are rejected instead of normalized."},
    {"feature-reservation", FeatureState::Reserved, "",
     "Reserved syntax must remain explicit and fail closed until implemented."},
}};

}  // namespace

const char *FeatureStateName(FeatureState state) {
  switch (state) {
    case FeatureState::Implemented:
      return "implemented";
    case FeatureState::Rejected:
      return "rejected";
    case FeatureState::Reserved:
      return "reserved";
    case FeatureState::Internal:
      return "internal";
  }
  return "unknown";
}

bool FeatureStateIsAccepted(FeatureState state) {
  return state == FeatureState::Implemented || state == FeatureState::Internal;
}

bool FeatureStateIsRejected(FeatureState state) {
  return state == FeatureState::Rejected || state == FeatureState::Reserved;
}

std::span<const LanguageFeatureState> CanonicalFeatureStates() {
  return std::span<const LanguageFeatureState>(kCanonicalFeatureStates.data(),
                                               kCanonicalFeatureStates.size());
}

const LanguageFeatureState *FindCanonicalFeatureState(
    std::string_view feature) {
  for (const auto &state : kCanonicalFeatureStates) {
    if (feature == state.feature) {
      return &state;
    }
  }
  return nullptr;
}

}  // namespace objc3c::config
