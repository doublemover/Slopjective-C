#pragma once

#include <cstddef>
#include <string>

// Metaprogramming expansion lowering owns derive/macro replay facts, property
// behavior lowering inputs, synthesized binding counts, and emitted
// synthesized artifacts for manifest and IR carriage.
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
