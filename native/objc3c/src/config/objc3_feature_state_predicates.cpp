#include "config/objc3_feature_state_catalog.h"

namespace objc3c::config {

bool FeatureStateIsAccepted(FeatureState state) {
  return state == FeatureState::Implemented || state == FeatureState::Internal;
}

bool FeatureStateIsRejected(FeatureState state) {
  return state == FeatureState::Rejected || state == FeatureState::Reserved;
}

}  // namespace objc3c::config
