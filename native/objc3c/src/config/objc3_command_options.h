#pragma once

#include <cstdint>
#include <span>
#include <string_view>

#include "config/objc3_config_validation.h"
#include "config/objc3_feature_state_catalog.h"
#include "contracts/objc3_removed_option_validation_contract_id.h"

namespace objc3c::config {

enum class RemovedCommandOptionOwner : std::uint8_t {
  kLanguageMode,
  kReporting,
  kRuntime,
};

struct CommandOptionState {
  const char *spelling;
  RemovedCommandOptionOwner owner;
  FeatureState state;
  const char *diagnostic_code;
  const char *summary;
  const char *contract_id =
      objc3c::contracts::kObjc3RemovedOptionValidationContractId;
};

const char *RemovedCommandOptionOwnerName(RemovedCommandOptionOwner owner);
std::span<const CommandOptionState> RemovedCommandOptions();
const CommandOptionState *FindRemovedCommandOption(std::string_view flag);
ConfigValidationResult ValidateRemovedCommandOption(std::string_view flag);

}  // namespace objc3c::config
