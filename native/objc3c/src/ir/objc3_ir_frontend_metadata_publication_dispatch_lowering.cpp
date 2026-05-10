#include "ir/objc3_ir_frontend_metadata_publication_dispatch_lowering.h"

#include <sstream>

#include "ir/objc3_ir_c_string.h"
#include "ir/objc3_ir_frontend_metadata.h"

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
