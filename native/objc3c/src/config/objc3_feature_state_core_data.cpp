#include "config/objc3_feature_state_core_data.h"

#include <array>

namespace objc3c::config {
namespace {

constexpr std::array<LanguageFeatureState, kImplementedFeatureStateDataCount>
    kImplementedFeatureStates = {{
        {"objc3-language-version", FeatureState::Implemented, "",
         "Objective-C 3.0 is the only accepted native frontend language version."},
    }};

constexpr std::array<LanguageFeatureState, kReservedFeatureStateDataCount>
    kReservedFeatureStates = {{
        {"reserved-syntax", FeatureState::Reserved, "",
         "Reserved syntax must remain explicit and fail closed until implemented."},
    }};

}  // namespace

std::span<const LanguageFeatureState> ImplementedFeatureStateData() {
  return std::span<const LanguageFeatureState>(kImplementedFeatureStates.data(),
                                               kImplementedFeatureStates.size());
}

std::span<const LanguageFeatureState> ReservedFeatureStateData() {
  return std::span<const LanguageFeatureState>(kReservedFeatureStates.data(),
                                               kReservedFeatureStates.size());
}

}  // namespace objc3c::config
