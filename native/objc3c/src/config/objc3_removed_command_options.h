#pragma once

#include <span>
#include <string_view>

#include "config/objc3_config_validation.h"
#include "config/objc3_feature_state_catalog.h"

namespace objc3c::config {

struct CommandOptionState {
  const char *spelling;
  FeatureState state;
  const char *diagnostic_code;
  const char *summary;
};

std::span<const CommandOptionState> RemovedCommandOptions();
const CommandOptionState *FindRemovedCommandOption(std::string_view flag);
ConfigValidationResult ValidateRemovedCommandOption(std::string_view flag);

}  // namespace objc3c::config
