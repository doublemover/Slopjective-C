#pragma once

#include <array>

#include "contracts/objc3_contract_description_fields.h"
#include "contracts/objc3_native_contract_descriptor.h"

inline constexpr std::array<Objc3NativeContractDescriptor, 3>
    kObjc3ConfigContractDescriptors = {{
        {Objc3NativeContractId::kCanonicalLanguageProfileV1,
         Objc3NativeContractIdSpelling(
             Objc3NativeContractId::kCanonicalLanguageProfileV1),
         kObjc3ConfigContractOwner, kObjc3ContractVersionV1, true},
        {Objc3NativeContractId::kCanonicalFeatureStateCatalogV1,
         Objc3NativeContractIdSpelling(
             Objc3NativeContractId::kCanonicalFeatureStateCatalogV1),
         kObjc3ConfigContractOwner, kObjc3ContractVersionV1, true},
        {Objc3NativeContractId::kRemovedOptionValidationV1,
         Objc3NativeContractIdSpelling(
             Objc3NativeContractId::kRemovedOptionValidationV1),
         kObjc3ConfigContractOwner, kObjc3ContractVersionV1, true},
    }};
