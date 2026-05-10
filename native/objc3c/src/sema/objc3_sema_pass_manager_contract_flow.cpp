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
  record.strict_no_fallback = input.strict_no_fallback;
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
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_publication_count == 3u &&
      record.passed_publication_count == record.required_publication_count &&
      record.failed_publication_count == 0u;
  return record;
}

Objc3SemaUnsafeErrorParityValidationReadinessRecord
BuildObjc3SemaUnsafeErrorParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_unsafe_pointer_extension_handoff,
    bool deterministic_inline_asm_intrinsic_governance_handoff,
    bool deterministic_ns_error_bridging_handoff,
    bool deterministic_error_diagnostics_recovery_handoff,
    bool deterministic_result_like_lowering_handoff) {
  Objc3SemaUnsafeErrorParityValidationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.unsafe_pointer_extension_ready =
      deterministic_unsafe_pointer_extension_handoff &&
      surface.unsafe_pointer_extension_summary.unsafe_pointer_extension_sites ==
          surface.unsafe_pointer_extension_sites_total &&
      surface.unsafe_pointer_extension_summary.unsafe_keyword_sites ==
          surface.unsafe_pointer_extension_unsafe_keyword_sites_total &&
      surface.unsafe_pointer_extension_summary.pointer_arithmetic_sites ==
          surface.unsafe_pointer_extension_pointer_arithmetic_sites_total &&
      surface.unsafe_pointer_extension_summary.raw_pointer_type_sites ==
          surface.unsafe_pointer_extension_raw_pointer_type_sites_total &&
      surface.unsafe_pointer_extension_summary.unsafe_operation_sites ==
          surface.unsafe_pointer_extension_unsafe_operation_sites_total &&
      surface.unsafe_pointer_extension_summary.normalized_sites ==
          surface.unsafe_pointer_extension_normalized_sites_total &&
      surface.unsafe_pointer_extension_summary.gate_blocked_sites ==
          surface.unsafe_pointer_extension_gate_blocked_sites_total &&
      surface.unsafe_pointer_extension_summary.contract_violation_sites ==
          surface.unsafe_pointer_extension_contract_violation_sites_total &&
      surface.unsafe_pointer_extension_summary.unsafe_keyword_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.pointer_arithmetic_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.raw_pointer_type_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.unsafe_operation_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.normalized_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.gate_blocked_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.contract_violation_sites <=
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.normalized_sites +
              surface.unsafe_pointer_extension_summary.gate_blocked_sites ==
          surface.unsafe_pointer_extension_summary
              .unsafe_pointer_extension_sites &&
      surface.unsafe_pointer_extension_summary.deterministic;
  record.inline_asm_intrinsic_governance_ready =
      deterministic_inline_asm_intrinsic_governance_handoff &&
      surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites ==
          surface.inline_asm_intrinsic_governance_sites_total &&
      surface.inline_asm_intrinsic_governance_summary.inline_asm_sites ==
          surface.inline_asm_intrinsic_governance_inline_asm_sites_total &&
      surface.inline_asm_intrinsic_governance_summary.intrinsic_sites ==
          surface.inline_asm_intrinsic_governance_intrinsic_sites_total &&
      surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites ==
          surface
              .inline_asm_intrinsic_governance_governed_intrinsic_sites_total &&
      surface.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites ==
          surface
              .inline_asm_intrinsic_governance_privileged_intrinsic_sites_total &&
      surface.inline_asm_intrinsic_governance_summary.normalized_sites ==
          surface.inline_asm_intrinsic_governance_normalized_sites_total &&
      surface.inline_asm_intrinsic_governance_summary.gate_blocked_sites ==
          surface.inline_asm_intrinsic_governance_gate_blocked_sites_total &&
      surface.inline_asm_intrinsic_governance_summary
              .contract_violation_sites ==
          surface
              .inline_asm_intrinsic_governance_contract_violation_sites_total &&
      surface.inline_asm_intrinsic_governance_summary.inline_asm_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.intrinsic_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites <=
          surface.inline_asm_intrinsic_governance_summary.intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.normalized_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.gate_blocked_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary
              .contract_violation_sites <=
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.inline_asm_sites ==
          surface.throws_propagation_summary.cache_invalidation_candidate_sites &&
      surface.inline_asm_intrinsic_governance_summary.intrinsic_sites ==
          surface.unsafe_pointer_extension_summary.unsafe_operation_sites &&
      surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites <=
          surface.throws_propagation_summary.normalized_sites &&
      surface.inline_asm_intrinsic_governance_summary
              .privileged_intrinsic_sites <=
          surface.unsafe_pointer_extension_summary.normalized_sites &&
      surface.inline_asm_intrinsic_governance_summary.normalized_sites >=
          surface.inline_asm_intrinsic_governance_summary.inline_asm_sites &&
      surface.inline_asm_intrinsic_governance_summary.normalized_sites -
              surface.inline_asm_intrinsic_governance_summary
                  .inline_asm_sites ==
          surface.inline_asm_intrinsic_governance_summary
              .governed_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.gate_blocked_sites ==
          surface.inline_asm_intrinsic_governance_summary.intrinsic_sites -
              surface.inline_asm_intrinsic_governance_summary
                  .governed_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.normalized_sites +
              surface.inline_asm_intrinsic_governance_summary
                  .gate_blocked_sites ==
          surface.inline_asm_intrinsic_governance_summary
              .inline_asm_intrinsic_sites &&
      surface.inline_asm_intrinsic_governance_summary.deterministic;
  record.ns_error_bridging_ready =
      deterministic_ns_error_bridging_handoff &&
      surface.ns_error_bridging_summary.ns_error_bridging_sites ==
          surface.ns_error_bridging_sites_total &&
      surface.ns_error_bridging_summary.ns_error_parameter_sites ==
          surface.ns_error_bridging_ns_error_parameter_sites_total &&
      surface.ns_error_bridging_summary.ns_error_out_parameter_sites ==
          surface.ns_error_bridging_ns_error_out_parameter_sites_total &&
      surface.ns_error_bridging_summary.ns_error_bridge_path_sites ==
          surface.ns_error_bridging_ns_error_bridge_path_sites_total &&
      surface.ns_error_bridging_summary.failable_call_sites ==
          surface.ns_error_bridging_failable_call_sites_total &&
      surface.ns_error_bridging_summary.normalized_sites ==
          surface.ns_error_bridging_normalized_sites_total &&
      surface.ns_error_bridging_summary.bridge_boundary_sites ==
          surface.ns_error_bridging_bridge_boundary_sites_total &&
      surface.ns_error_bridging_summary.contract_violation_sites ==
          surface.ns_error_bridging_contract_violation_sites_total &&
      surface.ns_error_bridging_summary.ns_error_parameter_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.ns_error_out_parameter_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.ns_error_bridge_path_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.failable_call_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.normalized_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.bridge_boundary_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.normalized_sites +
              surface.ns_error_bridging_summary.bridge_boundary_sites ==
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.contract_violation_sites <=
          surface.ns_error_bridging_summary.ns_error_bridging_sites &&
      surface.ns_error_bridging_summary.deterministic;
  record.error_diagnostics_recovery_ready =
      deterministic_error_diagnostics_recovery_handoff &&
      surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites ==
          surface.error_diagnostics_recovery_sites_total &&
      surface.error_diagnostics_recovery_summary.diagnostic_emit_sites ==
          surface.error_diagnostics_recovery_diagnostic_emit_sites_total &&
      surface.error_diagnostics_recovery_summary.recovery_anchor_sites ==
          surface.error_diagnostics_recovery_recovery_anchor_sites_total &&
      surface.error_diagnostics_recovery_summary.recovery_boundary_sites ==
          surface.error_diagnostics_recovery_recovery_boundary_sites_total &&
      surface.error_diagnostics_recovery_summary.fail_closed_diagnostic_sites ==
          surface
              .error_diagnostics_recovery_fail_closed_diagnostic_sites_total &&
      surface.error_diagnostics_recovery_summary.normalized_sites ==
          surface.error_diagnostics_recovery_normalized_sites_total &&
      surface.error_diagnostics_recovery_summary.gate_blocked_sites ==
          surface.error_diagnostics_recovery_gate_blocked_sites_total &&
      surface.error_diagnostics_recovery_summary.contract_violation_sites ==
          surface.error_diagnostics_recovery_contract_violation_sites_total &&
      surface.error_diagnostics_recovery_summary.diagnostic_emit_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.recovery_anchor_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.recovery_boundary_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.fail_closed_diagnostic_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.fail_closed_diagnostic_sites <=
          surface.error_diagnostics_recovery_summary.diagnostic_emit_sites &&
      surface.error_diagnostics_recovery_summary.normalized_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.gate_blocked_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.contract_violation_sites <=
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.normalized_sites +
              surface.error_diagnostics_recovery_summary.gate_blocked_sites ==
          surface.error_diagnostics_recovery_summary
              .error_diagnostics_recovery_sites &&
      surface.error_diagnostics_recovery_summary.deterministic;
  record.result_like_lowering_ready =
      deterministic_result_like_lowering_handoff &&
      surface.result_like_lowering_summary.result_like_sites ==
          surface.result_like_lowering_sites_total &&
      surface.result_like_lowering_summary.result_success_sites ==
          surface.result_like_lowering_result_success_sites_total &&
      surface.result_like_lowering_summary.result_failure_sites ==
          surface.result_like_lowering_result_failure_sites_total &&
      surface.result_like_lowering_summary.result_branch_sites ==
          surface.result_like_lowering_result_branch_sites_total &&
      surface.result_like_lowering_summary.result_payload_sites ==
          surface.result_like_lowering_result_payload_sites_total &&
      surface.result_like_lowering_summary.normalized_sites ==
          surface.result_like_lowering_normalized_sites_total &&
      surface.result_like_lowering_summary.branch_merge_sites ==
          surface.result_like_lowering_branch_merge_sites_total &&
      surface.result_like_lowering_summary.contract_violation_sites ==
          surface.result_like_lowering_contract_violation_sites_total &&
      surface.result_like_lowering_summary.result_success_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.result_failure_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.result_branch_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.result_payload_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.normalized_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.branch_merge_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.contract_violation_sites <=
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.result_success_sites +
              surface.result_like_lowering_summary.result_failure_sites ==
          surface.result_like_lowering_summary.normalized_sites &&
      surface.result_like_lowering_summary.normalized_sites +
              surface.result_like_lowering_summary.branch_merge_sites ==
          surface.result_like_lowering_summary.result_like_sites &&
      surface.result_like_lowering_summary.deterministic;
  record.passed_validation_count =
      Objc3SemaEvidenceCount(record.unsafe_pointer_extension_ready) +
      Objc3SemaEvidenceCount(record.inline_asm_intrinsic_governance_ready) +
      Objc3SemaEvidenceCount(record.ns_error_bridging_ready) +
      Objc3SemaEvidenceCount(record.error_diagnostics_recovery_ready) +
      Objc3SemaEvidenceCount(record.result_like_lowering_ready);
  record.failed_validation_count =
      record.required_validation_count >= record.passed_validation_count
          ? (record.required_validation_count -
             record.passed_validation_count)
          : record.required_validation_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.unsafe_error_parity_validation_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_validation_count == 5u &&
      record.passed_validation_count == record.required_validation_count &&
      record.failed_validation_count == 0u;
  return record;
}

Objc3SemaControlBindingParityValidationReadinessRecord
BuildObjc3SemaControlBindingParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_unwind_cleanup_handoff,
    bool deterministic_async_continuation_handoff,
    bool deterministic_symbol_graph_scope_resolution_handoff,
    bool deterministic_method_lookup_override_conflict_handoff,
    bool deterministic_property_synthesis_ivar_binding_handoff,
    bool deterministic_id_class_sel_object_pointer_type_checking_handoff) {
  Objc3SemaControlBindingParityValidationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.unwind_cleanup_ready =
      deterministic_unwind_cleanup_handoff &&
      surface.unwind_cleanup_summary.unwind_cleanup_sites ==
          surface.unwind_cleanup_sites_total &&
      surface.unwind_cleanup_summary.exceptional_exit_sites ==
          surface.unwind_cleanup_exceptional_exit_sites_total &&
      surface.unwind_cleanup_summary.cleanup_action_sites ==
          surface.unwind_cleanup_action_sites_total &&
      surface.unwind_cleanup_summary.cleanup_scope_sites ==
          surface.unwind_cleanup_scope_sites_total &&
      surface.unwind_cleanup_summary.cleanup_resume_sites ==
          surface.unwind_cleanup_resume_sites_total &&
      surface.unwind_cleanup_summary.normalized_sites ==
          surface.unwind_cleanup_normalized_sites_total &&
      surface.unwind_cleanup_summary.fail_closed_sites ==
          surface.unwind_cleanup_fail_closed_sites_total &&
      surface.unwind_cleanup_summary.contract_violation_sites ==
          surface.unwind_cleanup_contract_violation_sites_total &&
      surface.unwind_cleanup_summary.exceptional_exit_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.cleanup_action_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.cleanup_scope_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.cleanup_resume_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.normalized_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.fail_closed_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.contract_violation_sites <=
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.normalized_sites +
              surface.unwind_cleanup_summary.fail_closed_sites ==
          surface.unwind_cleanup_summary.unwind_cleanup_sites &&
      surface.unwind_cleanup_summary.deterministic;
  record.async_continuation_ready =
      deterministic_async_continuation_handoff &&
      surface.async_continuation_summary.async_continuation_sites ==
          surface.async_continuation_sites_total &&
      surface.async_continuation_summary.async_keyword_sites ==
          surface.async_continuation_async_keyword_sites_total &&
      surface.async_continuation_summary.async_function_sites ==
          surface.async_continuation_async_function_sites_total &&
      surface.async_continuation_summary.continuation_allocation_sites ==
          surface.async_continuation_allocation_sites_total &&
      surface.async_continuation_summary.continuation_resume_sites ==
          surface.async_continuation_resume_sites_total &&
      surface.async_continuation_summary.continuation_suspend_sites ==
          surface.async_continuation_suspend_sites_total &&
      surface.async_continuation_summary.async_state_machine_sites ==
          surface.async_continuation_state_machine_sites_total &&
      surface.async_continuation_summary.normalized_sites ==
          surface.async_continuation_normalized_sites_total &&
      surface.async_continuation_summary.gate_blocked_sites ==
          surface.async_continuation_gate_blocked_sites_total &&
      surface.async_continuation_summary.contract_violation_sites ==
          surface.async_continuation_contract_violation_sites_total &&
      surface.async_continuation_summary.async_keyword_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.async_function_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.continuation_allocation_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.continuation_resume_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.continuation_suspend_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.async_state_machine_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.normalized_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.gate_blocked_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.contract_violation_sites <=
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.normalized_sites +
              surface.async_continuation_summary.gate_blocked_sites ==
          surface.async_continuation_summary.async_continuation_sites &&
      surface.async_continuation_summary.deterministic;
  record.symbol_graph_scope_resolution_ready =
      deterministic_symbol_graph_scope_resolution_handoff &&
      surface.symbol_graph_scope_resolution_summary.global_symbol_nodes ==
          surface.symbol_graph_global_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.function_symbol_nodes ==
          surface.symbol_graph_function_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.interface_symbol_nodes ==
          surface.symbol_graph_interface_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.implementation_symbol_nodes ==
          surface.symbol_graph_implementation_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary
              .interface_property_symbol_nodes ==
          surface.symbol_graph_interface_property_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_property_symbol_nodes ==
          surface.symbol_graph_implementation_property_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.interface_method_symbol_nodes ==
          surface.symbol_graph_interface_method_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_method_symbol_nodes ==
          surface.symbol_graph_implementation_method_symbol_nodes_total &&
      surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols ==
          surface.symbol_graph_top_level_scope_symbols_total &&
      surface.symbol_graph_scope_resolution_summary.nested_scope_symbols ==
          surface.symbol_graph_nested_scope_symbols_total &&
      surface.symbol_graph_scope_resolution_summary.scope_frames_total ==
          surface.symbol_graph_scope_frames_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_sites ==
          surface
              .symbol_graph_implementation_interface_resolution_sites_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_hits ==
          surface
              .symbol_graph_implementation_interface_resolution_hits_total &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_misses ==
          surface
              .symbol_graph_implementation_interface_resolution_misses_total &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_sites ==
          surface.symbol_graph_method_resolution_sites_total &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_hits ==
          surface.symbol_graph_method_resolution_hits_total &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_misses ==
          surface.symbol_graph_method_resolution_misses_total &&
      surface.symbol_graph_scope_resolution_summary.symbol_nodes_total() ==
          surface.symbol_graph_scope_resolution_summary
              .top_level_scope_symbols +
              surface.symbol_graph_scope_resolution_summary
                  .nested_scope_symbols &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_hits <=
          surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_sites &&
      surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_hits +
              surface.symbol_graph_scope_resolution_summary
                  .implementation_interface_resolution_misses ==
          surface.symbol_graph_scope_resolution_summary
              .implementation_interface_resolution_sites &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_hits <=
          surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      surface.symbol_graph_scope_resolution_summary.method_resolution_hits +
              surface.symbol_graph_scope_resolution_summary
                  .method_resolution_misses ==
          surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      surface.symbol_graph_scope_resolution_summary.resolution_hits_total() <=
          surface.symbol_graph_scope_resolution_summary
              .resolution_sites_total() &&
      surface.symbol_graph_scope_resolution_summary.resolution_hits_total() +
              surface.symbol_graph_scope_resolution_summary
                  .resolution_misses_total() ==
          surface.symbol_graph_scope_resolution_summary
              .resolution_sites_total() &&
      surface.symbol_graph_scope_resolution_summary.deterministic;
  record.method_lookup_override_conflict_ready =
      deterministic_method_lookup_override_conflict_handoff &&
      surface.method_lookup_override_conflict_summary.method_lookup_sites ==
          surface.method_lookup_override_conflict_lookup_sites_total &&
      surface.method_lookup_override_conflict_summary.method_lookup_hits ==
          surface.method_lookup_override_conflict_lookup_hits_total &&
      surface.method_lookup_override_conflict_summary.method_lookup_misses ==
          surface.method_lookup_override_conflict_lookup_misses_total &&
      surface.method_lookup_override_conflict_summary.override_lookup_sites ==
          surface.method_lookup_override_conflict_override_sites_total &&
      surface.method_lookup_override_conflict_summary.override_lookup_hits ==
          surface.method_lookup_override_conflict_override_hits_total &&
      surface.method_lookup_override_conflict_summary.override_lookup_misses ==
          surface.method_lookup_override_conflict_override_misses_total &&
      surface.method_lookup_override_conflict_summary.override_conflicts ==
          surface.method_lookup_override_conflict_override_conflicts_total &&
      surface.method_lookup_override_conflict_summary
              .unresolved_base_interfaces ==
          surface
              .method_lookup_override_conflict_unresolved_base_interfaces_total &&
      surface.method_lookup_override_conflict_summary.method_lookup_hits <=
          surface.method_lookup_override_conflict_summary.method_lookup_sites &&
      surface.method_lookup_override_conflict_summary.method_lookup_hits +
              surface.method_lookup_override_conflict_summary
                  .method_lookup_misses ==
          surface.method_lookup_override_conflict_summary.method_lookup_sites &&
      surface.method_lookup_override_conflict_summary.override_lookup_hits <=
          surface.method_lookup_override_conflict_summary.override_lookup_sites &&
      surface.method_lookup_override_conflict_summary.override_lookup_hits +
              surface.method_lookup_override_conflict_summary
                  .override_lookup_misses ==
          surface.method_lookup_override_conflict_summary.override_lookup_sites &&
      surface.method_lookup_override_conflict_summary.override_conflicts <=
          surface.method_lookup_override_conflict_summary.override_lookup_hits &&
      surface.method_lookup_override_conflict_summary.deterministic;
  record.property_synthesis_ivar_binding_ready =
      deterministic_property_synthesis_ivar_binding_handoff &&
      surface.property_synthesis_ivar_binding_summary.property_synthesis_sites ==
          surface
              .property_synthesis_ivar_binding_property_synthesis_sites_total &&
      surface.property_synthesis_ivar_binding_summary
              .property_synthesis_explicit_ivar_bindings ==
          surface
              .property_synthesis_ivar_binding_explicit_ivar_bindings_total &&
      surface.property_synthesis_ivar_binding_summary
              .property_synthesis_default_ivar_bindings ==
          surface
              .property_synthesis_ivar_binding_default_ivar_bindings_total &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
          surface.property_synthesis_ivar_binding_ivar_binding_sites_total &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved ==
          surface.property_synthesis_ivar_binding_ivar_binding_resolved_total &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_missing ==
          surface.property_synthesis_ivar_binding_ivar_binding_missing_total &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
          surface.property_synthesis_ivar_binding_ivar_binding_conflicts_total &&
      surface.property_synthesis_ivar_binding_summary
              .property_synthesis_explicit_ivar_bindings +
              surface.property_synthesis_ivar_binding_summary
                  .property_synthesis_default_ivar_bindings ==
          surface.property_synthesis_ivar_binding_summary
              .property_synthesis_sites &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
          surface.property_synthesis_ivar_binding_summary
              .property_synthesis_sites &&
      surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved +
              surface.property_synthesis_ivar_binding_summary
                  .ivar_binding_missing +
              surface.property_synthesis_ivar_binding_summary
                  .ivar_binding_conflicts ==
          surface.property_synthesis_ivar_binding_summary
              .ivar_binding_sites &&
      surface.property_synthesis_ivar_binding_summary.deterministic;
  record.id_class_sel_object_pointer_type_checking_ready =
      deterministic_id_class_sel_object_pointer_type_checking_handoff &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_type_sites ==
          surface.id_class_sel_object_pointer_param_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_id_spelling_sites ==
          surface.id_class_sel_object_pointer_param_id_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_class_spelling_sites ==
          surface.id_class_sel_object_pointer_param_class_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_sel_spelling_sites ==
          surface.id_class_sel_object_pointer_param_sel_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_instancetype_spelling_sites ==
          surface
              .id_class_sel_object_pointer_param_instancetype_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_object_pointer_type_sites ==
          surface
              .id_class_sel_object_pointer_param_object_pointer_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_type_sites ==
          surface.id_class_sel_object_pointer_return_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_id_spelling_sites ==
          surface.id_class_sel_object_pointer_return_id_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_class_spelling_sites ==
          surface
              .id_class_sel_object_pointer_return_class_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_sel_spelling_sites ==
          surface.id_class_sel_object_pointer_return_sel_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_instancetype_spelling_sites ==
          surface
              .id_class_sel_object_pointer_return_instancetype_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_object_pointer_type_sites ==
          surface
              .id_class_sel_object_pointer_return_object_pointer_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_type_sites ==
          surface.id_class_sel_object_pointer_property_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_id_spelling_sites ==
          surface.id_class_sel_object_pointer_property_id_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_class_spelling_sites ==
          surface
              .id_class_sel_object_pointer_property_class_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_sel_spelling_sites ==
          surface.id_class_sel_object_pointer_property_sel_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_instancetype_spelling_sites ==
          surface
              .id_class_sel_object_pointer_property_instancetype_spelling_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_object_pointer_type_sites ==
          surface
              .id_class_sel_object_pointer_property_object_pointer_type_sites_total &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .param_id_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .param_class_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .param_sel_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .param_instancetype_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .param_object_pointer_type_sites <=
          surface.id_class_sel_object_pointer_type_checking_summary
              .param_type_sites &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .return_id_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .return_class_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .return_sel_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .return_instancetype_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .return_object_pointer_type_sites <=
          surface.id_class_sel_object_pointer_type_checking_summary
              .return_type_sites &&
      surface.id_class_sel_object_pointer_type_checking_summary
              .property_id_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .property_class_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .property_sel_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .property_instancetype_spelling_sites +
              surface.id_class_sel_object_pointer_type_checking_summary
                  .property_object_pointer_type_sites <=
          surface.id_class_sel_object_pointer_type_checking_summary
              .property_type_sites &&
      surface.id_class_sel_object_pointer_type_checking_summary.deterministic;
  record.passed_validation_count =
      Objc3SemaEvidenceCount(record.unwind_cleanup_ready) +
      Objc3SemaEvidenceCount(record.async_continuation_ready) +
      Objc3SemaEvidenceCount(record.symbol_graph_scope_resolution_ready) +
      Objc3SemaEvidenceCount(record.method_lookup_override_conflict_ready) +
      Objc3SemaEvidenceCount(record.property_synthesis_ivar_binding_ready) +
      Objc3SemaEvidenceCount(
          record.id_class_sel_object_pointer_type_checking_ready);
  record.failed_validation_count =
      record.required_validation_count >= record.passed_validation_count
          ? (record.required_validation_count -
             record.passed_validation_count)
          : record.required_validation_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.control_binding_parity_validation_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_validation_count == 6u &&
      record.passed_validation_count == record.required_validation_count &&
      record.failed_validation_count == 0u;
  return record;
}

Objc3SemaAsyncBlockMessageParityValidationReadinessRecord
BuildObjc3SemaAsyncBlockMessageParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_await_lowering_suspension_state_lowering_handoff,
    bool deterministic_block_literal_capture_semantics_handoff,
    bool deterministic_block_abi_invoke_trampoline_handoff,
    bool deterministic_block_storage_escape_handoff,
    bool deterministic_block_copy_dispose_handoff,
    bool deterministic_block_determinism_perf_baseline_handoff,
    bool deterministic_message_send_selector_lowering_handoff) {
  Objc3SemaAsyncBlockMessageParityValidationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  const auto &await_summary =
      surface.await_lowering_suspension_state_lowering_summary;
  record.await_lowering_suspension_state_lowering_ready =
      deterministic_await_lowering_suspension_state_lowering_handoff &&
      await_summary.await_suspension_sites ==
          surface.await_lowering_suspension_state_lowering_sites_total &&
      await_summary.await_keyword_sites ==
          surface
              .await_lowering_suspension_state_lowering_await_keyword_sites_total &&
      await_summary.await_suspension_point_sites ==
          surface
              .await_lowering_suspension_state_lowering_await_suspension_point_sites_total &&
      await_summary.await_resume_sites ==
          surface
              .await_lowering_suspension_state_lowering_await_resume_sites_total &&
      await_summary.await_state_machine_sites ==
          surface
              .await_lowering_suspension_state_lowering_await_state_machine_sites_total &&
      await_summary.await_continuation_sites ==
          surface
              .await_lowering_suspension_state_lowering_await_continuation_sites_total &&
      await_summary.normalized_sites ==
          surface
              .await_lowering_suspension_state_lowering_normalized_sites_total &&
      await_summary.gate_blocked_sites ==
          surface
              .await_lowering_suspension_state_lowering_gate_blocked_sites_total &&
      await_summary.contract_violation_sites ==
          surface
              .await_lowering_suspension_state_lowering_contract_violation_sites_total &&
      await_summary.await_keyword_sites <=
          await_summary.await_suspension_sites &&
      await_summary.await_suspension_point_sites <=
          await_summary.await_suspension_sites &&
      await_summary.await_resume_sites <=
          await_summary.await_suspension_sites &&
      await_summary.await_state_machine_sites <=
          await_summary.await_suspension_point_sites &&
      await_summary.await_continuation_sites <=
          await_summary.await_suspension_point_sites &&
      await_summary.normalized_sites <= await_summary.await_suspension_sites &&
      await_summary.gate_blocked_sites <=
          await_summary.await_suspension_sites &&
      await_summary.contract_violation_sites <=
          await_summary.await_suspension_sites &&
      await_summary.normalized_sites + await_summary.gate_blocked_sites ==
          await_summary.await_suspension_sites &&
      await_summary.deterministic;
  const auto &block_literal_summary =
      surface.block_literal_capture_semantics_summary;
  record.block_literal_capture_semantics_ready =
      deterministic_block_literal_capture_semantics_handoff &&
      block_literal_summary.block_literal_sites ==
          surface.block_literal_capture_semantics_sites_total &&
      block_literal_summary.block_parameter_entries ==
          surface.block_literal_capture_semantics_parameter_entries_total &&
      block_literal_summary.block_capture_entries ==
          surface.block_literal_capture_semantics_capture_entries_total &&
      block_literal_summary.block_body_statement_entries ==
          surface.block_literal_capture_semantics_body_statement_entries_total &&
      block_literal_summary.block_empty_capture_sites ==
          surface.block_literal_capture_semantics_empty_capture_sites_total &&
      block_literal_summary.block_nondeterministic_capture_sites ==
          surface
              .block_literal_capture_semantics_nondeterministic_capture_sites_total &&
      block_literal_summary.block_non_normalized_sites ==
          surface.block_literal_capture_semantics_non_normalized_sites_total &&
      block_literal_summary.contract_violation_sites ==
          surface.block_literal_capture_semantics_contract_violation_sites_total &&
      block_literal_summary.block_empty_capture_sites <=
          block_literal_summary.block_literal_sites &&
      block_literal_summary.block_nondeterministic_capture_sites <=
          block_literal_summary.block_literal_sites &&
      block_literal_summary.block_non_normalized_sites <=
          block_literal_summary.block_literal_sites &&
      block_literal_summary.contract_violation_sites <=
          block_literal_summary.block_literal_sites &&
      block_literal_summary.deterministic;
  const auto &block_abi_summary =
      surface.block_abi_invoke_trampoline_semantics_summary;
  record.block_abi_invoke_trampoline_ready =
      deterministic_block_abi_invoke_trampoline_handoff &&
      block_abi_summary.block_literal_sites ==
          surface.block_abi_invoke_trampoline_sites_total &&
      block_abi_summary.invoke_argument_slots_total ==
          surface.block_abi_invoke_trampoline_invoke_argument_slots_total &&
      block_abi_summary.capture_word_count_total ==
          surface.block_abi_invoke_trampoline_capture_word_count_total &&
      block_abi_summary.parameter_entries_total ==
          surface.block_abi_invoke_trampoline_parameter_entries_total &&
      block_abi_summary.capture_entries_total ==
          surface.block_abi_invoke_trampoline_capture_entries_total &&
      block_abi_summary.body_statement_entries_total ==
          surface.block_abi_invoke_trampoline_body_statement_entries_total &&
      block_abi_summary.descriptor_symbolized_sites ==
          surface.block_abi_invoke_trampoline_descriptor_symbolized_sites_total &&
      block_abi_summary.invoke_trampoline_symbolized_sites ==
          surface.block_abi_invoke_trampoline_invoke_symbolized_sites_total &&
      block_abi_summary.missing_invoke_trampoline_sites ==
          surface.block_abi_invoke_trampoline_missing_invoke_sites_total &&
      block_abi_summary.non_normalized_layout_sites ==
          surface.block_abi_invoke_trampoline_non_normalized_layout_sites_total &&
      block_abi_summary.contract_violation_sites ==
          surface.block_abi_invoke_trampoline_contract_violation_sites_total &&
      block_abi_summary.descriptor_symbolized_sites <=
          block_abi_summary.block_literal_sites &&
      block_abi_summary.invoke_trampoline_symbolized_sites <=
          block_abi_summary.block_literal_sites &&
      block_abi_summary.missing_invoke_trampoline_sites <=
          block_abi_summary.block_literal_sites &&
      block_abi_summary.non_normalized_layout_sites <=
          block_abi_summary.block_literal_sites &&
      block_abi_summary.contract_violation_sites <=
          block_abi_summary.block_literal_sites &&
      block_abi_summary.invoke_trampoline_symbolized_sites +
              block_abi_summary.missing_invoke_trampoline_sites ==
          block_abi_summary.block_literal_sites &&
      block_abi_summary.invoke_argument_slots_total ==
          block_abi_summary.parameter_entries_total &&
      block_abi_summary.capture_word_count_total ==
          block_abi_summary.capture_entries_total &&
      block_abi_summary.deterministic;
  const auto &block_storage_summary =
      surface.block_storage_escape_semantics_summary;
  record.block_storage_escape_ready =
      deterministic_block_storage_escape_handoff &&
      block_storage_summary.block_literal_sites ==
          surface.block_storage_escape_sites_total &&
      block_storage_summary.mutable_capture_count_total ==
          surface.block_storage_escape_mutable_capture_count_total &&
      block_storage_summary.byref_slot_count_total ==
          surface.block_storage_escape_byref_slot_count_total &&
      block_storage_summary.parameter_entries_total ==
          surface.block_storage_escape_parameter_entries_total &&
      block_storage_summary.capture_entries_total ==
          surface.block_storage_escape_capture_entries_total &&
      block_storage_summary.body_statement_entries_total ==
          surface.block_storage_escape_body_statement_entries_total &&
      block_storage_summary.requires_byref_cells_sites ==
          surface.block_storage_escape_requires_byref_cells_sites_total &&
      block_storage_summary.escape_analysis_enabled_sites ==
          surface.block_storage_escape_escape_analysis_enabled_sites_total &&
      block_storage_summary.escape_to_heap_sites ==
          surface.block_storage_escape_escape_to_heap_sites_total &&
      block_storage_summary.escape_profile_normalized_sites ==
          surface.block_storage_escape_escape_profile_normalized_sites_total &&
      block_storage_summary.byref_layout_symbolized_sites ==
          surface.block_storage_escape_byref_layout_symbolized_sites_total &&
      block_storage_summary.contract_violation_sites ==
          surface.block_storage_escape_contract_violation_sites_total &&
      block_storage_summary.requires_byref_cells_sites <=
          block_storage_summary.block_literal_sites &&
      block_storage_summary.escape_analysis_enabled_sites <=
          block_storage_summary.block_literal_sites &&
      block_storage_summary.escape_to_heap_sites <=
          block_storage_summary.block_literal_sites &&
      block_storage_summary.escape_profile_normalized_sites <=
          block_storage_summary.block_literal_sites &&
      block_storage_summary.byref_layout_symbolized_sites <=
          block_storage_summary.block_literal_sites &&
      block_storage_summary.contract_violation_sites <=
          block_storage_summary.block_literal_sites &&
      block_storage_summary.mutable_capture_count_total <=
          block_storage_summary.capture_entries_total &&
      block_storage_summary.byref_slot_count_total <=
          block_storage_summary.mutable_capture_count_total &&
      block_storage_summary.escape_analysis_enabled_sites ==
          block_storage_summary.block_literal_sites &&
      block_storage_summary.deterministic;
  const auto &block_copy_summary = surface.block_copy_dispose_semantics_summary;
  record.block_copy_dispose_ready =
      deterministic_block_copy_dispose_handoff &&
      block_copy_summary.block_literal_sites ==
          surface.block_copy_dispose_sites_total &&
      block_copy_summary.mutable_capture_count_total ==
          surface.block_copy_dispose_mutable_capture_count_total &&
      block_copy_summary.byref_slot_count_total ==
          surface.block_copy_dispose_byref_slot_count_total &&
      block_copy_summary.parameter_entries_total ==
          surface.block_copy_dispose_parameter_entries_total &&
      block_copy_summary.capture_entries_total ==
          surface.block_copy_dispose_capture_entries_total &&
      block_copy_summary.body_statement_entries_total ==
          surface.block_copy_dispose_body_statement_entries_total &&
      block_copy_summary.copy_helper_required_sites ==
          surface.block_copy_dispose_copy_helper_required_sites_total &&
      block_copy_summary.dispose_helper_required_sites ==
          surface.block_copy_dispose_dispose_helper_required_sites_total &&
      block_copy_summary.profile_normalized_sites ==
          surface.block_copy_dispose_profile_normalized_sites_total &&
      block_copy_summary.copy_helper_symbolized_sites ==
          surface.block_copy_dispose_copy_helper_symbolized_sites_total &&
      block_copy_summary.dispose_helper_symbolized_sites ==
          surface.block_copy_dispose_dispose_helper_symbolized_sites_total &&
      block_copy_summary.contract_violation_sites ==
          surface.block_copy_dispose_contract_violation_sites_total &&
      block_copy_summary.copy_helper_required_sites <=
          block_copy_summary.block_literal_sites &&
      block_copy_summary.dispose_helper_required_sites <=
          block_copy_summary.block_literal_sites &&
      block_copy_summary.profile_normalized_sites <=
          block_copy_summary.block_literal_sites &&
      block_copy_summary.copy_helper_symbolized_sites <=
          block_copy_summary.block_literal_sites &&
      block_copy_summary.dispose_helper_symbolized_sites <=
          block_copy_summary.block_literal_sites &&
      block_copy_summary.contract_violation_sites <=
          block_copy_summary.block_literal_sites &&
      block_copy_summary.mutable_capture_count_total <=
          block_copy_summary.capture_entries_total &&
      block_copy_summary.byref_slot_count_total <=
          block_copy_summary.mutable_capture_count_total &&
      block_copy_summary.copy_helper_required_sites <=
          block_copy_summary.dispose_helper_required_sites &&
      block_copy_summary.deterministic;
  const auto &block_determinism_summary =
      surface.block_determinism_perf_baseline_summary;
  record.block_determinism_perf_baseline_ready =
      deterministic_block_determinism_perf_baseline_handoff &&
      block_determinism_summary.block_literal_sites ==
          surface.block_determinism_perf_baseline_sites_total &&
      block_determinism_summary.baseline_weight_total ==
          surface.block_determinism_perf_baseline_weight_total &&
      block_determinism_summary.parameter_entries_total ==
          surface.block_determinism_perf_baseline_parameter_entries_total &&
      block_determinism_summary.capture_entries_total ==
          surface.block_determinism_perf_baseline_capture_entries_total &&
      block_determinism_summary.body_statement_entries_total ==
          surface.block_determinism_perf_baseline_body_statement_entries_total &&
      block_determinism_summary.deterministic_capture_sites ==
          surface
              .block_determinism_perf_baseline_deterministic_capture_sites_total &&
      block_determinism_summary.heavy_tier_sites ==
          surface.block_determinism_perf_baseline_heavy_tier_sites_total &&
      block_determinism_summary.normalized_profile_sites ==
          surface
              .block_determinism_perf_baseline_normalized_profile_sites_total &&
      block_determinism_summary.contract_violation_sites ==
          surface
              .block_determinism_perf_baseline_contract_violation_sites_total &&
      block_determinism_summary.deterministic_capture_sites <=
          block_determinism_summary.block_literal_sites &&
      block_determinism_summary.heavy_tier_sites <=
          block_determinism_summary.block_literal_sites &&
      block_determinism_summary.normalized_profile_sites <=
          block_determinism_summary.block_literal_sites &&
      block_determinism_summary.contract_violation_sites <=
          block_determinism_summary.block_literal_sites &&
      block_determinism_summary.deterministic;
  const auto &message_send_summary =
      surface.message_send_selector_lowering_summary;
  record.message_send_selector_lowering_ready =
      deterministic_message_send_selector_lowering_handoff &&
      message_send_summary.message_send_sites ==
          surface.message_send_selector_lowering_sites_total &&
      message_send_summary.unary_form_sites ==
          surface.message_send_selector_lowering_unary_form_sites_total &&
      message_send_summary.keyword_form_sites ==
          surface.message_send_selector_lowering_keyword_form_sites_total &&
      message_send_summary.selector_lowering_symbol_sites ==
          surface.message_send_selector_lowering_symbol_sites_total &&
      message_send_summary.selector_lowering_piece_entries ==
          surface.message_send_selector_lowering_piece_entries_total &&
      message_send_summary.selector_lowering_argument_piece_entries ==
          surface.message_send_selector_lowering_argument_piece_entries_total &&
      message_send_summary.selector_lowering_normalized_sites ==
          surface.message_send_selector_lowering_normalized_sites_total &&
      message_send_summary.selector_lowering_form_mismatch_sites ==
          surface.message_send_selector_lowering_form_mismatch_sites_total &&
      message_send_summary.selector_lowering_arity_mismatch_sites ==
          surface.message_send_selector_lowering_arity_mismatch_sites_total &&
      message_send_summary.selector_lowering_symbol_mismatch_sites ==
          surface.message_send_selector_lowering_symbol_mismatch_sites_total &&
      message_send_summary.selector_lowering_missing_symbol_sites ==
          surface.message_send_selector_lowering_missing_symbol_sites_total &&
      message_send_summary.selector_lowering_contract_violation_sites ==
          surface.message_send_selector_lowering_contract_violation_sites_total &&
      message_send_summary.unary_form_sites +
              message_send_summary.keyword_form_sites ==
          message_send_summary.message_send_sites &&
      message_send_summary.selector_lowering_symbol_sites <=
          message_send_summary.message_send_sites &&
      message_send_summary.selector_lowering_argument_piece_entries <=
          message_send_summary.selector_lowering_piece_entries &&
      message_send_summary.selector_lowering_normalized_sites <=
          message_send_summary.selector_lowering_symbol_sites &&
      message_send_summary.selector_lowering_form_mismatch_sites <=
          message_send_summary.message_send_sites &&
      message_send_summary.selector_lowering_arity_mismatch_sites <=
          message_send_summary.message_send_sites &&
      message_send_summary.selector_lowering_symbol_mismatch_sites <=
          message_send_summary.message_send_sites &&
      message_send_summary.selector_lowering_missing_symbol_sites <=
          message_send_summary.message_send_sites &&
      message_send_summary.selector_lowering_contract_violation_sites <=
          message_send_summary.message_send_sites &&
      message_send_summary.deterministic;
  record.passed_validation_count =
      Objc3SemaEvidenceCount(
          record.await_lowering_suspension_state_lowering_ready) +
      Objc3SemaEvidenceCount(record.block_literal_capture_semantics_ready) +
      Objc3SemaEvidenceCount(record.block_abi_invoke_trampoline_ready) +
      Objc3SemaEvidenceCount(record.block_storage_escape_ready) +
      Objc3SemaEvidenceCount(record.block_copy_dispose_ready) +
      Objc3SemaEvidenceCount(record.block_determinism_perf_baseline_ready) +
      Objc3SemaEvidenceCount(record.message_send_selector_lowering_ready);
  record.failed_validation_count =
      record.required_validation_count >= record.passed_validation_count
          ? (record.required_validation_count -
             record.passed_validation_count)
          : record.required_validation_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.async_block_message_parity_validation_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_validation_count == 7u &&
      record.passed_validation_count == record.required_validation_count &&
      record.failed_validation_count == 0u;
  return record;
}

Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord
BuildObjc3SemaDispatchRuntimeArcParityValidationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface,
    bool deterministic_dispatch_abi_marshalling_handoff,
    bool deterministic_nil_receiver_semantics_foldability_handoff,
    bool deterministic_super_dispatch_method_family_handoff,
    bool deterministic_runtime_link_host_link_handoff,
    bool deterministic_retain_release_operation_handoff,
    bool deterministic_weak_unowned_semantics_handoff,
    bool deterministic_arc_diagnostics_fixit_handoff,
    bool deterministic_autoreleasepool_scope_handoff) {
  Objc3SemaDispatchRuntimeArcParityValidationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.typed_semantic_handoff_owner = input.typed_semantic_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  const auto &dispatch_summary = surface.dispatch_abi_marshalling_summary;
  record.dispatch_abi_marshalling_ready =
      deterministic_dispatch_abi_marshalling_handoff &&
      dispatch_summary.message_send_sites ==
          surface.dispatch_abi_marshalling_sites_total &&
      dispatch_summary.receiver_slots ==
          surface.dispatch_abi_marshalling_receiver_slots_total &&
      dispatch_summary.selector_symbol_slots ==
          surface.dispatch_abi_marshalling_selector_symbol_slots_total &&
      dispatch_summary.argument_slots ==
          surface.dispatch_abi_marshalling_argument_slots_total &&
      dispatch_summary.keyword_argument_slots ==
          surface.dispatch_abi_marshalling_keyword_argument_slots_total &&
      dispatch_summary.unary_argument_slots ==
          surface.dispatch_abi_marshalling_unary_argument_slots_total &&
      dispatch_summary.arity_mismatch_sites ==
          surface.dispatch_abi_marshalling_arity_mismatch_sites_total &&
      dispatch_summary.missing_selector_symbol_sites ==
          surface.dispatch_abi_marshalling_missing_selector_symbol_sites_total &&
      dispatch_summary.contract_violation_sites ==
          surface.dispatch_abi_marshalling_contract_violation_sites_total &&
      dispatch_summary.receiver_slots == dispatch_summary.message_send_sites &&
      dispatch_summary.selector_symbol_slots +
              dispatch_summary.missing_selector_symbol_sites ==
          dispatch_summary.message_send_sites &&
      dispatch_summary.keyword_argument_slots +
              dispatch_summary.unary_argument_slots ==
          dispatch_summary.argument_slots &&
      dispatch_summary.keyword_argument_slots <= dispatch_summary.argument_slots &&
      dispatch_summary.unary_argument_slots <= dispatch_summary.argument_slots &&
      dispatch_summary.selector_symbol_slots <=
          dispatch_summary.message_send_sites &&
      dispatch_summary.missing_selector_symbol_sites <=
          dispatch_summary.message_send_sites &&
      dispatch_summary.arity_mismatch_sites <=
          dispatch_summary.message_send_sites &&
      dispatch_summary.contract_violation_sites <=
          dispatch_summary.message_send_sites &&
      dispatch_summary.deterministic;
  const auto &nil_receiver_summary =
      surface.nil_receiver_semantics_foldability_summary;
  record.nil_receiver_semantics_foldability_ready =
      deterministic_nil_receiver_semantics_foldability_handoff &&
      nil_receiver_summary.message_send_sites ==
          surface.nil_receiver_semantics_foldability_sites_total &&
      nil_receiver_summary.receiver_nil_literal_sites ==
          surface
              .nil_receiver_semantics_foldability_receiver_nil_literal_sites_total &&
      nil_receiver_summary.nil_receiver_semantics_enabled_sites ==
          surface.nil_receiver_semantics_foldability_enabled_sites_total &&
      nil_receiver_summary.nil_receiver_foldable_sites ==
          surface.nil_receiver_semantics_foldability_foldable_sites_total &&
      nil_receiver_summary.nil_receiver_runtime_dispatch_required_sites ==
          surface
              .nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total &&
      nil_receiver_summary.non_nil_receiver_sites ==
          surface.nil_receiver_semantics_foldability_non_nil_receiver_sites_total &&
      nil_receiver_summary.contract_violation_sites ==
          surface.nil_receiver_semantics_foldability_contract_violation_sites_total &&
      nil_receiver_summary.receiver_nil_literal_sites ==
          nil_receiver_summary.nil_receiver_semantics_enabled_sites &&
      nil_receiver_summary.nil_receiver_foldable_sites <=
          nil_receiver_summary.nil_receiver_semantics_enabled_sites &&
      nil_receiver_summary.nil_receiver_runtime_dispatch_required_sites +
              nil_receiver_summary.nil_receiver_foldable_sites ==
          nil_receiver_summary.message_send_sites &&
      nil_receiver_summary.nil_receiver_semantics_enabled_sites +
              nil_receiver_summary.non_nil_receiver_sites ==
          nil_receiver_summary.message_send_sites &&
      nil_receiver_summary.contract_violation_sites <=
          nil_receiver_summary.message_send_sites &&
      nil_receiver_summary.deterministic;
  const auto &super_dispatch_summary =
      surface.super_dispatch_method_family_summary;
  record.super_dispatch_method_family_ready =
      deterministic_super_dispatch_method_family_handoff &&
      super_dispatch_summary.message_send_sites ==
          surface.super_dispatch_method_family_sites_total &&
      super_dispatch_summary.receiver_super_identifier_sites ==
          surface
              .super_dispatch_method_family_receiver_super_identifier_sites_total &&
      super_dispatch_summary.super_dispatch_enabled_sites ==
          surface.super_dispatch_method_family_enabled_sites_total &&
      super_dispatch_summary.super_dispatch_requires_class_context_sites ==
          surface
              .super_dispatch_method_family_requires_class_context_sites_total &&
      super_dispatch_summary.method_family_init_sites ==
          surface.super_dispatch_method_family_init_sites_total &&
      super_dispatch_summary.method_family_copy_sites ==
          surface.super_dispatch_method_family_copy_sites_total &&
      super_dispatch_summary.method_family_mutable_copy_sites ==
          surface.super_dispatch_method_family_mutable_copy_sites_total &&
      super_dispatch_summary.method_family_new_sites ==
          surface.super_dispatch_method_family_new_sites_total &&
      super_dispatch_summary.method_family_none_sites ==
          surface.super_dispatch_method_family_none_sites_total &&
      super_dispatch_summary.method_family_returns_retained_result_sites ==
          surface
              .super_dispatch_method_family_returns_retained_result_sites_total &&
      super_dispatch_summary.method_family_returns_related_result_sites ==
          surface
              .super_dispatch_method_family_returns_related_result_sites_total &&
      super_dispatch_summary.contract_violation_sites ==
          surface.super_dispatch_method_family_contract_violation_sites_total &&
      super_dispatch_summary.receiver_super_identifier_sites ==
          super_dispatch_summary.super_dispatch_enabled_sites &&
      super_dispatch_summary.super_dispatch_requires_class_context_sites ==
          super_dispatch_summary.super_dispatch_enabled_sites &&
      super_dispatch_summary.method_family_init_sites +
              super_dispatch_summary.method_family_copy_sites +
              super_dispatch_summary.method_family_mutable_copy_sites +
              super_dispatch_summary.method_family_new_sites +
              super_dispatch_summary.method_family_none_sites ==
          super_dispatch_summary.message_send_sites &&
      super_dispatch_summary.method_family_returns_related_result_sites <=
          super_dispatch_summary.method_family_init_sites &&
      super_dispatch_summary.method_family_returns_retained_result_sites <=
          super_dispatch_summary.message_send_sites &&
      super_dispatch_summary.contract_violation_sites <=
          super_dispatch_summary.message_send_sites &&
      super_dispatch_summary.deterministic;
  const auto &runtime_link_summary = surface.runtime_link_host_link_summary;
  record.runtime_link_host_link_ready =
      deterministic_runtime_link_host_link_handoff &&
      runtime_link_summary.message_send_sites ==
          surface.runtime_link_host_link_message_send_sites_total &&
      runtime_link_summary.runtime_link_required_sites ==
          surface.runtime_link_host_link_required_sites_total &&
      runtime_link_summary.runtime_link_elided_sites ==
          surface.runtime_link_host_link_elided_sites_total &&
      runtime_link_summary.runtime_dispatch_arg_slots ==
          surface.runtime_link_host_link_runtime_dispatch_arg_slots_total &&
      runtime_link_summary.runtime_dispatch_declaration_parameter_count ==
          surface
              .runtime_link_host_link_runtime_dispatch_declaration_parameter_count_total &&
      runtime_link_summary.contract_violation_sites ==
          surface.runtime_link_host_link_contract_violation_sites_total &&
      runtime_link_summary.runtime_dispatch_symbol ==
          surface.runtime_link_host_link_runtime_dispatch_symbol &&
      runtime_link_summary.default_runtime_dispatch_symbol_binding ==
          surface
              .runtime_link_host_link_default_runtime_dispatch_symbol_binding &&
      runtime_link_summary.runtime_link_required_sites +
              runtime_link_summary.runtime_link_elided_sites ==
          runtime_link_summary.message_send_sites &&
      runtime_link_summary.contract_violation_sites <=
          runtime_link_summary.message_send_sites &&
      (runtime_link_summary.message_send_sites == 0 ||
       runtime_link_summary.runtime_dispatch_declaration_parameter_count ==
           runtime_link_summary.runtime_dispatch_arg_slots + 2u) &&
      (runtime_link_summary.default_runtime_dispatch_symbol_binding ==
       (runtime_link_summary.runtime_dispatch_symbol ==
        kObjc3RuntimeLinkHostLinkDefaultDispatchSymbol)) &&
      runtime_link_summary.deterministic;
  const auto &retain_release_summary = surface.retain_release_operation_summary;
  record.retain_release_operation_ready =
      deterministic_retain_release_operation_handoff &&
      retain_release_summary.ownership_qualified_sites ==
          surface.retain_release_operation_ownership_qualified_sites_total &&
      retain_release_summary.retain_insertion_sites ==
          surface.retain_release_operation_retain_insertion_sites_total &&
      retain_release_summary.release_insertion_sites ==
          surface.retain_release_operation_release_insertion_sites_total &&
      retain_release_summary.autorelease_insertion_sites ==
          surface.retain_release_operation_autorelease_insertion_sites_total &&
      retain_release_summary.contract_violation_sites ==
          surface.retain_release_operation_contract_violation_sites_total &&
      retain_release_summary.retain_insertion_sites <=
          retain_release_summary.ownership_qualified_sites +
              retain_release_summary.contract_violation_sites &&
      retain_release_summary.release_insertion_sites <=
          retain_release_summary.ownership_qualified_sites +
              retain_release_summary.contract_violation_sites &&
      retain_release_summary.autorelease_insertion_sites <=
          retain_release_summary.ownership_qualified_sites +
              retain_release_summary.contract_violation_sites &&
      retain_release_summary.deterministic;
  const auto &weak_unowned_summary = surface.weak_unowned_semantics_summary;
  record.weak_unowned_semantics_ready =
      deterministic_weak_unowned_semantics_handoff &&
      weak_unowned_summary.ownership_candidate_sites ==
          surface.weak_unowned_semantics_ownership_candidate_sites_total &&
      weak_unowned_summary.weak_reference_sites ==
          surface.weak_unowned_semantics_weak_reference_sites_total &&
      weak_unowned_summary.unowned_reference_sites ==
          surface.weak_unowned_semantics_unowned_reference_sites_total &&
      weak_unowned_summary.unowned_safe_reference_sites ==
          surface.weak_unowned_semantics_unowned_safe_reference_sites_total &&
      weak_unowned_summary.weak_unowned_conflict_sites ==
          surface.weak_unowned_semantics_conflict_sites_total &&
      weak_unowned_summary.contract_violation_sites ==
          surface.weak_unowned_semantics_contract_violation_sites_total &&
      weak_unowned_summary.unowned_safe_reference_sites <=
          weak_unowned_summary.unowned_reference_sites &&
      weak_unowned_summary.weak_unowned_conflict_sites <=
          weak_unowned_summary.ownership_candidate_sites &&
      weak_unowned_summary.contract_violation_sites <=
          weak_unowned_summary.ownership_candidate_sites +
              weak_unowned_summary.weak_unowned_conflict_sites &&
      weak_unowned_summary.deterministic;
  const auto &arc_diagnostics_summary = surface.arc_diagnostics_fixit_summary;
  record.arc_diagnostics_fixit_ready =
      deterministic_arc_diagnostics_fixit_handoff &&
      arc_diagnostics_summary.ownership_arc_diagnostic_candidate_sites ==
          surface.ownership_arc_diagnostic_candidate_sites_total &&
      arc_diagnostics_summary.ownership_arc_fixit_available_sites ==
          surface.ownership_arc_fixit_available_sites_total &&
      arc_diagnostics_summary.ownership_arc_profiled_sites ==
          surface.ownership_arc_profiled_sites_total &&
      arc_diagnostics_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites ==
          surface.ownership_arc_weak_unowned_conflict_diagnostic_sites_total &&
      arc_diagnostics_summary.ownership_arc_empty_fixit_hint_sites ==
          surface.ownership_arc_empty_fixit_hint_sites_total &&
      arc_diagnostics_summary.contract_violation_sites ==
          surface.ownership_arc_contract_violation_sites_total &&
      arc_diagnostics_summary.ownership_arc_fixit_available_sites <=
          arc_diagnostics_summary.ownership_arc_diagnostic_candidate_sites +
              arc_diagnostics_summary.contract_violation_sites &&
      arc_diagnostics_summary.ownership_arc_profiled_sites <=
          arc_diagnostics_summary.ownership_arc_diagnostic_candidate_sites +
              arc_diagnostics_summary.contract_violation_sites &&
      arc_diagnostics_summary
              .ownership_arc_weak_unowned_conflict_diagnostic_sites <=
          arc_diagnostics_summary.ownership_arc_diagnostic_candidate_sites +
              arc_diagnostics_summary.contract_violation_sites &&
      arc_diagnostics_summary.ownership_arc_empty_fixit_hint_sites <=
          arc_diagnostics_summary.ownership_arc_fixit_available_sites +
              arc_diagnostics_summary.contract_violation_sites &&
      arc_diagnostics_summary.deterministic;
  const auto &autoreleasepool_summary = surface.autoreleasepool_scope_summary;
  record.autoreleasepool_scope_ready =
      deterministic_autoreleasepool_scope_handoff &&
      autoreleasepool_summary.scope_sites ==
          surface.autoreleasepool_scope_sites_total &&
      autoreleasepool_summary.scope_symbolized_sites ==
          surface.autoreleasepool_scope_symbolized_sites_total &&
      autoreleasepool_summary.contract_violation_sites ==
          surface.autoreleasepool_scope_contract_violation_sites_total &&
      autoreleasepool_summary.max_scope_depth ==
          surface.autoreleasepool_scope_max_depth_total &&
      autoreleasepool_summary.scope_symbolized_sites <=
          autoreleasepool_summary.scope_sites &&
      autoreleasepool_summary.contract_violation_sites <=
          autoreleasepool_summary.scope_sites &&
      (autoreleasepool_summary.scope_sites > 0u ||
       autoreleasepool_summary.max_scope_depth == 0u) &&
      autoreleasepool_summary.max_scope_depth <=
          static_cast<unsigned>(autoreleasepool_summary.scope_sites) &&
      autoreleasepool_summary.deterministic;
  record.passed_validation_count =
      Objc3SemaEvidenceCount(record.dispatch_abi_marshalling_ready) +
      Objc3SemaEvidenceCount(
          record.nil_receiver_semantics_foldability_ready) +
      Objc3SemaEvidenceCount(record.super_dispatch_method_family_ready) +
      Objc3SemaEvidenceCount(record.runtime_link_host_link_ready) +
      Objc3SemaEvidenceCount(record.retain_release_operation_ready) +
      Objc3SemaEvidenceCount(record.weak_unowned_semantics_ready) +
      Objc3SemaEvidenceCount(record.arc_diagnostics_fixit_ready) +
      Objc3SemaEvidenceCount(record.autoreleasepool_scope_ready);
  record.failed_validation_count =
      record.required_validation_count >= record.passed_validation_count
          ? (record.required_validation_count -
             record.passed_validation_count)
          : record.required_validation_count;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.dispatch_runtime_arc_parity_validation_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.typed_semantic_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_validation_count == 8u &&
      record.passed_validation_count == record.required_validation_count &&
      record.failed_validation_count == 0u;
  return record;
}

Objc3ParserSemaConformanceEvidenceRecord
BuildObjc3ParserSemaConformanceEvidenceRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3ParserSemaConformanceEvidenceRecord record;
  const Objc3ParserSemaConformanceMatrix &matrix =
      surface.parser_sema_conformance_matrix;
  const Objc3ParserSemaConformanceCorpus &corpus =
      surface.parser_sema_conformance_corpus;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.conformance_matrix_deterministic =
      surface.deterministic_parser_sema_conformance_matrix &&
      matrix.deterministic;
  record.conformance_corpus_deterministic =
      surface.deterministic_parser_sema_conformance_corpus &&
      corpus.deterministic;
  record.declaration_count_evidence_ready =
      matrix.top_level_declaration_count_matches &&
      matrix.global_decl_count_matches && matrix.protocol_decl_count_matches &&
      matrix.interface_decl_count_matches &&
      matrix.implementation_decl_count_matches &&
      matrix.function_decl_count_matches;
  record.member_count_evidence_ready =
      matrix.protocol_property_decl_count_matches &&
      matrix.protocol_method_decl_count_matches &&
      matrix.protocol_class_method_decl_count_matches &&
      matrix.protocol_instance_method_decl_count_matches &&
      matrix.interface_property_decl_count_matches &&
      matrix.interface_method_decl_count_matches &&
      matrix.interface_class_method_decl_count_matches &&
      matrix.interface_instance_method_decl_count_matches &&
      matrix.implementation_property_decl_count_matches &&
      matrix.implementation_method_decl_count_matches &&
      matrix.implementation_class_method_decl_count_matches &&
      matrix.implementation_instance_method_decl_count_matches;
  record.category_function_evidence_ready =
      matrix.interface_category_decl_count_matches &&
      matrix.implementation_category_decl_count_matches &&
      matrix.function_prototype_count_matches &&
      matrix.function_pure_count_matches;
  record.fingerprint_evidence_ready =
      matrix.ast_shape_fingerprint_matches &&
      matrix.ast_top_level_layout_fingerprint_matches &&
      matrix.parser_contract_snapshot_fingerprint_matches;
  record.parser_budget_replay_evidence_ready =
      matrix.parser_diagnostic_budget_consistent &&
      matrix.parser_token_top_level_budget_consistent &&
      matrix.parser_subset_count_consistent &&
      matrix.parser_contract_snapshot_deterministic &&
      matrix.parser_recovery_replay_ready;
  record.passed_matrix_evidence_count =
      Objc3SemaEvidenceCount(matrix.top_level_declaration_count_matches) +
      Objc3SemaEvidenceCount(matrix.global_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.implementation_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.function_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_property_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_class_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.protocol_instance_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_property_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_class_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.interface_instance_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_property_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.implementation_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_class_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_instance_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_category_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_category_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.function_prototype_count_matches) +
      Objc3SemaEvidenceCount(matrix.function_pure_count_matches) +
      Objc3SemaEvidenceCount(matrix.ast_shape_fingerprint_matches) +
      Objc3SemaEvidenceCount(
          matrix.ast_top_level_layout_fingerprint_matches) +
      Objc3SemaEvidenceCount(
          matrix.parser_contract_snapshot_fingerprint_matches) +
      Objc3SemaEvidenceCount(matrix.parser_diagnostic_budget_consistent) +
      Objc3SemaEvidenceCount(matrix.parser_token_top_level_budget_consistent) +
      Objc3SemaEvidenceCount(matrix.parser_subset_count_consistent) +
      Objc3SemaEvidenceCount(matrix.parser_contract_snapshot_deterministic) +
      Objc3SemaEvidenceCount(matrix.parser_recovery_replay_ready);
  record.required_corpus_case_count = corpus.required_case_count;
  record.passed_corpus_case_count = corpus.passed_case_count;
  record.failed_corpus_case_count = corpus.failed_case_count;
  record.corpus_inventory_ready =
      corpus.required_case_count == 5u &&
      corpus.has_top_level_declaration_count_case &&
      corpus.has_snapshot_fingerprint_case &&
      corpus.has_diagnostic_budget_case && corpus.has_subset_count_case &&
      corpus.has_recovery_replay_case;
  record.corpus_cases_passed =
      corpus.passed_case_count == corpus.required_case_count &&
      corpus.failed_case_count == 0u &&
      corpus.top_level_declaration_count_case_passed &&
      corpus.snapshot_fingerprint_case_passed &&
      corpus.diagnostic_budget_case_passed &&
      corpus.subset_count_case_passed && corpus.recovery_replay_case_passed;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_conformance_evidence_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.required_matrix_evidence_count == 30u &&
      record.passed_matrix_evidence_count ==
          record.required_matrix_evidence_count &&
      record.required_corpus_case_count == 5u &&
      record.passed_corpus_case_count == record.required_corpus_case_count &&
      record.failed_corpus_case_count == 0u &&
      record.conformance_matrix_deterministic &&
      record.conformance_corpus_deterministic &&
      record.declaration_count_evidence_ready &&
      record.member_count_evidence_ready &&
      record.category_function_evidence_ready &&
      record.fingerprint_evidence_ready &&
      record.parser_budget_replay_evidence_ready &&
      record.corpus_inventory_ready && record.corpus_cases_passed;
  return record;
}

Objc3ParserSemaContractReadinessRecord
BuildObjc3ParserSemaContractReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3ParserSemaContractReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.conformance_evidence_ready =
      surface.deterministic_parser_sema_conformance_evidence_record &&
      IsReadyObjc3ParserSemaConformanceEvidenceRecord(
          surface.parser_sema_conformance_evidence_record);
  record.conformance_matrix_ready =
      record.conformance_evidence_ready &&
      surface.parser_sema_conformance_evidence_record
          .conformance_matrix_deterministic;
  record.conformance_corpus_ready =
      record.conformance_evidence_ready &&
      surface.parser_sema_conformance_evidence_record
          .conformance_corpus_deterministic;
  record.performance_quality_guardrails_ready =
      surface.deterministic_parser_sema_performance_quality_guardrails &&
      surface.parser_sema_performance_quality_guardrails.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          7u,
          surface.parser_sema_performance_quality_guardrails
              .required_guardrail_count,
          surface.parser_sema_performance_quality_guardrails
              .passed_guardrail_count,
          surface.parser_sema_performance_quality_guardrails
              .failed_guardrail_count) &&
      surface.parser_sema_performance_quality_guardrails
          .conformance_matrix_builder_budget_guarded &&
      surface.parser_sema_performance_quality_guardrails
          .conformance_corpus_builder_budget_guarded &&
      surface.parser_sema_performance_quality_guardrails
          .handoff_scaffold_builder_budget_guarded &&
      surface.parser_sema_performance_quality_guardrails
          .matrix_diagnostic_budget_consistent &&
      surface.parser_sema_performance_quality_guardrails
          .matrix_token_top_level_budget_consistent &&
      surface.parser_sema_performance_quality_guardrails
          .matrix_subset_budget_consistent &&
      surface.parser_sema_performance_quality_guardrails
          .corpus_case_budget_consistent;
  record.cross_lane_integration_sync_ready =
      surface.deterministic_parser_sema_cross_lane_integration_sync &&
      surface.parser_sema_cross_lane_integration_sync.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          4u,
          surface.parser_sema_cross_lane_integration_sync.required_sync_count,
          surface.parser_sema_cross_lane_integration_sync.passed_sync_count,
          surface.parser_sema_cross_lane_integration_sync.failed_sync_count) &&
      surface.parser_sema_cross_lane_integration_sync.matrix_consistent &&
      surface.parser_sema_cross_lane_integration_sync.corpus_consistent &&
      surface.parser_sema_cross_lane_integration_sync
          .performance_quality_guardrails_consistent &&
      surface.parser_sema_cross_lane_integration_sync
          .pass_manager_contract_surface_sync;
  record.docs_runbook_sync_ready =
      surface.deterministic_parser_sema_docs_runbook_sync &&
      surface.parser_sema_docs_runbook_sync.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_docs_runbook_sync.required_sync_count,
          surface.parser_sema_docs_runbook_sync.passed_sync_count,
          surface.parser_sema_docs_runbook_sync.failed_sync_count) &&
      surface.parser_sema_docs_runbook_sync.cross_lane_integration_sync_ready &&
      surface.parser_sema_docs_runbook_sync.pass_manager_contract_surface_sync &&
      surface.parser_sema_docs_runbook_sync.parity_surface_sync;
  record.release_candidate_replay_dry_run_ready =
      surface.deterministic_parser_sema_release_candidate_replay_dry_run &&
      surface.parser_sema_release_candidate_replay_dry_run.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_release_candidate_replay_dry_run
              .required_sync_count,
          surface.parser_sema_release_candidate_replay_dry_run
              .passed_sync_count,
          surface.parser_sema_release_candidate_replay_dry_run
              .failed_sync_count) &&
      surface.parser_sema_release_candidate_replay_dry_run
          .docs_runbook_sync_ready &&
      surface.parser_sema_release_candidate_replay_dry_run
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_release_candidate_replay_dry_run.replay_surface_sync;
  record.advanced_core_shard1_ready =
      surface.deterministic_parser_sema_advanced_core_shard1 &&
      surface.parser_sema_advanced_core_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_core_shard1.required_sync_count,
          surface.parser_sema_advanced_core_shard1.passed_sync_count,
          surface.parser_sema_advanced_core_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_core_shard1
          .release_candidate_replay_dry_run_ready &&
      surface.parser_sema_advanced_core_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_core_shard1.shard_surface_sync;
  record.advanced_contract_rejection_shard1_ready =
      surface.deterministic_parser_sema_advanced_contract_rejection_shard1 &&
      surface.parser_sema_advanced_contract_rejection_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_contract_rejection_shard1
              .required_sync_count,
          surface.parser_sema_advanced_contract_rejection_shard1
              .passed_sync_count,
          surface.parser_sema_advanced_contract_rejection_shard1
              .failed_sync_count) &&
      surface.parser_sema_advanced_contract_rejection_shard1
          .advanced_core_shard1_ready &&
      surface.parser_sema_advanced_contract_rejection_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_contract_rejection_shard1
          .shard_surface_sync;
  record.advanced_diagnostics_shard1_ready =
      surface.deterministic_parser_sema_advanced_diagnostics_shard1 &&
      surface.parser_sema_advanced_diagnostics_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_diagnostics_shard1.required_sync_count,
          surface.parser_sema_advanced_diagnostics_shard1.passed_sync_count,
          surface.parser_sema_advanced_diagnostics_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_diagnostics_shard1
          .advanced_contract_rejection_shard1_ready &&
      surface.parser_sema_advanced_diagnostics_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_diagnostics_shard1.shard_surface_sync;
  record.advanced_conformance_shard1_ready =
      surface.deterministic_parser_sema_advanced_conformance_shard1 &&
      surface.parser_sema_advanced_conformance_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_conformance_shard1.required_sync_count,
          surface.parser_sema_advanced_conformance_shard1.passed_sync_count,
          surface.parser_sema_advanced_conformance_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_conformance_shard1
          .advanced_diagnostics_shard1_ready &&
      surface.parser_sema_advanced_conformance_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_conformance_shard1.shard_surface_sync;
  record.advanced_integration_shard1_ready =
      surface.deterministic_parser_sema_advanced_integration_shard1 &&
      surface.parser_sema_advanced_integration_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_integration_shard1.required_sync_count,
          surface.parser_sema_advanced_integration_shard1.passed_sync_count,
          surface.parser_sema_advanced_integration_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_integration_shard1
          .advanced_conformance_shard1_ready &&
      surface.parser_sema_advanced_integration_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_integration_shard1.shard_surface_sync;
  record.advanced_performance_shard1_ready =
      surface.deterministic_parser_sema_advanced_performance_shard1 &&
      surface.parser_sema_advanced_performance_shard1.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_performance_shard1.required_sync_count,
          surface.parser_sema_advanced_performance_shard1.passed_sync_count,
          surface.parser_sema_advanced_performance_shard1.failed_sync_count) &&
      surface.parser_sema_advanced_performance_shard1
          .advanced_integration_shard1_ready &&
      surface.parser_sema_advanced_performance_shard1
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_performance_shard1.shard_surface_sync;
  record.advanced_core_shard2_ready =
      surface.deterministic_parser_sema_advanced_core_shard2 &&
      surface.parser_sema_advanced_core_shard2.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_core_shard2.required_sync_count,
          surface.parser_sema_advanced_core_shard2.passed_sync_count,
          surface.parser_sema_advanced_core_shard2.failed_sync_count) &&
      surface.parser_sema_advanced_core_shard2
          .advanced_performance_shard1_ready &&
      surface.parser_sema_advanced_core_shard2
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_core_shard2.shard_surface_sync;
  record.advanced_contract_rejection_shard2_ready =
      surface.deterministic_parser_sema_advanced_contract_rejection_shard2 &&
      surface.parser_sema_advanced_contract_rejection_shard2.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_contract_rejection_shard2
              .required_sync_count,
          surface.parser_sema_advanced_contract_rejection_shard2
              .passed_sync_count,
          surface.parser_sema_advanced_contract_rejection_shard2
              .failed_sync_count) &&
      surface.parser_sema_advanced_contract_rejection_shard2
          .advanced_core_shard2_ready &&
      surface.parser_sema_advanced_contract_rejection_shard2
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_contract_rejection_shard2
          .shard_surface_sync;
  record.advanced_diagnostics_shard2_ready =
      surface.deterministic_parser_sema_advanced_diagnostics_shard2 &&
      surface.parser_sema_advanced_diagnostics_shard2.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_advanced_diagnostics_shard2.required_sync_count,
          surface.parser_sema_advanced_diagnostics_shard2.passed_sync_count,
          surface.parser_sema_advanced_diagnostics_shard2.failed_sync_count) &&
      surface.parser_sema_advanced_diagnostics_shard2
          .advanced_contract_rejection_shard2_ready &&
      surface.parser_sema_advanced_diagnostics_shard2
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_advanced_diagnostics_shard2.shard_surface_sync;
  record.integration_closeout_ready =
      surface.deterministic_parser_sema_integration_closeout_signoff &&
      surface.parser_sema_integration_closeout_signoff.deterministic &&
      Objc3ParserSemaSyncCountsReady(
          3u,
          surface.parser_sema_integration_closeout_signoff.required_sync_count,
          surface.parser_sema_integration_closeout_signoff.passed_sync_count,
          surface.parser_sema_integration_closeout_signoff.failed_sync_count) &&
      surface.parser_sema_integration_closeout_signoff
          .advanced_diagnostics_shard2_ready &&
      surface.parser_sema_integration_closeout_signoff
          .pass_manager_contract_surface_sync &&
      surface.parser_sema_integration_closeout_signoff.gate_signoff_surface_sync;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_contract_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(record.parity_validation_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.conformance_evidence_ready &&
      record.conformance_matrix_ready && record.conformance_corpus_ready &&
      record.performance_quality_guardrails_ready &&
      record.cross_lane_integration_sync_ready &&
      record.docs_runbook_sync_ready &&
      record.release_candidate_replay_dry_run_ready &&
      record.advanced_core_shard1_ready &&
      record.advanced_contract_rejection_shard1_ready &&
      record.advanced_diagnostics_shard1_ready &&
      record.advanced_conformance_shard1_ready &&
      record.advanced_integration_shard1_ready &&
      record.advanced_performance_shard1_ready &&
      record.advanced_core_shard2_ready &&
      record.advanced_contract_rejection_shard2_ready &&
      record.advanced_diagnostics_shard2_ready &&
      record.integration_closeout_ready;
  return record;
}

Objc3SemaParityCloseoutPublicationReadinessRecord
BuildObjc3SemaParityCloseoutPublicationReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    bool pass_manager_executed,
    const Objc3SemaParityContractSurface &surface) {
  const Objc3SemaParityCloseoutReadinessInputs readiness =
      BuildObjc3SemaParityCloseoutReadinessInputs(surface);
  Objc3SemaParityCloseoutPublicationReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.pass_manager_executed = pass_manager_executed;
  record.parser_sema_contract_ready = readiness.parser_sema_contract_ready;
  record.pass_flow_summary_ready = readiness.pass_flow_summary_ready;
  record.publication_records_ready = readiness.publication_records_ready;
  record.diagnostics_publication_ready =
      readiness.diagnostics_publication_ready;
  record.pass_flow_recovery_ready = readiness.pass_flow_recovery_ready;
  record.type_metadata_cardinality_ready =
      readiness.type_metadata_cardinality_ready;
  record.typed_semantic_handoffs_ready =
      readiness.typed_semantic_handoffs_ready;
  record.mapping_summaries_ready = readiness.mapping_summaries_ready;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.parity_closeout_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.pass_manager_publication_owner) &&
      Objc3SemaOwnerIsExplicit(record.type_metadata_publication_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.type_metadata_mapping_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.typed_semantic_handoff_publication_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_contract_readiness_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      record.pass_manager_executed &&
      IsReadyObjc3SemaParityCloseoutReadinessInputs(readiness);
  return record;
}

Objc3SemaCloseoutSurfaceReadinessRecord
BuildObjc3SemaCloseoutSurfaceReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  const Objc3SemaCloseoutSurfaceReadinessInputs readiness =
      BuildObjc3SemaCloseoutSurfaceReadinessInputs(surface);
  Objc3SemaCloseoutSurfaceReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_fallback = input.strict_no_fallback;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.parser_sema_contract_ready = readiness.parser_sema_contract_ready;
  record.parser_sema_conformance_evidence_ready =
      readiness.parser_sema_conformance_evidence_ready;
  record.diagnostics_publication_ready =
      readiness.diagnostics_publication_ready;
  record.pass_flow_recovery_ready = readiness.pass_flow_recovery_ready;
  record.pass_manager_publication_ready =
      readiness.pass_manager_publication_ready;
  record.type_metadata_publication_ready =
      readiness.type_metadata_publication_ready;
  record.type_metadata_mapping_ready = readiness.type_metadata_mapping_ready;
  record.typed_semantic_handoff_ready =
      readiness.typed_semantic_handoff_ready;
  record.parity_closeout_publication_ready =
      readiness.parity_closeout_publication_ready;
  record.parity_validation_ready = readiness.parity_validation_ready;
  record.core_semantic_publication_ready =
      readiness.core_semantic_publication_ready;
  record.module_semantic_publication_ready =
      readiness.module_semantic_publication_ready;
  record.intermodule_flow_publication_ready =
      readiness.intermodule_flow_publication_ready;
  record.concurrency_publication_ready = readiness.concurrency_publication_ready;
  record.unsafe_error_validation_ready =
      readiness.unsafe_error_validation_ready;
  record.control_binding_validation_ready =
      readiness.control_binding_validation_ready;
  record.async_block_message_validation_ready =
      readiness.async_block_message_validation_ready;
  record.dispatch_runtime_arc_validation_ready =
      readiness.dispatch_runtime_arc_validation_ready;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.closeout_surface_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_contract_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_conformance_evidence_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.parity_closeout_publication_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.parity_validation_owner) &&
      record.owner_model == kObjc3SemaNoFallbackOwnerModel &&
      record.strict_no_fallback && record.strict_no_compatibility &&
      IsReadyObjc3SemaCloseoutSurfaceReadinessInputs(readiness);
  return record;
}

Objc3SemaCloseoutSignoffRecord BuildObjc3SemaCloseoutSignoffRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  const Objc3SemaParityCloseoutPublicationReadinessRecord
      &closeout_readiness =
          surface.parity_closeout_publication_readiness_record;
  return BuildObjc3SemaCloseoutSignoffRecord(
      input,
      surface.ready && surface.deterministic_parity_validation_record &&
          IsReadyObjc3SemaParityValidationRecord(
              surface.parity_validation_record),
      closeout_readiness.parser_sema_contract_ready,
      surface.deterministic_pass_manager_publication_record &&
          IsReadyObjc3SemaPassManagerPublicationRecord(
              surface.pass_manager_publication_record),
      surface.deterministic_type_metadata_publication_record &&
          IsReadyObjc3SemaTypeMetadataPublicationRecord(
              surface.type_metadata_publication_record),
      closeout_readiness.diagnostics_publication_ready,
      closeout_readiness.pass_flow_recovery_ready,
      closeout_readiness.mapping_summaries_ready,
      closeout_readiness.typed_semantic_handoffs_ready);
}

bool IsReadyObjc3SemaParityContractSurface(
    const Objc3SemaParityContractSurface &surface) {
  return IsReadyObjc3SemaParityContractSurfaceReadinessGates(
      BuildObjc3SemaParityContractSurfaceReadinessGates(surface));
}
