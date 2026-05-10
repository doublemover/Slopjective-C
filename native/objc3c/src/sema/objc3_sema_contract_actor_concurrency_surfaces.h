#pragma once

#include "sema/objc3_sema_contract_metaprogramming_surfaces.h"

inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendableSemanticModelDependencyContractId =
        kObjc3ActorMemberIsolationSourceClosureContractId;
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendableSemanticModelContractId =
        "objc3c.concurrency.actor.isolation.sendable.semantic.model.v1";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendableSemanticModelSurfacePath =
        "frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendable_semantic_model";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendableSemanticModelRule =
        "actor-member-source-closure-and-parser-owned-actor-sendability-profiles-now-publish-one-deterministic-sema-packet-while-cross-actor-legality-sendable-enforcement-and-runnable-actor-runtime-behavior-remain-later-runtime-work";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendableSemanticModelDeferredRule =
        "dedicated-actor-isolation-diagnostics-cross-actor-sendable-enforcement-executor-scheduling-and-runnable-actor-runtime-behavior-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyActorIsolationSendableSemanticModelSummary {
  std::string contract_id =
      kObjc3ConcurrencyActorIsolationSendableSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3ConcurrencyActorIsolationSendableSemanticModelDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyActorIsolationSendableSemanticModelSurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyActorIsolationSendableSemanticModelRule;
  std::string deferred_model =
      kObjc3ConcurrencyActorIsolationSendableSemanticModelDeferredRule;
  std::size_t actor_interface_sites = 0;
  std::size_t actor_method_sites = 0;
  std::size_t actor_property_sites = 0;
  std::size_t objc_nonisolated_annotation_sites = 0;
  std::size_t actor_member_executor_annotation_sites = 0;
  std::size_t actor_async_method_sites = 0;
  std::size_t actor_member_metadata_sites = 0;
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool source_dependency_required = false;
  bool actor_member_source_supported = false;
  bool actor_isolation_sendability_profile_normalized = false;
  bool strict_concurrency_selection_fail_closed = false;
  bool actor_runtime_deferred = false;
  bool executor_runtime_deferred = false;
  bool cross_actor_enforcement_deferred = false;
  bool deterministic = false;
  bool ready_for_semantic_expansion = false;
  std::string failure_reason;
  std::string replay_key;
};

inline bool IsReadyObjc3ConcurrencyActorIsolationSendableSemanticModelSummary(
    const Objc3ConcurrencyActorIsolationSendableSemanticModelSummary &summary) {
  return summary.source_dependency_required &&
         summary.actor_member_source_supported &&
         summary.actor_isolation_sendability_profile_normalized &&
         summary.strict_concurrency_selection_fail_closed &&
         summary.actor_runtime_deferred &&
         summary.executor_runtime_deferred &&
         summary.cross_actor_enforcement_deferred &&
         summary.deterministic && summary.ready_for_semantic_expansion &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendabilityEnforcementDependencyContractId =
        kObjc3ConcurrencyActorIsolationSendableSemanticModelContractId;
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendabilityEnforcementContractId =
        "objc3c.concurrency.actor.isolation.sendability.enforcement.v1";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendabilityEnforcementSurfacePath =
        "frontend.pipeline.semantic_surface.objc_concurrency_actor_isolation_and_sendability_enforcement";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendabilityEnforcementRule =
        "actor-method-semantics-now-fail-closed-for-non-actor-objc-nonisolated-usage-invalid-nonisolated-combinations-non-async-actor-hops-and-non-sendable-crossings-while-runnable-actor-mailbox-runtime-remains-later-runtime-work";
inline constexpr const char
    *kObjc3ConcurrencyActorIsolationSendabilityEnforcementDeferredRule =
        "full-cross-module-actor-runtime-mailboxes-race-hazard-closure-and-runnable-strict-concurrency-scheduling-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary {
  std::string contract_id =
      kObjc3ConcurrencyActorIsolationSendabilityEnforcementContractId;
  std::string dependency_contract_id =
      kObjc3ConcurrencyActorIsolationSendabilityEnforcementDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyActorIsolationSendabilityEnforcementSurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyActorIsolationSendabilityEnforcementRule;
  std::string deferred_model =
      kObjc3ConcurrencyActorIsolationSendabilityEnforcementDeferredRule;
  std::size_t actor_interface_sites = 0;
  std::size_t actor_method_sites = 0;
  std::size_t objc_nonisolated_annotation_sites = 0;
  std::size_t actor_member_executor_annotation_sites = 0;
  std::size_t actor_async_method_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t total_nonisolated_method_sites = 0;
  std::size_t illegal_non_actor_nonisolated_sites = 0;
  std::size_t illegal_nonisolated_async_sites = 0;
  std::size_t illegal_nonisolated_executor_sites = 0;
  std::size_t illegal_actor_hop_without_async_sites = 0;
  std::size_t illegal_non_sendable_crossing_sites = 0;
  bool dependency_required = false;
  bool non_actor_nonisolated_fail_closed = false;
  bool nonisolated_combination_fail_closed = false;
  bool actor_hop_async_boundary_enforced = false;
  bool non_sendable_crossing_fail_closed = false;
  bool runnable_lowering_deferred = false;
  bool actor_runtime_deferred = false;
  bool executor_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string failure_reason;
  std::string replay_key;
};

inline bool IsReadyObjc3ConcurrencyActorIsolationSendabilityEnforcementSummary(
    const Objc3ConcurrencyActorIsolationSendabilityEnforcementSummary &summary) {
  return summary.dependency_required &&
         summary.non_actor_nonisolated_fail_closed &&
         summary.nonisolated_combination_fail_closed &&
         summary.actor_hop_async_boundary_enforced &&
         summary.non_sendable_crossing_fail_closed &&
         summary.runnable_lowering_deferred && summary.actor_runtime_deferred &&
         summary.executor_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline constexpr const char
    *kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsDependencyContractId =
        kObjc3ConcurrencyActorIsolationSendabilityEnforcementContractId;
inline constexpr const char
    *kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsContractId =
        "objc3c.concurrency.actor.race.hazard.escape.diagnostics.v1";
inline constexpr const char
    *kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsSurfacePath =
        "frontend.pipeline.semantic_surface.objc_concurrency_actor_race_hazard_and_escape_diagnostics";
inline constexpr const char
    *kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsRule =
        "actor-method-task-handoff-now-fails-closed-without-race-guard-replay-proof-and-actor-isolation-coverage-while-escaping-block-literals-in-that-hazard-slice-remain-unsupported";
inline constexpr const char
    *kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsDeferredRule =
        "runnable-actor-mailboxes-cross-module-isolation-runtime-and-full-strict-concurrency-escape-analysis-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary {
  std::string contract_id =
      kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsContractId;
  std::string dependency_contract_id =
      kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsSurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsRule;
  std::string deferred_model =
      kObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsDeferredRule;
  std::size_t actor_method_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t escaping_block_literal_sites = 0;
  std::size_t illegal_missing_race_guard_sites = 0;
  std::size_t illegal_missing_replay_proof_sites = 0;
  std::size_t illegal_missing_actor_isolation_sites = 0;
  std::size_t illegal_escaping_block_literal_sites = 0;
  bool dependency_required = false;
  bool race_guard_fail_closed = false;
  bool replay_proof_fail_closed = false;
  bool actor_isolation_boundary_fail_closed = false;
  bool escaping_block_fail_closed = false;
  bool runnable_lowering_deferred = false;
  bool actor_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string failure_reason;
  std::string replay_key;
};

inline bool IsReadyObjc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary(
    const Objc3ConcurrencyActorRaceHazardEscapeDiagnosticsSummary &summary) {
  return summary.dependency_required && summary.race_guard_fail_closed &&
         summary.replay_proof_fail_closed &&
         summary.actor_isolation_boundary_fail_closed &&
         summary.escaping_block_fail_closed &&
         summary.runnable_lowering_deferred && summary.actor_runtime_deferred &&
         summary.deterministic && summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
