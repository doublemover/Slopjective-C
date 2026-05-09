#pragma once

#include <cstdint>
#include <string_view>

enum class Objc3NativeContractId : std::uint8_t {
  kDiagnosticPayloadV1,
  kFrontendDiagnosticsBusV1,
  kCanonicalLanguageProfileV1,
  kCanonicalFeatureStateCatalogV1,
  kRemovedOptionValidationV1,
  kRuntimeMetadataSourceOwnershipFreezeV1,
  kRuntimeMetadataSectionAbiSymbolPolicyFreezeV1,
  kRuntimeMetadataSectionPublicationV1,
  kRuntimeMetadataObjectInspectionHarnessV1,
  kRuntimeMetadataSourceToSectionMatrixV1,
  kRuntimeMetadataEmissionGateV1,
  kRuntimeMetadataObjectEmissionCloseoutV1,
};

inline constexpr std::string_view Objc3NativeContractIdSpelling(
    Objc3NativeContractId contract_id) {
  switch (contract_id) {
    case Objc3NativeContractId::kDiagnosticPayloadV1:
      return "objc3c.diagnostic.payload.v1";
    case Objc3NativeContractId::kFrontendDiagnosticsBusV1:
      return "objc3c.frontend.diagnostics_bus.v1";
    case Objc3NativeContractId::kCanonicalLanguageProfileV1:
      return "objc3c.config.language_profile.canonical.v1";
    case Objc3NativeContractId::kCanonicalFeatureStateCatalogV1:
      return "objc3c.config.feature_state_catalog.canonical.v1";
    case Objc3NativeContractId::kRemovedOptionValidationV1:
      return "objc3c.config.removed_option_validation.v1";
    case Objc3NativeContractId::kRuntimeMetadataSourceOwnershipFreezeV1:
      return "objc3c.runtime.metadata.source.ownership.freeze.v1";
    case Objc3NativeContractId::kRuntimeMetadataSectionAbiSymbolPolicyFreezeV1:
      return "objc3c.runtime.metadata.section.abi.symbol.policy.freeze.v1";
    case Objc3NativeContractId::kRuntimeMetadataSectionPublicationV1:
      return "objc3c.runtime.metadata.section.publication.v1";
    case Objc3NativeContractId::kRuntimeMetadataObjectInspectionHarnessV1:
      return "objc3c.runtime.metadata.object.inspection.harness.v1";
    case Objc3NativeContractId::kRuntimeMetadataSourceToSectionMatrixV1:
      return "objc3c.runtime.metadata.source.to.section.matrix.v1";
    case Objc3NativeContractId::kRuntimeMetadataEmissionGateV1:
      return "objc3c.runtime.metadata.emission.gate.v1";
    case Objc3NativeContractId::kRuntimeMetadataObjectEmissionCloseoutV1:
      return "objc3c.runtime.cross.lane.object.emission.closeout.v1";
  }
  return {};
}

inline constexpr bool Objc3NativeContractIdIsKnown(
    Objc3NativeContractId contract_id) {
  return !Objc3NativeContractIdSpelling(contract_id).empty();
}
