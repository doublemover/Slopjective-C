#pragma once

#include "contracts/objc3_config_contract_descriptions.h"
#include "contracts/objc3_frontend_contract_descriptions.h"
#include "contracts/objc3_native_contract_descriptor.h"
#include "contracts/objc3_native_contract_ids.h"
#include "contracts/objc3_runtime_metadata_contract_descriptions.h"

inline constexpr Objc3NativeContractDescriptor DescribeObjc3NativeContract(
    Objc3NativeContractId contract_id) {
  if (const auto frontend = DescribeObjc3FrontendContract(contract_id);
      frontend.valid) {
    return frontend;
  }
  if (const auto config = DescribeObjc3ConfigContract(contract_id);
      config.valid) {
    return config;
  }
  if (const auto runtime_metadata =
          DescribeObjc3RuntimeMetadataContract(contract_id);
      runtime_metadata.valid) {
    return runtime_metadata;
  }
  return {};
}
