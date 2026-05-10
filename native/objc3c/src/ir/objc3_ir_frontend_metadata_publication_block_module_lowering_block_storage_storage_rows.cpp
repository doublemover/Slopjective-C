#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block_storage_storage_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block_storage_row_helpers.h"

void EmitObjc3IRBlockStorageEscapeLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRBlockStorageLoweringCounterRow(
      "!21", metadata.block_storage_escape_lowering_block_literal_sites, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_storage_escape_lowering_mutable_capture_count_total, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_storage_escape_lowering_byref_slot_count_total, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_storage_escape_lowering_parameter_entries_total, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_storage_escape_lowering_capture_entries_total, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_storage_escape_lowering_body_statement_entries_total, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_storage_escape_lowering_requires_byref_cells_sites, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_storage_escape_lowering_escape_analysis_enabled_sites, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_storage_escape_lowering_escape_to_heap_sites, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_storage_escape_lowering_escape_profile_normalized_sites,
      out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_storage_escape_lowering_byref_layout_symbolized_sites, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_storage_escape_lowering_contract_violation_sites, out);
  EmitObjc3IRBlockStorageLoweringBoolField(
      metadata.deterministic_block_storage_escape_lowering_handoff, out);
  EndObjc3IRBlockStorageLoweringCounterRow(out);
}
