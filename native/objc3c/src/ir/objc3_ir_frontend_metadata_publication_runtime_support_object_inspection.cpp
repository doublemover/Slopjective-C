#include "ir/objc3_ir_frontend_metadata_publication_runtime_support_object_inspection.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRRuntimeObjectInspectionMetadataNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!50 = !{!\""
      << EscapeCStringLiteral(metadata.runtime_metadata_object_inspection_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_object_inspection_publication_contract_id)
      << "\", i1 "
      << (metadata.runtime_metadata_object_inspection_matrix_published ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_object_inspection_fail_closed ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_object_inspection_uses_llvm_readobj ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_object_inspection_uses_llvm_objdump ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_object_inspection_matrix_row_count)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_object_inspection_fixture_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_object_inspection_emit_prefix)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_object_inspection_object_relative_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_object_inspection_section_inventory_row_key)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_object_inspection_section_inventory_command)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_object_inspection_symbol_inventory_row_key)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_object_inspection_symbol_inventory_command)
      << "\"}\n";
}
