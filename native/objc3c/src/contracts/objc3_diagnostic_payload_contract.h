#pragma once

#include <string_view>

#include "contracts/objc3_contract_helpers.h"
#include "contracts/objc3_native_contract_ids.h"

struct Objc3DiagnosticPayloadContract {
  Objc3NativeContractDescriptor descriptor =
      DescribeObjc3NativeContract(Objc3NativeContractId::kDiagnosticPayloadV1);
  std::string_view contract_id =
      Objc3NativeContractIdSpelling(
          Objc3NativeContractId::kDiagnosticPayloadV1);
  std::string_view severity_field = "severity";
  std::string_view line_field = "line";
  std::string_view column_field = "column";
  std::string_view code_field = "code";
  std::string_view message_field = "message";
  bool requires_native_code = true;
};

inline constexpr Objc3DiagnosticPayloadContract
    kObjc3DiagnosticPayloadContractV1{};

inline constexpr bool Objc3DiagnosticPayloadContractIsValid(
    const Objc3DiagnosticPayloadContract &contract) {
  return Objc3NativeContractDescriptorIsValid(contract.descriptor) &&
         Objc3ContractIdMatches(
             contract.contract_id,
             Objc3NativeContractId::kDiagnosticPayloadV1) &&
         contract.severity_field == "severity" &&
         contract.line_field == "line" && contract.column_field == "column" &&
         contract.code_field == "code" &&
         contract.message_field == "message" &&
         contract.requires_native_code;
}
