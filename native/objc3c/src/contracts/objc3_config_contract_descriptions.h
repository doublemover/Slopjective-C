#pragma once

#include "contracts/objc3_contract_description_fields.h"
#include "contracts/objc3_contract_descriptor_builder.h"
#include "contracts/objc3_native_contract_ids.h"

inline constexpr Objc3NativeContractDescriptor DescribeObjc3ConfigContract(
    Objc3NativeContractId contract_id) {
  switch (contract_id) {
    case Objc3NativeContractId::kCanonicalLanguageProfileV1:
    case Objc3NativeContractId::kCanonicalFeatureStateCatalogV1:
    case Objc3NativeContractId::kRemovedOptionValidationV1:
      return BuildObjc3NativeContractDescriptor(
          contract_id, kObjc3ConfigContractOwner, kObjc3ContractVersionV1);
    default:
      return {};
  }
}
