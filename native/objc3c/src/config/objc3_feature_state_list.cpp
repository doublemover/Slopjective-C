#include "config/objc3_feature_state_catalog.h"

#include "config/objc3_feature_state_tables.h"

namespace objc3c::config {

std::span<const LanguageFeatureState> CanonicalFeatureStates() {
  return CanonicalFeatureStateEntries();
}

}  // namespace objc3c::config
