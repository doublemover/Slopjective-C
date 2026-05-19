#include "config/objc3_feature_state_catalog.h"

#include "config/objc3_feature_state_table.h"

namespace objc3c::config {

std::span<const LanguageFeatureState> CanonicalFeatureStates() {
  return CanonicalFeatureStateTable();
}

}  // namespace objc3c::config
