#pragma once

#include <cstddef>
#include <string>

// Interop lowering owns the foreign-call ABI, FFI metadata preservation, and
// bridge packaging boundary that survives beyond local lowering artifacts.
inline constexpr const char *kObjc3InteropInteropLoweringContractId =
    "objc3c.interop.interop.lowering.and.abi.contract.v1";
inline constexpr const char *kObjc3InteropInteropLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_interop_interop_lowering_and_abi_contract";
inline constexpr const char *kObjc3InteropInteropLoweringModel =
    "interop-foreign-callable-sema-runtime-parity-cpp-interaction-swift-isolation-and-interface-preservation-packets-now-feed-one-deterministic-lowering-contract-for-manifest-and-ir-carriage";
inline constexpr const char *kObjc3InteropInteropLoweringDeferredModel =
    "live-ffi-call-lowering-ownership-bridge-helper-emission-error-runtime-integration-and-cross-module-runtime-consumption-remain-later-interop-runtime-work";
inline constexpr const char *kObjc3InteropInteropLoweringLaneContract =
    "objc3c.interop.interop.lowering.abi.contract.v1";

inline constexpr const char *kObjc3InteropForeignCallLifetimeLoweringContractId =
    "objc3c.interop.foreign.call.and.lifetime.lowering.v1";
inline constexpr const char *kObjc3InteropForeignCallLifetimeLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_interop_foreign_call_and_lifetime_lowering";
inline constexpr const char *kObjc3InteropForeignCallLifetimeLoweringModel =
    "foreign-calls-and-cpp-swift-facing-free-functions-now-lower-through-one-deterministic-interop-call-boundary-that-preserves-ownership-lifetime-and-annotation-facts-in-manifest-and-ir";
inline constexpr const char
    *kObjc3InteropForeignCallLifetimeLoweringDeferredModel =
        "cross-module-runtime-consumption-live-foreign-linking-and-runnable-host-language-integration-remain-later-interop-closeout-work";
inline constexpr const char
    *kObjc3InteropForeignCallLifetimeLoweringDependencyContractId =
        kObjc3InteropInteropLoweringContractId;

inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationContractId =
        "objc3c.interop.ffi.metadata.interface.preservation.v1";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_interop_ffi_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationImportArtifactMemberName =
        "objc_interop_ffi_metadata_and_interface_preservation";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationSourceContractId =
        kObjc3InteropForeignCallLifetimeLoweringContractId;
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationPreservationModel =
        "provider-runtime-import-surfaces-now-preserve-interop-callable-and-annotation-metadata-beyond-local-manifest-ir-and-object-emission";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationSourceModel =
        "local-lane-c-interop-lowering-and-lane-a-preservation-packets-feed-one-runtime-import-surface-summary-for-separate-compilation-replay";
inline constexpr const char
    *kObjc3InteropFfiMetadataInterfacePreservationFailClosedModel =
        "missing-import-surface-packet-drifted-replay-keys-or-non-deterministic-provider-preservation-disables-cross-module-interop-preservation-claims";

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

struct Objc3InteropInteropLoweringContract {
  std::size_t foreign_callable_sites = 0;
  std::size_t c_foreign_callable_sites = 0;
  std::size_t objc_runtime_parity_callable_sites = 0;
  std::size_t ownership_bridge_callable_sites = 0;
  std::size_t error_surface_sites = 0;
  std::size_t async_boundary_sites = 0;
  std::size_t swift_concurrency_metadata_sites = 0;
  std::size_t interface_preserved_foreign_callable_sites = 0;
  std::size_t interface_preserved_metadata_annotation_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3InteropForeignCallLifetimeLoweringContract {
  std::size_t foreign_callable_sites = 0;
  std::size_t c_foreign_callable_sites = 0;
  std::size_t objc_runtime_parity_callable_sites = 0;
  std::size_t ownership_bridge_sites = 0;
  std::size_t lifetime_bridge_sites = 0;
  std::size_t metadata_preservation_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3InteropFfiMetadataInterfacePreservationContract {
  std::size_t local_foreign_callable_count = 0;
  std::size_t local_metadata_preservation_sites = 0;
  std::size_t local_interface_annotation_sites = 0;
  std::size_t imported_module_count = 0;
  std::size_t imported_foreign_callable_count = 0;
  std::size_t imported_metadata_preservation_sites = 0;
  std::size_t imported_interface_annotation_sites = 0;
  bool runtime_import_artifact_ready = false;
  bool separate_compilation_preservation_ready = false;
  bool deterministic = false;
};

struct Objc3InteropBridgePackagingToolchainContract {
  bool packaging_topology_ready = false;
  bool operator_visible_evidence_ready = false;
  bool header_generation_ready = false;
  bool module_generation_ready = false;
  bool bridge_generation_ready = false;
  bool deterministic = false;
};

bool IsValidObjc3InteropInteropLoweringContract(
    const Objc3InteropInteropLoweringContract &contract);
std::string Objc3InteropInteropLoweringReplayKey(
    const Objc3InteropInteropLoweringContract &contract);
bool IsValidObjc3InteropForeignCallLifetimeLoweringContract(
    const Objc3InteropForeignCallLifetimeLoweringContract &contract);
std::string Objc3InteropForeignCallLifetimeLoweringReplayKey(
    const Objc3InteropForeignCallLifetimeLoweringContract &contract);
bool IsValidObjc3InteropFfiMetadataInterfacePreservationContract(
    const Objc3InteropFfiMetadataInterfacePreservationContract &contract);
std::string Objc3InteropFfiMetadataInterfacePreservationReplayKey(
    const Objc3InteropFfiMetadataInterfacePreservationContract &contract);
std::string Objc3InteropBridgePackagingToolchainSummary();
std::string Objc3InteropHeaderModuleBridgeGenerationBoundarySummary();
