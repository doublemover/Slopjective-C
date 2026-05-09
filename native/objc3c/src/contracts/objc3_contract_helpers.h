#pragma once

#include <string_view>

#include "contracts/objc3_native_contract_ids.h"

struct Objc3NativeContractDescriptor {
  Objc3NativeContractId id = Objc3NativeContractId::kDiagnosticPayloadV1;
  std::string_view spelling;
  std::string_view owner;
  std::string_view version;
  bool valid = false;
};

inline constexpr Objc3NativeContractDescriptor DescribeObjc3NativeContract(
    Objc3NativeContractId contract_id) {
  switch (contract_id) {
    case Objc3NativeContractId::kDiagnosticPayloadV1:
      return {contract_id, Objc3NativeContractIdSpelling(contract_id),
              "diagnostics", "v1", true};
    case Objc3NativeContractId::kFrontendDiagnosticsBusV1:
      return {contract_id, Objc3NativeContractIdSpelling(contract_id),
              "frontend-diagnostics-bus", "v1", true};
    case Objc3NativeContractId::kCanonicalLanguageConfigV1:
      return {contract_id, Objc3NativeContractIdSpelling(contract_id),
              "config", "v1", true};
    case Objc3NativeContractId::kRemovedOptionValidationV1:
      return {contract_id, Objc3NativeContractIdSpelling(contract_id),
              "config", "v1", true};
  }
  return {};
}

inline constexpr bool Objc3ContractIdMatches(
    std::string_view candidate,
    Objc3NativeContractId contract_id) {
  return Objc3NativeContractIdIsKnown(contract_id) &&
         candidate == Objc3NativeContractIdSpelling(contract_id);
}

inline constexpr bool Objc3NativeContractDescriptorIsValid(
    const Objc3NativeContractDescriptor &descriptor) {
  return descriptor.valid &&
         Objc3ContractIdMatches(descriptor.spelling, descriptor.id) &&
         !descriptor.owner.empty() && !descriptor.version.empty();
}
