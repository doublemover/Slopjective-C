#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "sema/objc3_sema_contract_core.h"

inline constexpr const char
    *kObjc3MetaprogrammingExpansionBehaviorSemanticModelRule =
        "derive-macro-package-provenance-and-property-behavior-source-surfaces-now-share-one-truthful-sema-packet-while-real-derive-expansion-macro-execution-and-property-behavior-runtime-materialization-remain-later-runtime-work";
inline constexpr const char
    *kObjc3MetaprogrammingExpansionBehaviorSemanticModelDeferredRule =
        "derive-body-expansion-macro-sandbox-execution-and-property-behavior-runtime-hooks-remain-deferred-to-later-runtime-lanes";

struct Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary {
  std::string contract_id =
      kObjc3MetaprogrammingExpansionBehaviorSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3MetaprogrammingExpansionBehaviorSemanticModelDependencyContractId;
  std::string surface_path =
      kObjc3MetaprogrammingExpansionBehaviorSemanticModelSurfacePath;
  std::string semantic_model =
      kObjc3MetaprogrammingExpansionBehaviorSemanticModelRule;
  std::string deferred_model =
      kObjc3MetaprogrammingExpansionBehaviorSemanticModelDeferredRule;
  std::size_t derive_marker_sites = 0;
  std::size_t macro_marker_sites = 0;
  std::size_t macro_package_sites = 0;
  std::size_t macro_provenance_sites = 0;
  std::size_t expansion_visible_macro_sites = 0;
  std::size_t property_behavior_sites = 0;
  std::size_t interface_property_behavior_sites = 0;
  std::size_t implementation_property_behavior_sites = 0;
  std::size_t protocol_property_behavior_sites = 0;
  std::size_t synthesized_binding_visible_sites = 0;
  std::size_t synthesized_getter_visible_sites = 0;
  std::size_t synthesized_setter_visible_sites = 0;
  std::size_t property_behavior_contract_violation_sites = 0;
  bool source_dependency_required = false;
  bool derive_macro_source_supported = false;
  bool macro_package_provenance_surface_reused = false;
  bool property_behavior_source_supported = false;
  bool synthesized_visibility_surface_reused = false;
  bool derive_synthesis_deferred = false;
  bool macro_execution_deferred = false;
  bool property_behavior_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_core_implementation = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3MetaprogrammingExpansionBehaviorSemanticModelSummary(
    const Objc3MetaprogrammingExpansionBehaviorSemanticModelSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.frontend_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.source_dependency_required &&
         summary.derive_macro_source_supported &&
         summary.macro_package_provenance_surface_reused &&
         summary.property_behavior_source_supported &&
         summary.synthesized_visibility_surface_reused &&
         summary.derive_synthesis_deferred &&
         summary.macro_execution_deferred &&
         summary.property_behavior_runtime_deferred && summary.deterministic &&
         summary.ready_for_core_implementation && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3MetaprogrammingDeriveExpansionInventoryDependencyContractId =
        "objc3c.metaprogramming.expansion.behavior.semantic.model.v1";
inline constexpr const char
    *kObjc3MetaprogrammingDeriveExpansionInventoryContractId =
        "objc3c.metaprogramming.derive.expansion.inventory.v1";
inline constexpr const char
    *kObjc3MetaprogrammingDeriveExpansionInventorySurfacePath =
        "frontend.pipeline.semantic_surface.objc_metaprogramming_derive_expansion_inventory";
inline constexpr const char
    *kObjc3MetaprogrammingDeriveExpansionInventoryRule =
        "supported-derive-requests-now-expand-into-a-deterministic-selector-inventory-while-unsupported-derive-names-category-topologies-and-selector-collisions-fail-closed-in-sema";
inline constexpr const char
    *kObjc3MetaprogrammingDeriveExpansionInventoryDeferredRule =
        "runtime-backed-derived-method-body-materialization-macro-execution-and-property-behavior-runtime-hooks-remain-deferred-to-later-runtime-lanes";

struct Objc3MetaprogrammingDeriveExpansionInventorySummary {
  std::string contract_id = kObjc3MetaprogrammingDeriveExpansionInventoryContractId;
  std::string semantic_dependency_contract_id =
      kObjc3MetaprogrammingDeriveExpansionInventoryDependencyContractId;
  std::string surface_path = kObjc3MetaprogrammingDeriveExpansionInventorySurfacePath;
  std::string semantic_model = kObjc3MetaprogrammingDeriveExpansionInventoryRule;
  std::string deferred_model = kObjc3MetaprogrammingDeriveExpansionInventoryDeferredRule;
  std::size_t derive_request_sites = 0;
  std::size_t supported_derive_request_sites = 0;
  std::size_t unsupported_derive_request_sites = 0;
  std::size_t unsupported_topology_sites = 0;
  std::size_t equatable_alias_sites = 0;
  std::size_t equality_derive_sites = 0;
  std::size_t hash_derive_sites = 0;
  std::size_t debug_description_derive_sites = 0;
  std::size_t selector_conflict_sites = 0;
  std::size_t generated_method_entry_count = 0;
  std::vector<std::string> expansion_inventory_rows_lexicographic;
  bool semantic_dependency_required = false;
  bool supported_derive_inventory_landed = false;
  bool unsupported_derive_fail_closed = false;
  bool unsupported_topology_fail_closed = false;
  bool selector_conflicts_fail_closed = false;
  bool runtime_materialization_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3MetaprogrammingDeriveExpansionInventorySummary(
    const Objc3MetaprogrammingDeriveExpansionInventorySummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.semantic_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.semantic_dependency_required &&
         summary.supported_derive_inventory_landed &&
         summary.unsupported_derive_fail_closed &&
         summary.unsupported_topology_fail_closed &&
         summary.selector_conflicts_fail_closed &&
         summary.runtime_materialization_deferred &&
         summary.deterministic && summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3MetaprogrammingMacroSafetySandboxDeterminismDependencyContractId =
        "objc3c.metaprogramming.derive.expansion.inventory.v1";
inline constexpr const char
    *kObjc3MetaprogrammingMacroSafetySandboxDeterminismContractId =
        "objc3c.metaprogramming.macro.safety.sandbox.determinism.semantics.v1";
inline constexpr const char
    *kObjc3MetaprogrammingMacroSafetySandboxDeterminismSurfacePath =
        "frontend.pipeline.semantic_surface.objc_metaprogramming_macro_safety_sandbox_and_determinism_semantics";
inline constexpr const char
    *kObjc3MetaprogrammingMacroSafetySandboxDeterminismRule =
        "macro-callable-safety-sandbox-namespace-and-deterministic-provenance-semantics-are-live-in-sema-while-runnable-macro-execution-remains-deferred";
inline constexpr const char
    *kObjc3MetaprogrammingMacroSafetySandboxDeterminismDeferredRule =
        "runnable-macro-execution-runtime-package-loading-and-expanded-body-materialization-remain-deferred-to-later-runtime-lanes";

struct Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary {
  std::string contract_id =
      kObjc3MetaprogrammingMacroSafetySandboxDeterminismContractId;
  std::string semantic_dependency_contract_id =
      kObjc3MetaprogrammingMacroSafetySandboxDeterminismDependencyContractId;
  std::string surface_path =
      kObjc3MetaprogrammingMacroSafetySandboxDeterminismSurfacePath;
  std::string semantic_model =
      kObjc3MetaprogrammingMacroSafetySandboxDeterminismRule;
  std::string deferred_model =
      kObjc3MetaprogrammingMacroSafetySandboxDeterminismDeferredRule;
  std::size_t macro_marker_sites = 0;
  std::size_t macro_package_sites = 0;
  std::size_t macro_provenance_sites = 0;
  std::size_t expansion_visible_macro_sites = 0;
  std::size_t safe_macro_callable_sites = 0;
  std::size_t incomplete_macro_metadata_sites = 0;
  std::size_t orphan_macro_metadata_sites = 0;
  std::size_t invalid_package_sites = 0;
  std::size_t invalid_provenance_sites = 0;
  std::size_t nondeterministic_callable_sites = 0;
  std::size_t unsupported_callable_topology_sites = 0;
  bool semantic_dependency_required = false;
  bool metadata_completeness_enforced = false;
  bool sandbox_namespace_enforced = false;
  bool provenance_determinism_enforced = false;
  bool callable_determinism_enforced = false;
  bool macro_execution_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3MetaprogrammingMacroSafetySandboxDeterminismSummary(
    const Objc3MetaprogrammingMacroSafetySandboxDeterminismSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.semantic_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.semantic_dependency_required &&
         summary.metadata_completeness_enforced &&
         summary.sandbox_namespace_enforced &&
         summary.provenance_determinism_enforced &&
         summary.callable_determinism_enforced &&
         summary.macro_execution_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityDependencyContractId =
        "objc3c.metaprogramming.macro.safety.sandbox.determinism.semantics.v1";
inline constexpr const char
    *kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityContractId =
        "objc3c.metaprogramming.property.behavior.legality.interaction.completion.v1";
inline constexpr const char
    *kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySurfacePath =
        "frontend.pipeline.semantic_surface.objc_metaprogramming_property_behavior_legality_and_interaction_completion";
inline constexpr const char
    *kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityRule =
        "supported-property-behavior-names-and-owner-interaction-legality-now-fail-closed-in-sema-while-runtime-backed-behavior-materialization-remains-deferred";
inline constexpr const char
    *kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityDeferredRule =
        "runnable-property-behavior-hooks-observation-materialization-and-projection-runtime-support-remain-deferred-to-later-runtime-lanes";

struct Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary {
  std::string contract_id =
      kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityContractId;
  std::string semantic_dependency_contract_id =
      kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityDependencyContractId;
  std::string surface_path =
      kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySurfacePath;
  std::string semantic_model =
      kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityRule;
  std::string deferred_model =
      kObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilityDeferredRule;
  std::size_t property_behavior_sites = 0;
  std::size_t supported_behavior_sites = 0;
  std::size_t unsupported_behavior_sites = 0;
  std::size_t observed_behavior_sites = 0;
  std::size_t projected_behavior_sites = 0;
  std::size_t observed_on_protocol_sites = 0;
  std::size_t observed_readonly_conflict_sites = 0;
  std::size_t projected_writable_conflict_sites = 0;
  std::size_t non_object_behavior_sites = 0;
  bool semantic_dependency_required = false;
  bool supported_behavior_inventory_landed = false;
  bool unsupported_behavior_fail_closed = false;
  bool owner_topology_fail_closed = false;
  bool interaction_legality_fail_closed = false;
  bool storage_legality_fail_closed = false;
  bool runtime_materialization_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary(
    const Objc3MetaprogrammingPropertyBehaviorLegalityCompatibilitySummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.semantic_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.semantic_dependency_required &&
         summary.supported_behavior_inventory_landed &&
         summary.unsupported_behavior_fail_closed &&
         summary.owner_topology_fail_closed &&
         summary.interaction_legality_fail_closed &&
         summary.storage_legality_fail_closed &&
         summary.runtime_materialization_deferred &&
         summary.deterministic &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
