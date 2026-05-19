#include "artifacts/objc3_frontend_artifact_block_metadata.h"

#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/contracts/block_abi_lowering_contract_records.h"
#include "lower/contracts/block_source_closure_contracts.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendBlockMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &block_literal_capture_lowering_replay_key,
    const Objc3BlockLiteralCaptureLoweringContract
        &block_literal_capture_lowering_contract,
    const std::string &block_source_model_completion_replay_key,
    const Objc3BlockSourceModelCompletionContract
        &block_source_model_completion_contract,
    const std::string &block_source_storage_annotation_replay_key,
    const Objc3BlockSourceStorageAnnotationContract
        &block_source_storage_annotation_contract,
    const std::string &block_abi_invoke_trampoline_lowering_replay_key,
    const Objc3BlockAbiInvokeTrampolineLoweringContract
        &block_abi_invoke_trampoline_lowering_contract,
    const std::string &block_storage_escape_lowering_replay_key,
    const Objc3BlockStorageEscapeLoweringContract
        &block_storage_escape_lowering_contract,
    const std::string &block_copy_dispose_lowering_replay_key,
    const Objc3BlockCopyDisposeLoweringContract
        &block_copy_dispose_lowering_contract,
    const std::string &block_determinism_perf_baseline_lowering_replay_key,
    const Objc3BlockDeterminismPerfBaselineLoweringContract
        &block_determinism_perf_baseline_lowering_contract) {
  ir_frontend_metadata.lowering_block_literal_capture_replay_key =
      block_literal_capture_lowering_replay_key;
  ir_frontend_metadata.block_literal_capture_lowering_block_literal_sites =
      block_literal_capture_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_literal_capture_lowering_block_parameter_entries =
      block_literal_capture_lowering_contract.block_parameter_entries;
  ir_frontend_metadata.block_literal_capture_lowering_block_capture_entries =
      block_literal_capture_lowering_contract.block_capture_entries;
  ir_frontend_metadata
      .block_literal_capture_lowering_block_body_statement_entries =
      block_literal_capture_lowering_contract.block_body_statement_entries;
  ir_frontend_metadata.block_literal_capture_lowering_block_empty_capture_sites =
      block_literal_capture_lowering_contract.block_empty_capture_sites;
  ir_frontend_metadata
      .block_literal_capture_lowering_block_nondeterministic_capture_sites =
      block_literal_capture_lowering_contract.block_nondeterministic_capture_sites;
  ir_frontend_metadata
      .block_literal_capture_lowering_block_non_normalized_sites =
      block_literal_capture_lowering_contract.block_non_normalized_sites;
  ir_frontend_metadata.block_literal_capture_lowering_contract_violation_sites =
      block_literal_capture_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_literal_capture_lowering_handoff =
      block_literal_capture_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_block_source_model_completion_replay_key =
      block_source_model_completion_replay_key;
  ir_frontend_metadata.block_source_model_completion_block_literal_sites =
      block_source_model_completion_contract.block_literal_sites;
  ir_frontend_metadata.block_source_model_completion_signature_entries_total =
      block_source_model_completion_contract.signature_entries_total;
  ir_frontend_metadata
      .block_source_model_completion_explicit_typed_parameter_entries_total =
      block_source_model_completion_contract
          .explicit_typed_parameter_entries_total;
  ir_frontend_metadata
      .block_source_model_completion_implicit_parameter_entries_total =
      block_source_model_completion_contract.implicit_parameter_entries_total;
  ir_frontend_metadata
      .block_source_model_completion_capture_inventory_entries_total =
      block_source_model_completion_contract.capture_inventory_entries_total;
  ir_frontend_metadata
      .block_source_model_completion_byvalue_readonly_capture_entries_total =
      block_source_model_completion_contract
          .byvalue_readonly_capture_entries_total;
  ir_frontend_metadata.block_source_model_completion_invoke_surface_entries_total =
      block_source_model_completion_contract.invoke_surface_entries_total;
  ir_frontend_metadata.block_source_model_completion_non_normalized_sites =
      block_source_model_completion_contract.non_normalized_sites;
  ir_frontend_metadata.block_source_model_completion_contract_violation_sites =
      block_source_model_completion_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_source_model_completion_handoff =
      block_source_model_completion_contract.deterministic;

  ir_frontend_metadata.lowering_block_source_storage_annotation_replay_key =
      block_source_storage_annotation_replay_key;
  ir_frontend_metadata.block_source_storage_annotation_block_literal_sites =
      block_source_storage_annotation_contract.block_literal_sites;
  ir_frontend_metadata.block_source_storage_annotation_capture_entries_total =
      block_source_storage_annotation_contract.capture_entries_total;
  ir_frontend_metadata
      .block_source_storage_annotation_mutated_capture_entries_total =
      block_source_storage_annotation_contract.mutated_capture_entries_total;
  ir_frontend_metadata
      .block_source_storage_annotation_byref_capture_entries_total =
      block_source_storage_annotation_contract.byref_capture_entries_total;
  ir_frontend_metadata.block_source_storage_annotation_copy_helper_intent_sites =
      block_source_storage_annotation_contract.copy_helper_intent_sites;
  ir_frontend_metadata
      .block_source_storage_annotation_dispose_helper_intent_sites =
      block_source_storage_annotation_contract.dispose_helper_intent_sites;
  ir_frontend_metadata.block_source_storage_annotation_heap_candidate_sites =
      block_source_storage_annotation_contract.heap_candidate_sites;
  ir_frontend_metadata.block_source_storage_annotation_expression_sites =
      block_source_storage_annotation_contract.expression_sites;
  ir_frontend_metadata.block_source_storage_annotation_global_initializer_sites =
      block_source_storage_annotation_contract.global_initializer_sites;
  ir_frontend_metadata.block_source_storage_annotation_binding_initializer_sites =
      block_source_storage_annotation_contract.binding_initializer_sites;
  ir_frontend_metadata.block_source_storage_annotation_assignment_value_sites =
      block_source_storage_annotation_contract.assignment_value_sites;
  ir_frontend_metadata.block_source_storage_annotation_return_value_sites =
      block_source_storage_annotation_contract.return_value_sites;
  ir_frontend_metadata.block_source_storage_annotation_call_argument_sites =
      block_source_storage_annotation_contract.call_argument_sites;
  ir_frontend_metadata.block_source_storage_annotation_message_argument_sites =
      block_source_storage_annotation_contract.message_argument_sites;
  ir_frontend_metadata.block_source_storage_annotation_non_normalized_sites =
      block_source_storage_annotation_contract.non_normalized_sites;
  ir_frontend_metadata.block_source_storage_annotation_contract_violation_sites =
      block_source_storage_annotation_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_source_storage_annotation_handoff =
      block_source_storage_annotation_contract.deterministic;

  ir_frontend_metadata.lowering_block_abi_invoke_trampoline_replay_key =
      block_abi_invoke_trampoline_lowering_replay_key;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_block_literal_sites =
      block_abi_invoke_trampoline_lowering_contract.block_literal_sites;
  ir_frontend_metadata
      .block_abi_invoke_trampoline_lowering_invoke_argument_slots_total =
      block_abi_invoke_trampoline_lowering_contract.invoke_argument_slots_total;
  ir_frontend_metadata
      .block_abi_invoke_trampoline_lowering_capture_word_count_total =
      block_abi_invoke_trampoline_lowering_contract.capture_word_count_total;
  ir_frontend_metadata
      .block_abi_invoke_trampoline_lowering_parameter_entries_total =
      block_abi_invoke_trampoline_lowering_contract.parameter_entries_total;
  ir_frontend_metadata
      .block_abi_invoke_trampoline_lowering_capture_entries_total =
      block_abi_invoke_trampoline_lowering_contract.capture_entries_total;
  ir_frontend_metadata
      .block_abi_invoke_trampoline_lowering_body_statement_entries_total =
      block_abi_invoke_trampoline_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata
      .block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites =
      block_abi_invoke_trampoline_lowering_contract.descriptor_symbolized_sites;
  ir_frontend_metadata
      .block_abi_invoke_trampoline_lowering_invoke_symbolized_sites =
      block_abi_invoke_trampoline_lowering_contract
          .invoke_trampoline_symbolized_sites;
  ir_frontend_metadata.block_abi_invoke_trampoline_lowering_missing_invoke_sites =
      block_abi_invoke_trampoline_lowering_contract
          .missing_invoke_trampoline_sites;
  ir_frontend_metadata
      .block_abi_invoke_trampoline_lowering_non_normalized_layout_sites =
      block_abi_invoke_trampoline_lowering_contract.non_normalized_layout_sites;
  ir_frontend_metadata
      .block_abi_invoke_trampoline_lowering_contract_violation_sites =
      block_abi_invoke_trampoline_lowering_contract.contract_violation_sites;
  ir_frontend_metadata
      .deterministic_block_abi_invoke_trampoline_lowering_handoff =
      block_abi_invoke_trampoline_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_block_storage_escape_replay_key =
      block_storage_escape_lowering_replay_key;
  ir_frontend_metadata.block_storage_escape_lowering_block_literal_sites =
      block_storage_escape_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_storage_escape_lowering_mutable_capture_count_total =
      block_storage_escape_lowering_contract.mutable_capture_count_total;
  ir_frontend_metadata.block_storage_escape_lowering_byref_slot_count_total =
      block_storage_escape_lowering_contract.byref_slot_count_total;
  ir_frontend_metadata.block_storage_escape_lowering_parameter_entries_total =
      block_storage_escape_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_storage_escape_lowering_capture_entries_total =
      block_storage_escape_lowering_contract.capture_entries_total;
  ir_frontend_metadata
      .block_storage_escape_lowering_body_statement_entries_total =
      block_storage_escape_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata
      .block_storage_escape_lowering_requires_byref_cells_sites =
      block_storage_escape_lowering_contract.requires_byref_cells_sites;
  ir_frontend_metadata
      .block_storage_escape_lowering_escape_analysis_enabled_sites =
      block_storage_escape_lowering_contract.escape_analysis_enabled_sites;
  ir_frontend_metadata.block_storage_escape_lowering_escape_to_heap_sites =
      block_storage_escape_lowering_contract.escape_to_heap_sites;
  ir_frontend_metadata
      .block_storage_escape_lowering_escape_profile_normalized_sites =
      block_storage_escape_lowering_contract.escape_profile_normalized_sites;
  ir_frontend_metadata
      .block_storage_escape_lowering_byref_layout_symbolized_sites =
      block_storage_escape_lowering_contract.byref_layout_symbolized_sites;
  ir_frontend_metadata.block_storage_escape_lowering_contract_violation_sites =
      block_storage_escape_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_storage_escape_lowering_handoff =
      block_storage_escape_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_block_copy_dispose_replay_key =
      block_copy_dispose_lowering_replay_key;
  ir_frontend_metadata.block_copy_dispose_lowering_block_literal_sites =
      block_copy_dispose_lowering_contract.block_literal_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_mutable_capture_count_total =
      block_copy_dispose_lowering_contract.mutable_capture_count_total;
  ir_frontend_metadata.block_copy_dispose_lowering_byref_slot_count_total =
      block_copy_dispose_lowering_contract.byref_slot_count_total;
  ir_frontend_metadata.block_copy_dispose_lowering_parameter_entries_total =
      block_copy_dispose_lowering_contract.parameter_entries_total;
  ir_frontend_metadata.block_copy_dispose_lowering_capture_entries_total =
      block_copy_dispose_lowering_contract.capture_entries_total;
  ir_frontend_metadata.block_copy_dispose_lowering_body_statement_entries_total =
      block_copy_dispose_lowering_contract.body_statement_entries_total;
  ir_frontend_metadata.block_copy_dispose_lowering_copy_helper_required_sites =
      block_copy_dispose_lowering_contract.copy_helper_required_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_dispose_helper_required_sites =
      block_copy_dispose_lowering_contract.dispose_helper_required_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_profile_normalized_sites =
      block_copy_dispose_lowering_contract.profile_normalized_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_copy_helper_symbolized_sites =
      block_copy_dispose_lowering_contract.copy_helper_symbolized_sites;
  ir_frontend_metadata
      .block_copy_dispose_lowering_dispose_helper_symbolized_sites =
      block_copy_dispose_lowering_contract.dispose_helper_symbolized_sites;
  ir_frontend_metadata.block_copy_dispose_lowering_contract_violation_sites =
      block_copy_dispose_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_block_copy_dispose_lowering_handoff =
      block_copy_dispose_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_block_determinism_perf_baseline_replay_key =
      block_determinism_perf_baseline_lowering_replay_key;
  ir_frontend_metadata
      .block_determinism_perf_baseline_lowering_block_literal_sites =
      block_determinism_perf_baseline_lowering_contract.block_literal_sites;
  ir_frontend_metadata
      .block_determinism_perf_baseline_lowering_baseline_weight_total =
      block_determinism_perf_baseline_lowering_contract.baseline_weight_total;
  ir_frontend_metadata
      .block_determinism_perf_baseline_lowering_parameter_entries_total =
      block_determinism_perf_baseline_lowering_contract.parameter_entries_total;
  ir_frontend_metadata
      .block_determinism_perf_baseline_lowering_capture_entries_total =
      block_determinism_perf_baseline_lowering_contract.capture_entries_total;
  ir_frontend_metadata
      .block_determinism_perf_baseline_lowering_body_statement_entries_total =
      block_determinism_perf_baseline_lowering_contract
          .body_statement_entries_total;
  ir_frontend_metadata
      .block_determinism_perf_baseline_lowering_deterministic_capture_sites =
      block_determinism_perf_baseline_lowering_contract
          .deterministic_capture_sites;
  ir_frontend_metadata.block_determinism_perf_baseline_lowering_heavy_tier_sites =
      block_determinism_perf_baseline_lowering_contract.heavy_tier_sites;
  ir_frontend_metadata
      .block_determinism_perf_baseline_lowering_normalized_profile_sites =
      block_determinism_perf_baseline_lowering_contract.normalized_profile_sites;
  ir_frontend_metadata
      .block_determinism_perf_baseline_lowering_contract_violation_sites =
      block_determinism_perf_baseline_lowering_contract.contract_violation_sites;
  ir_frontend_metadata
      .deterministic_block_determinism_perf_baseline_lowering_handoff =
      block_determinism_perf_baseline_lowering_contract.deterministic;
}

}  // namespace objc3::artifacts::frontend
