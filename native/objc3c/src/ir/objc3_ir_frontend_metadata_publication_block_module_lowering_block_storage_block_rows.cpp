#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block_storage_block_rows.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication_block_module_lowering_block_storage_row_helpers.h"

void EmitObjc3IRBlockCopyDisposeLoweringCounterNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  BeginObjc3IRBlockStorageLoweringCounterRow(
      "!22", metadata.block_copy_dispose_lowering_block_literal_sites, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_copy_dispose_lowering_mutable_capture_count_total, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_copy_dispose_lowering_byref_slot_count_total, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_copy_dispose_lowering_parameter_entries_total, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_copy_dispose_lowering_capture_entries_total, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_copy_dispose_lowering_body_statement_entries_total, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_copy_dispose_lowering_copy_helper_required_sites, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_copy_dispose_lowering_dispose_helper_required_sites, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_copy_dispose_lowering_profile_normalized_sites, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_copy_dispose_lowering_copy_helper_symbolized_sites, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_copy_dispose_lowering_dispose_helper_symbolized_sites, out);
  EmitObjc3IRBlockStorageLoweringSizeField(
      metadata.block_copy_dispose_lowering_contract_violation_sites, out);
  EmitObjc3IRBlockStorageLoweringBoolField(
      metadata.deterministic_block_copy_dispose_lowering_handoff, out);
  EndObjc3IRBlockStorageLoweringCounterRow(out);
}
