#include "config/objc3_feature_state_tables.h"

#include "config/objc3_feature_state_data.h"

namespace objc3c::config {

std::span<const LanguageFeatureState> CanonicalFeatureStateEntries() {
  return CanonicalFeatureStateData();
}

}  // namespace objc3c::config
