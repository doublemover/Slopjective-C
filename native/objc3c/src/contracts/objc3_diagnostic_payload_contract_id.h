#pragma once

#include <string_view>

#include "contracts/objc3_native_contract_ids.h"

namespace objc3c::contracts {

inline constexpr Objc3NativeContractId kObjc3DiagnosticPayloadContract =
    Objc3NativeContractId::kDiagnosticPayloadV1;
inline constexpr const char *kObjc3DiagnosticPayloadContractId =
    Objc3NativeContractIdSpelling(kObjc3DiagnosticPayloadContract).data();

}  // namespace objc3c::contracts

inline constexpr std::string_view Objc3DiagnosticPayloadContractId() {
  return objc3c::contracts::kObjc3DiagnosticPayloadContractId;
}
