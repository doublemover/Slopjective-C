#pragma once

#include <string_view>

#include "contracts/objc3_native_contract_ids.h"

inline constexpr std::string_view Objc3FrontendDiagnosticsBusContractId() {
  return Objc3NativeContractIdSpelling(
      Objc3NativeContractId::kFrontendDiagnosticsBusV1);
}
