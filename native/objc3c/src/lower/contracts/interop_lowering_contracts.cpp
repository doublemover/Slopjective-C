#include "lower/contracts/interop_lowering_contracts.h"

#include "lower/metadata/lowering_metadata_helpers.h"

#include <sstream>
#include <string>

bool IsValidObjc3InteropInteropLoweringContract(
    const Objc3InteropInteropLoweringContract &contract) {
  if (contract.c_foreign_callable_sites > contract.foreign_callable_sites ||
      contract.objc_runtime_parity_callable_sites >
          contract.foreign_callable_sites ||
      contract.interface_preserved_foreign_callable_sites <
          contract.foreign_callable_sites) {
    return false;
  }
  if (contract.contract_violation_sites > contract.guard_blocked_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3InteropInteropLoweringReplayKey(
    const Objc3InteropInteropLoweringContract &contract) {
  return std::string("foreign_callable_sites=") +
             std::to_string(contract.foreign_callable_sites) +
         ";c_foreign_callable_sites=" +
         std::to_string(contract.c_foreign_callable_sites) +
         ";objc_runtime_parity_callable_sites=" +
         std::to_string(contract.objc_runtime_parity_callable_sites) +
         ";ownership_bridge_callable_sites=" +
         std::to_string(contract.ownership_bridge_callable_sites) +
         ";error_surface_sites=" +
         std::to_string(contract.error_surface_sites) +
         ";async_boundary_sites=" +
         std::to_string(contract.async_boundary_sites) +
         ";swift_concurrency_metadata_sites=" +
         std::to_string(contract.swift_concurrency_metadata_sites) +
         ";interface_preserved_foreign_callable_sites=" +
         std::to_string(contract.interface_preserved_foreign_callable_sites) +
         ";interface_preserved_metadata_annotation_sites=" +
         std::to_string(contract.interface_preserved_metadata_annotation_sites) +
         ";guard_blocked_sites=" +
         std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3InteropInteropLoweringLaneContract;
}

bool IsValidObjc3InteropForeignCallLifetimeLoweringContract(
    const Objc3InteropForeignCallLifetimeLoweringContract &contract) {
  if (contract.c_foreign_callable_sites > contract.foreign_callable_sites ||
      contract.objc_runtime_parity_callable_sites >
          contract.foreign_callable_sites) {
    return false;
  }
  if (contract.ownership_bridge_sites > contract.metadata_preservation_sites ||
      contract.lifetime_bridge_sites > contract.metadata_preservation_sites) {
    return false;
  }
  if (contract.contract_violation_sites > contract.guard_blocked_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3InteropForeignCallLifetimeLoweringReplayKey(
    const Objc3InteropForeignCallLifetimeLoweringContract &contract) {
  return std::string("foreign_callable_sites=") +
             std::to_string(contract.foreign_callable_sites) +
         ";c_foreign_callable_sites=" +
         std::to_string(contract.c_foreign_callable_sites) +
         ";objc_runtime_parity_callable_sites=" +
         std::to_string(contract.objc_runtime_parity_callable_sites) +
         ";ownership_bridge_sites=" +
         std::to_string(contract.ownership_bridge_sites) +
         ";lifetime_bridge_sites=" +
         std::to_string(contract.lifetime_bridge_sites) +
         ";metadata_preservation_sites=" +
         std::to_string(contract.metadata_preservation_sites) +
         ";guard_blocked_sites=" +
         std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";dependency_contract=" +
         kObjc3InteropForeignCallLifetimeLoweringDependencyContractId;
}

bool IsValidObjc3InteropFfiMetadataInterfacePreservationContract(
    const Objc3InteropFfiMetadataInterfacePreservationContract &contract) {
  if (contract.imported_module_count == 0 &&
      (contract.imported_foreign_callable_count != 0 ||
       contract.imported_metadata_preservation_sites != 0 ||
       contract.imported_interface_annotation_sites != 0)) {
    return false;
  }
  if (contract.separate_compilation_preservation_ready &&
      !contract.runtime_import_artifact_ready) {
    return false;
  }
  if (contract.separate_compilation_preservation_ready &&
      !contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3InteropFfiMetadataInterfacePreservationReplayKey(
    const Objc3InteropFfiMetadataInterfacePreservationContract &contract) {
  return std::string("local_foreign_callable_count=") +
             std::to_string(contract.local_foreign_callable_count) +
         ";local_metadata_preservation_sites=" +
         std::to_string(contract.local_metadata_preservation_sites) +
         ";local_interface_annotation_sites=" +
         std::to_string(contract.local_interface_annotation_sites) +
         ";imported_module_count=" +
         std::to_string(contract.imported_module_count) +
         ";imported_foreign_callable_count=" +
         std::to_string(contract.imported_foreign_callable_count) +
         ";imported_metadata_preservation_sites=" +
         std::to_string(contract.imported_metadata_preservation_sites) +
         ";imported_interface_annotation_sites=" +
         std::to_string(contract.imported_interface_annotation_sites) +
         ";runtime_import_artifact_ready=" +
         BoolToken(contract.runtime_import_artifact_ready) +
         ";separate_compilation_preservation_ready=" +
         BoolToken(contract.separate_compilation_preservation_ready) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";source_contract=" +
         kObjc3InteropFfiMetadataInterfacePreservationSourceContractId;
}

std::string Objc3InteropBridgePackagingToolchainSummary() {
  std::ostringstream out;
  // packaging/toolchain anchor: lane-D freezes one truthful Part 11
  // packaging boundary over the already-landed runtime-import-surface and
  // cross-module link-plan topology without claiming live header/module/bridge
  // generation yet.
  out << "contract=" << kObjc3InteropBridgePackagingToolchainContractId
      << ";source_contract="
      << kObjc3InteropBridgePackagingToolchainSourceContractId
      << ";preservation_contract="
      << kObjc3InteropBridgePackagingToolchainPreservationContractId
      << ";packaging_model="
      << kObjc3InteropBridgePackagingToolchainPackagingModel
      << ";operator_evidence_model="
      << kObjc3InteropBridgePackagingToolchainEvidenceModel
      << ";fail_closed_model="
      << kObjc3InteropBridgePackagingToolchainFailClosedModel
      << ";packaging_topology_ready=true"
      << ";operator_visible_evidence_ready=true"
      << ";header_generation_ready=false"
      << ";module_generation_ready=false"
      << ";bridge_generation_ready=false"
      << ";deterministic=true"
      << ";follow_on_surface=objc3c.interop.bridgegeneration.runtime.v1";
  return out.str();
}

std::string Objc3InteropHeaderModuleBridgeGenerationBoundarySummary() {
  std::ostringstream out;
  out << "contract="
      << kObjc3InteropHeaderModuleBridgeGenerationLoweringContractId
      << ";source_contract="
      << kObjc3InteropHeaderModuleBridgeGenerationLoweringSourceContractId
      << ";preservation_contract="
      << kObjc3InteropHeaderModuleBridgeGenerationLoweringPreservationContractId
      << ";packaging_model="
      << kObjc3InteropHeaderModuleBridgeGenerationLoweringPackagingModel
      << ";fail_closed_model="
      << kObjc3InteropHeaderModuleBridgeGenerationLoweringFailClosedModel
      << ";header_generation_ready=true"
      << ";module_generation_ready=true"
      << ";bridge_generation_ready=true"
      << ";cross_module_packaging_ready=true"
      << ";deterministic=true";
  return out.str();
}
