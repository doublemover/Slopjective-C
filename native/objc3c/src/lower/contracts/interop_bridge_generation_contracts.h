#pragma once

#include "lower/contracts/interop_ffi_metadata_contracts.h"

#include <string>

// Interop bridge generation owns the toolchain-visible packaging topology,
// bridge/header/module artifact generation boundary, and fail-closed evidence
// model for live interop bridge publication.
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainContractId =
        "objc3c.interop.bridge.packaging.and.toolchain.contract.v1";
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainSourceContractId =
        kObjc3InteropFfiMetadataInterfacePreservationContractId;
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainPreservationContractId =
        "objc3c.interop.foreign.surface.interface.preservation.v1";
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainPackagingModel =
        "runtime-registration-manifests-runtime-import-surfaces-cross-module-link-plans-and-linker-response-sidecars-are-the-current-toolchain-visible-interop-packaging-topology";
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainEvidenceModel =
        "operator-visible-interop-evidence-is-published-through-the-packaged-runtime-archive-registration-manifest-cross-module-link-plan-and-ir-summary";
inline constexpr const char
    *kObjc3InteropBridgePackagingToolchainFailClosedModel =
        "header-module-and-bridge-generation-remain-unclaimed-until-next-runtime-phase";

inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationLoweringContractId =
        "objc3c.interop.header.module.and.bridge.generation.v1";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationLoweringSourceContractId =
        "objc3c.interop.bridge.packaging.and.toolchain.contract.v1";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationLoweringPreservationContractId =
        "objc3c.interop.ffi.metadata.interface.preservation.v1";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationLoweringPackagingModel =
        "compiler-emits-deterministic-interop-bridge-header-modulemap-and-bridge-json-artifacts-and-preserves-them-through-runtime-import-surfaces-and-cross-module-link-plans";
inline constexpr const char
    *kObjc3InteropHeaderModuleBridgeGenerationLoweringFailClosedModel =
        "missing-generated-bridge-artifacts-or-drifted-import-surface-paths-disable-live-interop-bridge-generation-claims";

struct Objc3InteropBridgePackagingToolchainContract {
  bool packaging_topology_ready = false;
  bool operator_visible_evidence_ready = false;
  bool header_generation_ready = false;
  bool module_generation_ready = false;
  bool bridge_generation_ready = false;
  bool deterministic = false;
};

std::string Objc3InteropBridgePackagingToolchainSummary();
std::string Objc3InteropHeaderModuleBridgeGenerationBoundarySummary();
