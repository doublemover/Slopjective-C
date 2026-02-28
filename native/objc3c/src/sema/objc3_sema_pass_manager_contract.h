#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "sema/objc3_sema_contract.h"

inline constexpr std::uint32_t kObjc3SemaPassManagerContractVersionMajor = 1;
inline constexpr std::uint32_t kObjc3SemaPassManagerContractVersionMinor = 0;
inline constexpr std::uint32_t kObjc3SemaPassManagerContractVersionPatch = 0;

enum class Objc3SemaPassId {
  BuildIntegrationSurface = 0,
  ValidateBodies = 1,
  ValidatePureContract = 2,
};

enum class Objc3SemaCompatibilityMode : std::uint8_t {
  Canonical = 0,
  Legacy = 1,
};

struct Objc3SemaMigrationHints {
  std::size_t legacy_yes_count = 0;
  std::size_t legacy_no_count = 0;
  std::size_t legacy_null_count = 0;

  std::size_t legacy_total() const { return legacy_yes_count + legacy_no_count + legacy_null_count; }
};

inline constexpr std::array<Objc3SemaPassId, 3> kObjc3SemaPassOrder = {
    Objc3SemaPassId::BuildIntegrationSurface,
    Objc3SemaPassId::ValidateBodies,
    Objc3SemaPassId::ValidatePureContract,
};

inline bool IsMonotonicObjc3SemaDiagnosticsAfterPass(const std::array<std::size_t, 3> &diagnostics_after_pass) {
  for (std::size_t i = 1; i < diagnostics_after_pass.size(); ++i) {
    if (diagnostics_after_pass[i] < diagnostics_after_pass[i - 1]) {
      return false;
    }
  }
  return true;
}

struct Objc3SemaDiagnosticsBus {
  std::vector<std::string> *diagnostics = nullptr;

  void Publish(const std::string &diagnostic) const {
    if (diagnostics == nullptr) {
      return;
    }
    diagnostics->push_back(diagnostic);
  }

  void PublishBatch(const std::vector<std::string> &batch) const {
    if (diagnostics == nullptr || batch.empty()) {
      return;
    }
    diagnostics->insert(diagnostics->end(), batch.begin(), batch.end());
  }

  std::size_t Count() const {
    if (diagnostics == nullptr) {
      return 0;
    }
    return diagnostics->size();
  }
};

struct Objc3SemaPassManagerInput {
  const Objc3ParsedProgram *program = nullptr;
  Objc3SemanticValidationOptions validation_options;
  Objc3SemaCompatibilityMode compatibility_mode = Objc3SemaCompatibilityMode::Canonical;
  bool migration_assist = false;
  Objc3SemaMigrationHints migration_hints;
  Objc3SemaDiagnosticsBus diagnostics_bus;
};

struct Objc3SemaParityContractSurface {
  std::array<std::size_t, 3> diagnostics_after_pass = {0, 0, 0};
  std::array<std::size_t, 3> diagnostics_emitted_by_pass = {0, 0, 0};
  std::size_t diagnostics_total = 0;
  std::size_t globals_total = 0;
  std::size_t functions_total = 0;
  std::size_t interfaces_total = 0;
  std::size_t implementations_total = 0;
  std::size_t type_metadata_global_entries = 0;
  std::size_t type_metadata_function_entries = 0;
  std::size_t type_metadata_interface_entries = 0;
  std::size_t type_metadata_implementation_entries = 0;
  std::size_t interface_method_symbols_total = 0;
  std::size_t implementation_method_symbols_total = 0;
  std::size_t linked_implementation_symbols_total = 0;
  std::size_t protocol_composition_sites_total = 0;
  std::size_t protocol_composition_symbols_total = 0;
  std::size_t category_composition_sites_total = 0;
  std::size_t category_composition_symbols_total = 0;
  std::size_t invalid_protocol_composition_sites_total = 0;
  std::size_t selector_normalization_methods_total = 0;
  std::size_t selector_normalization_normalized_methods_total = 0;
  std::size_t selector_normalization_piece_entries_total = 0;
  std::size_t selector_normalization_parameter_piece_entries_total = 0;
  std::size_t selector_normalization_pieceless_methods_total = 0;
  std::size_t selector_normalization_spelling_mismatches_total = 0;
  std::size_t selector_normalization_arity_mismatches_total = 0;
  std::size_t selector_normalization_parameter_linkage_mismatches_total = 0;
  std::size_t selector_normalization_flag_mismatches_total = 0;
  std::size_t selector_normalization_missing_keyword_pieces_total = 0;
  std::size_t property_attribute_properties_total = 0;
  std::size_t property_attribute_entries_total = 0;
  std::size_t property_attribute_readonly_modifiers_total = 0;
  std::size_t property_attribute_readwrite_modifiers_total = 0;
  std::size_t property_attribute_atomic_modifiers_total = 0;
  std::size_t property_attribute_nonatomic_modifiers_total = 0;
  std::size_t property_attribute_copy_modifiers_total = 0;
  std::size_t property_attribute_strong_modifiers_total = 0;
  std::size_t property_attribute_weak_modifiers_total = 0;
  std::size_t property_attribute_assign_modifiers_total = 0;
  std::size_t property_attribute_getter_modifiers_total = 0;
  std::size_t property_attribute_setter_modifiers_total = 0;
  std::size_t property_attribute_invalid_attribute_entries_total = 0;
  std::size_t property_attribute_contract_violations_total = 0;
  std::size_t type_annotation_generic_suffix_sites_total = 0;
  std::size_t type_annotation_pointer_declarator_sites_total = 0;
  std::size_t type_annotation_nullability_suffix_sites_total = 0;
  std::size_t type_annotation_ownership_qualifier_sites_total = 0;
  std::size_t type_annotation_object_pointer_type_sites_total = 0;
  std::size_t type_annotation_invalid_generic_suffix_sites_total = 0;
  std::size_t type_annotation_invalid_pointer_declarator_sites_total = 0;
  std::size_t type_annotation_invalid_nullability_suffix_sites_total = 0;
  std::size_t type_annotation_invalid_ownership_qualifier_sites_total = 0;
  std::size_t symbol_graph_global_symbol_nodes_total = 0;
  std::size_t symbol_graph_function_symbol_nodes_total = 0;
  std::size_t symbol_graph_interface_symbol_nodes_total = 0;
  std::size_t symbol_graph_implementation_symbol_nodes_total = 0;
  std::size_t symbol_graph_interface_property_symbol_nodes_total = 0;
  std::size_t symbol_graph_implementation_property_symbol_nodes_total = 0;
  std::size_t symbol_graph_interface_method_symbol_nodes_total = 0;
  std::size_t symbol_graph_implementation_method_symbol_nodes_total = 0;
  std::size_t symbol_graph_top_level_scope_symbols_total = 0;
  std::size_t symbol_graph_nested_scope_symbols_total = 0;
  std::size_t symbol_graph_scope_frames_total = 0;
  std::size_t symbol_graph_implementation_interface_resolution_sites_total = 0;
  std::size_t symbol_graph_implementation_interface_resolution_hits_total = 0;
  std::size_t symbol_graph_implementation_interface_resolution_misses_total = 0;
  std::size_t symbol_graph_method_resolution_sites_total = 0;
  std::size_t symbol_graph_method_resolution_hits_total = 0;
  std::size_t symbol_graph_method_resolution_misses_total = 0;
  std::size_t method_lookup_override_conflict_lookup_sites_total = 0;
  std::size_t method_lookup_override_conflict_lookup_hits_total = 0;
  std::size_t method_lookup_override_conflict_lookup_misses_total = 0;
  std::size_t method_lookup_override_conflict_override_sites_total = 0;
  std::size_t method_lookup_override_conflict_override_hits_total = 0;
  std::size_t method_lookup_override_conflict_override_misses_total = 0;
  std::size_t method_lookup_override_conflict_override_conflicts_total = 0;
  std::size_t method_lookup_override_conflict_unresolved_base_interfaces_total = 0;
  std::size_t property_synthesis_ivar_binding_property_synthesis_sites_total = 0;
  std::size_t property_synthesis_ivar_binding_explicit_ivar_bindings_total = 0;
  std::size_t property_synthesis_ivar_binding_default_ivar_bindings_total = 0;
  std::size_t property_synthesis_ivar_binding_ivar_binding_sites_total = 0;
  std::size_t property_synthesis_ivar_binding_ivar_binding_resolved_total = 0;
  std::size_t property_synthesis_ivar_binding_ivar_binding_missing_total = 0;
  std::size_t property_synthesis_ivar_binding_ivar_binding_conflicts_total = 0;
  std::size_t id_class_sel_object_pointer_param_type_sites_total = 0;
  std::size_t id_class_sel_object_pointer_param_id_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_param_class_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_param_sel_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_param_instancetype_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_param_object_pointer_type_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_type_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_id_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_class_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_sel_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_instancetype_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_return_object_pointer_type_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_type_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_id_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_class_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_sel_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_instancetype_spelling_sites_total = 0;
  std::size_t id_class_sel_object_pointer_property_object_pointer_type_sites_total = 0;
  std::size_t block_literal_capture_semantics_sites_total = 0;
  std::size_t block_literal_capture_semantics_parameter_entries_total = 0;
  std::size_t block_literal_capture_semantics_capture_entries_total = 0;
  std::size_t block_literal_capture_semantics_body_statement_entries_total = 0;
  std::size_t block_literal_capture_semantics_empty_capture_sites_total = 0;
  std::size_t block_literal_capture_semantics_nondeterministic_capture_sites_total = 0;
  std::size_t block_literal_capture_semantics_non_normalized_sites_total = 0;
  std::size_t block_literal_capture_semantics_contract_violation_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_invoke_argument_slots_total = 0;
  std::size_t block_abi_invoke_trampoline_capture_word_count_total = 0;
  std::size_t block_abi_invoke_trampoline_parameter_entries_total = 0;
  std::size_t block_abi_invoke_trampoline_capture_entries_total = 0;
  std::size_t block_abi_invoke_trampoline_body_statement_entries_total = 0;
  std::size_t block_abi_invoke_trampoline_descriptor_symbolized_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_invoke_symbolized_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_missing_invoke_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_non_normalized_layout_sites_total = 0;
  std::size_t block_abi_invoke_trampoline_contract_violation_sites_total = 0;
  std::size_t message_send_selector_lowering_sites_total = 0;
  std::size_t message_send_selector_lowering_unary_form_sites_total = 0;
  std::size_t message_send_selector_lowering_keyword_form_sites_total = 0;
  std::size_t message_send_selector_lowering_symbol_sites_total = 0;
  std::size_t message_send_selector_lowering_piece_entries_total = 0;
  std::size_t message_send_selector_lowering_argument_piece_entries_total = 0;
  std::size_t message_send_selector_lowering_normalized_sites_total = 0;
  std::size_t message_send_selector_lowering_form_mismatch_sites_total = 0;
  std::size_t message_send_selector_lowering_arity_mismatch_sites_total = 0;
  std::size_t message_send_selector_lowering_symbol_mismatch_sites_total = 0;
  std::size_t message_send_selector_lowering_missing_symbol_sites_total = 0;
  std::size_t message_send_selector_lowering_contract_violation_sites_total = 0;
  std::size_t dispatch_abi_marshalling_sites_total = 0;
  std::size_t dispatch_abi_marshalling_receiver_slots_total = 0;
  std::size_t dispatch_abi_marshalling_selector_symbol_slots_total = 0;
  std::size_t dispatch_abi_marshalling_argument_slots_total = 0;
  std::size_t dispatch_abi_marshalling_keyword_argument_slots_total = 0;
  std::size_t dispatch_abi_marshalling_unary_argument_slots_total = 0;
  std::size_t dispatch_abi_marshalling_arity_mismatch_sites_total = 0;
  std::size_t dispatch_abi_marshalling_missing_selector_symbol_sites_total = 0;
  std::size_t dispatch_abi_marshalling_contract_violation_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_receiver_nil_literal_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_enabled_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_foldable_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_non_nil_receiver_sites_total = 0;
  std::size_t nil_receiver_semantics_foldability_contract_violation_sites_total = 0;
  std::size_t super_dispatch_method_family_sites_total = 0;
  std::size_t super_dispatch_method_family_receiver_super_identifier_sites_total = 0;
  std::size_t super_dispatch_method_family_enabled_sites_total = 0;
  std::size_t super_dispatch_method_family_requires_class_context_sites_total = 0;
  std::size_t super_dispatch_method_family_init_sites_total = 0;
  std::size_t super_dispatch_method_family_copy_sites_total = 0;
  std::size_t super_dispatch_method_family_mutable_copy_sites_total = 0;
  std::size_t super_dispatch_method_family_new_sites_total = 0;
  std::size_t super_dispatch_method_family_none_sites_total = 0;
  std::size_t super_dispatch_method_family_returns_retained_result_sites_total = 0;
  std::size_t super_dispatch_method_family_returns_related_result_sites_total = 0;
  std::size_t super_dispatch_method_family_contract_violation_sites_total = 0;
  std::size_t runtime_shim_host_link_message_send_sites_total = 0;
  std::size_t runtime_shim_host_link_required_sites_total = 0;
  std::size_t runtime_shim_host_link_elided_sites_total = 0;
  std::size_t runtime_shim_host_link_runtime_dispatch_arg_slots_total = 0;
  std::size_t runtime_shim_host_link_runtime_dispatch_declaration_parameter_count_total = 0;
  std::size_t runtime_shim_host_link_contract_violation_sites_total = 0;
  std::string runtime_shim_host_link_runtime_dispatch_symbol;
  bool runtime_shim_host_link_default_runtime_dispatch_symbol_binding = true;
  std::size_t retain_release_operation_ownership_qualified_sites_total = 0;
  std::size_t retain_release_operation_retain_insertion_sites_total = 0;
  std::size_t retain_release_operation_release_insertion_sites_total = 0;
  std::size_t retain_release_operation_autorelease_insertion_sites_total = 0;
  std::size_t retain_release_operation_contract_violation_sites_total = 0;
  std::size_t weak_unowned_semantics_ownership_candidate_sites_total = 0;
  std::size_t weak_unowned_semantics_weak_reference_sites_total = 0;
  std::size_t weak_unowned_semantics_unowned_reference_sites_total = 0;
  std::size_t weak_unowned_semantics_unowned_safe_reference_sites_total = 0;
  std::size_t weak_unowned_semantics_conflict_sites_total = 0;
  std::size_t weak_unowned_semantics_contract_violation_sites_total = 0;
  std::size_t ownership_arc_diagnostic_candidate_sites_total = 0;
  std::size_t ownership_arc_fixit_available_sites_total = 0;
  std::size_t ownership_arc_profiled_sites_total = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites_total = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites_total = 0;
  std::size_t ownership_arc_contract_violation_sites_total = 0;
  std::size_t autoreleasepool_scope_sites_total = 0;
  std::size_t autoreleasepool_scope_symbolized_sites_total = 0;
  std::size_t autoreleasepool_scope_contract_violation_sites_total = 0;
  unsigned autoreleasepool_scope_max_depth_total = 0;
  bool diagnostics_after_pass_monotonic = false;
  bool deterministic_semantic_diagnostics = false;
  bool deterministic_type_metadata_handoff = false;
  bool deterministic_interface_implementation_handoff = false;
  bool deterministic_protocol_category_composition_handoff = false;
  bool deterministic_class_protocol_category_linking_handoff = false;
  bool deterministic_selector_normalization_handoff = false;
  bool deterministic_property_attribute_handoff = false;
  bool deterministic_type_annotation_surface_handoff = false;
  bool deterministic_symbol_graph_scope_resolution_handoff = false;
  bool deterministic_method_lookup_override_conflict_handoff = false;
  bool deterministic_property_synthesis_ivar_binding_handoff = false;
  bool deterministic_id_class_sel_object_pointer_type_checking_handoff = false;
  bool deterministic_block_literal_capture_semantics_handoff = false;
  bool deterministic_block_abi_invoke_trampoline_handoff = false;
  bool deterministic_message_send_selector_lowering_handoff = false;
  bool deterministic_dispatch_abi_marshalling_handoff = false;
  bool deterministic_nil_receiver_semantics_foldability_handoff = false;
  bool deterministic_super_dispatch_method_family_handoff = false;
  bool deterministic_runtime_shim_host_link_handoff = false;
  bool deterministic_retain_release_operation_handoff = false;
  bool deterministic_weak_unowned_semantics_handoff = false;
  bool deterministic_arc_diagnostics_fixit_handoff = false;
  bool deterministic_autoreleasepool_scope_handoff = false;
  Objc3InterfaceImplementationSummary interface_implementation_summary;
  Objc3ProtocolCategoryCompositionSummary protocol_category_composition_summary;
  Objc3ClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;
  Objc3SelectorNormalizationSummary selector_normalization_summary;
  Objc3PropertyAttributeSummary property_attribute_summary;
  Objc3TypeAnnotationSurfaceSummary type_annotation_surface_summary;
  Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;
  Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary;
  Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary;
  Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary;
  Objc3BlockLiteralCaptureSemanticsSummary block_literal_capture_semantics_summary;
  Objc3BlockAbiInvokeTrampolineSemanticsSummary block_abi_invoke_trampoline_semantics_summary;
  Objc3MessageSendSelectorLoweringSummary message_send_selector_lowering_summary;
  Objc3DispatchAbiMarshallingSummary dispatch_abi_marshalling_summary;
  Objc3NilReceiverSemanticsFoldabilitySummary nil_receiver_semantics_foldability_summary;
  Objc3SuperDispatchMethodFamilySummary super_dispatch_method_family_summary;
  Objc3RuntimeShimHostLinkSummary runtime_shim_host_link_summary;
  Objc3RetainReleaseOperationSummary retain_release_operation_summary;
  Objc3WeakUnownedSemanticsSummary weak_unowned_semantics_summary;
  Objc3ArcDiagnosticsFixitSummary arc_diagnostics_fixit_summary;
  Objc3AutoreleasePoolScopeSummary autoreleasepool_scope_summary;
  Objc3AtomicMemoryOrderMappingSummary atomic_memory_order_mapping;
  bool deterministic_atomic_memory_order_mapping = false;
  Objc3VectorTypeLoweringSummary vector_type_lowering;
  bool deterministic_vector_type_lowering = false;
  bool ready = false;
};

inline bool IsReadyObjc3SemaParityContractSurface(const Objc3SemaParityContractSurface &surface) {
  return surface.ready && surface.diagnostics_after_pass_monotonic && surface.deterministic_semantic_diagnostics &&
         surface.deterministic_type_metadata_handoff && surface.deterministic_atomic_memory_order_mapping &&
         surface.deterministic_vector_type_lowering &&
         surface.atomic_memory_order_mapping.deterministic &&
         surface.vector_type_lowering.deterministic &&
         surface.globals_total == surface.type_metadata_global_entries &&
         surface.functions_total == surface.type_metadata_function_entries &&
         surface.interfaces_total == surface.type_metadata_interface_entries &&
         surface.implementations_total == surface.type_metadata_implementation_entries &&
         surface.interface_implementation_summary.interface_method_symbols == surface.interface_method_symbols_total &&
         surface.interface_implementation_summary.implementation_method_symbols ==
             surface.implementation_method_symbols_total &&
         surface.interface_implementation_summary.linked_implementation_symbols ==
             surface.linked_implementation_symbols_total &&
         surface.interface_implementation_summary.deterministic &&
         surface.deterministic_interface_implementation_handoff &&
         surface.protocol_category_composition_summary.protocol_composition_sites ==
             surface.protocol_composition_sites_total &&
         surface.protocol_category_composition_summary.protocol_composition_symbols ==
             surface.protocol_composition_symbols_total &&
         surface.protocol_category_composition_summary.category_composition_sites ==
             surface.category_composition_sites_total &&
         surface.protocol_category_composition_summary.category_composition_symbols ==
             surface.category_composition_symbols_total &&
         surface.protocol_category_composition_summary.invalid_protocol_composition_sites ==
             surface.invalid_protocol_composition_sites_total &&
         surface.protocol_category_composition_summary.invalid_protocol_composition_sites <=
             surface.protocol_category_composition_summary.total_composition_sites() &&
         surface.protocol_category_composition_summary.deterministic &&
         surface.deterministic_protocol_category_composition_handoff &&
         surface.class_protocol_category_linking_summary.declared_interfaces ==
             surface.interface_implementation_summary.declared_interfaces &&
         surface.class_protocol_category_linking_summary.resolved_interfaces ==
             surface.interface_implementation_summary.resolved_interfaces &&
         surface.class_protocol_category_linking_summary.declared_implementations ==
             surface.interface_implementation_summary.declared_implementations &&
         surface.class_protocol_category_linking_summary.resolved_implementations ==
             surface.interface_implementation_summary.resolved_implementations &&
         surface.class_protocol_category_linking_summary.interface_method_symbols ==
             surface.interface_method_symbols_total &&
         surface.class_protocol_category_linking_summary.implementation_method_symbols ==
             surface.implementation_method_symbols_total &&
         surface.class_protocol_category_linking_summary.linked_implementation_symbols ==
             surface.linked_implementation_symbols_total &&
         surface.class_protocol_category_linking_summary.protocol_composition_sites ==
             surface.protocol_composition_sites_total &&
         surface.class_protocol_category_linking_summary.protocol_composition_symbols ==
             surface.protocol_composition_symbols_total &&
         surface.class_protocol_category_linking_summary.category_composition_sites ==
             surface.category_composition_sites_total &&
         surface.class_protocol_category_linking_summary.category_composition_symbols ==
             surface.category_composition_symbols_total &&
         surface.class_protocol_category_linking_summary.invalid_protocol_composition_sites ==
             surface.invalid_protocol_composition_sites_total &&
         surface.class_protocol_category_linking_summary.invalid_protocol_composition_sites <=
             surface.class_protocol_category_linking_summary.total_composition_sites() &&
         surface.class_protocol_category_linking_summary.deterministic &&
         surface.deterministic_class_protocol_category_linking_handoff &&
         surface.selector_normalization_summary.methods_total == surface.selector_normalization_methods_total &&
         surface.selector_normalization_summary.normalized_methods ==
             surface.selector_normalization_normalized_methods_total &&
         surface.selector_normalization_summary.selector_piece_entries ==
             surface.selector_normalization_piece_entries_total &&
         surface.selector_normalization_summary.selector_parameter_piece_entries ==
             surface.selector_normalization_parameter_piece_entries_total &&
         surface.selector_normalization_summary.selector_pieceless_methods ==
             surface.selector_normalization_pieceless_methods_total &&
         surface.selector_normalization_summary.selector_spelling_mismatches ==
             surface.selector_normalization_spelling_mismatches_total &&
         surface.selector_normalization_summary.selector_arity_mismatches ==
             surface.selector_normalization_arity_mismatches_total &&
         surface.selector_normalization_summary.selector_parameter_linkage_mismatches ==
             surface.selector_normalization_parameter_linkage_mismatches_total &&
         surface.selector_normalization_summary.selector_normalization_flag_mismatches ==
             surface.selector_normalization_flag_mismatches_total &&
         surface.selector_normalization_summary.selector_missing_keyword_pieces ==
             surface.selector_normalization_missing_keyword_pieces_total &&
         surface.selector_normalization_summary.selector_parameter_piece_entries <=
             surface.selector_normalization_summary.selector_piece_entries &&
         surface.selector_normalization_summary.normalized_methods <= surface.selector_normalization_summary.methods_total &&
         surface.selector_normalization_summary.contract_violations() <=
             surface.selector_normalization_summary.methods_total &&
         surface.selector_normalization_summary.deterministic &&
         surface.deterministic_selector_normalization_handoff &&
         surface.property_attribute_summary.properties_total == surface.property_attribute_properties_total &&
         surface.property_attribute_summary.attribute_entries == surface.property_attribute_entries_total &&
         surface.property_attribute_summary.readonly_modifiers == surface.property_attribute_readonly_modifiers_total &&
         surface.property_attribute_summary.readwrite_modifiers == surface.property_attribute_readwrite_modifiers_total &&
         surface.property_attribute_summary.atomic_modifiers == surface.property_attribute_atomic_modifiers_total &&
         surface.property_attribute_summary.nonatomic_modifiers == surface.property_attribute_nonatomic_modifiers_total &&
         surface.property_attribute_summary.copy_modifiers == surface.property_attribute_copy_modifiers_total &&
         surface.property_attribute_summary.strong_modifiers == surface.property_attribute_strong_modifiers_total &&
         surface.property_attribute_summary.weak_modifiers == surface.property_attribute_weak_modifiers_total &&
         surface.property_attribute_summary.assign_modifiers == surface.property_attribute_assign_modifiers_total &&
         surface.property_attribute_summary.getter_modifiers == surface.property_attribute_getter_modifiers_total &&
         surface.property_attribute_summary.setter_modifiers == surface.property_attribute_setter_modifiers_total &&
         surface.property_attribute_summary.invalid_attribute_entries ==
             surface.property_attribute_invalid_attribute_entries_total &&
         surface.property_attribute_summary.property_contract_violations ==
             surface.property_attribute_contract_violations_total &&
         surface.property_attribute_summary.getter_modifiers <= surface.property_attribute_summary.properties_total &&
         surface.property_attribute_summary.setter_modifiers <= surface.property_attribute_summary.properties_total &&
         surface.property_attribute_summary.deterministic &&
         surface.deterministic_property_attribute_handoff &&
         surface.type_annotation_surface_summary.generic_suffix_sites == surface.type_annotation_generic_suffix_sites_total &&
         surface.type_annotation_surface_summary.pointer_declarator_sites ==
             surface.type_annotation_pointer_declarator_sites_total &&
         surface.type_annotation_surface_summary.nullability_suffix_sites ==
             surface.type_annotation_nullability_suffix_sites_total &&
         surface.type_annotation_surface_summary.ownership_qualifier_sites ==
             surface.type_annotation_ownership_qualifier_sites_total &&
         surface.type_annotation_surface_summary.object_pointer_type_sites ==
             surface.type_annotation_object_pointer_type_sites_total &&
         surface.type_annotation_surface_summary.invalid_generic_suffix_sites ==
             surface.type_annotation_invalid_generic_suffix_sites_total &&
         surface.type_annotation_surface_summary.invalid_pointer_declarator_sites ==
             surface.type_annotation_invalid_pointer_declarator_sites_total &&
         surface.type_annotation_surface_summary.invalid_nullability_suffix_sites ==
             surface.type_annotation_invalid_nullability_suffix_sites_total &&
         surface.type_annotation_surface_summary.invalid_ownership_qualifier_sites ==
             surface.type_annotation_invalid_ownership_qualifier_sites_total &&
         surface.type_annotation_surface_summary.invalid_generic_suffix_sites <=
             surface.type_annotation_surface_summary.generic_suffix_sites &&
         surface.type_annotation_surface_summary.invalid_pointer_declarator_sites <=
             surface.type_annotation_surface_summary.pointer_declarator_sites &&
         surface.type_annotation_surface_summary.invalid_nullability_suffix_sites <=
             surface.type_annotation_surface_summary.nullability_suffix_sites &&
         surface.type_annotation_surface_summary.invalid_ownership_qualifier_sites <=
             surface.type_annotation_surface_summary.ownership_qualifier_sites &&
         surface.type_annotation_surface_summary.invalid_type_annotation_sites() <=
             surface.type_annotation_surface_summary.total_type_annotation_sites() &&
         surface.type_annotation_surface_summary.deterministic &&
         surface.deterministic_type_annotation_surface_handoff &&
         surface.symbol_graph_scope_resolution_summary.global_symbol_nodes ==
             surface.symbol_graph_global_symbol_nodes_total &&
         surface.symbol_graph_scope_resolution_summary.function_symbol_nodes ==
             surface.symbol_graph_function_symbol_nodes_total &&
         surface.symbol_graph_scope_resolution_summary.interface_symbol_nodes ==
             surface.symbol_graph_interface_symbol_nodes_total &&
         surface.symbol_graph_scope_resolution_summary.implementation_symbol_nodes ==
             surface.symbol_graph_implementation_symbol_nodes_total &&
         surface.symbol_graph_scope_resolution_summary.interface_property_symbol_nodes ==
             surface.symbol_graph_interface_property_symbol_nodes_total &&
         surface.symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes ==
             surface.symbol_graph_implementation_property_symbol_nodes_total &&
         surface.symbol_graph_scope_resolution_summary.interface_method_symbol_nodes ==
             surface.symbol_graph_interface_method_symbol_nodes_total &&
         surface.symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes ==
             surface.symbol_graph_implementation_method_symbol_nodes_total &&
         surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols ==
             surface.symbol_graph_top_level_scope_symbols_total &&
         surface.symbol_graph_scope_resolution_summary.nested_scope_symbols ==
             surface.symbol_graph_nested_scope_symbols_total &&
         surface.symbol_graph_scope_resolution_summary.scope_frames_total == surface.symbol_graph_scope_frames_total &&
         surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites ==
             surface.symbol_graph_implementation_interface_resolution_sites_total &&
         surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits ==
             surface.symbol_graph_implementation_interface_resolution_hits_total &&
         surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses ==
             surface.symbol_graph_implementation_interface_resolution_misses_total &&
         surface.symbol_graph_scope_resolution_summary.method_resolution_sites ==
             surface.symbol_graph_method_resolution_sites_total &&
         surface.symbol_graph_scope_resolution_summary.method_resolution_hits ==
             surface.symbol_graph_method_resolution_hits_total &&
         surface.symbol_graph_scope_resolution_summary.method_resolution_misses ==
             surface.symbol_graph_method_resolution_misses_total &&
         surface.symbol_graph_scope_resolution_summary.symbol_nodes_total() ==
             surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols +
                 surface.symbol_graph_scope_resolution_summary.nested_scope_symbols &&
         surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits <=
             surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
         surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits +
                 surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses ==
             surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
         surface.symbol_graph_scope_resolution_summary.method_resolution_hits <=
             surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
         surface.symbol_graph_scope_resolution_summary.method_resolution_hits +
                 surface.symbol_graph_scope_resolution_summary.method_resolution_misses ==
             surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
         surface.symbol_graph_scope_resolution_summary.resolution_hits_total() <=
             surface.symbol_graph_scope_resolution_summary.resolution_sites_total() &&
         surface.symbol_graph_scope_resolution_summary.resolution_hits_total() +
                 surface.symbol_graph_scope_resolution_summary.resolution_misses_total() ==
             surface.symbol_graph_scope_resolution_summary.resolution_sites_total() &&
         surface.symbol_graph_scope_resolution_summary.deterministic &&
         surface.deterministic_symbol_graph_scope_resolution_handoff &&
         surface.method_lookup_override_conflict_summary.method_lookup_sites ==
             surface.method_lookup_override_conflict_lookup_sites_total &&
         surface.method_lookup_override_conflict_summary.method_lookup_hits ==
             surface.method_lookup_override_conflict_lookup_hits_total &&
         surface.method_lookup_override_conflict_summary.method_lookup_misses ==
             surface.method_lookup_override_conflict_lookup_misses_total &&
         surface.method_lookup_override_conflict_summary.override_lookup_sites ==
             surface.method_lookup_override_conflict_override_sites_total &&
         surface.method_lookup_override_conflict_summary.override_lookup_hits ==
             surface.method_lookup_override_conflict_override_hits_total &&
         surface.method_lookup_override_conflict_summary.override_lookup_misses ==
             surface.method_lookup_override_conflict_override_misses_total &&
         surface.method_lookup_override_conflict_summary.override_conflicts ==
             surface.method_lookup_override_conflict_override_conflicts_total &&
         surface.method_lookup_override_conflict_summary.unresolved_base_interfaces ==
             surface.method_lookup_override_conflict_unresolved_base_interfaces_total &&
         surface.method_lookup_override_conflict_summary.method_lookup_hits <=
             surface.method_lookup_override_conflict_summary.method_lookup_sites &&
         surface.method_lookup_override_conflict_summary.method_lookup_hits +
                 surface.method_lookup_override_conflict_summary.method_lookup_misses ==
             surface.method_lookup_override_conflict_summary.method_lookup_sites &&
         surface.method_lookup_override_conflict_summary.override_lookup_hits <=
             surface.method_lookup_override_conflict_summary.override_lookup_sites &&
         surface.method_lookup_override_conflict_summary.override_lookup_hits +
                 surface.method_lookup_override_conflict_summary.override_lookup_misses ==
             surface.method_lookup_override_conflict_summary.override_lookup_sites &&
         surface.method_lookup_override_conflict_summary.override_conflicts <=
             surface.method_lookup_override_conflict_summary.override_lookup_hits &&
         surface.method_lookup_override_conflict_summary.deterministic &&
         surface.deterministic_method_lookup_override_conflict_handoff &&
         surface.property_synthesis_ivar_binding_summary.property_synthesis_sites ==
             surface.property_synthesis_ivar_binding_property_synthesis_sites_total &&
         surface.property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings ==
             surface.property_synthesis_ivar_binding_explicit_ivar_bindings_total &&
         surface.property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings ==
             surface.property_synthesis_ivar_binding_default_ivar_bindings_total &&
         surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
             surface.property_synthesis_ivar_binding_ivar_binding_sites_total &&
         surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved ==
             surface.property_synthesis_ivar_binding_ivar_binding_resolved_total &&
         surface.property_synthesis_ivar_binding_summary.ivar_binding_missing ==
             surface.property_synthesis_ivar_binding_ivar_binding_missing_total &&
         surface.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
             surface.property_synthesis_ivar_binding_ivar_binding_conflicts_total &&
         surface.property_synthesis_ivar_binding_summary.property_synthesis_explicit_ivar_bindings +
                 surface.property_synthesis_ivar_binding_summary.property_synthesis_default_ivar_bindings ==
             surface.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
         surface.property_synthesis_ivar_binding_summary.ivar_binding_sites ==
             surface.property_synthesis_ivar_binding_summary.property_synthesis_sites &&
         surface.property_synthesis_ivar_binding_summary.ivar_binding_resolved +
                 surface.property_synthesis_ivar_binding_summary.ivar_binding_missing +
                 surface.property_synthesis_ivar_binding_summary.ivar_binding_conflicts ==
             surface.property_synthesis_ivar_binding_summary.ivar_binding_sites &&
         surface.property_synthesis_ivar_binding_summary.deterministic &&
         surface.deterministic_property_synthesis_ivar_binding_handoff &&
         surface.id_class_sel_object_pointer_type_checking_summary.param_type_sites ==
             surface.id_class_sel_object_pointer_param_type_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites ==
             surface.id_class_sel_object_pointer_param_id_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites ==
             surface.id_class_sel_object_pointer_param_class_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites ==
             surface.id_class_sel_object_pointer_param_sel_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.param_instancetype_spelling_sites ==
             surface.id_class_sel_object_pointer_param_instancetype_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites ==
             surface.id_class_sel_object_pointer_param_object_pointer_type_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.return_type_sites ==
             surface.id_class_sel_object_pointer_return_type_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites ==
             surface.id_class_sel_object_pointer_return_id_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites ==
             surface.id_class_sel_object_pointer_return_class_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites ==
             surface.id_class_sel_object_pointer_return_sel_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.return_instancetype_spelling_sites ==
             surface.id_class_sel_object_pointer_return_instancetype_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.return_object_pointer_type_sites ==
             surface.id_class_sel_object_pointer_return_object_pointer_type_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.property_type_sites ==
             surface.id_class_sel_object_pointer_property_type_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites ==
             surface.id_class_sel_object_pointer_property_id_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.property_class_spelling_sites ==
             surface.id_class_sel_object_pointer_property_class_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites ==
             surface.id_class_sel_object_pointer_property_sel_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.property_instancetype_spelling_sites ==
             surface.id_class_sel_object_pointer_property_instancetype_spelling_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.property_object_pointer_type_sites ==
             surface.id_class_sel_object_pointer_property_object_pointer_type_sites_total &&
         surface.id_class_sel_object_pointer_type_checking_summary.param_id_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.param_class_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.param_sel_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.param_instancetype_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.param_object_pointer_type_sites <=
             surface.id_class_sel_object_pointer_type_checking_summary.param_type_sites &&
         surface.id_class_sel_object_pointer_type_checking_summary.return_id_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.return_class_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.return_sel_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.return_instancetype_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.return_object_pointer_type_sites <=
             surface.id_class_sel_object_pointer_type_checking_summary.return_type_sites &&
         surface.id_class_sel_object_pointer_type_checking_summary.property_id_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.property_class_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.property_sel_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.property_instancetype_spelling_sites +
                 surface.id_class_sel_object_pointer_type_checking_summary.property_object_pointer_type_sites <=
             surface.id_class_sel_object_pointer_type_checking_summary.property_type_sites &&
         surface.id_class_sel_object_pointer_type_checking_summary.deterministic &&
         surface.deterministic_id_class_sel_object_pointer_type_checking_handoff &&
         surface.block_literal_capture_semantics_summary.block_literal_sites ==
             surface.block_literal_capture_semantics_sites_total &&
         surface.block_literal_capture_semantics_summary.block_parameter_entries ==
             surface.block_literal_capture_semantics_parameter_entries_total &&
         surface.block_literal_capture_semantics_summary.block_capture_entries ==
             surface.block_literal_capture_semantics_capture_entries_total &&
         surface.block_literal_capture_semantics_summary.block_body_statement_entries ==
             surface.block_literal_capture_semantics_body_statement_entries_total &&
         surface.block_literal_capture_semantics_summary.block_empty_capture_sites ==
             surface.block_literal_capture_semantics_empty_capture_sites_total &&
         surface.block_literal_capture_semantics_summary.block_nondeterministic_capture_sites ==
             surface.block_literal_capture_semantics_nondeterministic_capture_sites_total &&
         surface.block_literal_capture_semantics_summary.block_non_normalized_sites ==
             surface.block_literal_capture_semantics_non_normalized_sites_total &&
         surface.block_literal_capture_semantics_summary.contract_violation_sites ==
             surface.block_literal_capture_semantics_contract_violation_sites_total &&
         surface.block_literal_capture_semantics_summary.block_empty_capture_sites <=
             surface.block_literal_capture_semantics_summary.block_literal_sites &&
         surface.block_literal_capture_semantics_summary.block_nondeterministic_capture_sites <=
             surface.block_literal_capture_semantics_summary.block_literal_sites &&
         surface.block_literal_capture_semantics_summary.block_non_normalized_sites <=
             surface.block_literal_capture_semantics_summary.block_literal_sites &&
         surface.block_literal_capture_semantics_summary.contract_violation_sites <=
             surface.block_literal_capture_semantics_summary.block_literal_sites &&
         surface.block_literal_capture_semantics_summary.deterministic &&
         surface.deterministic_block_literal_capture_semantics_handoff &&
         surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites ==
             surface.block_abi_invoke_trampoline_sites_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.invoke_argument_slots_total ==
             surface.block_abi_invoke_trampoline_invoke_argument_slots_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.capture_word_count_total ==
             surface.block_abi_invoke_trampoline_capture_word_count_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.parameter_entries_total ==
             surface.block_abi_invoke_trampoline_parameter_entries_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.capture_entries_total ==
             surface.block_abi_invoke_trampoline_capture_entries_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.body_statement_entries_total ==
             surface.block_abi_invoke_trampoline_body_statement_entries_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.descriptor_symbolized_sites ==
             surface.block_abi_invoke_trampoline_descriptor_symbolized_sites_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites ==
             surface.block_abi_invoke_trampoline_invoke_symbolized_sites_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites ==
             surface.block_abi_invoke_trampoline_missing_invoke_sites_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.non_normalized_layout_sites ==
             surface.block_abi_invoke_trampoline_non_normalized_layout_sites_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.contract_violation_sites ==
             surface.block_abi_invoke_trampoline_contract_violation_sites_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.descriptor_symbolized_sites <=
             surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         surface.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites <=
             surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         surface.block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites <=
             surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         surface.block_abi_invoke_trampoline_semantics_summary.non_normalized_layout_sites <=
             surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         surface.block_abi_invoke_trampoline_semantics_summary.contract_violation_sites <=
             surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         surface.block_abi_invoke_trampoline_semantics_summary.invoke_trampoline_symbolized_sites +
                 surface.block_abi_invoke_trampoline_semantics_summary.missing_invoke_trampoline_sites ==
             surface.block_abi_invoke_trampoline_semantics_summary.block_literal_sites &&
         surface.block_abi_invoke_trampoline_semantics_summary.invoke_argument_slots_total ==
             surface.block_abi_invoke_trampoline_semantics_summary.parameter_entries_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.capture_word_count_total ==
             surface.block_abi_invoke_trampoline_semantics_summary.capture_entries_total &&
         surface.block_abi_invoke_trampoline_semantics_summary.deterministic &&
         surface.deterministic_block_abi_invoke_trampoline_handoff &&
         surface.message_send_selector_lowering_summary.message_send_sites ==
             surface.message_send_selector_lowering_sites_total &&
         surface.message_send_selector_lowering_summary.unary_form_sites ==
             surface.message_send_selector_lowering_unary_form_sites_total &&
         surface.message_send_selector_lowering_summary.keyword_form_sites ==
             surface.message_send_selector_lowering_keyword_form_sites_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_symbol_sites ==
             surface.message_send_selector_lowering_symbol_sites_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_piece_entries ==
             surface.message_send_selector_lowering_piece_entries_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries ==
             surface.message_send_selector_lowering_argument_piece_entries_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_normalized_sites ==
             surface.message_send_selector_lowering_normalized_sites_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites ==
             surface.message_send_selector_lowering_form_mismatch_sites_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites ==
             surface.message_send_selector_lowering_arity_mismatch_sites_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites ==
             surface.message_send_selector_lowering_symbol_mismatch_sites_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites ==
             surface.message_send_selector_lowering_missing_symbol_sites_total &&
         surface.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites ==
             surface.message_send_selector_lowering_contract_violation_sites_total &&
         surface.message_send_selector_lowering_summary.unary_form_sites +
                 surface.message_send_selector_lowering_summary.keyword_form_sites ==
             surface.message_send_selector_lowering_summary.message_send_sites &&
         surface.message_send_selector_lowering_summary.selector_lowering_symbol_sites <=
             surface.message_send_selector_lowering_summary.message_send_sites &&
         surface.message_send_selector_lowering_summary.selector_lowering_piece_entries >=
             surface.message_send_selector_lowering_summary.selector_lowering_argument_piece_entries &&
         surface.message_send_selector_lowering_summary.selector_lowering_normalized_sites <=
             surface.message_send_selector_lowering_summary.selector_lowering_symbol_sites &&
         surface.message_send_selector_lowering_summary.selector_lowering_form_mismatch_sites <=
             surface.message_send_selector_lowering_summary.message_send_sites &&
         surface.message_send_selector_lowering_summary.selector_lowering_arity_mismatch_sites <=
             surface.message_send_selector_lowering_summary.message_send_sites &&
         surface.message_send_selector_lowering_summary.selector_lowering_symbol_mismatch_sites <=
             surface.message_send_selector_lowering_summary.message_send_sites &&
         surface.message_send_selector_lowering_summary.selector_lowering_missing_symbol_sites <=
             surface.message_send_selector_lowering_summary.message_send_sites &&
         surface.message_send_selector_lowering_summary.selector_lowering_contract_violation_sites <=
             surface.message_send_selector_lowering_summary.message_send_sites &&
         surface.message_send_selector_lowering_summary.deterministic &&
         surface.deterministic_message_send_selector_lowering_handoff &&
         surface.dispatch_abi_marshalling_summary.message_send_sites ==
             surface.dispatch_abi_marshalling_sites_total &&
         surface.dispatch_abi_marshalling_summary.receiver_slots ==
             surface.dispatch_abi_marshalling_receiver_slots_total &&
         surface.dispatch_abi_marshalling_summary.selector_symbol_slots ==
             surface.dispatch_abi_marshalling_selector_symbol_slots_total &&
         surface.dispatch_abi_marshalling_summary.argument_slots ==
             surface.dispatch_abi_marshalling_argument_slots_total &&
         surface.dispatch_abi_marshalling_summary.keyword_argument_slots ==
             surface.dispatch_abi_marshalling_keyword_argument_slots_total &&
         surface.dispatch_abi_marshalling_summary.unary_argument_slots ==
             surface.dispatch_abi_marshalling_unary_argument_slots_total &&
         surface.dispatch_abi_marshalling_summary.arity_mismatch_sites ==
             surface.dispatch_abi_marshalling_arity_mismatch_sites_total &&
         surface.dispatch_abi_marshalling_summary.missing_selector_symbol_sites ==
             surface.dispatch_abi_marshalling_missing_selector_symbol_sites_total &&
         surface.dispatch_abi_marshalling_summary.contract_violation_sites ==
             surface.dispatch_abi_marshalling_contract_violation_sites_total &&
         surface.dispatch_abi_marshalling_summary.receiver_slots ==
             surface.dispatch_abi_marshalling_summary.message_send_sites &&
         surface.dispatch_abi_marshalling_summary.selector_symbol_slots +
                 surface.dispatch_abi_marshalling_summary.missing_selector_symbol_sites ==
             surface.dispatch_abi_marshalling_summary.message_send_sites &&
         surface.dispatch_abi_marshalling_summary.keyword_argument_slots +
                 surface.dispatch_abi_marshalling_summary.unary_argument_slots ==
             surface.dispatch_abi_marshalling_summary.argument_slots &&
         surface.dispatch_abi_marshalling_summary.keyword_argument_slots <=
             surface.dispatch_abi_marshalling_summary.argument_slots &&
         surface.dispatch_abi_marshalling_summary.unary_argument_slots <=
             surface.dispatch_abi_marshalling_summary.argument_slots &&
         surface.dispatch_abi_marshalling_summary.selector_symbol_slots <=
             surface.dispatch_abi_marshalling_summary.message_send_sites &&
         surface.dispatch_abi_marshalling_summary.missing_selector_symbol_sites <=
             surface.dispatch_abi_marshalling_summary.message_send_sites &&
         surface.dispatch_abi_marshalling_summary.arity_mismatch_sites <=
             surface.dispatch_abi_marshalling_summary.message_send_sites &&
         surface.dispatch_abi_marshalling_summary.contract_violation_sites <=
             surface.dispatch_abi_marshalling_summary.message_send_sites &&
         surface.dispatch_abi_marshalling_summary.deterministic &&
         surface.deterministic_dispatch_abi_marshalling_handoff &&
         surface.nil_receiver_semantics_foldability_summary.message_send_sites ==
             surface.nil_receiver_semantics_foldability_sites_total &&
         surface.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites ==
             surface.nil_receiver_semantics_foldability_receiver_nil_literal_sites_total &&
         surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites ==
             surface.nil_receiver_semantics_foldability_enabled_sites_total &&
         surface.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites ==
             surface.nil_receiver_semantics_foldability_foldable_sites_total &&
         surface.nil_receiver_semantics_foldability_summary.nil_receiver_runtime_dispatch_required_sites ==
             surface.nil_receiver_semantics_foldability_runtime_dispatch_required_sites_total &&
         surface.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites ==
             surface.nil_receiver_semantics_foldability_non_nil_receiver_sites_total &&
         surface.nil_receiver_semantics_foldability_summary.contract_violation_sites ==
             surface.nil_receiver_semantics_foldability_contract_violation_sites_total &&
         surface.nil_receiver_semantics_foldability_summary.receiver_nil_literal_sites ==
             surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
         surface.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites <=
             surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites &&
         surface.nil_receiver_semantics_foldability_summary.nil_receiver_runtime_dispatch_required_sites +
                 surface.nil_receiver_semantics_foldability_summary.nil_receiver_foldable_sites ==
             surface.nil_receiver_semantics_foldability_summary.message_send_sites &&
         surface.nil_receiver_semantics_foldability_summary.nil_receiver_semantics_enabled_sites +
                 surface.nil_receiver_semantics_foldability_summary.non_nil_receiver_sites ==
             surface.nil_receiver_semantics_foldability_summary.message_send_sites &&
         surface.nil_receiver_semantics_foldability_summary.contract_violation_sites <=
             surface.nil_receiver_semantics_foldability_summary.message_send_sites &&
         surface.nil_receiver_semantics_foldability_summary.deterministic &&
         surface.deterministic_nil_receiver_semantics_foldability_handoff &&
         surface.super_dispatch_method_family_summary.message_send_sites ==
             surface.super_dispatch_method_family_sites_total &&
         surface.super_dispatch_method_family_summary.receiver_super_identifier_sites ==
             surface.super_dispatch_method_family_receiver_super_identifier_sites_total &&
         surface.super_dispatch_method_family_summary.super_dispatch_enabled_sites ==
             surface.super_dispatch_method_family_enabled_sites_total &&
         surface.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites ==
             surface.super_dispatch_method_family_requires_class_context_sites_total &&
         surface.super_dispatch_method_family_summary.method_family_init_sites ==
             surface.super_dispatch_method_family_init_sites_total &&
         surface.super_dispatch_method_family_summary.method_family_copy_sites ==
             surface.super_dispatch_method_family_copy_sites_total &&
         surface.super_dispatch_method_family_summary.method_family_mutable_copy_sites ==
             surface.super_dispatch_method_family_mutable_copy_sites_total &&
         surface.super_dispatch_method_family_summary.method_family_new_sites ==
             surface.super_dispatch_method_family_new_sites_total &&
         surface.super_dispatch_method_family_summary.method_family_none_sites ==
             surface.super_dispatch_method_family_none_sites_total &&
         surface.super_dispatch_method_family_summary.method_family_returns_retained_result_sites ==
             surface.super_dispatch_method_family_returns_retained_result_sites_total &&
         surface.super_dispatch_method_family_summary.method_family_returns_related_result_sites ==
             surface.super_dispatch_method_family_returns_related_result_sites_total &&
         surface.super_dispatch_method_family_summary.contract_violation_sites ==
             surface.super_dispatch_method_family_contract_violation_sites_total &&
         surface.super_dispatch_method_family_summary.receiver_super_identifier_sites ==
             surface.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
         surface.super_dispatch_method_family_summary.super_dispatch_requires_class_context_sites ==
             surface.super_dispatch_method_family_summary.super_dispatch_enabled_sites &&
         surface.super_dispatch_method_family_summary.method_family_init_sites +
                 surface.super_dispatch_method_family_summary.method_family_copy_sites +
                 surface.super_dispatch_method_family_summary.method_family_mutable_copy_sites +
                 surface.super_dispatch_method_family_summary.method_family_new_sites +
                 surface.super_dispatch_method_family_summary.method_family_none_sites ==
             surface.super_dispatch_method_family_summary.message_send_sites &&
         surface.super_dispatch_method_family_summary.method_family_returns_related_result_sites <=
             surface.super_dispatch_method_family_summary.method_family_init_sites &&
         surface.super_dispatch_method_family_summary.method_family_returns_retained_result_sites <=
             surface.super_dispatch_method_family_summary.message_send_sites &&
         surface.super_dispatch_method_family_summary.contract_violation_sites <=
             surface.super_dispatch_method_family_summary.message_send_sites &&
         surface.super_dispatch_method_family_summary.deterministic &&
         surface.deterministic_super_dispatch_method_family_handoff &&
         surface.runtime_shim_host_link_summary.message_send_sites ==
             surface.runtime_shim_host_link_message_send_sites_total &&
         surface.runtime_shim_host_link_summary.runtime_shim_required_sites ==
             surface.runtime_shim_host_link_required_sites_total &&
         surface.runtime_shim_host_link_summary.runtime_shim_elided_sites ==
             surface.runtime_shim_host_link_elided_sites_total &&
         surface.runtime_shim_host_link_summary.runtime_dispatch_arg_slots ==
             surface.runtime_shim_host_link_runtime_dispatch_arg_slots_total &&
         surface.runtime_shim_host_link_summary.runtime_dispatch_declaration_parameter_count ==
             surface.runtime_shim_host_link_runtime_dispatch_declaration_parameter_count_total &&
         surface.runtime_shim_host_link_summary.contract_violation_sites ==
             surface.runtime_shim_host_link_contract_violation_sites_total &&
         surface.runtime_shim_host_link_summary.runtime_dispatch_symbol ==
             surface.runtime_shim_host_link_runtime_dispatch_symbol &&
         surface.runtime_shim_host_link_summary.default_runtime_dispatch_symbol_binding ==
             surface.runtime_shim_host_link_default_runtime_dispatch_symbol_binding &&
         surface.runtime_shim_host_link_summary.runtime_shim_required_sites +
                 surface.runtime_shim_host_link_summary.runtime_shim_elided_sites ==
             surface.runtime_shim_host_link_summary.message_send_sites &&
         surface.runtime_shim_host_link_summary.contract_violation_sites <=
             surface.runtime_shim_host_link_summary.message_send_sites &&
         (surface.runtime_shim_host_link_summary.message_send_sites == 0 ||
          surface.runtime_shim_host_link_summary.runtime_dispatch_declaration_parameter_count ==
              surface.runtime_shim_host_link_summary.runtime_dispatch_arg_slots + 2u) &&
         (surface.runtime_shim_host_link_summary.default_runtime_dispatch_symbol_binding ==
          (surface.runtime_shim_host_link_summary.runtime_dispatch_symbol ==
           kObjc3RuntimeShimHostLinkDefaultDispatchSymbol)) &&
         surface.runtime_shim_host_link_summary.deterministic &&
         surface.deterministic_runtime_shim_host_link_handoff &&
         surface.retain_release_operation_summary.ownership_qualified_sites ==
             surface.retain_release_operation_ownership_qualified_sites_total &&
         surface.retain_release_operation_summary.retain_insertion_sites ==
             surface.retain_release_operation_retain_insertion_sites_total &&
         surface.retain_release_operation_summary.release_insertion_sites ==
             surface.retain_release_operation_release_insertion_sites_total &&
         surface.retain_release_operation_summary.autorelease_insertion_sites ==
             surface.retain_release_operation_autorelease_insertion_sites_total &&
         surface.retain_release_operation_summary.contract_violation_sites ==
             surface.retain_release_operation_contract_violation_sites_total &&
         surface.retain_release_operation_summary.retain_insertion_sites <=
             surface.retain_release_operation_summary.ownership_qualified_sites +
                 surface.retain_release_operation_summary.contract_violation_sites &&
         surface.retain_release_operation_summary.release_insertion_sites <=
             surface.retain_release_operation_summary.ownership_qualified_sites +
                 surface.retain_release_operation_summary.contract_violation_sites &&
         surface.retain_release_operation_summary.autorelease_insertion_sites <=
             surface.retain_release_operation_summary.ownership_qualified_sites +
                 surface.retain_release_operation_summary.contract_violation_sites &&
         surface.retain_release_operation_summary.deterministic &&
         surface.deterministic_retain_release_operation_handoff &&
         surface.weak_unowned_semantics_summary.ownership_candidate_sites ==
             surface.weak_unowned_semantics_ownership_candidate_sites_total &&
         surface.weak_unowned_semantics_summary.weak_reference_sites ==
             surface.weak_unowned_semantics_weak_reference_sites_total &&
         surface.weak_unowned_semantics_summary.unowned_reference_sites ==
             surface.weak_unowned_semantics_unowned_reference_sites_total &&
         surface.weak_unowned_semantics_summary.unowned_safe_reference_sites ==
             surface.weak_unowned_semantics_unowned_safe_reference_sites_total &&
         surface.weak_unowned_semantics_summary.weak_unowned_conflict_sites ==
             surface.weak_unowned_semantics_conflict_sites_total &&
         surface.weak_unowned_semantics_summary.contract_violation_sites ==
             surface.weak_unowned_semantics_contract_violation_sites_total &&
         surface.weak_unowned_semantics_summary.unowned_safe_reference_sites <=
             surface.weak_unowned_semantics_summary.unowned_reference_sites &&
         surface.weak_unowned_semantics_summary.weak_unowned_conflict_sites <=
             surface.weak_unowned_semantics_summary.ownership_candidate_sites &&
         surface.weak_unowned_semantics_summary.contract_violation_sites <=
             surface.weak_unowned_semantics_summary.ownership_candidate_sites +
                 surface.weak_unowned_semantics_summary.weak_unowned_conflict_sites &&
         surface.weak_unowned_semantics_summary.deterministic &&
         surface.deterministic_weak_unowned_semantics_handoff &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites ==
             surface.ownership_arc_diagnostic_candidate_sites_total &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites ==
             surface.ownership_arc_fixit_available_sites_total &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites ==
             surface.ownership_arc_profiled_sites_total &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites ==
             surface.ownership_arc_weak_unowned_conflict_diagnostic_sites_total &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites ==
             surface.ownership_arc_empty_fixit_hint_sites_total &&
         surface.arc_diagnostics_fixit_summary.contract_violation_sites ==
             surface.ownership_arc_contract_violation_sites_total &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites <=
             surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
                 surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_profiled_sites <=
             surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
                 surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_weak_unowned_conflict_diagnostic_sites <=
             surface.arc_diagnostics_fixit_summary.ownership_arc_diagnostic_candidate_sites +
                 surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
         surface.arc_diagnostics_fixit_summary.ownership_arc_empty_fixit_hint_sites <=
             surface.arc_diagnostics_fixit_summary.ownership_arc_fixit_available_sites +
                 surface.arc_diagnostics_fixit_summary.contract_violation_sites &&
         surface.arc_diagnostics_fixit_summary.deterministic &&
         surface.deterministic_arc_diagnostics_fixit_handoff &&
         surface.autoreleasepool_scope_summary.scope_sites == surface.autoreleasepool_scope_sites_total &&
         surface.autoreleasepool_scope_summary.scope_symbolized_sites ==
             surface.autoreleasepool_scope_symbolized_sites_total &&
         surface.autoreleasepool_scope_summary.contract_violation_sites ==
             surface.autoreleasepool_scope_contract_violation_sites_total &&
         surface.autoreleasepool_scope_summary.max_scope_depth == surface.autoreleasepool_scope_max_depth_total &&
         surface.autoreleasepool_scope_summary.scope_symbolized_sites <=
             surface.autoreleasepool_scope_summary.scope_sites &&
         surface.autoreleasepool_scope_summary.contract_violation_sites <=
             surface.autoreleasepool_scope_summary.scope_sites &&
         (surface.autoreleasepool_scope_summary.scope_sites > 0u ||
          surface.autoreleasepool_scope_summary.max_scope_depth == 0u) &&
         surface.autoreleasepool_scope_summary.max_scope_depth <=
             static_cast<unsigned>(surface.autoreleasepool_scope_summary.scope_sites) &&
         surface.autoreleasepool_scope_summary.deterministic &&
         surface.deterministic_autoreleasepool_scope_handoff;
}

struct Objc3SemaPassManagerResult {
  Objc3SemanticIntegrationSurface integration_surface;
  std::vector<std::string> diagnostics;
  std::array<std::size_t, 3> diagnostics_after_pass = {0, 0, 0};
  std::array<std::size_t, 3> diagnostics_emitted_by_pass = {0, 0, 0};
  Objc3SemanticTypeMetadataHandoff type_metadata_handoff;
  bool deterministic_semantic_diagnostics = false;
  bool deterministic_type_metadata_handoff = false;
  bool deterministic_interface_implementation_handoff = false;
  bool deterministic_protocol_category_composition_handoff = false;
  bool deterministic_class_protocol_category_linking_handoff = false;
  Objc3ClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;
  bool deterministic_selector_normalization_handoff = false;
  Objc3SelectorNormalizationSummary selector_normalization_summary;
  bool deterministic_property_attribute_handoff = false;
  Objc3PropertyAttributeSummary property_attribute_summary;
  bool deterministic_type_annotation_surface_handoff = false;
  Objc3TypeAnnotationSurfaceSummary type_annotation_surface_summary;
  bool deterministic_symbol_graph_scope_resolution_handoff = false;
  Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;
  bool deterministic_method_lookup_override_conflict_handoff = false;
  Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary;
  bool deterministic_property_synthesis_ivar_binding_handoff = false;
  Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary;
  bool deterministic_id_class_sel_object_pointer_type_checking_handoff = false;
  Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary;
  bool deterministic_block_literal_capture_semantics_handoff = false;
  Objc3BlockLiteralCaptureSemanticsSummary block_literal_capture_semantics_summary;
  bool deterministic_block_abi_invoke_trampoline_handoff = false;
  Objc3BlockAbiInvokeTrampolineSemanticsSummary block_abi_invoke_trampoline_semantics_summary;
  bool deterministic_message_send_selector_lowering_handoff = false;
  Objc3MessageSendSelectorLoweringSummary message_send_selector_lowering_summary;
  bool deterministic_dispatch_abi_marshalling_handoff = false;
  Objc3DispatchAbiMarshallingSummary dispatch_abi_marshalling_summary;
  bool deterministic_nil_receiver_semantics_foldability_handoff = false;
  Objc3NilReceiverSemanticsFoldabilitySummary nil_receiver_semantics_foldability_summary;
  bool deterministic_super_dispatch_method_family_handoff = false;
  Objc3SuperDispatchMethodFamilySummary super_dispatch_method_family_summary;
  bool deterministic_runtime_shim_host_link_handoff = false;
  Objc3RuntimeShimHostLinkSummary runtime_shim_host_link_summary;
  bool deterministic_retain_release_operation_handoff = false;
  Objc3RetainReleaseOperationSummary retain_release_operation_summary;
  bool deterministic_weak_unowned_semantics_handoff = false;
  Objc3WeakUnownedSemanticsSummary weak_unowned_semantics_summary;
  bool deterministic_arc_diagnostics_fixit_handoff = false;
  Objc3ArcDiagnosticsFixitSummary arc_diagnostics_fixit_summary;
  bool deterministic_autoreleasepool_scope_handoff = false;
  Objc3AutoreleasePoolScopeSummary autoreleasepool_scope_summary;
  Objc3AtomicMemoryOrderMappingSummary atomic_memory_order_mapping;
  bool deterministic_atomic_memory_order_mapping = false;
  Objc3VectorTypeLoweringSummary vector_type_lowering;
  bool deterministic_vector_type_lowering = false;
  Objc3SemaParityContractSurface parity_surface;
  bool executed = false;
};
