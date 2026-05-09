#pragma once

#include <span>

#include "config/objc3_feature_state_catalog.h"

namespace objc3c::config {

std::span<const LanguageFeatureState> CanonicalFeatureStateData();

}  // namespace objc3c::config
