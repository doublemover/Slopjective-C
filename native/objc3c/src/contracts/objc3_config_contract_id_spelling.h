#pragma once

#include <string_view>

#include "contracts/objc3_native_contract_id_kind.h"

inline constexpr std::string_view Objc3ConfigContractIdSpelling(
    Objc3NativeContractId contract_id) {
  switch (contract_id) {
    case Objc3NativeContractId::kCanonicalLanguageProfileV1:
      return "objc3c.config.language_profile.canonical.v1";
    case Objc3NativeContractId::kCanonicalFeatureStateCatalogV1:
      return "objc3c.config.feature_state_catalog.canonical.v1";
    case Objc3NativeContractId::kRemovedOptionValidationV1:
      return "objc3c.config.removed_option_validation.v1";
    default:
      return {};
  }
}
