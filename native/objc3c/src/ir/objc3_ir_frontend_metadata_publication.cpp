#include "ir/objc3_ir_frontend_metadata_publication.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"

std::string BuildObjc3IRFrontendProfileComment(
    const Objc3IRFrontendMetadata &metadata) {
  std::ostringstream out;
  out << "; frontend_profile = language_version="
      << static_cast<unsigned>(metadata.language_version)
      << ", language_profile=" << metadata.language_profile
      << ", arc_mode=" << metadata.arc_mode
      << ", canonical_literal_rejection_total="
      << metadata.canonical_literal_rejection_total();
  return out.str();
}

void EmitObjc3IRFrontendCoreMetadataPublication(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  // executable source-closure freeze anchor: IR currently publishes
  // interface/protocol/category/linking metadata as the canonical
  // source-closure proof surface only. Later realization work must preserve
  // these identities while adding runnable class/category/protocol behavior.
  // executable class/metaclass source-closure anchor: the IR handoff now
  // carries declaration-owned parent identities, method-owner identities, and
  // class/metaclass object identities so later realization work consumes the
  // same fail-closed source model.
  if (metadata.executable_class_metaclass_source_closure_ready) {
    out << "; executable_class_metaclass_source_closure = contract="
        << metadata.executable_class_metaclass_source_closure_contract_id
        << ";parent_identity_model="
        << metadata.executable_class_metaclass_parent_identity_model
        << ";method_owner_identity_model="
        << metadata.executable_class_metaclass_method_owner_identity_model
        << ";class_object_identity_model="
        << metadata.executable_class_metaclass_object_identity_model
        << ";declaration_node_count="
        << metadata.executable_class_metaclass_declaration_node_count
        << ";parent_identity_edge_count="
        << metadata.executable_class_metaclass_parent_identity_edge_count
        << ";method_owner_identity_edge_count="
        << metadata.executable_class_metaclass_method_owner_identity_edge_count
        << ";class_object_identity_edge_count="
        << metadata.executable_class_metaclass_object_identity_edge_count
        << "\n";
  }
  // executable protocol/category source-closure anchor: the IR handoff now
  // carries protocol inheritance, category attachment, and adopted-protocol
  // conformance identities so later object-model semantic issues consume the
  // same fail-closed source model.
  // object-model semantic-rule freeze anchor: IR stays evidence-only for the
  // frozen semantic boundary covering realization legality, inheritance
  // legality, override compatibility, protocol conformance, and deterministic
  // category merge behavior; executable enforcement begins in later lane-B
  // work.
  // protocol-conformance implementation anchor: IR remains an evidence-only
  // consumer of the sema-owned protocol conformance result while publishing the
  // same protocol/category source identities after sema starts enforcing
  // required-vs-optional protocol member coverage with fail-closed diagnostics.
  // category-merge implementation anchor: IR remains downstream of the
  // sema-owned realized-class category merge/conflict decision and must not
  // reinterpret attachment legality or concrete message resolution.
  // inheritance/override legality anchor: IR remains downstream of the
  // sema-owned realized-class inheritance and override legality result and must
  // not reinterpret superclass cycles, missing realization closure, or inherited
  // member compatibility.
  if (metadata.executable_protocol_category_source_closure_ready) {
    out << "; executable_protocol_category_source_closure = contract="
        << metadata.executable_protocol_category_source_closure_contract_id
        << ";protocol_inheritance_model="
        << metadata.executable_protocol_inheritance_identity_model
        << ";category_attachment_model="
        << metadata.executable_category_attachment_identity_model
        << ";protocol_category_conformance_model="
        << metadata.executable_protocol_category_conformance_identity_model
        << ";protocol_node_count="
        << metadata.executable_protocol_category_protocol_node_count
        << ";category_node_count="
        << metadata.executable_protocol_category_category_node_count
        << ";protocol_inheritance_edge_count="
        << metadata.executable_protocol_inheritance_identity_edge_count
        << ";category_attachment_edge_count="
        << metadata.executable_category_attachment_identity_edge_count
        << ";protocol_category_conformance_edge_count="
        << metadata.executable_protocol_category_conformance_identity_edge_count
        << "\n";
  }

  out << BuildObjc3IRFrontendNamedMetadataTable();
  out << BuildObjc3IRFrontendMetadataNode(metadata) << "\n";
  out << "!1 = !{i64 "
      << static_cast<unsigned long long>(metadata.declared_interfaces)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.declared_implementations)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.resolved_interface_symbols)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.resolved_implementation_symbols)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.interface_method_symbols)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.implementation_method_symbols)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.linked_implementation_symbols)
      << ", i1 "
      << (metadata.deterministic_interface_implementation_handoff ? 1 : 0)
      << "}\n";
  out << "!2 = !{i64 "
      << static_cast<unsigned long long>(metadata.declared_protocols)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.declared_categories)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.resolved_protocol_symbols)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.resolved_category_symbols)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.protocol_method_symbols)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.category_method_symbols)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.linked_category_symbols)
      << ", i1 "
      << (metadata.deterministic_protocol_category_handoff ? 1 : 0) << "}\n";
  out << "!3 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.selector_method_declaration_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.selector_normalized_method_declarations)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.selector_piece_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.selector_piece_parameter_links)
      << ", i1 "
      << (metadata.deterministic_selector_normalization_handoff ? 1 : 0)
      << "}\n";
  out << "!4 = !{i64 "
      << static_cast<unsigned long long>(metadata.property_declaration_entries)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.property_attribute_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.property_attribute_value_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.property_accessor_modifier_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.property_getter_selector_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.property_setter_selector_entries)
      << ", i1 "
      << (metadata.deterministic_property_attribute_handoff ? 1 : 0)
      << "}\n\n";
}

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

void EmitObjc3IRRuntimeSupportMetadataNodes(
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
  out << "!51 = !{!\""
      << EscapeCStringLiteral(metadata.runtime_support_library_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_metadata_publication_contract_id)
      << "\", i1 "
      << (metadata.runtime_support_library_boundary_frozen ? 1 : 0)
      << ", i1 " << (metadata.runtime_support_library_fail_closed ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_target_name_frozen ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_exported_entrypoints_frozen ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_ownership_boundaries_frozen ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_build_constraints_frozen ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_strict_dispatch_errors_required ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_native_library_present ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_driver_link_wiring_pending ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_ready_for_skeleton ? 1 : 0)
      << ", !\"" << EscapeCStringLiteral(metadata.runtime_support_library_target_name)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_support_library_public_header_path)
      << "\", !\"" << EscapeCStringLiteral(metadata.runtime_support_library_source_root)
      << "\", !\"" << EscapeCStringLiteral(metadata.runtime_support_library_library_kind)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_support_library_archive_basename)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_support_library_register_image_symbol)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_support_library_lookup_selector_symbol)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_support_library_dispatch_i32_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_reset_for_testing_symbol)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_support_library_driver_link_mode)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_compiler_ownership_boundary)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_runtime_ownership_boundary)
      << "\"}\n";
  out << "!52 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_support_library_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata
                 .runtime_support_library_core_feature_metadata_publication_contract_id)
      << "\", i1 "
      << (metadata.runtime_support_library_core_feature_fail_closed ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_core_feature_sources_present ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_core_feature_header_present ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_core_feature_archive_build_enabled ? 1
                                                                              : 0)
      << ", i1 "
      << (metadata.runtime_support_library_core_feature_entrypoints_implemented ? 1
                                                                                : 0)
      << ", i1 "
      << (metadata.runtime_support_library_core_feature_selector_lookup_stateful
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .runtime_support_library_core_feature_dispatch_formula_matches_runtime_test_helper
              ? 1
              : 0)
      << ", i1 "
      << (metadata.runtime_support_library_core_feature_reset_for_testing_supported
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .runtime_support_library_core_feature_strict_dispatch_errors_required
              ? 1
              : 0)
      << ", i1 "
      << (metadata.runtime_support_library_core_feature_driver_link_wiring_pending
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .runtime_support_library_core_feature_ready_for_driver_link_wiring
              ? 1
              : 0)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_target_name)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_public_header_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_source_root)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata
                 .runtime_support_library_core_feature_implementation_source_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_library_kind)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_archive_basename)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_archive_relative_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_probe_source_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_register_image_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_lookup_selector_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_dispatch_i32_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_reset_for_testing_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_core_feature_driver_link_mode)
      << "\"}\n";
  out << "!53 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_link_wiring_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_link_wiring_core_feature_contract_id)
      << "\", i1 "
      << (metadata.runtime_support_library_link_wiring_fail_closed ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_support_library_link_wiring_archive_available ? 1 : 0)
      << ", i1 "
      << (metadata
                  .runtime_support_library_link_wiring_driver_emits_runtime_link_contract
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .runtime_support_library_link_wiring_execution_smoke_consumes_runtime_library
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .runtime_support_library_link_wiring_strict_dispatch_errors_required
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .runtime_support_library_link_wiring_ready_for_runtime_library_consumption
              ? 1
              : 0)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_link_wiring_archive_relative_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_link_wiring_runtime_dispatch_symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_link_wiring_execution_smoke_script_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_support_library_link_wiring_driver_link_mode)
      << "\"}\n";
  out << "!54 = !{!\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_typed_handoff_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_source_graph_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_named_metadata_name)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_manifest_surface_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata
                 .executable_metadata_debug_projection_typed_handoff_surface_path)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_source_graph_surface_path)
      << "\", i1 "
      << (metadata.executable_metadata_debug_projection_matrix_published ? 1 : 0)
      << ", i1 "
      << (metadata.executable_metadata_debug_projection_fail_closed ? 1 : 0)
      << ", i1 "
      << (metadata
                  .executable_metadata_debug_projection_manifest_debug_surface_published
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .executable_metadata_debug_projection_ir_named_metadata_published
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .executable_metadata_debug_projection_replay_anchor_deterministic
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .executable_metadata_debug_projection_active_typed_handoff_ready
              ? 1
              : 0)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.executable_metadata_debug_projection_matrix_row_count)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_replay_key)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata
                 .executable_metadata_debug_projection_active_typed_handoff_replay_key)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_row0_descriptor)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_row1_descriptor)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.executable_metadata_debug_projection_row2_descriptor)
      << "\"}\n";
}

std::string BuildObjc3IRFrontendNamedMetadataTable() {
  return
      "!objc3.frontend = !{!0}\n"
      "!objc3.objc_interface_implementation = !{!1}\n"
      "!objc3.objc_protocol_category = !{!2}\n"
      "!objc3.objc_class_protocol_category_linking = !{!7}\n"
      "!objc3.objc_selector_normalization = !{!3}\n"
      "!objc3.objc_property_attribute = !{!4}\n"
      "!objc3.objc_runtime_metadata_source_ownership = !{!45}\n"
      "!objc3.objc_runtime_export_legality = !{!46}\n"
      "!objc3.objc_runtime_export_enforcement = !{!47}\n"
      "!objc3.objc_runtime_metadata_section_abi = !{!48}\n"
      "!objc3.objc_runtime_metadata_section_publication = !{!49}\n"
      "!objc3.objc_runtime_metadata_layout_policy = !{!55}\n"
      "!objc3.objc_runtime_class_metaclass_emission = !{!56}\n"
      "!objc3.objc_runtime_protocol_category_emission = !{!57}\n"
      "!objc3.objc_runtime_member_table_emission = !{!58}\n"
      "!objc3.objc_runtime_selector_string_pool_emission = !{!59}\n"
      "!objc3.objc_runtime_binary_inspection_harness = !{!60}\n"
      "!objc3.objc_runtime_object_packaging_retention = !{!61}\n"
      "!objc3.objc_runtime_linker_retention = !{!62}\n"
      "!objc3.objc_runtime_archive_static_link_discovery = !{!63}\n"
      "!objc3.objc_runtime_metadata_emission_gate = !{!64}\n"
      "!objc3.objc_runtime_metadata_object_emission_closeout = !{!65}\n"
      "!objc3.objc_runtime_metadata_object_inspection = !{!50}\n"
      "!objc3.objc_runtime_support_library = !{!51}\n"
      "!objc3.objc_runtime_support_library_core_feature = !{!52}\n"
      "!objc3.objc_runtime_support_library_link_wiring = !{!53}\n"
      "!objc3.objc_executable_metadata_debug_projection = !{!54}\n"
      "!objc3.objc_object_pointer_nullability_generics = !{!5}\n"
      "!objc3.objc_symbol_graph_scope_resolution = !{!6}\n"
      "!objc3.objc_id_class_sel_object_pointer_typecheck = !{!8}\n"
      "!objc3.objc_dispatch_surface_classification = !{!66}\n"
      "!objc3.objc_executable_ivar_layout_emission = !{!67}\n"
      "!objc3.objc_executable_synthesized_accessor_property_lowering = !{!68}\n"
      "!objc3.objc_runtime_ownership_hook_emission = !{!69}\n"
      "!objc3.objc_runtime_memory_management_api = !{!70}\n"
      "!objc3.objc_runtime_memory_management_implementation = !{!71}\n"
      "!objc3.objc_ownership_runtime_gate = !{!72}\n"
      "!objc3.objc_runnable_block_runtime_gate = !{!73}\n"
      "!objc3.objc_runnable_block_execution_matrix = !{!74}\n"
      "!objc3.objc_arc_source_mode_boundary = !{!75}\n"
      "!objc3.objc_arc_mode_handling = !{!76}\n"
      "!objc3.objc_arc_semantic_rules = !{!77}\n"
      "!objc3.objc_arc_inference_lifetime = !{!78}\n"
      "!objc3.objc_arc_interaction_semantics = !{!79}\n"
      "!objc3.objc_arc_automatic_insertions = !{!80}\n"
      "!objc3.objc_arc_cleanup_weak_lifetime_hooks = !{!81}\n"
      "!objc3.objc_arc_block_autorelease_return_lowering = !{!82}\n"
      "!objc3.objc_runtime_arc_helper_api_surface = !{!83}\n"
      "!objc3.objc_runtime_arc_helper_runtime_support = !{!84}\n"
      "!objc3.objc_runtime_arc_debug_instrumentation = !{!85}\n"
      "!objc3.objc_runnable_arc_runtime_gate = !{!86}\n"
      "!objc3.objc_message_send_selector_lowering = !{!9}\n"
      "!objc3.objc_dispatch_abi_marshalling = !{!10}\n"
      "!objc3.objc_nil_receiver_semantics_foldability = !{!11}\n"
      "!objc3.objc_super_dispatch_method_family = !{!12}\n"
      "!objc3.objc_runtime_link_host_link = !{!13}\n"
      "!objc3.objc_ownership_qualifier_lowering = !{!14}\n"
      "!objc3.objc_retain_release_operation_lowering = !{!15}\n"
      "!objc3.objc_autoreleasepool_scope_lowering = !{!16}\n"
      "!objc3.objc_weak_unowned_semantics_lowering = !{!17}\n"
      "!objc3.objc_arc_diagnostics_fixit_lowering = !{!18}\n"
      "!objc3.objc_block_literal_capture_lowering = !{!19}\n"
      "!objc3.objc_block_abi_invoke_trampoline_lowering = !{!20}\n"
      "!objc3.objc_block_storage_escape_lowering = !{!21}\n"
      "!objc3.objc_block_copy_dispose_lowering = !{!22}\n"
      "!objc3.objc_block_determinism_perf_baseline_lowering = !{!23}\n"
      "!objc3.objc_lightweight_generic_constraint_lowering = !{!24}\n"
      "!objc3.objc_nullability_flow_warning_precision_lowering = !{!25}\n"
      "!objc3.objc_protocol_qualified_object_type_lowering = !{!26}\n"
      "!objc3.objc_variance_bridge_cast_lowering = !{!27}\n"
      "!objc3.objc_generic_metadata_abi_lowering = !{!28}\n"
      "!objc3.objc_module_import_graph_lowering = !{!29}\n"
      "!objc3.objc_namespace_collision_shadowing_lowering = !{!30}\n"
      "!objc3.objc_public_private_api_partition_lowering = !{!31}\n"
      "!objc3.objc_incremental_module_cache_invalidation_lowering = !{!32}\n"
      "!objc3.objc_cross_module_conformance_lowering = !{!33}\n"
      "!objc3.objc_error_handling_throws_abi_propagation_lowering = !{!87}\n"
      "!objc3.objc_error_handling_result_and_bridging_artifact_replay = !{!88}\n"
      "!objc3.objc_error_handling_error_runtime_bridge_helper = !{!89}\n"
      "!objc3.objc_error_handling_live_error_runtime_integration = !{!90}\n"
      "!objc3.objc_concurrency_continuation_runtime_helper = !{!91}\n"
      "!objc3.objc_concurrency_live_continuation_runtime_integration = !{!92}\n"
      "!objc3.objc_concurrency_task_runtime_abi_completion = !{!93}\n"
      "!objc3.objc_concurrency_scheduler_executor_runtime_contract = !{!94}\n"
      "!objc3.objc_concurrency_live_task_runtime_integration = !{!95}\n"
      "!objc3.objc_concurrency_task_runtime_hardening = !{!96}\n"
      "!objc3.objc_concurrency_actor_lowering_and_metadata = !{!97}\n"
      "!objc3.objc_dispatch_dispatch_control_lowering_contract = !{!102}\n"
      "!objc3.objc_interop_interop_lowering_and_abi_contract = !{!108}\n"
      "!objc3.objc_interop_foreign_call_and_lifetime_lowering = !{!109}\n"
      "!objc3.objc_interop_ffi_metadata_and_interface_preservation = !{!110}\n"
      "!objc3.objc_interop_bridge_packaging_and_toolchain_contract = !{!111}\n"
      "!objc3.objc_interop_header_module_and_bridge_generation = !{!112}\n"
      "!objc3.objc_metaprogramming_expansion_and_lowering_contract = !{!104}\n"
      "!objc3.objc_metaprogramming_synthesized_ast_and_ir_emission = !{!105}\n"
      "!objc3.objc_metaprogramming_module_interface_and_replay_preservation = !{!106}\n"
      "!objc3.objc_metaprogramming_expansion_host_and_runtime_boundary = !{!107}\n"
      "!objc3.objc_dispatch_dispatch_metadata_and_interface_preservation = !{!103}\n"
      "!objc3.objc_ownership_system_extension_lowering_contract = !{!98}\n"
      "!objc3.objc_ownership_borrowed_pointer_and_retainable_family_abi_completion = !{!99}\n"
      "!objc3.objc_ownership_system_helper_runtime_contract = !{!100}\n"
      "!objc3.objc_ownership_live_cleanup_retainable_runtime_integration = !{!101}\n"
      "!objc3.objc_throws_propagation_lowering = !{!34}\n"
      "!objc3.objc_unwind_cleanup_lowering = !{!35}\n"
      "!objc3.objc_ns_error_bridging_lowering = !{!36}\n"
      "!objc3.objc_unsafe_pointer_extension_lowering = !{!37}\n"
      "!objc3.objc_inline_asm_intrinsic_governance_lowering = !{!38}\n"
      "!objc3.objc_concurrency_replay_race_guard_lowering = !{!39}\n"
      "!objc3.objc_task_runtime_interop_cancellation_lowering = !{!40}\n"
      "!objc3.objc_actor_isolation_sendability_lowering = !{!41}\n"
      "!objc3.objc_await_lowering_suspension_state_lowering = !{!42}\n"
      "!objc3.objc_async_continuation_lowering = !{!43}\n"
      "!objc3.objc_error_diagnostics_recovery_lowering = !{!44}\n";
}

std::string BuildObjc3IRFrontendMetadataNode(
    const Objc3IRFrontendMetadata &metadata) {
  std::ostringstream out;
  out << "!0 = !{i32 " << static_cast<unsigned>(metadata.language_version)
      << ", !\"" << EscapeCStringLiteral(metadata.language_profile)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata.canonical_literal_yes_rejection_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.canonical_literal_no_rejection_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.canonical_literal_null_rejection_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.canonical_literal_rejection_total())
      << "}";
  return out.str();
}
