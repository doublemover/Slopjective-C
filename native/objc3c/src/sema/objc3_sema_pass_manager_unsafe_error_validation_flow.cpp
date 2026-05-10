#include "sema/objc3_sema_pass_manager_contract_flow.h"

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
  record.strict_no_retired_route = input.strict_no_retired_route;
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
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.required_validation_count == 5u &&
      record.passed_validation_count == record.required_validation_count &&
      record.failed_validation_count == 0u;
  return record;
}
