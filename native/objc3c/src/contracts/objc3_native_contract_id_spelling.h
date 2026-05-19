#pragma once

#include <string_view>

#include "contracts/objc3_config_contract_id_spelling.h"
#include "contracts/objc3_frontend_contract_id_spelling.h"
#include "contracts/objc3_native_contract_id_kind.h"
#include "contracts/objc3_runtime_metadata_contract_id_spelling.h"

inline constexpr std::string_view Objc3NativeContractIdSpelling(
    Objc3NativeContractId contract_id) {
  if (const auto frontend = Objc3FrontendContractIdSpelling(contract_id);
      !frontend.empty()) {
    return frontend;
  }
  if (const auto config = Objc3ConfigContractIdSpelling(contract_id);
      !config.empty()) {
    return config;
  }
  if (const auto runtime_metadata =
          Objc3RuntimeMetadataContractIdSpelling(contract_id);
      !runtime_metadata.empty()) {
    return runtime_metadata;
  }
  return {};
}
