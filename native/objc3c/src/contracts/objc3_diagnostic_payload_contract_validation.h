#pragma once

#include "contracts/objc3_contract_helpers.h"
#include "contracts/objc3_diagnostic_payload_contract_fields.h"
#include "contracts/objc3_diagnostic_payload_contract_id.h"
#include "contracts/objc3_diagnostic_payload_contract_record.h"
#include "contracts/objc3_native_contract_ids.h"

inline constexpr bool Objc3DiagnosticPayloadContractIsValid(
    const Objc3DiagnosticPayloadContract &contract) {
  return Objc3NativeContractDescriptorIsValid(contract.descriptor) &&
         Objc3ContractIdMatches(
             contract.contract_id,
             objc3c::contracts::kObjc3DiagnosticPayloadContract) &&
         contract.severity_field == kObjc3DiagnosticPayloadSeverityField &&
         contract.line_field == kObjc3DiagnosticPayloadLineField &&
         contract.column_field == kObjc3DiagnosticPayloadColumnField &&
         contract.code_field == kObjc3DiagnosticPayloadCodeField &&
         contract.message_field == kObjc3DiagnosticPayloadMessageField &&
         contract.requires_native_code;
}
