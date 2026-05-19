#include "config/objc3_feature_state_catalog.h"

#include "config/objc3_feature_state_table.h"

namespace objc3c::config {

const LanguageFeatureState *FindCanonicalFeatureState(
    std::string_view feature) {
  for (const auto &state : CanonicalFeatureStateTable()) {
    if (feature == state.feature) {
      return &state;
    }
  }
  return nullptr;
}

}  // namespace objc3c::config
