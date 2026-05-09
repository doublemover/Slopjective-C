#pragma once

#include <array>

#include "contracts/objc3_contract_description_fields.h"
#include "contracts/objc3_native_contract_descriptor.h"

inline constexpr std::array<Objc3NativeContractDescriptor, 2>
    kObjc3FrontendContractDescriptors = {{
        {Objc3NativeContractId::kDiagnosticPayloadV1,
         Objc3NativeContractIdSpelling(
             Objc3NativeContractId::kDiagnosticPayloadV1),
         kObjc3DiagnosticPayloadContractOwner, kObjc3ContractVersionV1, true},
        {Objc3NativeContractId::kFrontendDiagnosticsBusV1,
         Objc3NativeContractIdSpelling(
             Objc3NativeContractId::kFrontendDiagnosticsBusV1),
         kObjc3FrontendDiagnosticsBusContractOwner, kObjc3ContractVersionV1,
         true},
    }};
