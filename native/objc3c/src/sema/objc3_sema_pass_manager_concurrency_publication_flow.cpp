#include "sema/objc3_sema_pass_manager_contract_flow.h"

Objc3SemaConcurrencyParityPublicationReadinessRecord
BuildObjc3SemaConcurrencyParityPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_actor_isolation_sendability_handoff,
    bool deterministic_task_runtime_cancellation_handoff,
    bool deterministic_concurrency_replay_race_guard_handoff) {
  Objc3SemaConcurrencyParityPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.actor_isolation_sendability_ready =
      deterministic_actor_isolation_sendability_handoff &&
      surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites ==
          surface.actor_isolation_sendability_sites_total &&
      surface.actor_isolation_sendability_summary.actor_isolation_decl_sites ==
          surface.actor_isolation_decl_sites_total &&
      surface.actor_isolation_sendability_summary.actor_hop_sites ==
          surface.actor_hop_sites_total &&
      surface.actor_isolation_sendability_summary.sendable_annotation_sites ==
          surface.sendable_annotation_sites_total &&
      surface.actor_isolation_sendability_summary.non_sendable_crossing_sites ==
          surface.non_sendable_crossing_sites_total &&
      surface.actor_isolation_sendability_summary.isolation_boundary_sites ==
          surface.actor_isolation_sendability_isolation_boundary_sites_total &&
      surface.actor_isolation_sendability_summary.normalized_sites ==
          surface.actor_isolation_sendability_normalized_sites_total &&
      surface.actor_isolation_sendability_summary.gate_blocked_sites ==
          surface.actor_isolation_sendability_gate_blocked_sites_total &&
      surface.actor_isolation_sendability_summary.contract_violation_sites ==
          surface.actor_isolation_sendability_contract_violation_sites_total &&
      surface.actor_isolation_sendability_summary.actor_isolation_decl_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.actor_hop_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.sendable_annotation_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.non_sendable_crossing_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.isolation_boundary_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.normalized_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.gate_blocked_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.gate_blocked_sites <=
          surface.actor_isolation_sendability_summary
              .non_sendable_crossing_sites &&
      surface.actor_isolation_sendability_summary.contract_violation_sites <=
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.normalized_sites +
              surface.actor_isolation_sendability_summary.gate_blocked_sites ==
          surface.actor_isolation_sendability_summary
              .actor_isolation_sendability_sites &&
      surface.actor_isolation_sendability_summary.deterministic;
  record.task_runtime_cancellation_ready =
      deterministic_task_runtime_cancellation_handoff &&
      surface.task_runtime_cancellation_summary.task_runtime_interop_sites ==
          surface.task_runtime_cancellation_sites_total &&
      surface.task_runtime_cancellation_summary.runtime_hook_sites ==
          surface.task_runtime_cancellation_runtime_hook_sites_total &&
      surface.task_runtime_cancellation_summary.cancellation_check_sites ==
          surface.task_runtime_cancellation_cancellation_check_sites_total &&
      surface.task_runtime_cancellation_summary.cancellation_handler_sites ==
          surface
              .task_runtime_cancellation_cancellation_handler_sites_total &&
      surface.task_runtime_cancellation_summary.suspension_point_sites ==
          surface.task_runtime_cancellation_suspension_point_sites_total &&
      surface.task_runtime_cancellation_summary
              .cancellation_propagation_sites ==
          surface
              .task_runtime_cancellation_cancellation_propagation_sites_total &&
      surface.task_runtime_cancellation_summary.normalized_sites ==
          surface.task_runtime_cancellation_normalized_sites_total &&
      surface.task_runtime_cancellation_summary.gate_blocked_sites ==
          surface.task_runtime_cancellation_gate_blocked_sites_total &&
      surface.task_runtime_cancellation_summary.contract_violation_sites ==
          surface.task_runtime_cancellation_contract_violation_sites_total &&
      surface.task_runtime_cancellation_summary.runtime_hook_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.cancellation_check_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.cancellation_handler_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.suspension_point_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.cancellation_propagation_sites <=
          surface.task_runtime_cancellation_summary.cancellation_check_sites &&
      surface.task_runtime_cancellation_summary.cancellation_propagation_sites <=
          surface.task_runtime_cancellation_summary.cancellation_handler_sites &&
      surface.task_runtime_cancellation_summary.normalized_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.gate_blocked_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.gate_blocked_sites <=
          surface.task_runtime_cancellation_summary
              .cancellation_propagation_sites &&
      surface.task_runtime_cancellation_summary.contract_violation_sites <=
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.normalized_sites +
              surface.task_runtime_cancellation_summary.gate_blocked_sites ==
          surface.task_runtime_cancellation_summary.task_runtime_interop_sites &&
      surface.task_runtime_cancellation_summary.deterministic;
  record.concurrency_replay_race_guard_ready =
      deterministic_concurrency_replay_race_guard_handoff &&
      surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites ==
          surface.concurrency_replay_race_guard_sites_total &&
      surface.concurrency_replay_race_guard_summary.concurrency_replay_sites ==
          surface
              .concurrency_replay_race_guard_concurrency_replay_sites_total &&
      surface.concurrency_replay_race_guard_summary.replay_proof_sites ==
          surface.concurrency_replay_race_guard_replay_proof_sites_total &&
      surface.concurrency_replay_race_guard_summary.race_guard_sites ==
          surface.concurrency_replay_race_guard_race_guard_sites_total &&
      surface.concurrency_replay_race_guard_summary.task_handoff_sites ==
          surface.concurrency_replay_race_guard_task_handoff_sites_total &&
      surface.concurrency_replay_race_guard_summary.actor_isolation_sites ==
          surface.concurrency_replay_race_guard_actor_isolation_sites_total &&
      surface.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites ==
          surface
              .concurrency_replay_race_guard_deterministic_schedule_sites_total &&
      surface.concurrency_replay_race_guard_summary.guard_blocked_sites ==
          surface.concurrency_replay_race_guard_guard_blocked_sites_total &&
      surface.concurrency_replay_race_guard_summary.contract_violation_sites ==
          surface.concurrency_replay_race_guard_contract_violation_sites_total &&
      surface.concurrency_replay_race_guard_summary.concurrency_replay_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary.replay_proof_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary.race_guard_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary.task_handoff_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary.actor_isolation_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      surface.concurrency_replay_race_guard_summary.guard_blocked_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      surface.concurrency_replay_race_guard_summary.contract_violation_sites <=
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_race_guard_sites &&
      surface.concurrency_replay_race_guard_summary
              .deterministic_schedule_sites +
              surface.concurrency_replay_race_guard_summary
                  .guard_blocked_sites ==
          surface.concurrency_replay_race_guard_summary
              .concurrency_replay_sites &&
      surface.concurrency_replay_race_guard_summary.deterministic;
  record.passed_publication_count =
      Objc3SemaEvidenceCount(record.actor_isolation_sendability_ready) +
      Objc3SemaEvidenceCount(record.task_runtime_cancellation_ready) +
      Objc3SemaEvidenceCount(record.concurrency_replay_race_guard_ready);
  record.failed_publication_count =
      record.required_publication_count >= record.passed_publication_count
          ? (record.required_publication_count -
             record.passed_publication_count)
          : record.required_publication_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.concurrency_parity_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.required_publication_count == 3u &&
      record.passed_publication_count == record.required_publication_count &&
      record.failed_publication_count == 0u;
  return record;
}
