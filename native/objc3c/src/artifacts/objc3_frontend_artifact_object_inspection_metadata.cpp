#include "artifacts/objc3_frontend_artifact_object_inspection_metadata.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendObjectInspectionMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const Objc3RuntimeMetadataObjectInspectionHarnessSummary
        &runtime_metadata_object_inspection) {
  ir_frontend_metadata.runtime_metadata_object_inspection_contract_id =
      runtime_metadata_object_inspection.contract_id;
  ir_frontend_metadata.runtime_metadata_object_inspection_publication_contract_id =
      runtime_metadata_object_inspection.publication_contract_id;
  ir_frontend_metadata.runtime_metadata_object_inspection_matrix_published =
      runtime_metadata_object_inspection.matrix_published;
  ir_frontend_metadata.runtime_metadata_object_inspection_fail_closed =
      runtime_metadata_object_inspection.fail_closed;
  ir_frontend_metadata.runtime_metadata_object_inspection_uses_llvm_readobj =
      runtime_metadata_object_inspection.uses_llvm_readobj;
  ir_frontend_metadata.runtime_metadata_object_inspection_uses_llvm_objdump =
      runtime_metadata_object_inspection.uses_llvm_objdump;
  ir_frontend_metadata.runtime_metadata_object_inspection_matrix_row_count =
      runtime_metadata_object_inspection.matrix_row_count;
  ir_frontend_metadata.runtime_metadata_object_inspection_fixture_path =
      runtime_metadata_object_inspection.fixture_path;
  ir_frontend_metadata.runtime_metadata_object_inspection_emit_prefix =
      runtime_metadata_object_inspection.emit_prefix;
  ir_frontend_metadata.runtime_metadata_object_inspection_object_relative_path =
      runtime_metadata_object_inspection.object_relative_path;
  ir_frontend_metadata.runtime_metadata_object_inspection_section_inventory_row_key =
      runtime_metadata_object_inspection.section_inventory_row_key;
  ir_frontend_metadata.runtime_metadata_object_inspection_section_inventory_command =
      runtime_metadata_object_inspection.section_inventory_command;
  ir_frontend_metadata.runtime_metadata_object_inspection_symbol_inventory_row_key =
      runtime_metadata_object_inspection.symbol_inventory_row_key;
  ir_frontend_metadata.runtime_metadata_object_inspection_symbol_inventory_command =
      runtime_metadata_object_inspection.symbol_inventory_command;
}

}  // namespace objc3::artifacts::frontend
