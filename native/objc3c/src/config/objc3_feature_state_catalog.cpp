#include "config/objc3_feature_state_catalog.h"

#include "config/objc3_feature_state_tables.h"
#include "contracts/objc3_config_capability_contract.h"

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

bool FeatureStateIsAccepted(FeatureState state) {
  return state == FeatureState::Implemented || state == FeatureState::Internal;
}

bool FeatureStateIsRejected(FeatureState state) {
  return state == FeatureState::Rejected || state == FeatureState::Reserved;
}

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

const char *CanonicalFeatureStateCatalogContractId() {
  return objc3c::contracts::kObjc3CanonicalFeatureStateCatalogContractId;
}

}  // namespace objc3c::config
