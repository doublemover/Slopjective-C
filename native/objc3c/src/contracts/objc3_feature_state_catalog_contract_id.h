#pragma once

#include "contracts/objc3_native_contract_ids.h"

namespace objc3c::contracts {

inline constexpr Objc3NativeContractId
    kObjc3CanonicalFeatureStateCatalogContract =
        Objc3NativeContractId::kCanonicalFeatureStateCatalogV1;
inline constexpr const char *kObjc3CanonicalFeatureStateCatalogContractId =
    Objc3NativeContractIdSpelling(kObjc3CanonicalFeatureStateCatalogContract)
        .data();

}  // namespace objc3c::contracts
