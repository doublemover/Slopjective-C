#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRBlockLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!19 = !{i64 "
      << static_cast<unsigned long long>(metadata.block_literal_capture_lowering_block_literal_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.block_literal_capture_lowering_block_parameter_entries)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.block_literal_capture_lowering_block_capture_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_literal_capture_lowering_block_body_statement_entries)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.block_literal_capture_lowering_block_empty_capture_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_literal_capture_lowering_block_nondeterministic_capture_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_literal_capture_lowering_block_non_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.block_literal_capture_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_block_literal_capture_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!20 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_block_literal_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_invoke_argument_slots_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_capture_word_count_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_parameter_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_capture_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_body_statement_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_invoke_symbolized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_missing_invoke_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_non_normalized_layout_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_abi_invoke_trampoline_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_block_abi_invoke_trampoline_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!21 = !{i64 "
      << static_cast<unsigned long long>(metadata.block_storage_escape_lowering_block_literal_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_mutable_capture_count_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_byref_slot_count_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_parameter_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_capture_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_body_statement_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_requires_byref_cells_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_escape_analysis_enabled_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_escape_to_heap_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_escape_profile_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_byref_layout_symbolized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_storage_escape_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_block_storage_escape_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!22 = !{i64 "
      << static_cast<unsigned long long>(metadata.block_copy_dispose_lowering_block_literal_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_mutable_capture_count_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_byref_slot_count_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_parameter_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_capture_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_body_statement_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_copy_helper_required_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_dispose_helper_required_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_profile_normalized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_copy_helper_symbolized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_dispose_helper_symbolized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_copy_dispose_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_block_copy_dispose_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!23 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_block_literal_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_baseline_weight_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_parameter_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_capture_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_body_statement_entries_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_deterministic_capture_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_heavy_tier_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_normalized_profile_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.block_determinism_perf_baseline_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_block_determinism_perf_baseline_lowering_handoff ? 1 : 0)
      << "}\n\n";
}
