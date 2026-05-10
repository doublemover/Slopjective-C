#include "ir/objc3_ir_module_metadata_publication_core_profiles.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "ir/objc3_ir_frontend_metadata_publication.h"

void EmitObjc3IRModuleMetadataCoreProfilePublication(
    const Objc3IRFrontendMetadata &frontend_metadata_,
    std::size_t synthesized_property_accessor_count_,
    std::ostringstream &out) {
  out << BuildObjc3IRFrontendProfileComment(frontend_metadata_) << "\n";
  out << "; frontend_objc_interface_implementation_profile = declared_interfaces="
      << frontend_metadata_.declared_interfaces
      << ", declared_implementations=" << frontend_metadata_.declared_implementations
      << ", resolved_interface_symbols=" << frontend_metadata_.resolved_interface_symbols
      << ", resolved_implementation_symbols=" << frontend_metadata_.resolved_implementation_symbols
      << ", interface_method_symbols=" << frontend_metadata_.interface_method_symbols
      << ", implementation_method_symbols=" << frontend_metadata_.implementation_method_symbols
      << ", linked_implementation_symbols=" << frontend_metadata_.linked_implementation_symbols
      << ", deterministic_interface_implementation_handoff="
      << (frontend_metadata_.deterministic_interface_implementation_handoff ? "true" : "false") << "\n";
  out << "; frontend_objc_protocol_category_profile = declared_protocols="
      << frontend_metadata_.declared_protocols
      << ", declared_categories=" << frontend_metadata_.declared_categories
      << ", resolved_protocol_symbols=" << frontend_metadata_.resolved_protocol_symbols
      << ", resolved_category_symbols=" << frontend_metadata_.resolved_category_symbols
      << ", protocol_method_symbols=" << frontend_metadata_.protocol_method_symbols
      << ", category_method_symbols=" << frontend_metadata_.category_method_symbols
      << ", linked_category_symbols=" << frontend_metadata_.linked_category_symbols
      << ", deterministic_protocol_category_handoff="
      << (frontend_metadata_.deterministic_protocol_category_handoff ? "true" : "false") << "\n";
  out << "; frontend_objc_class_protocol_category_linking_profile = declared_class_interfaces="
      << frontend_metadata_.declared_class_interfaces
      << ", declared_class_implementations=" << frontend_metadata_.declared_class_implementations
      << ", resolved_class_interfaces=" << frontend_metadata_.resolved_class_interfaces
      << ", resolved_class_implementations=" << frontend_metadata_.resolved_class_implementations
      << ", linked_class_method_symbols=" << frontend_metadata_.linked_class_method_symbols
      << ", linked_category_method_symbols=" << frontend_metadata_.linked_category_method_symbols
      << ", protocol_composition_sites=" << frontend_metadata_.protocol_composition_sites
      << ", protocol_composition_symbols=" << frontend_metadata_.protocol_composition_symbols
      << ", category_composition_sites=" << frontend_metadata_.category_composition_sites
      << ", category_composition_symbols=" << frontend_metadata_.category_composition_symbols
      << ", invalid_protocol_composition_sites=" << frontend_metadata_.invalid_protocol_composition_sites
      << ", deterministic_class_protocol_category_linking_handoff="
      << (frontend_metadata_.deterministic_class_protocol_category_linking_handoff ? "true" : "false") << "\n";
  out << "; frontend_objc_selector_normalization_profile = method_declaration_entries="
      << frontend_metadata_.selector_method_declaration_entries
      << ", normalized_method_declarations=" << frontend_metadata_.selector_normalized_method_declarations
      << ", selector_piece_entries=" << frontend_metadata_.selector_piece_entries
      << ", selector_piece_parameter_links=" << frontend_metadata_.selector_piece_parameter_links
      << ", deterministic_selector_normalization_handoff="
      << (frontend_metadata_.deterministic_selector_normalization_handoff ? "true" : "false") << "\n";
  out << "; frontend_objc_property_attribute_profile = property_declaration_entries="
      << frontend_metadata_.property_declaration_entries
      << ", property_attribute_entries=" << frontend_metadata_.property_attribute_entries
      << ", property_attribute_value_entries=" << frontend_metadata_.property_attribute_value_entries
      << ", property_accessor_modifier_entries=" << frontend_metadata_.property_accessor_modifier_entries
      << ", property_getter_selector_entries=" << frontend_metadata_.property_getter_selector_entries
      << ", property_setter_selector_entries=" << frontend_metadata_.property_setter_selector_entries
      << ", deterministic_property_attribute_handoff="
      << (frontend_metadata_.deterministic_property_attribute_handoff ? "true" : "false") << "\n";
  out << "; frontend_objc_id_class_sel_object_pointer_typecheck_profile = id_typecheck_sites="
      << frontend_metadata_.id_typecheck_sites
      << ", class_typecheck_sites=" << frontend_metadata_.class_typecheck_sites
      << ", sel_typecheck_sites=" << frontend_metadata_.sel_typecheck_sites
      << ", object_pointer_typecheck_sites=" << frontend_metadata_.object_pointer_typecheck_sites
      << ", total_typecheck_sites=" << frontend_metadata_.id_class_sel_object_pointer_typecheck_sites_total
      << ", deterministic_id_class_sel_object_pointer_typecheck_handoff="
      << (frontend_metadata_.deterministic_id_class_sel_object_pointer_typecheck_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_dispatch_surface_classification_profile = instance_dispatch_sites="
      << frontend_metadata_.dispatch_surface_classification_instance_sites
      << ", class_dispatch_sites="
      << frontend_metadata_.dispatch_surface_classification_class_sites
      << ", super_dispatch_sites="
      << frontend_metadata_.dispatch_surface_classification_super_sites
      << ", direct_dispatch_sites="
      << frontend_metadata_.dispatch_surface_classification_direct_sites
      << ", dynamic_dispatch_sites="
      << frontend_metadata_.dispatch_surface_classification_dynamic_sites
      << ", instance_entrypoint_family="
      << frontend_metadata_
             .dispatch_surface_classification_instance_entrypoint_family
      << ", class_entrypoint_family="
      << frontend_metadata_.dispatch_surface_classification_class_entrypoint_family
      << ", super_entrypoint_family="
      << frontend_metadata_.dispatch_surface_classification_super_entrypoint_family
      << ", direct_entrypoint_family="
      << frontend_metadata_.dispatch_surface_classification_direct_entrypoint_family
      << ", dynamic_entrypoint_family="
      << frontend_metadata_
             .dispatch_surface_classification_dynamic_entrypoint_family
      << ", deterministic_dispatch_surface_classification_handoff="
      << (frontend_metadata_.deterministic_dispatch_surface_classification_handoff
              ? "true"
              : "false")
      << "\n";
  out << "; frontend_objc_message_send_selector_lowering_profile = message_send_sites="
      << frontend_metadata_.message_send_selector_lowering_sites
      << ", unary_selector_sites=" << frontend_metadata_.message_send_selector_lowering_unary_sites
      << ", keyword_selector_sites=" << frontend_metadata_.message_send_selector_lowering_keyword_sites
      << ", selector_piece_sites=" << frontend_metadata_.message_send_selector_lowering_selector_piece_sites
      << ", argument_expression_sites="
      << frontend_metadata_.message_send_selector_lowering_argument_expression_sites
      << ", receiver_expression_sites=" << frontend_metadata_.message_send_selector_lowering_receiver_sites
      << ", selector_literal_entries="
      << frontend_metadata_.message_send_selector_lowering_selector_literal_entries
      << ", selector_literal_characters="
      << frontend_metadata_.message_send_selector_lowering_selector_literal_characters
      << ", deterministic_message_send_selector_lowering_handoff="
      << (frontend_metadata_.deterministic_message_send_selector_lowering_handoff ? "true" : "false") << "\n";
  out << "; frontend_objc_dispatch_abi_marshalling_profile = message_send_sites="
      << frontend_metadata_.dispatch_abi_marshalling_message_send_sites
      << ", receiver_slots_marshaled=" << frontend_metadata_.dispatch_abi_marshalling_receiver_slots_marshaled
      << ", selector_slots_marshaled=" << frontend_metadata_.dispatch_abi_marshalling_selector_slots_marshaled
      << ", argument_value_slots_marshaled="
      << frontend_metadata_.dispatch_abi_marshalling_argument_value_slots_marshaled
      << ", argument_padding_slots_marshaled="
      << frontend_metadata_.dispatch_abi_marshalling_argument_padding_slots_marshaled
      << ", argument_total_slots_marshaled="
      << frontend_metadata_.dispatch_abi_marshalling_argument_total_slots_marshaled
      << ", total_marshaled_slots=" << frontend_metadata_.dispatch_abi_marshalling_total_marshaled_slots
      << ", runtime_dispatch_arg_slots="
      << frontend_metadata_.dispatch_abi_marshalling_runtime_dispatch_arg_slots
      << ", deterministic_dispatch_abi_marshalling_handoff="
      << (frontend_metadata_.deterministic_dispatch_abi_marshalling_handoff ? "true" : "false") << "\n";
  out << "; frontend_objc_nil_receiver_semantics_foldability_profile = message_send_sites="
      << frontend_metadata_.nil_receiver_semantics_foldability_message_send_sites
      << ", receiver_nil_literal_sites="
      << frontend_metadata_.nil_receiver_semantics_foldability_receiver_nil_literal_sites
      << ", nil_receiver_semantics_enabled_sites="
      << frontend_metadata_.nil_receiver_semantics_foldability_enabled_sites
      << ", nil_receiver_foldable_sites="
      << frontend_metadata_.nil_receiver_semantics_foldability_foldable_sites
      << ", nil_receiver_runtime_dispatch_required_sites="
      << frontend_metadata_.nil_receiver_semantics_foldability_runtime_dispatch_required_sites
      << ", non_nil_receiver_sites="
      << frontend_metadata_.nil_receiver_semantics_foldability_non_nil_receiver_sites
      << ", contract_violation_sites="
      << frontend_metadata_.nil_receiver_semantics_foldability_contract_violation_sites
      << ", deterministic_nil_receiver_semantics_foldability_handoff="
      << (frontend_metadata_.deterministic_nil_receiver_semantics_foldability_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_super_dispatch_method_family_profile = message_send_sites="
      << frontend_metadata_.super_dispatch_method_family_message_send_sites
      << ", receiver_super_identifier_sites="
      << frontend_metadata_.super_dispatch_method_family_receiver_super_identifier_sites
      << ", super_dispatch_enabled_sites=" << frontend_metadata_.super_dispatch_method_family_enabled_sites
      << ", super_dispatch_requires_class_context_sites="
      << frontend_metadata_.super_dispatch_method_family_requires_class_context_sites
      << ", method_family_init_sites=" << frontend_metadata_.super_dispatch_method_family_init_sites
      << ", method_family_copy_sites=" << frontend_metadata_.super_dispatch_method_family_copy_sites
      << ", method_family_mutable_copy_sites="
      << frontend_metadata_.super_dispatch_method_family_mutable_copy_sites
      << ", method_family_new_sites=" << frontend_metadata_.super_dispatch_method_family_new_sites
      << ", method_family_none_sites=" << frontend_metadata_.super_dispatch_method_family_none_sites
      << ", method_family_returns_retained_result_sites="
      << frontend_metadata_.super_dispatch_method_family_returns_retained_result_sites
      << ", method_family_returns_related_result_sites="
      << frontend_metadata_.super_dispatch_method_family_returns_related_result_sites
      << ", contract_violation_sites="
      << frontend_metadata_.super_dispatch_method_family_contract_violation_sites
      << ", deterministic_super_dispatch_method_family_handoff="
      << (frontend_metadata_.deterministic_super_dispatch_method_family_handoff ? "true" : "false")
      << "\n";
  out << "; frontend_objc_runtime_link_host_link_profile = message_send_sites="
      << frontend_metadata_.runtime_link_host_link_message_send_sites
      << ", runtime_link_required_sites=" << frontend_metadata_.runtime_link_host_link_required_sites
      << ", runtime_link_elided_sites=" << frontend_metadata_.runtime_link_host_link_elided_sites
      << ", runtime_dispatch_arg_slots="
      << frontend_metadata_.runtime_link_host_link_runtime_dispatch_arg_slots
      << ", runtime_dispatch_declaration_parameter_count="
      << frontend_metadata_.runtime_link_host_link_runtime_dispatch_declaration_parameter_count
      << ", runtime_dispatch_symbol=" << frontend_metadata_.runtime_link_host_link_runtime_dispatch_symbol
      << ", default_runtime_dispatch_symbol_binding="
      << (frontend_metadata_.runtime_link_host_link_default_runtime_dispatch_symbol_binding ? "true" : "false")
      << ", contract_violation_sites="
      << frontend_metadata_.runtime_link_host_link_contract_violation_sites
      << ", deterministic_runtime_link_host_link_handoff="
      << (frontend_metadata_.deterministic_runtime_link_host_link_handoff ? "true" : "false")
      << "\n";
  // runtime-backed-object-ownership freeze anchor: the current
  // runnable object slice preserves ownership through property/accessor
  // metadata profiles plus these source-side ownership lowering summaries. No
  // live ARC runtime retain/release/autorelease execution hooks are emitted
  // here yet.
  // retainable-object semantic-rule freeze anchor: retain/release,
  // autoreleasepool, and destruction-order behavior are still represented by
  // deterministic summary lanes only; runtime-backed property/member
  // ownership metadata and storage legality are now live sema-enforced
  // surfaces before metadata emission.
  out << "; retainable_object_semantic_rules_freeze = "
      << Objc3RetainableObjectSemanticRulesFreezeSummary() << "\n";
  // runtime-backed storage ownership legality anchor: explicit
  // object-property ownership qualifiers now participate in live semantic
  // legality, so the IR closeout surface publishes the exact owned/weak/
  // unowned contract now enforced before metadata emission.
  out << "; runtime_backed_storage_ownership_legality = "
      << Objc3RuntimeBackedStorageOwnershipLegalitySummary() << "\n";
  // autoreleasepool/destruction-order semantic expansion anchor:
  // lane-B still fail-closes autoreleasepool, but the IR closeout surface
  // now publishes the ownership-sensitive destruction-order contract that
  // distinguishes plain pool rejection from owned runtime-backed object
  // storage edges.
  out << "; runtime_backed_autoreleasepool_destruction_order = "
      << Objc3RuntimeBackedAutoreleasepoolDestructionOrderSummary() << "\n";
  // ownership-lowering baseline freeze anchor: the current IR
  // surface keeps ownership qualifier, retain/release, autoreleasepool, and
  // weak/unowned lowering on deterministic summary lanes only.
  // No live runtime ownership hooks are emitted here before the next runtime step.
  out << "; ownership_lowering_baseline = "
      << Objc3OwnershipLoweringBaselineSummary() << "\n";
  if (synthesized_property_accessor_count_ > 0u) {
    // runtime hook emission anchor: synthesized accessors now
    // execute through runtime-owned helper entrypoints that consume the
    // current dispatch-frame property context rather than the old summary-only
    // ownership lane, and owned and weak execution paths use the live runtime
    // hooks below.
    out << "; ownership_runtime_hook_emission = "
        << Objc3OwnershipRuntimeHookEmissionSummary()
        << ";synthesized_accessor_entries="
        << synthesized_property_accessor_count_ << "\n";
    // runtime memory-management API freeze anchor: the public
    // runtime ABI remains narrow while lane-C emits the private helper
    // surface consumed by synthesized ownership accessors.
    out << "; runtime_memory_management_api = "
        << Objc3RuntimeMemoryManagementApiSummary()
        << ";synthesized_accessor_entries="
        << synthesized_property_accessor_count_ << "\n";
  }
  if (synthesized_property_accessor_count_ > 0u ||
      frontend_metadata_.autoreleasepool_scope_lowering_scope_sites > 0u ||
      frontend_metadata_.block_storage_escape_lowering_escape_to_heap_sites >
          0u ||
      frontend_metadata_.block_copy_dispose_lowering_copy_helper_required_sites > 0u ||
      frontend_metadata_.block_copy_dispose_lowering_dispose_helper_required_sites > 0u) {
    // runtime memory-management implementation anchor: emitted IR
    // now carries the private autoreleasepool push/pop runtime surface and
    // the live refcount/weak/autoreleasepool execution model that native mode
    // consumes for runtime-backed object programs.
    out << "; runtime_memory_management_implementation = "
        << Objc3RuntimeMemoryManagementImplementationSummary()
        << ";synthesized_accessor_entries="
        << synthesized_property_accessor_count_
        << ";autoreleasepool_scope_sites="
        << frontend_metadata_.autoreleasepool_scope_lowering_scope_sites
        << "\n";
  }
}
