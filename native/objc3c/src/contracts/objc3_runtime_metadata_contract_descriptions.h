#pragma once

#include "contracts/objc3_native_contract_descriptor.h"
#include "contracts/objc3_native_contract_ids.h"

inline constexpr Objc3NativeContractDescriptor
DescribeObjc3RuntimeMetadataContract(Objc3NativeContractId contract_id) {
  switch (contract_id) {
    case Objc3NativeContractId::kRuntimeMetadataSourceOwnershipFreezeV1:
    case Objc3NativeContractId::kRuntimeMetadataSectionAbiSymbolPolicyFreezeV1:
    case Objc3NativeContractId::kRuntimeMetadataSectionPublicationV1:
    case Objc3NativeContractId::kRuntimeMetadataObjectInspectionHarnessV1:
    case Objc3NativeContractId::kRuntimeMetadataSourceToSectionMatrixV1:
    case Objc3NativeContractId::kRuntimeMetadataEmissionGateV1:
    case Objc3NativeContractId::kRuntimeMetadataObjectEmissionCloseoutV1:
      return {contract_id, Objc3NativeContractIdSpelling(contract_id),
              "runtime-metadata", "v1", true};
    default:
      return {};
  }
}
