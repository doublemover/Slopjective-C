#pragma once

#include <array>

#include "contracts/objc3_contract_description_fields.h"
#include "contracts/objc3_native_contract_descriptor.h"

inline constexpr std::array<Objc3NativeContractDescriptor, 7>
    kObjc3RuntimeMetadataContractDescriptors = {{
        {Objc3NativeContractId::kRuntimeMetadataSourceOwnershipFreezeV1,
         Objc3NativeContractIdSpelling(
             Objc3NativeContractId::kRuntimeMetadataSourceOwnershipFreezeV1),
         kObjc3RuntimeMetadataContractOwner, kObjc3ContractVersionV1, true},
        {Objc3NativeContractId::kRuntimeMetadataSectionAbiSymbolPolicyFreezeV1,
         Objc3NativeContractIdSpelling(
             Objc3NativeContractId::
                 kRuntimeMetadataSectionAbiSymbolPolicyFreezeV1),
         kObjc3RuntimeMetadataContractOwner, kObjc3ContractVersionV1, true},
        {Objc3NativeContractId::kRuntimeMetadataSectionPublicationV1,
         Objc3NativeContractIdSpelling(
             Objc3NativeContractId::kRuntimeMetadataSectionPublicationV1),
         kObjc3RuntimeMetadataContractOwner, kObjc3ContractVersionV1, true},
        {Objc3NativeContractId::kRuntimeMetadataObjectInspectionHarnessV1,
         Objc3NativeContractIdSpelling(
             Objc3NativeContractId::kRuntimeMetadataObjectInspectionHarnessV1),
         kObjc3RuntimeMetadataContractOwner, kObjc3ContractVersionV1, true},
        {Objc3NativeContractId::kRuntimeMetadataSourceToSectionMatrixV1,
         Objc3NativeContractIdSpelling(
             Objc3NativeContractId::kRuntimeMetadataSourceToSectionMatrixV1),
         kObjc3RuntimeMetadataContractOwner, kObjc3ContractVersionV1, true},
        {Objc3NativeContractId::kRuntimeMetadataEmissionGateV1,
         Objc3NativeContractIdSpelling(
             Objc3NativeContractId::kRuntimeMetadataEmissionGateV1),
         kObjc3RuntimeMetadataContractOwner, kObjc3ContractVersionV1, true},
        {Objc3NativeContractId::kRuntimeMetadataObjectEmissionCloseoutV1,
         Objc3NativeContractIdSpelling(
             Objc3NativeContractId::kRuntimeMetadataObjectEmissionCloseoutV1),
         kObjc3RuntimeMetadataContractOwner, kObjc3ContractVersionV1, true},
    }};
