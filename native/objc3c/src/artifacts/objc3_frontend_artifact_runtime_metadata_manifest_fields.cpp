#include "artifacts/objc3_frontend_artifact_runtime_metadata_manifest_fields.h"

#include <ostream>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

void WriteRuntimeMetadataPublicationManifestFields(
    std::ostream &manifest,
    const Objc3RuntimeMetadataSourceOwnershipBoundary
        &runtime_metadata_source_ownership,
    const Objc3RuntimeExportLegalityBoundary &runtime_export_legality,
    const Objc3RuntimeExportEnforcementSummary &runtime_export_enforcement,
    const Objc3RuntimeMetadataSectionAbiFreezeSummary
        &runtime_metadata_section_abi,
    const Objc3RuntimeMetadataSectionPublicationSummary
        &runtime_metadata_section_publication,
    const Objc3RuntimeMetadataObjectInspectionHarnessSummary
        &runtime_metadata_object_inspection) {
  manifest << ",\"runtime_metadata_source_ownership_contract_id\":\""
           << runtime_metadata_source_ownership.contract_id
           << "\",\"runtime_metadata_source_schema\":\""
           << runtime_metadata_source_ownership.canonical_source_schema
           << "\",\"runtime_metadata_ivar_source_model\":\""
           << runtime_metadata_source_ownership.ivar_record_source_model
           << "\",\"frontend_owns_runtime_metadata_source_records\":"
           << (runtime_metadata_source_ownership
                       .frontend_owns_runtime_metadata_source_records
                   ? "true"
                   : "false")
           << ",\"runtime_metadata_source_records_ready_for_lowering\":"
           << (runtime_metadata_source_ownership
                       .runtime_metadata_source_records_ready_for_lowering
                   ? "true"
                   : "false")
           << ",\"native_runtime_library_present\":"
           << (runtime_metadata_source_ownership.native_runtime_library_present
                   ? "true"
                   : "false")
           << ",\"runtime_link_test_only\":"
           << (runtime_metadata_source_ownership.runtime_link_test_only ? "true"
                                                                        : "false")
           << ",\"runtime_metadata_source_boundary_fail_closed\":"
           << (runtime_metadata_source_ownership.fail_closed ? "true"
                                                            : "false")
           << ",\"runtime_metadata_source_boundary_ready\":"
           << (IsReadyObjc3RuntimeMetadataSourceOwnershipBoundary(
                   runtime_metadata_source_ownership)
                   ? "true"
                   : "false")
           << ",\"deterministic_runtime_metadata_source_schema\":"
           << (runtime_metadata_source_ownership.deterministic_source_schema
                   ? "true"
                   : "false")
           << ",\"runtime_metadata_class_record_count\":"
           << runtime_metadata_source_ownership.class_record_count
           << ",\"runtime_metadata_protocol_record_count\":"
           << runtime_metadata_source_ownership.protocol_record_count
           << ",\"runtime_metadata_category_interface_record_count\":"
           << runtime_metadata_source_ownership.category_interface_record_count
           << ",\"runtime_metadata_category_implementation_record_count\":"
           << runtime_metadata_source_ownership
                  .category_implementation_record_count
           << ",\"runtime_metadata_property_record_count\":"
           << runtime_metadata_source_ownership.property_record_count
           << ",\"runtime_metadata_method_record_count\":"
           << runtime_metadata_source_ownership.method_record_count
           << ",\"runtime_metadata_ivar_record_count\":"
           << runtime_metadata_source_ownership.ivar_record_count
           << ",\"runtime_metadata_source_boundary_failure_reason\":\""
           << runtime_metadata_source_ownership.failure_reason
           << "\""
           << ",\"runtime_export_legality_contract_id\":\""
           << runtime_export_legality.contract_id
           << "\",\"runtime_export_semantic_boundary_frozen\":"
           << (runtime_export_legality.semantic_boundary_frozen ? "true"
                                                                : "false")
           << ",\"runtime_export_metadata_export_enforcement_ready\":"
           << (runtime_export_legality.metadata_export_enforcement_ready
                   ? "true"
                   : "false")
           << ",\"runtime_export_fail_closed\":"
           << (runtime_export_legality.fail_closed ? "true" : "false")
           << ",\"runtime_export_boundary_ready\":"
           << (IsReadyObjc3RuntimeExportLegalityBoundary(runtime_export_legality)
                   ? "true"
                   : "false")
           << ",\"runtime_export_duplicate_runtime_identity_enforcement_pending\":"
           << (runtime_export_legality
                       .duplicate_runtime_identity_enforcement_pending
                   ? "true"
                   : "false")
           << ",\"runtime_export_incomplete_declaration_export_blocking_pending\":"
           << (runtime_export_legality
                       .incomplete_declaration_export_blocking_pending
                   ? "true"
                   : "false")
           << ",\"runtime_export_illegal_redeclaration_mix_export_blocking_pending\":"
           << (runtime_export_legality
                       .illegal_redeclaration_mix_export_blocking_pending
                   ? "true"
                   : "false")
           << ",\"runtime_export_class_record_count\":"
           << runtime_export_legality.class_record_count
           << ",\"runtime_export_protocol_record_count\":"
           << runtime_export_legality.protocol_record_count
           << ",\"runtime_export_category_record_count\":"
           << runtime_export_legality.category_record_count
           << ",\"runtime_export_property_record_count\":"
           << runtime_export_legality.property_record_count
           << ",\"runtime_export_method_record_count\":"
           << runtime_export_legality.method_record_count
           << ",\"runtime_export_ivar_record_count\":"
           << runtime_export_legality.ivar_record_count
           << ",\"runtime_export_invalid_protocol_composition_sites\":"
           << runtime_export_legality.invalid_protocol_composition_sites
           << ",\"runtime_export_property_attribute_invalid_entries\":"
           << runtime_export_legality.property_attribute_invalid_entries
           << ",\"runtime_export_property_attribute_contract_violations\":"
           << runtime_export_legality.property_attribute_contract_violations
           << ",\"runtime_export_invalid_type_annotation_sites\":"
           << runtime_export_legality.invalid_type_annotation_sites
           << ",\"runtime_export_property_ivar_binding_missing\":"
           << runtime_export_legality.property_ivar_binding_missing
           << ",\"runtime_export_property_ivar_binding_conflicts\":"
           << runtime_export_legality.property_ivar_binding_conflicts
           << ",\"runtime_export_implementation_resolution_misses\":"
           << runtime_export_legality.implementation_resolution_misses
           << ",\"runtime_export_method_resolution_misses\":"
           << runtime_export_legality.method_resolution_misses
           << ",\"runtime_export_failure_reason\":\""
           << runtime_export_legality.failure_reason
           << "\""
           << ",\"runtime_export_enforcement_contract_id\":\""
           << runtime_export_enforcement.contract_id
           << "\",\"runtime_export_metadata_completeness_enforced\":"
           << (runtime_export_enforcement.metadata_completeness_enforced
                   ? "true"
                   : "false")
           << ",\"runtime_export_duplicate_runtime_identity_suppression_enforced\":"
           << (runtime_export_enforcement
                       .duplicate_runtime_identity_suppression_enforced
                   ? "true"
                   : "false")
           << ",\"runtime_export_illegal_redeclaration_mix_blocking_enforced\":"
           << (runtime_export_enforcement
                       .illegal_redeclaration_mix_blocking_enforced
                   ? "true"
                   : "false")
           << ",\"runtime_export_metadata_shape_drift_blocking_enforced\":"
           << (runtime_export_enforcement
                       .metadata_shape_drift_blocking_enforced
                   ? "true"
                   : "false")
           << ",\"runtime_export_enforcement_fail_closed\":"
           << (runtime_export_enforcement.fail_closed ? "true" : "false")
           << ",\"runtime_export_ready_for_runtime_export\":"
           << (runtime_export_enforcement.ready_for_runtime_export ? "true"
                                                                   : "false")
           << ",\"runtime_export_duplicate_runtime_identity_sites\":"
           << runtime_export_enforcement.duplicate_runtime_identity_sites
           << ",\"runtime_export_incomplete_declaration_sites\":"
           << runtime_export_enforcement.incomplete_declaration_sites
           << ",\"runtime_export_illegal_redeclaration_mix_sites\":"
           << runtime_export_enforcement.illegal_redeclaration_mix_sites
           << ",\"runtime_export_metadata_shape_drift_sites\":"
           << runtime_export_enforcement.metadata_shape_drift_sites
           << ",\"runtime_export_enforcement_failure_reason\":\""
           << runtime_export_enforcement.failure_reason
           << "\""
           << ",\"runtime_metadata_section_abi_contract_id\":\""
           << runtime_metadata_section_abi.contract_id
           << "\",\"runtime_metadata_section_boundary_frozen\":"
           << (runtime_metadata_section_abi.boundary_frozen ? "true" : "false")
           << ",\"runtime_metadata_section_fail_closed\":"
           << (runtime_metadata_section_abi.fail_closed ? "true" : "false")
           << ",\"runtime_metadata_section_object_file_inventory_frozen\":"
           << (runtime_metadata_section_abi.object_file_section_inventory_frozen
                   ? "true"
                   : "false")
           << ",\"runtime_metadata_section_symbol_policy_frozen\":"
           << (runtime_metadata_section_abi.symbol_policy_frozen ? "true"
                                                                 : "false")
           << ",\"runtime_metadata_section_visibility_model_frozen\":"
           << (runtime_metadata_section_abi.visibility_model_frozen ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_section_retention_policy_frozen\":"
           << (runtime_metadata_section_abi.retention_policy_frozen ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_section_ready_for_scaffold\":"
           << (runtime_metadata_section_abi.ready_for_section_scaffold ? "true"
                                                                       : "false")
           << ",\"runtime_metadata_section_logical_image_info_section\":\""
           << runtime_metadata_section_abi.logical_image_info_section
           << "\",\"runtime_metadata_section_logical_class_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_class_descriptor_section
           << "\",\"runtime_metadata_section_logical_protocol_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_protocol_descriptor_section
           << "\",\"runtime_metadata_section_logical_category_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_category_descriptor_section
           << "\",\"runtime_metadata_section_logical_property_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_property_descriptor_section
           << "\",\"runtime_metadata_section_logical_ivar_descriptor_section\":\""
           << runtime_metadata_section_abi.logical_ivar_descriptor_section
           << "\",\"runtime_metadata_section_descriptor_symbol_prefix\":\""
           << runtime_metadata_section_abi.descriptor_symbol_prefix
           << "\",\"runtime_metadata_section_aggregate_symbol_prefix\":\""
           << runtime_metadata_section_abi.aggregate_symbol_prefix
           << "\",\"runtime_metadata_section_image_info_symbol\":\""
           << runtime_metadata_section_abi.image_info_symbol
           << "\",\"runtime_metadata_section_descriptor_linkage\":\""
           << runtime_metadata_section_abi.descriptor_linkage
           << "\",\"runtime_metadata_section_aggregate_linkage\":\""
           << runtime_metadata_section_abi.aggregate_linkage
           << "\",\"runtime_metadata_section_visibility\":\""
           << runtime_metadata_section_abi.metadata_visibility
           << "\",\"runtime_metadata_section_retention_root\":\""
           << runtime_metadata_section_abi.retention_root
           << "\",\"runtime_metadata_section_failure_reason\":\""
           << runtime_metadata_section_abi.failure_reason
           << "\""
           << ",\"runtime_metadata_section_publication_contract_id\":\""
           << runtime_metadata_section_publication.contract_id
           << "\",\"runtime_metadata_section_publication_abi_contract_id\":\""
           << runtime_metadata_section_publication.abi_contract_id
           << "\",\"runtime_metadata_section_publication_emitted\":"
           << (runtime_metadata_section_publication.publication_emitted ? "true"
                                                                        : "false")
           << ",\"runtime_metadata_section_publication_fail_closed\":"
           << (runtime_metadata_section_publication.fail_closed ? "true"
                                                               : "false")
           << ",\"runtime_metadata_section_publication_uses_llvm_used\":"
           << (runtime_metadata_section_publication.uses_llvm_used ? "true"
                                                                   : "false")
           << ",\"runtime_metadata_section_publication_image_info_emitted\":"
           << (runtime_metadata_section_publication.image_info_emitted ? "true"
                                                                       : "false")
           << ",\"runtime_metadata_section_publication_class_descriptor_count\":"
           << runtime_metadata_section_publication.class_descriptor_count
           << ",\"runtime_metadata_section_publication_protocol_descriptor_count\":"
           << runtime_metadata_section_publication.protocol_descriptor_count
           << ",\"runtime_metadata_section_publication_category_descriptor_count\":"
           << runtime_metadata_section_publication.category_descriptor_count
           << ",\"runtime_metadata_section_publication_property_descriptor_count\":"
           << runtime_metadata_section_publication.property_descriptor_count
           << ",\"runtime_metadata_section_publication_ivar_descriptor_count\":"
           << runtime_metadata_section_publication.ivar_descriptor_count
           << ",\"runtime_metadata_section_publication_total_descriptor_count\":"
           << runtime_metadata_section_publication.total_descriptor_count
           << ",\"runtime_metadata_section_publication_total_retained_global_count\":"
           << runtime_metadata_section_publication.total_retained_global_count
           << ",\"runtime_metadata_section_publication_image_info_symbol\":\""
           << runtime_metadata_section_publication.image_info_symbol
           << "\",\"runtime_metadata_section_publication_class_aggregate_symbol\":\""
           << runtime_metadata_section_publication.class_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_protocol_aggregate_symbol\":\""
           << runtime_metadata_section_publication.protocol_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_category_aggregate_symbol\":\""
           << runtime_metadata_section_publication.category_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_property_aggregate_symbol\":\""
           << runtime_metadata_section_publication.property_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_ivar_aggregate_symbol\":\""
           << runtime_metadata_section_publication.ivar_aggregate_symbol
           << "\",\"runtime_metadata_section_publication_failure_reason\":\""
           << runtime_metadata_section_publication.failure_reason
           << "\",\"runtime_metadata_object_inspection_contract_id\":\""
           << runtime_metadata_object_inspection.contract_id
           << "\",\"runtime_metadata_object_inspection_publication_contract_id\":\""
           << runtime_metadata_object_inspection.publication_contract_id
           << "\",\"runtime_metadata_object_inspection_matrix_published\":"
           << (runtime_metadata_object_inspection.matrix_published ? "true"
                                                                   : "false")
           << ",\"runtime_metadata_object_inspection_fail_closed\":"
           << (runtime_metadata_object_inspection.fail_closed ? "true"
                                                              : "false")
           << ",\"runtime_metadata_object_inspection_uses_llvm_readobj\":"
           << (runtime_metadata_object_inspection.uses_llvm_readobj ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_object_inspection_uses_llvm_objdump\":"
           << (runtime_metadata_object_inspection.uses_llvm_objdump ? "true"
                                                                    : "false")
           << ",\"runtime_metadata_object_inspection_matrix_row_count\":"
           << runtime_metadata_object_inspection.matrix_row_count
           << ",\"runtime_metadata_object_inspection_fixture_path\":\""
           << runtime_metadata_object_inspection.fixture_path
           << "\",\"runtime_metadata_object_inspection_emit_prefix\":\""
           << runtime_metadata_object_inspection.emit_prefix
           << "\",\"runtime_metadata_object_inspection_object_relative_path\":\""
           << runtime_metadata_object_inspection.object_relative_path
           << "\",\"runtime_metadata_object_inspection_section_inventory_row_key\":\""
           << runtime_metadata_object_inspection.section_inventory_row_key
           << "\",\"runtime_metadata_object_inspection_section_inventory_command\":\""
           << runtime_metadata_object_inspection.section_inventory_command
           << "\",\"runtime_metadata_object_inspection_symbol_inventory_row_key\":\""
           << runtime_metadata_object_inspection.symbol_inventory_row_key
           << "\",\"runtime_metadata_object_inspection_symbol_inventory_command\":\""
           << runtime_metadata_object_inspection.symbol_inventory_command
           << "\",\"runtime_metadata_object_inspection_failure_reason\":\""
           << runtime_metadata_object_inspection.failure_reason
           << "\"";
}

}  // namespace objc3::artifacts::frontend
