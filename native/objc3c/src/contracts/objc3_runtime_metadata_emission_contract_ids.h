#pragma once

#include "contracts/objc3_native_contract_ids.h"

namespace objc3c::contracts {

inline constexpr Objc3NativeContractId
    kObjc3RuntimeMetadataEmissionGateContract =
        Objc3NativeContractId::kRuntimeMetadataEmissionGateV1;
inline constexpr Objc3NativeContractId
    kObjc3RuntimeMetadataObjectEmissionCloseoutContract =
        Objc3NativeContractId::kRuntimeMetadataObjectEmissionCloseoutV1;

inline constexpr const char *kObjc3RuntimeMetadataEmissionGateContractId =
    Objc3NativeContractIdSpelling(kObjc3RuntimeMetadataEmissionGateContract)
        .data();
inline constexpr const char
    *kObjc3RuntimeMetadataObjectEmissionCloseoutContractId =
        Objc3NativeContractIdSpelling(
            kObjc3RuntimeMetadataObjectEmissionCloseoutContract)
            .data();

}  // namespace objc3c::contracts
