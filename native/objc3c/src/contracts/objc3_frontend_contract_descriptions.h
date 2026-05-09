#pragma once

#include "contracts/objc3_frontend_contract_descriptor_table.h"

inline constexpr Objc3NativeContractDescriptor DescribeObjc3FrontendContract(
    Objc3NativeContractId contract_id) {
  for (const auto &descriptor : kObjc3FrontendContractDescriptors) {
    if (descriptor.id == contract_id) {
      return descriptor;
    }
  }
  return {};
}
