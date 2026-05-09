#pragma once

#include <cstdint>

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
