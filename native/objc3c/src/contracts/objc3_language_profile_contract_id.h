#pragma once

#include "contracts/objc3_native_contract_ids.h"

namespace objc3c::contracts {

inline constexpr Objc3NativeContractId kObjc3CanonicalLanguageProfileContract =
    Objc3NativeContractId::kCanonicalLanguageProfileV1;
inline constexpr const char *kObjc3CanonicalLanguageProfileContractId =
    Objc3NativeContractIdSpelling(kObjc3CanonicalLanguageProfileContract)
        .data();

}  // namespace objc3c::contracts
