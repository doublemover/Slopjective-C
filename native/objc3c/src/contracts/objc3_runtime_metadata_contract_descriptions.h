#pragma once

#include "contracts/objc3_contract_description_fields.h"
#include "contracts/objc3_contract_descriptor_builder.h"
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
      return BuildObjc3NativeContractDescriptor(
          contract_id, kObjc3RuntimeMetadataContractOwner,
          kObjc3ContractVersionV1);
    default:
      return {};
  }
}
