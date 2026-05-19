#pragma once

#include <cstddef>
#include <span>

#include "config/objc3_feature_state_catalog.h"

namespace objc3c::config {

inline constexpr std::size_t kImplementedFeatureStateDataCount = 1;
inline constexpr std::size_t kReservedFeatureStateDataCount = 1;

std::span<const LanguageFeatureState> ImplementedFeatureStateData();
std::span<const LanguageFeatureState> ReservedFeatureStateData();

}  // namespace objc3c::config
