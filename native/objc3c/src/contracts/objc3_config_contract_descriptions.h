#pragma once

#include "contracts/objc3_config_contract_descriptor_table.h"

inline constexpr Objc3NativeContractDescriptor DescribeObjc3ConfigContract(
    Objc3NativeContractId contract_id) {
  for (const auto &descriptor : kObjc3ConfigContractDescriptors) {
    if (descriptor.id == contract_id) {
      return descriptor;
    }
  }
  return {};
}
