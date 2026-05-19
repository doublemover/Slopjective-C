#pragma once

#include <cstddef>
#include <string>

inline constexpr const char
    *kObjc3DispatchDispatchIntentSemanticModelDependencyContractId =
        "objc3c.dispatch.dispatch.intent.source.completion.v1";
inline constexpr const char *kObjc3DispatchDispatchIntentSemanticModelContractId =
    "objc3c.dispatch.dynamism.dispatch.control.semantic.model.v1";
inline constexpr const char *kObjc3DispatchDispatchIntentSemanticModelSurfacePath =
    "frontend.pipeline.semantic_surface.objc_dispatch_dynamism_and_dispatch_control_semantic_model";
inline constexpr const char *kObjc3DispatchDispatchIntentSemanticModelRule =
    "direct-members-defaulting-final-sealed-and-override-accounting-now-share-one-truthful-sema-packet-while-direct-dispatch-legality-final-sealed-enforcement-and-runnable-dispatch-boundary-realization-remain-later-runtime-work";
inline constexpr const char
    *kObjc3DispatchDispatchIntentSemanticModelDeferredRule =
        "direct-call-lowering-final-sealed-diagnostics-and-runnable-dispatch-boundary-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3DispatchDispatchIntentSemanticModelSummary {
  std::string contract_id = kObjc3DispatchDispatchIntentSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3DispatchDispatchIntentSemanticModelDependencyContractId;
  std::string surface_path = kObjc3DispatchDispatchIntentSemanticModelSurfacePath;
  std::string semantic_model = kObjc3DispatchDispatchIntentSemanticModelRule;
  std::string deferred_model =
      kObjc3DispatchDispatchIntentSemanticModelDeferredRule;
  std::size_t prefixed_container_attribute_sites = 0;
  std::size_t direct_members_container_sites = 0;
  std::size_t final_container_sites = 0;
  std::size_t sealed_container_sites = 0;
  std::size_t effective_direct_member_sites = 0;
  std::size_t direct_members_defaulted_method_sites = 0;
  std::size_t direct_members_dynamic_opt_out_sites = 0;
  std::size_t override_lookup_sites = 0;
  std::size_t override_lookup_hits = 0;
  std::size_t override_lookup_misses = 0;
  std::size_t override_conflicts = 0;
  std::size_t unresolved_base_interfaces = 0;
  bool source_dependency_required = false;
  bool dispatch_intent_source_supported = false;
  bool override_semantic_surface_reused = false;
  bool direct_dispatch_reserved_non_goal = false;
  bool final_sealed_enforcement_deferred = false;
  bool lowering_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_core_implementation = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3DispatchDispatchIntentSemanticModelSummary(
    const Objc3DispatchDispatchIntentSemanticModelSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.frontend_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.source_dependency_required &&
         summary.dispatch_intent_source_supported &&
         summary.override_semantic_surface_reused &&
         summary.direct_dispatch_reserved_non_goal &&
         summary.final_sealed_enforcement_deferred &&
         summary.lowering_runtime_deferred && summary.deterministic &&
         summary.ready_for_core_implementation && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3DispatchDispatchIntentLegalitySummaryDependencyContractId =
        "objc3c.dispatch.dynamism.dispatch.control.semantic.model.v1";
inline constexpr const char *
    kObjc3DispatchDispatchIntentLegalitySummaryContractId =
        "objc3c.dispatch.override.finality.sealing.legality.v1";
inline constexpr const char *
    kObjc3DispatchDispatchIntentLegalitySummarySurfacePath =
        "frontend.pipeline.semantic_surface.objc_dispatch_override_finality_and_sealing_legality";
inline constexpr const char *
    kObjc3DispatchDispatchIntentLegalitySummaryRule =
        "superclass-finality-superclass-sealing-and-direct-final-override-restrictions-are-now-live-fail-closed-sema-rules-while-direct-call-lowering-and-runnable-dispatch-boundary-realization-remain-later-runtime-work";
inline constexpr const char *
    kObjc3DispatchDispatchIntentLegalitySummaryDeferredRule =
        "direct-call-lowering-selector-dispatch-bypass-metadata-realization-and-runnable-dispatch-boundary-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3DispatchDispatchIntentLegalitySummary {
  std::string contract_id = kObjc3DispatchDispatchIntentLegalitySummaryContractId;
  std::string dependency_contract_id =
      kObjc3DispatchDispatchIntentLegalitySummaryDependencyContractId;
  std::string surface_path =
      kObjc3DispatchDispatchIntentLegalitySummarySurfacePath;
  std::string semantic_model = kObjc3DispatchDispatchIntentLegalitySummaryRule;
  std::string deferred_model =
      kObjc3DispatchDispatchIntentLegalitySummaryDeferredRule;
  std::size_t subclass_sites = 0;
  std::size_t override_sites = 0;
  std::size_t illegal_final_superclass_sites = 0;
  std::size_t illegal_sealed_superclass_sites = 0;
  std::size_t illegal_final_override_sites = 0;
  std::size_t illegal_direct_override_sites = 0;
  bool dependency_required = false;
  bool final_superclass_fail_closed = false;
  bool sealed_superclass_fail_closed = false;
  bool final_override_fail_closed = false;
  bool direct_override_fail_closed = false;
  bool lowering_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3DispatchDispatchIntentLegalitySummary(
    const Objc3DispatchDispatchIntentLegalitySummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.final_superclass_fail_closed &&
         summary.sealed_superclass_fail_closed &&
         summary.final_override_fail_closed &&
         summary.direct_override_fail_closed &&
         summary.lowering_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3DispatchDispatchIntentCompatibilitySummaryDependencyContractId =
        "objc3c.dispatch.override.finality.sealing.legality.v1";
inline constexpr const char *
    kObjc3DispatchDispatchIntentCompatibilitySummaryContractId =
        "objc3c.dispatch.dynamism.control.compatibility.diagnostics.v1";
inline constexpr const char *
    kObjc3DispatchDispatchIntentCompatibilitySummarySurfacePath =
        "frontend.pipeline.semantic_surface.objc_dispatch_dynamism_control_compatibility_diagnostics";
inline constexpr const char *
    kObjc3DispatchDispatchIntentCompatibilitySummaryRule =
        "dispatch-dispatch-intent-now-fails-closed-on-conflicting-direct-dynamic-final-dynamic-callable-markers-plus-unsupported-function-protocol-and-category-topologies-before-lowering-and-runtime-dispatch-boundary-realization";
inline constexpr const char *
    kObjc3DispatchDispatchIntentCompatibilitySummaryDeferredRule =
        "direct-call-lowering-metadata-realization-and-runnable-dispatch-boundary-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3DispatchDispatchIntentCompatibilitySummary {
  std::string contract_id =
      kObjc3DispatchDispatchIntentCompatibilitySummaryContractId;
  std::string dependency_contract_id =
      kObjc3DispatchDispatchIntentCompatibilitySummaryDependencyContractId;
  std::string surface_path =
      kObjc3DispatchDispatchIntentCompatibilitySummarySurfacePath;
  std::string semantic_model =
      kObjc3DispatchDispatchIntentCompatibilitySummaryRule;
  std::string deferred_model =
      kObjc3DispatchDispatchIntentCompatibilitySummaryDeferredRule;
  std::size_t callable_dispatch_intent_sites = 0;
  std::size_t container_dispatch_intent_sites = 0;
  std::size_t illegal_direct_dynamic_conflict_sites = 0;
  std::size_t illegal_final_dynamic_conflict_sites = 0;
  std::size_t illegal_non_method_callable_sites = 0;
  std::size_t illegal_protocol_method_sites = 0;
  std::size_t illegal_category_method_sites = 0;
  std::size_t illegal_category_container_sites = 0;
  bool dependency_required = false;
  bool callable_conflict_fail_closed = false;
  bool unsupported_callable_topology_fail_closed = false;
  bool unsupported_container_topology_fail_closed = false;
  bool lowering_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3DispatchDispatchIntentCompatibilitySummary(
    const Objc3DispatchDispatchIntentCompatibilitySummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() && summary.dependency_required &&
         summary.callable_conflict_fail_closed &&
         summary.unsupported_callable_topology_fail_closed &&
         summary.unsupported_container_topology_fail_closed &&
         summary.lowering_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime && !summary.replay_key.empty() &&
         summary.failure_reason.empty();
}
