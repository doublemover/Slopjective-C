#include "ir/objc3_ir_frontend_metadata_publication.h"

#include <sstream>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_core_metadata_nodes.h"
#include "ir/objc3_ir_lowering_extension_metadata_publication.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"
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

void EmitObjc3IRFrontendMetadataPublication(
    const Objc3IRFrontendMetadata &metadata,
    const Objc3IRRuntimeMetadataSymbols &runtime_metadata_symbols,
    std::size_t selector_pool_global_count,
    std::size_t runtime_string_pool_global_count,
    std::size_t synthesized_property_accessor_count, std::ostringstream &out) {
  EmitObjc3IRFrontendCoreMetadataPublication(metadata, out);
  EmitObjc3IRRuntimeMetadataBoundaryNodes(metadata, out);
  Objc3RuntimeMetadataLayoutPolicy runtime_metadata_layout_policy;
  std::string runtime_metadata_layout_policy_error;
  if (!BuildObjc3IRRuntimeMetadataLayoutPolicy(
          metadata, runtime_metadata_layout_policy,
          runtime_metadata_layout_policy_error) &&
      runtime_metadata_layout_policy.failure_reason.empty()) {
    runtime_metadata_layout_policy.failure_reason =
        runtime_metadata_layout_policy_error;
  }
  EmitObjc3IRRuntimeSupportMetadataNodes(metadata, out);
  EmitObjc3IRRuntimeMetadataObjectPublicationNodes(
      metadata, runtime_metadata_layout_policy,
      runtime_metadata_symbols.linker_anchor_symbol,
      runtime_metadata_symbols.discovery_root_symbol, selector_pool_global_count,
      runtime_string_pool_global_count, out);
  EmitObjc3IRDispatchOwnershipMetadataNodes(
      metadata, synthesized_property_accessor_count, out);
  EmitObjc3IRBlockArcMetadataNodes(metadata, out);
  EmitObjc3IRErrorHandlingMetadataNodes(metadata, out);
  EmitObjc3IRConcurrencyRuntimeMetadataNodes(out);
  EmitObjc3IRTypeSymbolDispatchCounterNodes(metadata, out);
  EmitObjc3IRDispatchOwnershipLoweringCounterNodes(metadata, out);
  EmitObjc3IRBlockLoweringCounterNodes(metadata, out);
  EmitObjc3IRTypeModuleLoweringCounterNodes(metadata, out);
  EmitObjc3IRModuleGovernanceLoweringCounterNodes(metadata, out);
  EmitObjc3IRErrorHandlingLoweringCounterNodes(metadata, out);
  EmitObjc3IRSafetyConcurrencyLoweringCounterNodes(metadata, out);
  EmitObjc3IRActorDispatchControlMetadataNodes(metadata, out);
  EmitObjc3IRInteropLoweringMetadataNodes(metadata, out);
  EmitObjc3IRMetaprogrammingLoweringMetadataNodes(metadata, out);
  EmitObjc3IRDispatchMetadataPreservationNodes(metadata, out);
  EmitObjc3IROwnershipExtensionMetadataNodes(metadata, out);
  EmitObjc3IRAsyncDiagnosticLoweringCounterNodes(metadata, out);
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
