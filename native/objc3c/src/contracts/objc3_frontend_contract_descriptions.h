#pragma once

#include "contracts/objc3_native_contract_descriptor.h"
#include "contracts/objc3_native_contract_ids.h"

inline constexpr Objc3NativeContractDescriptor DescribeObjc3FrontendContract(
    Objc3NativeContractId contract_id) {
  switch (contract_id) {
    case Objc3NativeContractId::kDiagnosticPayloadV1:
      return {contract_id, Objc3NativeContractIdSpelling(contract_id),
              "diagnostics", "v1", true};
    case Objc3NativeContractId::kFrontendDiagnosticsBusV1:
      return {contract_id, Objc3NativeContractIdSpelling(contract_id),
              "frontend-diagnostics-bus", "v1", true};
    default:
      return {};
  }
}
