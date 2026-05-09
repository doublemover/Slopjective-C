#pragma once

#include "contracts/objc3_contract_helpers.h"
#include "contracts/objc3_diagnostic_payload_contract_record.h"
#include "contracts/objc3_native_contract_ids.h"

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
