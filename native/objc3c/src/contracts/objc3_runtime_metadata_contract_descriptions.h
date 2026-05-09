#pragma once

#include "contracts/objc3_runtime_metadata_contract_descriptor_table.h"

inline constexpr Objc3NativeContractDescriptor
DescribeObjc3RuntimeMetadataContract(Objc3NativeContractId contract_id) {
  for (const auto &descriptor : kObjc3RuntimeMetadataContractDescriptors) {
    if (descriptor.id == contract_id) {
      return descriptor;
    }
  }
  return {};
}
