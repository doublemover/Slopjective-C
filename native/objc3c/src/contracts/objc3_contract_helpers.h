#pragma once

#include <string_view>

#include "contracts/objc3_native_contract_ids.h"

struct Objc3NativeContractDescriptor {
  Objc3NativeContractId id =
      Objc3NativeContractId::kDiagnosticPayloadV1;
  std::string_view spelling =
      Objc3NativeContractIdSpelling(
          Objc3NativeContractId::kDiagnosticPayloadV1);
  std::string_view owner = "native";
  std::string_view version = "v1";
};

inline constexpr Objc3NativeContractDescriptor DescribeObjc3NativeContract(
    Objc3NativeContractId contract_id) {
  switch (contract_id) {
    case Objc3NativeContractId::kDiagnosticPayloadV1:
      return {contract_id, Objc3NativeContractIdSpelling(contract_id),
              "diagnostics", "v1"};
    case Objc3NativeContractId::kFrontendDiagnosticsBusV1:
      return {contract_id, Objc3NativeContractIdSpelling(contract_id),
              "frontend-diagnostics-bus", "v1"};
    case Objc3NativeContractId::kCanonicalLanguageConfigV1:
      return {contract_id, Objc3NativeContractIdSpelling(contract_id),
              "config", "v1"};
    case Objc3NativeContractId::kRemovedOptionValidationV1:
      return {contract_id, Objc3NativeContractIdSpelling(contract_id),
              "config", "v1"};
  }
  return {};
}

inline constexpr bool Objc3ContractIdMatches(
    std::string_view candidate,
    Objc3NativeContractId contract_id) {
  return candidate == Objc3NativeContractIdSpelling(contract_id);
}
