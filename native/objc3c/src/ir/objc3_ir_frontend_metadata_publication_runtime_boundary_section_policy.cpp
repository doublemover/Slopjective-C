#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary_section_policy.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRRuntimeMetadataSectionPolicyNode(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!48 = !{!\""
      << EscapeCStringLiteral(metadata.runtime_metadata_section_abi_contract_id)
      << "\", i1 "
      << (metadata.runtime_metadata_section_boundary_frozen ? 1 : 0)
      << ", i1 " << (metadata.runtime_metadata_section_fail_closed ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_section_object_file_inventory_frozen ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_section_symbol_policy_frozen ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_section_visibility_model_frozen ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_section_retention_policy_frozen ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_section_ready_for_scaffold ? 1 : 0)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_logical_image_info_section)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_logical_class_descriptor_section)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_logical_protocol_descriptor_section)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_logical_category_descriptor_section)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_logical_property_descriptor_section)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_logical_ivar_descriptor_section)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_section_descriptor_symbol_prefix)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_section_aggregate_symbol_prefix)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_section_image_info_symbol)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_section_descriptor_linkage)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_section_aggregate_linkage)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_section_visibility)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_section_retention_root)
      << "\"}\n";
}
