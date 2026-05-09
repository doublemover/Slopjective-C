#include "ir/objc3_ir_frontend_metadata_publication.h"

#include <sstream>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_c_string.h"
#include "lower/objc3_lowering_contract.h"

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

void EmitObjc3IRRuntimeMetadataObjectPublicationNodes(
    const Objc3IRFrontendMetadata &metadata,
    const Objc3RuntimeMetadataLayoutPolicy &runtime_metadata_layout_policy,
    const std::string &runtime_metadata_linker_anchor_symbol,
    const std::string &runtime_metadata_discovery_root_symbol,
    std::size_t selector_pool_global_count,
    std::size_t runtime_string_pool_global_count, std::ostringstream &out) {
  out << "!55 = !{!\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.contract_id)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.abi_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.scaffold_contract_id)
      << "\", i1 " << (runtime_metadata_layout_policy.ready ? 1 : 0)
      << ", i1 " << (runtime_metadata_layout_policy.fail_closed ? 1 : 0)
      << ", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.family_ordering_model)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.descriptor_ordering_model)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.aggregate_relocation_policy)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.comdat_policy)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.visibility_spelling_policy)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.retention_ordering_model)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.object_format_policy_model)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.object_format_surface_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.object_format)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.section_spelling_model)
      << "\", !\""
      << EscapeCStringLiteral(
             runtime_metadata_layout_policy.retention_anchor_model)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.descriptor_linkage)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.aggregate_linkage)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.metadata_visibility)
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.retention_root)
      << "\", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_layout_policy.total_retained_global_count)
      << ", !\""
      << EscapeCStringLiteral(Objc3RuntimeMetadataLayoutPolicyReplayKey(
             runtime_metadata_layout_policy))
      << "\", !\""
      << EscapeCStringLiteral(runtime_metadata_layout_policy.failure_reason)
      << "\"}\n";

  std::size_t runtime_metadata_class_bundle_count = 0;
  std::size_t runtime_metadata_instance_method_reference_total = 0;
  std::size_t runtime_metadata_class_method_reference_total = 0;
  for (const auto &bundle :
       metadata.runtime_metadata_class_metaclass_bundles_lexicographic) {
    ++runtime_metadata_class_bundle_count;
    runtime_metadata_instance_method_reference_total +=
        bundle.instance_method_count;
    runtime_metadata_class_method_reference_total += bundle.class_method_count;
  }
  out << "!56 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_class_metaclass_emission_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_class_metaclass_payload_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_class_metaclass_name_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_class_metaclass_super_link_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_class_metaclass_method_list_reference_model)
      << "\", i1 "
      << (metadata.runtime_metadata_class_metaclass_emission_ready ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_class_metaclass_emission_fail_closed ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_metadata_class_bundle_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_instance_method_reference_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_class_method_reference_total)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_class_metaclass_typed_handoff_replay_key)
      << "\"}\n";

  std::size_t runtime_metadata_protocol_bundle_count = 0;
  std::size_t runtime_metadata_protocol_inherited_reference_total = 0;
  for (const auto &bundle :
       metadata.runtime_metadata_protocol_bundles_lexicographic) {
    ++runtime_metadata_protocol_bundle_count;
    runtime_metadata_protocol_inherited_reference_total +=
        bundle.inherited_protocol_owner_identities_lexicographic.size();
  }
  std::size_t runtime_metadata_category_bundle_count = 0;
  std::size_t runtime_metadata_category_adopted_reference_total = 0;
  std::size_t runtime_metadata_category_attachment_reference_total = 0;
  for (const auto &bundle :
       metadata.runtime_metadata_category_bundles_lexicographic) {
    ++runtime_metadata_category_bundle_count;
    runtime_metadata_category_adopted_reference_total +=
        bundle.adopted_protocol_owner_identities_lexicographic.size();
    runtime_metadata_category_attachment_reference_total += 3u;
  }
  out << "!57 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_protocol_category_emission_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_protocol_emission_payload_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_category_emission_payload_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_protocol_reference_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_category_attachment_model)
      << "\", i1 "
      << (metadata.runtime_metadata_protocol_category_emission_ready ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_protocol_category_emission_fail_closed ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_metadata_protocol_bundle_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_protocol_inherited_reference_total)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_metadata_category_bundle_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_category_adopted_reference_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             runtime_metadata_category_attachment_reference_total)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_protocol_category_typed_handoff_replay_key)
      << "\"}\n";

  std::size_t runtime_metadata_method_list_bundle_count = 0;
  std::size_t runtime_metadata_method_entry_total = 0;
  for (const auto &bundle :
       metadata.runtime_metadata_method_list_bundles_lexicographic) {
    ++runtime_metadata_method_list_bundle_count;
    runtime_metadata_method_entry_total += bundle.entries_lexicographic.size();
  }
  out << "!58 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_member_table_emission_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_method_list_emission_payload_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.runtime_metadata_method_list_grouping_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_property_descriptor_emission_payload_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_ivar_descriptor_emission_payload_model)
      << "\", i1 "
      << (metadata.runtime_metadata_member_table_emission_ready ? 1 : 0)
      << ", i1 "
      << (metadata.runtime_metadata_member_table_emission_fail_closed ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_metadata_method_list_bundle_count)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_metadata_method_entry_total)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_property_bundles_lexicographic.size())
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_metadata_ivar_bundles_lexicographic.size())
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_member_table_typed_handoff_replay_key)
      << "\"}\n";

  out << "!59 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeSelectorStringPoolEmissionContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeSelectorPoolEmissionPayloadModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStringPoolEmissionPayloadModel)
      << "\", i64 "
      << static_cast<unsigned long long>(selector_pool_global_count)
      << ", i64 "
      << static_cast<unsigned long long>(runtime_string_pool_global_count)
      << ", !\""
      << EscapeCStringLiteral(Objc3RuntimeMetadataHostSectionForLogicalName(
             kObjc3RuntimeSelectorPoolLogicalSection))
      << "\", !\""
      << EscapeCStringLiteral(Objc3RuntimeMetadataHostSectionForLogicalName(
             kObjc3RuntimeStringPoolLogicalSection))
      << "\"}\n";
  out << "!60 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionHarnessContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionPositiveCorpusModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionNegativeCorpusModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionSectionCommand)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBinaryInspectionSymbolCommand)
      << "\", i64 4, i64 1}\n";
  out << "!61 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeObjectPackagingRetentionContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeObjectPackagingRetentionBoundaryModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeObjectPackagingRetentionAnchorModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeObjectPackagingRetentionArtifact)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeObjectPackagingRetentionSymbolPrefix)
      << "\"}\n";
  out << "!62 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeLinkerRetentionContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLinkerRetentionAnchorModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLinkerDiscoveryModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLinkerAnchorLogicalSection)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLinkerDiscoveryRootLogicalSection)
      << "\", !\"" << EscapeCStringLiteral(runtime_metadata_linker_anchor_symbol)
      << "\", !\"" << EscapeCStringLiteral(runtime_metadata_discovery_root_symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLinkerResponseArtifactSuffix)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLinkerDiscoveryArtifactSuffix)
      << "\"}\n";
  out << "!63 = !{!\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_archive_static_link_discovery_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_archive_static_link_anchor_seed_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata
                 .runtime_metadata_archive_static_link_translation_unit_identity_model)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_archive_static_link_merge_model)
      << "\", i1 "
      << (metadata.runtime_metadata_archive_static_link_discovery_ready ? 1 : 0)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_archive_static_link_response_artifact_suffix)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.runtime_metadata_archive_static_link_discovery_artifact_suffix)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata
                 .runtime_metadata_archive_static_link_translation_unit_identity_key)
      << "\"}\n";
  out << "!64 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateEvidenceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMetadataEmissionGateFailureModel)
      << "\"}\n";
  out << "!65 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeMetadataObjectEmissionCloseoutContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMetadataObjectEmissionCloseoutEvidenceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMetadataObjectEmissionCloseoutFailureModel)
      << "\"}\n";
}

void EmitObjc3IRDispatchOwnershipMetadataNodes(
    const Objc3IRFrontendMetadata &metadata,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out) {
  out << "!66 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_instance_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_class_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_super_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_direct_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_surface_classification_dynamic_sites)
      << ", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_instance_entrypoint_family)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_class_entrypoint_family)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_super_entrypoint_family)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_direct_entrypoint_family)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.dispatch_surface_classification_dynamic_entrypoint_family)
      << "\", i1 "
      << (metadata.deterministic_dispatch_surface_classification_handoff ? 1 : 0)
      << "}\n";
  out << "!67 = !{!\""
      << EscapeCStringLiteral(metadata.executable_ivar_layout_emission_contract_id)
      << "\", !\""
      << EscapeCStringLiteral(metadata.executable_ivar_layout_descriptor_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.executable_ivar_offset_global_model)
      << "\", !\""
      << EscapeCStringLiteral(metadata.executable_ivar_layout_table_model)
      << "\", i1 " << (metadata.executable_ivar_layout_emission_ready ? 1 : 0)
      << ", i1 "
      << (metadata.executable_ivar_layout_emission_fail_closed ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.executable_ivar_offset_global_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.executable_ivar_layout_table_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.executable_ivar_layout_owner_entries)
      << ", !\""
      << EscapeCStringLiteral(metadata.executable_ivar_layout_emission_replay_key)
      << "\"}\n";
  out << "!68 = !{!\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringStorageModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringPropertyDescriptorModel)
      << "\", i64 "
      << static_cast<unsigned long long>(synthesized_property_accessor_count)
      << "}\n";
  out << "!69 = !{!\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionAccessorModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionPropertyContextModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionAutoreleaseModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", i64 "
      << static_cast<unsigned long long>(synthesized_property_accessor_count)
      << "}\n";
  out << "!70 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiReferenceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiWeakModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiAutoreleasepoolModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", i64 "
      << static_cast<unsigned long long>(synthesized_property_accessor_count)
      << "}\n";
  out << "!71 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementImplementationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMemoryManagementImplementationRefcountModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementImplementationWeakModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMemoryManagementImplementationAutoreleasepoolModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMemoryManagementImplementationFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", i64 "
      << static_cast<unsigned long long>(synthesized_property_accessor_count)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.autoreleasepool_scope_lowering_scope_sites)
      << "}\n";
  out << "!72 = !{!\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateSupportedModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateEvidenceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateNonGoalModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipRuntimeGateFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3OwnershipRuntimeHookEmissionContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeMemoryManagementApiContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeMemoryManagementImplementationContractId)
      << "\"}\n";
}

void EmitObjc3IRBlockArcMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!73 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateEvidenceModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateActiveModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateNonGoalModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(
             Expr::kObjc3ExecutableBlockSourceStorageAnnotationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             Expr::kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId)
      << "\"}\n";
  out << "!74 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixContractId)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixEvidenceModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixActiveModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixNonGoalModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockExecutionMatrixFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
      << "\"}\n";
  out << "!75 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundarySourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryModeModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipQualifierLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryNonGoalModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSourceModeBoundaryFailClosedModel)
      << "\"}\n";
  out << "!76 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingSourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingModeModel)
      << "\", !\"" << EscapeCStringLiteral(metadata.arc_mode)
      << "\", !\"" << EscapeCStringLiteral(kObjc3OwnershipQualifierLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3RunnableBlockRuntimeGateContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingNonGoalModel)
      << "\"}\n";
  out << "!77 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesSourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesSemanticModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcDiagnosticsFixitLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesNonGoalModel)
      << "\"}\n";
  out << "!78 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeSourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeSemanticModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcSemanticRulesContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3BlockStorageEscapeLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeFailClosedModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeNonGoalModel)
      << "\"}\n";
  out << "!79 = !{!\""
      << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsSourceModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsSemanticModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3WeakUnownedSemanticsLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RetainReleaseOperationLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3AutoreleasePoolScopeLoweringLaneContract)
      << "\", !\"" << EscapeCStringLiteral(kObjc3BlockStorageEscapeLoweringLaneContract)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsNonGoalModel)
      << "\"}\n";
  out << "!80 = !{!\""
      << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionSourceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionLoweringModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInferenceLifetimeContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcLoweringAbiCleanupModelContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionFailureModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionNonGoalModel)
      << "\"}\n";
  out << "!81 = !{!\""
      << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksSourceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksLoweringModel)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcModeHandlingContractId)
      << "\", !\"" << EscapeCStringLiteral(Expr::kObjc3ArcInteractionSemanticsContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcLoweringAbiCleanupModelContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksFailureModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksNonGoalModel)
      << "\"}\n";
  out << "!82 = !{!\""
      << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringSourceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcCleanupWeakLifetimeHooksContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcAutomaticInsertionContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePromoteBlockI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeInvokeBlockI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringFailureModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringNonGoalModel)
      << "\"}\n";
  out << "!83 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceReferenceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceWeakModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceAutoreleasepoolModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeArcHelperApiSurfaceFailClosedModel)
      << "\"}\n";
  out << "!84 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportDependencyModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportWeakModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeArcHelperRuntimeSupportAutoreleaseReturnModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportExecutionModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeArcHelperRuntimeSupportFailClosedModel)
      << "\"}\n";
  out << "!85 = !{!\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationDependencyModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationCoverageModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationValidationModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeRetainI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeAutoreleaseI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeReadCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeWriteCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeExchangeCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationFailClosedModel)
      << "\"}\n";
  out << "!86 = !{!\""
      << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateEvidenceModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateActiveModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateNonGoalModel)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcModeHandlingContractId)
      << "\", !\"" << EscapeCStringLiteral(kObjc3ArcInteractionSemanticsContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ArcBlockAutoreleaseReturnLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeArcDebugInstrumentationContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RunnableArcRuntimeGateFailClosedModel)
      << "\"}\n";
}

void EmitObjc3IRErrorHandlingMetadataNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!87 = !{!\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingThrowsAbiPropagationLoweringSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingThrowsAbiPropagationLoweringAbiModel)
      << "\", !\""
      << EscapeCStringLiteral(metadata.lowering_throws_propagation_replay_key)
      << "\", !\""
      << EscapeCStringLiteral(metadata.lowering_result_like_replay_key)
      << "\", !\""
      << EscapeCStringLiteral(metadata.lowering_ns_error_bridging_replay_key)
      << "\", !\""
      << EscapeCStringLiteral(metadata.lowering_unwind_cleanup_replay_key)
      << "\", i1 "
      << (metadata.deterministic_throws_propagation_lowering_handoff ? 1 : 0)
      << ", i1 "
      << (metadata.deterministic_result_like_lowering_handoff ? 1 : 0)
      << ", i1 "
      << (metadata.deterministic_ns_error_bridging_lowering_handoff ? 1 : 0)
      << ", i1 "
      << (metadata.deterministic_unwind_cleanup_lowering_handoff ? 1 : 0)
      << ", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingThrowsAbiPropagationLoweringFailClosedModel)
      << "\"}\n";
  out << "!88 = !{!\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingResultAndBridgingArtifactReplaySourceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingResultAndBridgingArtifactReplayModel)
      << "\", !\""
      << EscapeCStringLiteral(
             metadata.lowering_error_handling_result_and_bridging_artifact_replay_key)
      << "\", i64 "
      << static_cast<unsigned long long>(
             metadata.imported_error_handling_result_and_bridging_artifact_modules)
      << ", i1 "
      << (metadata.error_handling_result_and_bridging_binary_artifact_replay_ready
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .error_handling_result_and_bridging_runtime_import_artifact_ready
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .error_handling_result_and_bridging_separate_compilation_replay_ready
              ? 1
              : 0)
      << ", i1 "
      << (metadata
                  .deterministic_error_handling_result_and_bridging_artifact_replay_handoff
              ? 1
              : 0)
      << ", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingResultAndBridgingArtifactReplayFailClosedModel)
      << "\"}\n";
  out << "!89 = !{!\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingErrorRuntimeBridgeHelperContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingErrorRuntimeBridgeHelperSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingErrorRuntimeBridgeHelperAbiModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStoreThrownErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLoadThrownErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBridgeStatusErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBridgeNSErrorErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeCatchMatchesErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingErrorRuntimeBridgeHelperFailClosedModel)
      << "\"}\n";
  out << "!90 = !{!\""
      << EscapeCStringLiteral(kObjc3ErrorHandlingLiveErrorRuntimeIntegrationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingLiveErrorRuntimeIntegrationSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingLiveErrorRuntimeIntegrationExecutionModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingLiveErrorRuntimeIntegrationPackagingModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeStoreThrownErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeLoadThrownErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeBridgeStatusErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeCatchMatchesErrorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ErrorHandlingLiveErrorRuntimeIntegrationFailClosedModel)
      << "\"}\n";
}

void EmitObjc3IRConcurrencyRuntimeMetadataNodes(std::ostringstream &out) {
  out << "!91 = !{!\""
      << EscapeCStringLiteral(kObjc3ConcurrencyContinuationRuntimeHelperContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyContinuationRuntimeHelperSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyContinuationRuntimeHelperAbiModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyContinuationRuntimeHelperExecutionModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeAllocateAsyncContinuationI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeResumeAsyncContinuationI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyContinuationRuntimeHelperFailClosedModel)
      << "\"}\n";
  out << "!92 = !{!\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveContinuationRuntimeIntegrationContractId)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveContinuationRuntimeIntegrationSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveContinuationRuntimeIntegrationExecutionModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveContinuationRuntimeIntegrationPackagingModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeAllocateAsyncContinuationI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeResumeAsyncContinuationI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveContinuationRuntimeIntegrationFailClosedModel)
      << "\"}\n";
  out << "!93 = !{!\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeAbiCompletionContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeAbiCompletionSurfacePath)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeSpawnTaskI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeEnterTaskGroupScopeI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeAddTaskGroupTaskI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeWaitTaskGroupNextI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeCancelTaskGroupI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeTaskIsCancelledI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeTaskOnCancelI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeExecutorHopI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral("objc3_runtime_copy_task_runtime_state_for_testing")
      << "\"}\n";
  out << "!94 = !{!\""
      << EscapeCStringLiteral(kObjc3ConcurrencySchedulerExecutorRuntimeContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencySchedulerExecutorRuntimeSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencySchedulerExecutorRuntimeAbiModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencySchedulerExecutorRuntimeExecutionModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencySchedulerExecutorRuntimePackagingModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencySchedulerExecutorRuntimeFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeSpawnTaskI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeEnterTaskGroupScopeI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeAddTaskGroupTaskI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeWaitTaskGroupNextI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeCancelTaskGroupI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeTaskIsCancelledI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeTaskOnCancelI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeExecutorHopI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral("objc3_runtime_copy_task_runtime_state_for_testing")
      << "\"}\n";
  out << "!95 = !{!\""
      << EscapeCStringLiteral(kObjc3ConcurrencyLiveTaskRuntimeIntegrationContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyLiveTaskRuntimeIntegrationSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveTaskRuntimeIntegrationExecutionModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveTaskRuntimeIntegrationPackagingModel)
      << "\", !\""
      << EscapeCStringLiteral(
             kObjc3ConcurrencyLiveTaskRuntimeIntegrationFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeSpawnTaskI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeEnterTaskGroupScopeI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeAddTaskGroupTaskI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeWaitTaskGroupNextI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeCancelTaskGroupI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeTaskIsCancelledI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeTaskOnCancelI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimeExecutorHopI32Symbol)
      << "\", !\""
      << EscapeCStringLiteral("objc3_runtime_copy_task_runtime_state_for_testing")
      << "\"}\n";
  out << "!96 = !{!\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeHardeningContractId)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeHardeningSourceModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeHardeningExecutionModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeHardeningPackagingModel)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3ConcurrencyTaskRuntimeHardeningFailClosedModel)
      << "\", !\""
      << EscapeCStringLiteral("objc3_runtime_reset_for_testing")
      << "\", !\""
      << EscapeCStringLiteral("objc3_runtime_copy_task_runtime_state_for_testing")
      << "\", !\""
      << EscapeCStringLiteral(
             "objc3_runtime_copy_memory_management_state_for_testing")
      << "\", !\""
      << EscapeCStringLiteral("objc3_runtime_copy_arc_debug_state_for_testing")
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimePushAutoreleasepoolScopeSymbol)
      << "\", !\""
      << EscapeCStringLiteral(kObjc3RuntimePopAutoreleasepoolScopeSymbol)
      << "\"}\n";
}

void EmitObjc3IRTypeSymbolDispatchCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!5 = !{i64 " << static_cast<unsigned long long>(metadata.object_pointer_type_spellings)
      << ", i64 " << static_cast<unsigned long long>(metadata.pointer_declarator_entries) << ", i64 "
      << static_cast<unsigned long long>(metadata.pointer_declarator_depth_total) << ", i64 "
      << static_cast<unsigned long long>(metadata.pointer_declarator_token_entries) << ", i64 "
      << static_cast<unsigned long long>(metadata.nullability_suffix_entries) << ", i64 "
      << static_cast<unsigned long long>(metadata.generic_suffix_entries) << ", i64 "
      << static_cast<unsigned long long>(metadata.terminated_generic_suffix_entries) << ", i64 "
      << static_cast<unsigned long long>(metadata.unterminated_generic_suffix_entries) << ", i1 "
      << (metadata.deterministic_object_pointer_nullability_generics_handoff ? 1 : 0) << "}\n";
  out << "!6 = !{i64 " << static_cast<unsigned long long>(metadata.global_symbol_nodes) << ", i64 "
      << static_cast<unsigned long long>(metadata.function_symbol_nodes) << ", i64 "
      << static_cast<unsigned long long>(metadata.interface_symbol_nodes) << ", i64 "
      << static_cast<unsigned long long>(metadata.implementation_symbol_nodes) << ", i64 "
      << static_cast<unsigned long long>(metadata.interface_property_symbol_nodes) << ", i64 "
      << static_cast<unsigned long long>(metadata.implementation_property_symbol_nodes) << ", i64 "
      << static_cast<unsigned long long>(metadata.interface_method_symbol_nodes) << ", i64 "
      << static_cast<unsigned long long>(metadata.implementation_method_symbol_nodes) << ", i64 "
      << static_cast<unsigned long long>(metadata.top_level_scope_symbols) << ", i64 "
      << static_cast<unsigned long long>(metadata.nested_scope_symbols) << ", i64 "
      << static_cast<unsigned long long>(metadata.scope_frames_total) << ", i64 "
      << static_cast<unsigned long long>(metadata.implementation_interface_resolution_sites) << ", i64 "
      << static_cast<unsigned long long>(metadata.implementation_interface_resolution_hits) << ", i64 "
      << static_cast<unsigned long long>(metadata.implementation_interface_resolution_misses) << ", i64 "
      << static_cast<unsigned long long>(metadata.method_resolution_sites) << ", i64 "
      << static_cast<unsigned long long>(metadata.method_resolution_hits) << ", i64 "
      << static_cast<unsigned long long>(metadata.method_resolution_misses) << ", i1 "
      << (metadata.deterministic_symbol_graph_handoff ? 1 : 0) << ", i1 "
      << (metadata.deterministic_scope_resolution_handoff ? 1 : 0) << ", !\""
      << EscapeCStringLiteral(metadata.deterministic_symbol_graph_scope_resolution_handoff_key)
      << "\"}\n";
  out << "!7 = !{i64 " << static_cast<unsigned long long>(metadata.declared_class_interfaces) << ", i64 "
      << static_cast<unsigned long long>(metadata.declared_class_implementations) << ", i64 "
      << static_cast<unsigned long long>(metadata.resolved_class_interfaces) << ", i64 "
      << static_cast<unsigned long long>(metadata.resolved_class_implementations) << ", i64 "
      << static_cast<unsigned long long>(metadata.linked_class_method_symbols) << ", i64 "
      << static_cast<unsigned long long>(metadata.linked_category_method_symbols) << ", i64 "
      << static_cast<unsigned long long>(metadata.protocol_composition_sites) << ", i64 "
      << static_cast<unsigned long long>(metadata.protocol_composition_symbols) << ", i64 "
      << static_cast<unsigned long long>(metadata.category_composition_sites) << ", i64 "
      << static_cast<unsigned long long>(metadata.category_composition_symbols) << ", i64 "
      << static_cast<unsigned long long>(metadata.invalid_protocol_composition_sites) << ", i1 "
      << (metadata.deterministic_class_protocol_category_linking_handoff ? 1 : 0) << "}\n";
  out << "!8 = !{i64 " << static_cast<unsigned long long>(metadata.id_typecheck_sites) << ", i64 "
      << static_cast<unsigned long long>(metadata.class_typecheck_sites) << ", i64 "
      << static_cast<unsigned long long>(metadata.sel_typecheck_sites) << ", i64 "
      << static_cast<unsigned long long>(metadata.object_pointer_typecheck_sites) << ", i64 "
      << static_cast<unsigned long long>(metadata.id_class_sel_object_pointer_typecheck_sites_total)
      << ", i1 "
      << (metadata.deterministic_id_class_sel_object_pointer_typecheck_handoff ? 1 : 0) << "}\n";
  out << "!9 = !{i64 " << static_cast<unsigned long long>(metadata.message_send_selector_lowering_sites)
      << ", i64 " << static_cast<unsigned long long>(metadata.message_send_selector_lowering_unary_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.message_send_selector_lowering_keyword_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.message_send_selector_lowering_selector_piece_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.message_send_selector_lowering_argument_expression_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.message_send_selector_lowering_receiver_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.message_send_selector_lowering_selector_literal_entries)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.message_send_selector_lowering_selector_literal_characters)
      << ", i1 " << (metadata.deterministic_message_send_selector_lowering_handoff ? 1 : 0) << "}\n";
  out << "!10 = !{i64 " << static_cast<unsigned long long>(metadata.dispatch_abi_marshalling_message_send_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.dispatch_abi_marshalling_receiver_slots_marshaled)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.dispatch_abi_marshalling_selector_slots_marshaled)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_abi_marshalling_argument_value_slots_marshaled)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_abi_marshalling_argument_padding_slots_marshaled)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.dispatch_abi_marshalling_argument_total_slots_marshaled)
      << ", i64 " << static_cast<unsigned long long>(metadata.dispatch_abi_marshalling_total_marshaled_slots)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.dispatch_abi_marshalling_runtime_dispatch_arg_slots)
      << ", i1 " << (metadata.deterministic_dispatch_abi_marshalling_handoff ? 1 : 0) << "}\n";
}

void EmitObjc3IRDispatchOwnershipLoweringCounterNodes(
    const Objc3IRFrontendMetadata &metadata, std::ostringstream &out) {
  out << "!11 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.nil_receiver_semantics_foldability_message_send_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nil_receiver_semantics_foldability_receiver_nil_literal_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.nil_receiver_semantics_foldability_enabled_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.nil_receiver_semantics_foldability_foldable_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nil_receiver_semantics_foldability_runtime_dispatch_required_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.nil_receiver_semantics_foldability_non_nil_receiver_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.nil_receiver_semantics_foldability_contract_violation_sites)
      << ", i1 " << (metadata.deterministic_nil_receiver_semantics_foldability_handoff ? 1 : 0)
      << "}\n\n";
  out << "!12 = !{i64 "
      << static_cast<unsigned long long>(metadata.super_dispatch_method_family_message_send_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.super_dispatch_method_family_receiver_super_identifier_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.super_dispatch_method_family_enabled_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.super_dispatch_method_family_requires_class_context_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.super_dispatch_method_family_init_sites)
      << ", i64 " << static_cast<unsigned long long>(metadata.super_dispatch_method_family_copy_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.super_dispatch_method_family_mutable_copy_sites)
      << ", i64 " << static_cast<unsigned long long>(metadata.super_dispatch_method_family_new_sites)
      << ", i64 " << static_cast<unsigned long long>(metadata.super_dispatch_method_family_none_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.super_dispatch_method_family_returns_retained_result_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.super_dispatch_method_family_returns_related_result_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.super_dispatch_method_family_contract_violation_sites)
      << ", i1 " << (metadata.deterministic_super_dispatch_method_family_handoff ? 1 : 0)
      << "}\n\n";
  out << "!13 = !{i64 "
      << static_cast<unsigned long long>(metadata.runtime_link_host_link_message_send_sites)
      << ", i64 " << static_cast<unsigned long long>(metadata.runtime_link_host_link_required_sites)
      << ", i64 " << static_cast<unsigned long long>(metadata.runtime_link_host_link_elided_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.runtime_link_host_link_runtime_dispatch_arg_slots)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.runtime_link_host_link_runtime_dispatch_declaration_parameter_count)
      << ", !\"" << EscapeCStringLiteral(metadata.runtime_link_host_link_runtime_dispatch_symbol)
      << "\", i1 "
      << (metadata.runtime_link_host_link_default_runtime_dispatch_symbol_binding ? 1 : 0)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.runtime_link_host_link_contract_violation_sites)
      << ", i1 " << (metadata.deterministic_runtime_link_host_link_handoff ? 1 : 0)
      << "}\n\n";
  out << "!14 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_qualifier_lowering_ownership_qualifier_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_qualifier_lowering_invalid_ownership_qualifier_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.ownership_qualifier_lowering_object_pointer_type_annotation_sites)
      << ", i1 " << (metadata.deterministic_ownership_qualifier_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!15 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.retain_release_operation_lowering_ownership_qualified_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.retain_release_operation_lowering_retain_insertion_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.retain_release_operation_lowering_release_insertion_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.retain_release_operation_lowering_autorelease_insertion_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.retain_release_operation_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_retain_release_operation_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!16 = !{i64 "
      << static_cast<unsigned long long>(metadata.autoreleasepool_scope_lowering_scope_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.autoreleasepool_scope_lowering_scope_symbolized_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.autoreleasepool_scope_lowering_max_scope_depth)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.autoreleasepool_scope_lowering_scope_entry_transition_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.autoreleasepool_scope_lowering_scope_exit_transition_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.autoreleasepool_scope_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_autoreleasepool_scope_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!17 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.weak_unowned_semantics_lowering_ownership_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.weak_unowned_semantics_lowering_weak_reference_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.weak_unowned_semantics_lowering_unowned_reference_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.weak_unowned_semantics_lowering_unowned_safe_reference_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.weak_unowned_semantics_lowering_conflict_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.weak_unowned_semantics_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_weak_unowned_semantics_lowering_handoff ? 1 : 0)
      << "}\n\n";
  out << "!18 = !{i64 "
      << static_cast<unsigned long long>(
             metadata.arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata
                 .arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites)
      << ", i64 "
      << static_cast<unsigned long long>(
             metadata.arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites)
      << ", i64 "
      << static_cast<unsigned long long>(metadata.arc_diagnostics_fixit_lowering_contract_violation_sites)
      << ", i1 "
      << (metadata.deterministic_arc_diagnostics_fixit_lowering_handoff ? 1 : 0)
      << "}\n\n";
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
