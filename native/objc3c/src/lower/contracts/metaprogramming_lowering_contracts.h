#pragma once

#include <cstddef>
#include <string>

// Metaprogramming lowering owns expansion replay facts, synthesized emission
// artifacts, module-interface preservation, and macro-host/cache integration.
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringContractId =
    "objc3c.metaprogramming.expansion.lowering.contract.v1";
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_metaprogramming_expansion_and_lowering_contract";
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringModel =
    "metaprogramming-derived-selector-inventory-macro-replay-visibility-and-synthesized-property-metadata-now-feed-one-deterministic-lowering-contract-for-manifest-and-ir-carriage";
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringDeferredModel =
    "runnable-derive-body-emission-macro-execution-and-property-behavior-runtime-materialization-remain-later-expansion-runtime-work";
inline constexpr const char *kObjc3MetaprogrammingExpansionLoweringLaneContract =
    "objc3c.metaprogramming.expansion.lowering.contract.v1";

inline constexpr const char *kObjc3MetaprogrammingSynthesizedArtifactEmissionContractId =
    "objc3c.metaprogramming.synthesized.ast.ir.emission.v1";
inline constexpr const char *kObjc3MetaprogrammingSynthesizedArtifactEmissionSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_metaprogramming_synthesized_ast_and_ir_emission";
inline constexpr const char *kObjc3MetaprogrammingSynthesizedArtifactEmissionModel =
    "supported-derive-macro-and-property-behavior-sites-now-materialize-deterministic-synthesized-ir-artifacts-and-runtime-visible-method-bodies";
inline constexpr const char
    *kObjc3MetaprogrammingSynthesizedArtifactEmissionDeferredModel =
        "cross-module-preservation-expansion-host-execution-and-cached-macro-toolchain-integration-remain-deferred-to-later-runtime-work";
inline constexpr const char *kObjc3MetaprogrammingSynthesizedArtifactEmissionLaneContract =
    "objc3c.metaprogramming.synthesized.ast.ir.emission.v1";

inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationContractId =
        "objc3c.metaprogramming.module.interface.replay.preservation.v1";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_metaprogramming_module_interface_and_replay_preservation";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationImportArtifactMemberName =
        "objc_metaprogramming_module_interface_and_replay_preservation";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationSourceModel =
        "runtime-import-surface-artifacts-preserve-metaprogramming-derived-method-macro-and-property-behavior-replay-facts-for-separate-compilation-and-interface-inspection";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationModel =
        "provider-and-consumer-import-surfaces-preserve-metaprogramming-synthesized-emission-counts-replay-keys-and-interface-vs-implementation-property-behavior-splits-beyond-local-ir-object-emission";
inline constexpr const char
    *kObjc3MetaprogrammingModuleInterfaceReplayPreservationFailClosedModel =
        "missing-or-drifted-metaprogramming-preservation-packets-disable-cross-module-metaprogramming-preservation-claims";

inline constexpr const char *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryContractId =
    "objc3c.metaprogramming.expansion.host.runtime.boundary.v1";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionHostRuntimeBoundarySourceContractId =
        "objc3c.metaprogramming.module.interface.replay.preservation.v1";
inline constexpr const char *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryHostModel =
    "macro-host-execution-process-launch-and-runtime-package-loading-remain-disabled-and-fail-closed";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryPropertyRuntimeModel =
        "supported-property-behavior-lowering-reuses-existing-private-runtime-property-accessor-layout-and-current-property-hooks";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryPackagingModel =
        "native-driver-packaging-still-hands-off-metaprogramming-runtime-support-through-artifacts-lib-objc3_runtime-lib-and-runtime-registration-manifests";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionHostRuntimeBoundaryFailClosedModel =
        "no-live-macro-expansion-host-or-runtime-package-loader-is-claimed-yet";

inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationContractId =
        "objc3c.metaprogramming.macro.host.process.cache.runtime.integration.v1";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSourceContractId =
        "objc3c.metaprogramming.expansion.host.runtime.boundary.v1";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationImportArtifactMemberName =
        "objc_metaprogramming_macro_host_process_and_cache_runtime_integration";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostExecutableRelativePath =
        "artifacts/bin/objc3c-frontend-c-api-runner.exe";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheRootRelativePath =
        "tmp/artifacts/objc3c-native/cache/metaprogramming";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationHostModel =
        "native-driver-launches-objc3c-frontend-c-api-runner-for-supported-metaprogramming-expansion-cache-materialization";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationToolchainModel =
        "frontend-runner-executes-with-manifest-enabled-and-ir-object-emission-disabled-for-deterministic-cache-materialization";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationCacheModel =
        "cache-entry-path-is-derived-from-a-stable-fnv1a64-key-over-the-metaprogramming-replay-surface-and-reused-on-subsequent-runs";
inline constexpr const char
    *kObjc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationFailClosedModel =
        "missing-runner-corrupt-cache-or-import-surface-drift-disables-metaprogramming-host-process-cache-claims";

struct Objc3MetaprogrammingExpansionLoweringContract {
  std::size_t derive_inventory_sites = 0;
  std::size_t derived_selector_artifact_sites = 0;
  std::size_t macro_replay_visible_sites = 0;
  std::size_t property_behavior_sites = 0;
  std::size_t synthesized_binding_sites = 0;
  std::size_t synthesized_getter_sites = 0;
  std::size_t synthesized_setter_sites = 0;
  std::size_t replay_visible_metadata_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3MetaprogrammingSynthesizedArtifactEmissionContract {
  std::size_t derive_inventory_sites = 0;
  std::size_t emitted_derive_method_sites = 0;
  std::size_t emitted_macro_artifact_sites = 0;
  std::size_t emitted_property_behavior_artifact_sites = 0;
  std::size_t emitted_global_artifact_sites = 0;
  std::size_t emitted_runtime_method_list_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

bool IsValidObjc3MetaprogrammingExpansionLoweringContract(
    const Objc3MetaprogrammingExpansionLoweringContract &contract);
std::string Objc3MetaprogrammingExpansionLoweringReplayKey(
    const Objc3MetaprogrammingExpansionLoweringContract &contract);
bool IsValidObjc3MetaprogrammingSynthesizedArtifactEmissionContract(
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract &contract);
std::string Objc3MetaprogrammingSynthesizedArtifactEmissionReplayKey(
    const Objc3MetaprogrammingSynthesizedArtifactEmissionContract &contract);
std::string Objc3MetaprogrammingModuleInterfaceReplayPreservationSummary();
std::string Objc3MetaprogrammingExpansionHostRuntimeBoundarySummary();
std::string Objc3MetaprogrammingMacroHostProcessCacheRuntimeIntegrationSummary();
