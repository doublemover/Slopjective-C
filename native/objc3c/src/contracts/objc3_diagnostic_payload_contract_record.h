#pragma once

#include <string_view>

#include "contracts/objc3_contract_helpers.h"
#include "contracts/objc3_diagnostic_payload_contract_id.h"
#include "contracts/objc3_native_contract_ids.h"

struct Objc3DiagnosticPayloadContract {
  Objc3NativeContractDescriptor descriptor =
      DescribeObjc3NativeContract(
          objc3c::contracts::kObjc3DiagnosticPayloadContract);
  std::string_view contract_id = Objc3DiagnosticPayloadContractId();
  std::string_view severity_field = "severity";
  std::string_view line_field = "line";
  std::string_view column_field = "column";
  std::string_view code_field = "code";
  std::string_view message_field = "message";
  bool requires_native_code = true;
};

inline constexpr Objc3DiagnosticPayloadContract
    kObjc3DiagnosticPayloadContractV1{};
