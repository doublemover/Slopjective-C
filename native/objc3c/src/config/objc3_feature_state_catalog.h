#pragma once

#include <cstdint>
#include <span>
#include <string_view>

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

const char *FeatureStateName(FeatureState state);
bool FeatureStateIsAccepted(FeatureState state);
bool FeatureStateIsRejected(FeatureState state);
std::span<const LanguageFeatureState> CanonicalFeatureStates();
const LanguageFeatureState *FindCanonicalFeatureState(
    std::string_view feature);
const char *CanonicalFeatureStateCatalogContractId();

}  // namespace objc3c::config
