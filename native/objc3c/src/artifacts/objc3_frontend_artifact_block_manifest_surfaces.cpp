#include "artifacts/objc3_frontend_artifact_block_manifest_surfaces.h"

#include <ostream>

#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"

namespace objc3::artifacts::frontend {

void WriteBlockManifestSurfaces(
    std::ostream &manifest,
    const Objc3BlockLiteralCaptureLoweringContract
        &block_literal_capture_lowering_contract,
    const std::string &block_literal_capture_lowering_replay_key,
    const Objc3BlockSourceModelCompletionContract
        &block_source_model_completion_contract,
    const std::string &block_source_model_completion_replay_key,
    const Objc3BlockSourceStorageAnnotationContract
        &block_source_storage_annotation_contract,
    const std::string &block_source_storage_annotation_replay_key,
    const Objc3BlockAbiInvokeTrampolineLoweringContract
        &block_abi_invoke_trampoline_lowering_contract,
    const std::string &block_abi_invoke_trampoline_lowering_replay_key,
    const Objc3BlockStorageEscapeLoweringContract
        &block_storage_escape_lowering_contract,
    const std::string &block_storage_escape_lowering_replay_key,
    const Objc3BlockCopyDisposeLoweringContract
        &block_copy_dispose_lowering_contract,
    const std::string &block_copy_dispose_lowering_replay_key,
    const Objc3BlockDeterminismPerfBaselineLoweringContract
        &block_determinism_perf_baseline_lowering_contract,
    const std::string &block_determinism_perf_baseline_lowering_replay_key) {
  manifest
      << ",\"objc_block_literal_capture_lowering_surface\":{\"block_literal_sites\":"
      << block_literal_capture_lowering_contract.block_literal_sites
      << ",\"block_parameter_entries\":"
      << block_literal_capture_lowering_contract.block_parameter_entries
      << ",\"block_capture_entries\":"
      << block_literal_capture_lowering_contract.block_capture_entries
      << ",\"block_body_statement_entries\":"
      << block_literal_capture_lowering_contract.block_body_statement_entries
      << ",\"block_empty_capture_sites\":"
      << block_literal_capture_lowering_contract.block_empty_capture_sites
      << ",\"block_nondeterministic_capture_sites\":"
      << block_literal_capture_lowering_contract
             .block_nondeterministic_capture_sites
      << ",\"block_non_normalized_sites\":"
      << block_literal_capture_lowering_contract.block_non_normalized_sites
      << ",\"contract_violation_sites\":"
      << block_literal_capture_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << block_literal_capture_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (block_literal_capture_lowering_contract.deterministic ? "true"
                                                                : "false")
      << "}"
      << ",\"objc_block_source_model_completion_surface\":{\"block_literal_sites\":"
      << block_source_model_completion_contract.block_literal_sites
      << ",\"signature_entries_total\":"
      << block_source_model_completion_contract.signature_entries_total
      << ",\"explicit_typed_parameter_entries_total\":"
      << block_source_model_completion_contract
             .explicit_typed_parameter_entries_total
      << ",\"implicit_parameter_entries_total\":"
      << block_source_model_completion_contract
             .implicit_parameter_entries_total
      << ",\"capture_inventory_entries_total\":"
      << block_source_model_completion_contract.capture_inventory_entries_total
      << ",\"byvalue_readonly_capture_entries_total\":"
      << block_source_model_completion_contract
             .byvalue_readonly_capture_entries_total
      << ",\"invoke_surface_entries_total\":"
      << block_source_model_completion_contract.invoke_surface_entries_total
      << ",\"non_normalized_sites\":"
      << block_source_model_completion_contract.non_normalized_sites
      << ",\"contract_violation_sites\":"
      << block_source_model_completion_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << block_source_model_completion_replay_key
      << "\",\"deterministic_handoff\":"
      << (block_source_model_completion_contract.deterministic ? "true"
                                                               : "false")
      << "}"
      << ",\"objc_block_source_storage_annotation_surface\":{\"block_literal_sites\":"
      << block_source_storage_annotation_contract.block_literal_sites
      << ",\"capture_entries_total\":"
      << block_source_storage_annotation_contract.capture_entries_total
      << ",\"mutated_capture_entries_total\":"
      << block_source_storage_annotation_contract
             .mutated_capture_entries_total
      << ",\"byref_capture_entries_total\":"
      << block_source_storage_annotation_contract.byref_capture_entries_total
      << ",\"copy_helper_intent_sites\":"
      << block_source_storage_annotation_contract.copy_helper_intent_sites
      << ",\"dispose_helper_intent_sites\":"
      << block_source_storage_annotation_contract.dispose_helper_intent_sites
      << ",\"heap_candidate_sites\":"
      << block_source_storage_annotation_contract.heap_candidate_sites
      << ",\"expression_sites\":"
      << block_source_storage_annotation_contract.expression_sites
      << ",\"global_initializer_sites\":"
      << block_source_storage_annotation_contract.global_initializer_sites
      << ",\"binding_initializer_sites\":"
      << block_source_storage_annotation_contract.binding_initializer_sites
      << ",\"assignment_value_sites\":"
      << block_source_storage_annotation_contract.assignment_value_sites
      << ",\"return_value_sites\":"
      << block_source_storage_annotation_contract.return_value_sites
      << ",\"call_argument_sites\":"
      << block_source_storage_annotation_contract.call_argument_sites
      << ",\"message_argument_sites\":"
      << block_source_storage_annotation_contract.message_argument_sites
      << ",\"non_normalized_sites\":"
      << block_source_storage_annotation_contract.non_normalized_sites
      << ",\"contract_violation_sites\":"
      << block_source_storage_annotation_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << block_source_storage_annotation_replay_key
      << "\",\"deterministic_handoff\":"
      << (block_source_storage_annotation_contract.deterministic ? "true"
                                                                 : "false")
      << "}"
      << ",\"objc_block_abi_invoke_trampoline_lowering_surface\":{\"block_literal_sites\":"
      << block_abi_invoke_trampoline_lowering_contract.block_literal_sites
      << ",\"invoke_argument_slots_total\":"
      << block_abi_invoke_trampoline_lowering_contract
             .invoke_argument_slots_total
      << ",\"capture_word_count_total\":"
      << block_abi_invoke_trampoline_lowering_contract.capture_word_count_total
      << ",\"parameter_entries_total\":"
      << block_abi_invoke_trampoline_lowering_contract.parameter_entries_total
      << ",\"capture_entries_total\":"
      << block_abi_invoke_trampoline_lowering_contract.capture_entries_total
      << ",\"body_statement_entries_total\":"
      << block_abi_invoke_trampoline_lowering_contract
             .body_statement_entries_total
      << ",\"descriptor_symbolized_sites\":"
      << block_abi_invoke_trampoline_lowering_contract
             .descriptor_symbolized_sites
      << ",\"invoke_trampoline_symbolized_sites\":"
      << block_abi_invoke_trampoline_lowering_contract
             .invoke_trampoline_symbolized_sites
      << ",\"missing_invoke_trampoline_sites\":"
      << block_abi_invoke_trampoline_lowering_contract
             .missing_invoke_trampoline_sites
      << ",\"non_normalized_layout_sites\":"
      << block_abi_invoke_trampoline_lowering_contract
             .non_normalized_layout_sites
      << ",\"contract_violation_sites\":"
      << block_abi_invoke_trampoline_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << block_abi_invoke_trampoline_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (block_abi_invoke_trampoline_lowering_contract.deterministic
              ? "true"
              : "false")
      << "}"
      << ",\"objc_block_storage_escape_lowering_surface\":{\"block_literal_sites\":"
      << block_storage_escape_lowering_contract.block_literal_sites
      << ",\"mutable_capture_count_total\":"
      << block_storage_escape_lowering_contract.mutable_capture_count_total
      << ",\"byref_slot_count_total\":"
      << block_storage_escape_lowering_contract.byref_slot_count_total
      << ",\"parameter_entries_total\":"
      << block_storage_escape_lowering_contract.parameter_entries_total
      << ",\"capture_entries_total\":"
      << block_storage_escape_lowering_contract.capture_entries_total
      << ",\"body_statement_entries_total\":"
      << block_storage_escape_lowering_contract.body_statement_entries_total
      << ",\"requires_byref_cells_sites\":"
      << block_storage_escape_lowering_contract.requires_byref_cells_sites
      << ",\"escape_analysis_enabled_sites\":"
      << block_storage_escape_lowering_contract.escape_analysis_enabled_sites
      << ",\"escape_to_heap_sites\":"
      << block_storage_escape_lowering_contract.escape_to_heap_sites
      << ",\"escape_profile_normalized_sites\":"
      << block_storage_escape_lowering_contract.escape_profile_normalized_sites
      << ",\"byref_layout_symbolized_sites\":"
      << block_storage_escape_lowering_contract.byref_layout_symbolized_sites
      << ",\"contract_violation_sites\":"
      << block_storage_escape_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << block_storage_escape_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (block_storage_escape_lowering_contract.deterministic ? "true"
                                                               : "false")
      << "}"
      << ",\"objc_block_copy_dispose_lowering_surface\":{\"block_literal_sites\":"
      << block_copy_dispose_lowering_contract.block_literal_sites
      << ",\"mutable_capture_count_total\":"
      << block_copy_dispose_lowering_contract.mutable_capture_count_total
      << ",\"byref_slot_count_total\":"
      << block_copy_dispose_lowering_contract.byref_slot_count_total
      << ",\"parameter_entries_total\":"
      << block_copy_dispose_lowering_contract.parameter_entries_total
      << ",\"capture_entries_total\":"
      << block_copy_dispose_lowering_contract.capture_entries_total
      << ",\"body_statement_entries_total\":"
      << block_copy_dispose_lowering_contract.body_statement_entries_total
      << ",\"copy_helper_required_sites\":"
      << block_copy_dispose_lowering_contract.copy_helper_required_sites
      << ",\"dispose_helper_required_sites\":"
      << block_copy_dispose_lowering_contract.dispose_helper_required_sites
      << ",\"profile_normalized_sites\":"
      << block_copy_dispose_lowering_contract.profile_normalized_sites
      << ",\"copy_helper_symbolized_sites\":"
      << block_copy_dispose_lowering_contract.copy_helper_symbolized_sites
      << ",\"dispose_helper_symbolized_sites\":"
      << block_copy_dispose_lowering_contract.dispose_helper_symbolized_sites
      << ",\"contract_violation_sites\":"
      << block_copy_dispose_lowering_contract.contract_violation_sites
      << ",\"replay_key\":\""
      << block_copy_dispose_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (block_copy_dispose_lowering_contract.deterministic ? "true"
                                                            : "false")
      << "}"
      << ",\"objc_block_determinism_perf_baseline_lowering_surface\":{\"block_literal_sites\":"
      << block_determinism_perf_baseline_lowering_contract.block_literal_sites
      << ",\"baseline_weight_total\":"
      << block_determinism_perf_baseline_lowering_contract.baseline_weight_total
      << ",\"parameter_entries_total\":"
      << block_determinism_perf_baseline_lowering_contract
             .parameter_entries_total
      << ",\"capture_entries_total\":"
      << block_determinism_perf_baseline_lowering_contract
             .capture_entries_total
      << ",\"body_statement_entries_total\":"
      << block_determinism_perf_baseline_lowering_contract
             .body_statement_entries_total
      << ",\"deterministic_capture_sites\":"
      << block_determinism_perf_baseline_lowering_contract
             .deterministic_capture_sites
      << ",\"heavy_tier_sites\":"
      << block_determinism_perf_baseline_lowering_contract.heavy_tier_sites
      << ",\"normalized_profile_sites\":"
      << block_determinism_perf_baseline_lowering_contract
             .normalized_profile_sites
      << ",\"contract_violation_sites\":"
      << block_determinism_perf_baseline_lowering_contract
             .contract_violation_sites
      << ",\"replay_key\":\""
      << block_determinism_perf_baseline_lowering_replay_key
      << "\",\"deterministic_handoff\":"
      << (block_determinism_perf_baseline_lowering_contract.deterministic
              ? "true"
              : "false")
      << "}";
}

}  // namespace objc3::artifacts::frontend
