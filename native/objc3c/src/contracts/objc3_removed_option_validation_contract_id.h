#pragma once

#include "contracts/objc3_native_contract_ids.h"

namespace objc3c::contracts {

inline constexpr Objc3NativeContractId kObjc3RemovedOptionValidationContract =
    Objc3NativeContractId::kRemovedOptionValidationV1;
inline constexpr const char *kObjc3RemovedOptionValidationContractId =
    Objc3NativeContractIdSpelling(kObjc3RemovedOptionValidationContract)
        .data();

}  // namespace objc3c::contracts
