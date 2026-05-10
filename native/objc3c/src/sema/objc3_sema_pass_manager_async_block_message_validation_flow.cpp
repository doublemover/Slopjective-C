#include "sema/objc3_sema_pass_manager_contract_flow.h"

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
