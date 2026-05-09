#pragma once

#include "contracts/objc3_contract_description_fields.h"
#include "contracts/objc3_contract_descriptor_builder.h"
#include "contracts/objc3_native_contract_ids.h"

inline constexpr Objc3NativeContractDescriptor DescribeObjc3FrontendContract(
    Objc3NativeContractId contract_id) {
  switch (contract_id) {
    case Objc3NativeContractId::kDiagnosticPayloadV1:
      return BuildObjc3NativeContractDescriptor(
          contract_id, kObjc3DiagnosticPayloadContractOwner,
          kObjc3ContractVersionV1);
    case Objc3NativeContractId::kFrontendDiagnosticsBusV1:
      return BuildObjc3NativeContractDescriptor(
          contract_id, kObjc3FrontendDiagnosticsBusContractOwner,
          kObjc3ContractVersionV1);
    default:
      return {};
  }
}
