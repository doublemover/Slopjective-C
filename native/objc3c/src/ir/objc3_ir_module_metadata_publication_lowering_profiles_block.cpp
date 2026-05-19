#include "ir/objc3_ir_module_metadata_publication_lowering_profiles_block.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRModuleMetadataBlockLoweringProfilePublication(
    const Objc3IRFrontendMetadata &frontend_metadata_,
    std::ostringstream &out) {
  out << "; frontend_objc_block_literal_capture_lowering_profile = block_literal_sites="
      << frontend_metadata_.block_literal_capture_lowering_block_literal_sites
      << ", block_parameter_entries="
      << frontend_metadata_.block_literal_capture_lowering_block_parameter_entries
      << ", block_capture_entries="
      << frontend_metadata_.block_literal_capture_lowering_block_capture_entries
      << ", block_body_statement_entries="
      << frontend_metadata_.block_literal_capture_lowering_block_body_statement_entries
      << ", block_empty_capture_sites="
      << frontend_metadata_.block_literal_capture_lowering_block_empty_capture_sites
      << ", block_nondeterministic_capture_sites="
      << frontend_metadata_.block_literal_capture_lowering_block_nondeterministic_capture_sites
      << ", block_non_normalized_sites="
      << frontend_metadata_.block_literal_capture_lowering_block_non_normalized_sites
      << ", contract_violation_sites="
      << frontend_metadata_.block_literal_capture_lowering_contract_violation_sites
      << ", deterministic_block_literal_capture_lowering_handoff="
      << (frontend_metadata_.deterministic_block_literal_capture_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_block_abi_invoke_trampoline_lowering_profile = block_literal_sites="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_block_literal_sites
      << ", invoke_argument_slots_total="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_invoke_argument_slots_total
      << ", capture_word_count_total="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_capture_word_count_total
      << ", parameter_entries_total="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_parameter_entries_total
      << ", capture_entries_total="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_capture_entries_total
      << ", body_statement_entries_total="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_body_statement_entries_total
      << ", descriptor_symbolized_sites="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites
      << ", invoke_trampoline_symbolized_sites="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_invoke_symbolized_sites
      << ", missing_invoke_trampoline_sites="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_missing_invoke_sites
      << ", non_normalized_layout_sites="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_non_normalized_layout_sites
      << ", contract_violation_sites="
      << frontend_metadata_.block_abi_invoke_trampoline_lowering_contract_violation_sites
      << ", deterministic_block_abi_invoke_trampoline_lowering_handoff="
      << (frontend_metadata_.deterministic_block_abi_invoke_trampoline_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_block_storage_escape_lowering_profile = block_literal_sites="
      << frontend_metadata_.block_storage_escape_lowering_block_literal_sites
      << ", mutable_capture_count_total="
      << frontend_metadata_.block_storage_escape_lowering_mutable_capture_count_total
      << ", byref_slot_count_total="
      << frontend_metadata_.block_storage_escape_lowering_byref_slot_count_total
      << ", parameter_entries_total="
      << frontend_metadata_.block_storage_escape_lowering_parameter_entries_total
      << ", capture_entries_total="
      << frontend_metadata_.block_storage_escape_lowering_capture_entries_total
      << ", body_statement_entries_total="
      << frontend_metadata_.block_storage_escape_lowering_body_statement_entries_total
      << ", requires_byref_cells_sites="
      << frontend_metadata_.block_storage_escape_lowering_requires_byref_cells_sites
      << ", escape_analysis_enabled_sites="
      << frontend_metadata_.block_storage_escape_lowering_escape_analysis_enabled_sites
      << ", escape_to_heap_sites="
      << frontend_metadata_.block_storage_escape_lowering_escape_to_heap_sites
      << ", escape_profile_normalized_sites="
      << frontend_metadata_.block_storage_escape_lowering_escape_profile_normalized_sites
      << ", byref_layout_symbolized_sites="
      << frontend_metadata_.block_storage_escape_lowering_byref_layout_symbolized_sites
      << ", contract_violation_sites="
      << frontend_metadata_.block_storage_escape_lowering_contract_violation_sites
      << ", deterministic_block_storage_escape_lowering_handoff="
      << (frontend_metadata_.deterministic_block_storage_escape_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_block_copy_dispose_lowering_profile = block_literal_sites="
      << frontend_metadata_.block_copy_dispose_lowering_block_literal_sites
      << ", mutable_capture_count_total="
      << frontend_metadata_.block_copy_dispose_lowering_mutable_capture_count_total
      << ", byref_slot_count_total="
      << frontend_metadata_.block_copy_dispose_lowering_byref_slot_count_total
      << ", parameter_entries_total="
      << frontend_metadata_.block_copy_dispose_lowering_parameter_entries_total
      << ", capture_entries_total="
      << frontend_metadata_.block_copy_dispose_lowering_capture_entries_total
      << ", body_statement_entries_total="
      << frontend_metadata_.block_copy_dispose_lowering_body_statement_entries_total
      << ", copy_helper_required_sites="
      << frontend_metadata_.block_copy_dispose_lowering_copy_helper_required_sites
      << ", dispose_helper_required_sites="
      << frontend_metadata_.block_copy_dispose_lowering_dispose_helper_required_sites
      << ", profile_normalized_sites="
      << frontend_metadata_.block_copy_dispose_lowering_profile_normalized_sites
      << ", copy_helper_symbolized_sites="
      << frontend_metadata_.block_copy_dispose_lowering_copy_helper_symbolized_sites
      << ", dispose_helper_symbolized_sites="
      << frontend_metadata_.block_copy_dispose_lowering_dispose_helper_symbolized_sites
      << ", contract_violation_sites="
      << frontend_metadata_.block_copy_dispose_lowering_contract_violation_sites
      << ", deterministic_block_copy_dispose_lowering_handoff="
      << (frontend_metadata_.deterministic_block_copy_dispose_lowering_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_block_determinism_perf_baseline_lowering_profile = block_literal_sites="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_block_literal_sites
      << ", baseline_weight_total="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_baseline_weight_total
      << ", parameter_entries_total="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_parameter_entries_total
      << ", capture_entries_total="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_capture_entries_total
      << ", body_statement_entries_total="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_body_statement_entries_total
      << ", deterministic_capture_sites="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_deterministic_capture_sites
      << ", heavy_tier_sites="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_heavy_tier_sites
      << ", normalized_profile_sites="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_normalized_profile_sites
      << ", contract_violation_sites="
      << frontend_metadata_.block_determinism_perf_baseline_lowering_contract_violation_sites
      << ", deterministic_block_determinism_perf_baseline_lowering_handoff="
      << (frontend_metadata_.deterministic_block_determinism_perf_baseline_lowering_handoff ? "true" : "false")
      << "\n";
}
