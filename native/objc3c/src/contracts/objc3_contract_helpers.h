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
    case Objc3NativeContractId::kCanonicalLanguageProfileV1:
      return {contract_id, Objc3NativeContractIdSpelling(contract_id),
              "config", "v1", true};
    case Objc3NativeContractId::kCanonicalFeatureStateCatalogV1:
      return {contract_id, Objc3NativeContractIdSpelling(contract_id),
              "config", "v1", true};
    case Objc3NativeContractId::kRemovedOptionValidationV1:
      return {contract_id, Objc3NativeContractIdSpelling(contract_id),
              "config", "v1", true};
    case Objc3NativeContractId::kRuntimeMetadataSourceOwnershipFreezeV1:
    case Objc3NativeContractId::kRuntimeMetadataSectionAbiSymbolPolicyFreezeV1:
    case Objc3NativeContractId::kRuntimeMetadataSectionPublicationV1:
    case Objc3NativeContractId::kRuntimeMetadataObjectInspectionHarnessV1:
    case Objc3NativeContractId::kRuntimeMetadataSourceToSectionMatrixV1:
    case Objc3NativeContractId::kRuntimeMetadataEmissionGateV1:
    case Objc3NativeContractId::kRuntimeMetadataObjectEmissionCloseoutV1:
      return {contract_id, Objc3NativeContractIdSpelling(contract_id),
              "runtime-metadata", "v1", true};
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
