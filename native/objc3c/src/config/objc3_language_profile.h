#pragma once

#include <cstdint>
#include <string_view>

#include "config/objc3_config_validation.h"
#include "config/objc3_feature_state_catalog.h"
#include "config/objc3_language_version.h"
#include "config/objc3_command_options.h"
#include "contracts/objc3_config_capability_contract.h"

namespace objc3c::config {

enum class LanguageProfileId : std::uint8_t {
  kCanonical,
  kStrict,
  kStrictConcurrency,
};

struct LanguageProfileContract {
  LanguageProfileId id = LanguageProfileId::kCanonical;
  const char *name = kCanonicalLanguageProfileName;
  std::uint8_t language_version = kCanonicalLanguageVersion;
  const char *contract_id =
      objc3c::contracts::kObjc3CanonicalLanguageProfileContractId;
  bool strict_diagnostics = false;
  bool strict_concurrency = false;
};

const LanguageProfileContract &CanonicalLanguageProfile();
const LanguageProfileContract *FindLanguageProfile(std::string_view name);
bool IsCanonicalLanguageProfile(std::string_view name);
bool IsSupportedLanguageProfile(std::string_view name);
const char *LanguageProfileName(LanguageProfileId id);
ConfigValidationResult ValidateLanguageProfileName(std::string_view name);

}  // namespace objc3c::config
