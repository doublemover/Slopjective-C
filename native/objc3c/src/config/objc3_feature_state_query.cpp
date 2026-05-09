#include "config/objc3_feature_state_catalog.h"

#include "config/objc3_feature_state_tables.h"

namespace objc3c::config {

std::span<const LanguageFeatureState> CanonicalFeatureStates() {
  return CanonicalFeatureStateEntries();
}

const LanguageFeatureState *FindCanonicalFeatureState(
    std::string_view feature) {
  for (const auto &state : CanonicalFeatureStateEntries()) {
    if (feature == state.feature) {
      return &state;
    }
  }
  return nullptr;
}

}  // namespace objc3c::config
