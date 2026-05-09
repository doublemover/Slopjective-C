#pragma once

#include "contracts/objc3_native_contract_descriptor.h"
#include "contracts/objc3_native_contract_ids.h"

inline constexpr Objc3NativeContractDescriptor DescribeObjc3ConfigContract(
    Objc3NativeContractId contract_id) {
  switch (contract_id) {
    case Objc3NativeContractId::kCanonicalLanguageProfileV1:
    case Objc3NativeContractId::kCanonicalFeatureStateCatalogV1:
    case Objc3NativeContractId::kRemovedOptionValidationV1:
      return {contract_id, Objc3NativeContractIdSpelling(contract_id),
              "config", "v1", true};
    default:
      return {};
  }
}
