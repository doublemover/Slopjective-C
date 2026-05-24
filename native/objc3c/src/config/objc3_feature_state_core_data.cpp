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
        {"reserved-typed-throws", FeatureState::Reserved, "O3P182",
         "throws(E) single payloads are source/interface preserved and lower through the hidden error-out ABI for supported exact-catch and id<Error> bridge paths; empty, multi, malformed, non-type, incompatible, and foreign carrier cases remain fail-closed."},
        {"reserved-value-optionals", FeatureState::Reserved, "O3P159",
         "Optional<T> value optional type signatures have bounded scalar packed runtime ABI lowering; nested, generic, property, ivar, nullability bridge, implicit nil, unchecked unwrap, throws/result, and nil-to-scalar forms remain unavailable."},
        {"reserved-match-expressions", FeatureState::Reserved, "O3P156",
         "Expression-form match remains unavailable; the current frontend admits only statement match."},
        {"reserved-guarded-match-patterns", FeatureState::Reserved, "O3P157",
         "Guarded match patterns remain unavailable until semantic and lowering support land."},
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
