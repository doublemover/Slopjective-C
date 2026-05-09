#pragma once

#include "contracts/objc3_native_contract_ids.h"

namespace objc3c::contracts {

inline constexpr Objc3NativeContractId
    kObjc3RuntimeMetadataSourceOwnershipContract =
        Objc3NativeContractId::kRuntimeMetadataSourceOwnershipFreezeV1;
inline constexpr Objc3NativeContractId kObjc3RuntimeMetadataSectionAbiContract =
    Objc3NativeContractId::kRuntimeMetadataSectionAbiSymbolPolicyFreezeV1;
inline constexpr Objc3NativeContractId
    kObjc3RuntimeMetadataSectionPublicationContract =
        Objc3NativeContractId::kRuntimeMetadataSectionPublicationV1;
inline constexpr Objc3NativeContractId
    kObjc3RuntimeMetadataObjectInspectionContract =
        Objc3NativeContractId::kRuntimeMetadataObjectInspectionHarnessV1;
inline constexpr Objc3NativeContractId
    kObjc3RuntimeMetadataSourceToSectionMatrixContract =
        Objc3NativeContractId::kRuntimeMetadataSourceToSectionMatrixV1;

inline constexpr const char *kObjc3RuntimeMetadataSourceOwnershipContractId =
    Objc3NativeContractIdSpelling(
        kObjc3RuntimeMetadataSourceOwnershipContract)
        .data();
inline constexpr const char *kObjc3RuntimeMetadataSectionAbiContractId =
    Objc3NativeContractIdSpelling(kObjc3RuntimeMetadataSectionAbiContract)
        .data();
inline constexpr const char *kObjc3RuntimeMetadataSectionPublicationContractId =
    Objc3NativeContractIdSpelling(
        kObjc3RuntimeMetadataSectionPublicationContract)
        .data();
inline constexpr const char *kObjc3RuntimeMetadataObjectInspectionContractId =
    Objc3NativeContractIdSpelling(
        kObjc3RuntimeMetadataObjectInspectionContract)
        .data();
inline constexpr const char
    *kObjc3RuntimeMetadataSourceToSectionMatrixContractId =
        Objc3NativeContractIdSpelling(
            kObjc3RuntimeMetadataSourceToSectionMatrixContract)
            .data();

}  // namespace objc3c::contracts
