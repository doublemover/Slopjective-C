#pragma once

#include <cstddef>
#include <span>

#include "config/objc3_feature_state_catalog.h"

namespace objc3c::config {

inline constexpr std::size_t kRejectedFeatureStateDataCount = 9;

std::span<const LanguageFeatureState> RejectedFeatureStateData();

}  // namespace objc3c::config
