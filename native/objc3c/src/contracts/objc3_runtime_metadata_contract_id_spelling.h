#pragma once

#include <string_view>

#include "contracts/objc3_native_contract_id_kind.h"

inline constexpr std::string_view Objc3RuntimeMetadataContractIdSpelling(
    Objc3NativeContractId contract_id) {
  switch (contract_id) {
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
    default:
      return {};
  }
}
