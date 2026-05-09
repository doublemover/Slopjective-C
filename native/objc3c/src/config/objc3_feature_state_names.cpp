#include "config/objc3_feature_state_catalog.h"

namespace objc3c::config {

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

}  // namespace objc3c::config
