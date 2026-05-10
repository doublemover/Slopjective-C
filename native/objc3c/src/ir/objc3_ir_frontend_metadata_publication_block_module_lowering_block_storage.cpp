#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block_storage.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRBlockStorageLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
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
}
