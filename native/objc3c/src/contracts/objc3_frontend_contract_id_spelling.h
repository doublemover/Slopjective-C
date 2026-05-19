#pragma once

#include <string_view>

#include "contracts/objc3_native_contract_id_kind.h"

inline constexpr std::string_view Objc3FrontendContractIdSpelling(
    Objc3NativeContractId contract_id) {
  switch (contract_id) {
    case Objc3NativeContractId::kDiagnosticPayloadV1:
      return "objc3c.diagnostic.payload.v1";
    case Objc3NativeContractId::kFrontendDiagnosticsBusV1:
      return "objc3c.frontend.diagnostics_bus.v1";
    default:
      return {};
  }
}
