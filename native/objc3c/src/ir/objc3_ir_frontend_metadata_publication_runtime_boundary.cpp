#include "ir/objc3_ir_frontend_metadata_publication_runtime_boundary.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

void EmitObjc3IRRuntimeMetadataBoundaryNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!45 = !{!\""
      << EscapeCStringLiteral(metadata.runtime_metadata_source_ownership_contract_id)
      << "\", !\"" << EscapeCStringLiteral(metadata.runtime_metadata_source_schema)
      << "\", !\"" << EscapeCStringLiteral(metadata.runtime_metadata_ivar_source_model)
      << "\", i64 "
      << static_cast<unsigned long long>(metadata.runtime_metadata_class_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_protocol_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_category_interface_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_category_implementation_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_property_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_method_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.runtime_metadata_ivar_record_count)
      << ", i1 "
      << (metadata.frontend_owns_runtime_metadata_source_records ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_source_records_ready_for_lowering ? 1 : 0)
      << ", i1 " << (metadata.native_runtime_library_present ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_source_boundary_fail_closed ? 1 : 0)
      << ", i1 " << (metadata.runtime_link_test_only ? 1 : 0) << ", i1 "
      << (metadata.deterministic_runtime_metadata_source_schema ? 1 : 0)
      << "}\n";
  out << "!46 = !{!\""
      << EscapeCStringLiteral(metadata.runtime_export_legality_contract_id)
      << "\", i1 "
      << (metadata.runtime_export_semantic_boundary_frozen ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_export_metadata_export_enforcement_ready ? 1 : 0)
      << ", i1 " << (metadata.runtime_export_fail_closed ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_export_duplicate_runtime_identity_enforcement_pending
              ? 1
              : 0)
      << ", i1 "
      << (metadata.runtime_export_incomplete_declaration_export_blocking_pending
              ? 1
              : 0)
      << ", i1 "
      << (metadata.runtime_export_illegal_redeclaration_mix_export_blocking_pending
              ? 1
              : 0)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.runtime_export_class_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_export_protocol_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_export_category_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_export_property_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.runtime_export_method_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.runtime_export_ivar_record_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_export_invalid_protocol_composition_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_export_property_attribute_invalid_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_export_property_attribute_contract_violations)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_export_invalid_type_annotation_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_export_property_ivar_binding_missing)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_export_property_ivar_binding_conflicts)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_export_implementation_resolution_misses)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_export_method_resolution_misses)
      << ", i1 " << (metadata.runtime_export_boundary_ready ? 1 : 0)
      << "}\n";
  out << "!47 = !{!\""
      << EscapeCStringLiteral(metadata.runtime_export_enforcement_contract_id)
      << "\", i1 "
      << (metadata.runtime_export_metadata_completeness_enforced ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_export_duplicate_runtime_identity_suppression_enforced
              ? 1
              : 0)
      << ", i1 "
      << (metadata.runtime_export_illegal_redeclaration_mix_blocking_enforced ? 1
                                                                              : 0)
      << ", i1 "
      << (metadata.runtime_export_metadata_shape_drift_blocking_enforced ? 1 : 0)
      << ", i1 " << (metadata.runtime_export_enforcement_fail_closed ? 1 : 0)
      << ", i1 " << (metadata.runtime_export_ready_for_runtime_export ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_export_duplicate_runtime_identity_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_export_incomplete_declaration_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_export_illegal_redeclaration_mix_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_export_metadata_shape_drift_sites)
      << "}\n";
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
  out << "!49 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_publication_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_publication_abi_contract_id)
      << "\", i1 "
      << (metadata.runtime_metadata_section_publication_emitted ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_section_publication_fail_closed ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_section_publication_uses_llvm_used ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_section_publication_image_info_emitted ? 1
                                                                           : 0)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_section_publication_class_descriptor_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_section_publication_protocol_descriptor_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_section_publication_category_descriptor_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_section_publication_property_descriptor_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_section_publication_ivar_descriptor_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_section_publication_total_descriptor_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_section_publication_total_retained_global_count)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_publication_image_info_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_publication_class_aggregate_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_publication_protocol_aggregate_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_publication_category_aggregate_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_publication_property_aggregate_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_section_publication_ivar_aggregate_symbol)
      << "\"}\n";
}
