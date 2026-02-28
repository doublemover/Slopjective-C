#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "parse/objc3_parser_contract.h"

inline constexpr std::uint32_t kObjc3SemaBoundaryContractVersionMajor = 1;
inline constexpr std::uint32_t kObjc3SemaBoundaryContractVersionMinor = 0;
inline constexpr std::uint32_t kObjc3SemaBoundaryContractVersionPatch = 0;
inline constexpr const char *kObjc3RuntimeShimHostLinkDefaultDispatchSymbol = "objc3_msgsend_i32";

enum class Objc3SemaAtomicMemoryOrder : std::uint8_t {
  Relaxed = 0,
  Acquire = 1,
  Release = 2,
  AcqRel = 3,
  SeqCst = 4,
  Unsupported = 5,
};

struct Objc3AtomicMemoryOrderMappingSummary {
  std::size_t relaxed = 0;
  std::size_t acquire = 0;
  std::size_t release = 0;
  std::size_t acq_rel = 0;
  std::size_t seq_cst = 0;
  std::size_t unsupported = 0;
  bool deterministic = true;

  std::size_t total() const { return relaxed + acquire + release + acq_rel + seq_cst + unsupported; }
};

struct Objc3VectorTypeLoweringSummary {
  std::size_t return_annotations = 0;
  std::size_t param_annotations = 0;
  std::size_t i32_annotations = 0;
  std::size_t bool_annotations = 0;
  std::size_t lane2_annotations = 0;
  std::size_t lane4_annotations = 0;
  std::size_t lane8_annotations = 0;
  std::size_t lane16_annotations = 0;
  std::size_t unsupported_annotations = 0;
  bool deterministic = true;

  std::size_t total() const { return return_annotations + param_annotations; }
};

struct Objc3ProtocolCategoryCompositionSummary {
  std::size_t protocol_composition_sites = 0;
  std::size_t protocol_composition_symbols = 0;
  std::size_t category_composition_sites = 0;
  std::size_t category_composition_symbols = 0;
  std::size_t invalid_protocol_composition_sites = 0;
  bool deterministic = true;

  std::size_t total_composition_sites() const { return protocol_composition_sites + category_composition_sites; }
};

struct Objc3ClassProtocolCategoryLinkingSummary {
  std::size_t declared_interfaces = 0;
  std::size_t resolved_interfaces = 0;
  std::size_t declared_implementations = 0;
  std::size_t resolved_implementations = 0;
  std::size_t interface_method_symbols = 0;
  std::size_t implementation_method_symbols = 0;
  std::size_t linked_implementation_symbols = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t protocol_composition_symbols = 0;
  std::size_t category_composition_sites = 0;
  std::size_t category_composition_symbols = 0;
  std::size_t invalid_protocol_composition_sites = 0;
  bool deterministic = true;

  std::size_t total_composition_sites() const { return protocol_composition_sites + category_composition_sites; }
};

struct Objc3SelectorNormalizationSummary {
  std::size_t methods_total = 0;
  std::size_t normalized_methods = 0;
  std::size_t selector_piece_entries = 0;
  std::size_t selector_parameter_piece_entries = 0;
  std::size_t selector_pieceless_methods = 0;
  std::size_t selector_spelling_mismatches = 0;
  std::size_t selector_arity_mismatches = 0;
  std::size_t selector_parameter_linkage_mismatches = 0;
  std::size_t selector_normalization_flag_mismatches = 0;
  std::size_t selector_missing_keyword_pieces = 0;
  bool deterministic = true;

  std::size_t contract_violations() const {
    return selector_pieceless_methods + selector_spelling_mismatches + selector_arity_mismatches +
           selector_parameter_linkage_mismatches + selector_normalization_flag_mismatches +
           selector_missing_keyword_pieces;
  }
};

struct Objc3PropertyAttributeSummary {
  std::size_t properties_total = 0;
  std::size_t attribute_entries = 0;
  std::size_t readonly_modifiers = 0;
  std::size_t readwrite_modifiers = 0;
  std::size_t atomic_modifiers = 0;
  std::size_t nonatomic_modifiers = 0;
  std::size_t copy_modifiers = 0;
  std::size_t strong_modifiers = 0;
  std::size_t weak_modifiers = 0;
  std::size_t assign_modifiers = 0;
  std::size_t getter_modifiers = 0;
  std::size_t setter_modifiers = 0;
  std::size_t invalid_attribute_entries = 0;
  std::size_t property_contract_violations = 0;
  bool deterministic = true;

  std::size_t ownership_modifiers() const { return copy_modifiers + strong_modifiers + weak_modifiers + assign_modifiers; }
  std::size_t contract_violations() const { return invalid_attribute_entries + property_contract_violations; }
};

struct Objc3TypeAnnotationSurfaceSummary {
  std::size_t generic_suffix_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t nullability_suffix_sites = 0;
  std::size_t ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t invalid_generic_suffix_sites = 0;
  std::size_t invalid_pointer_declarator_sites = 0;
  std::size_t invalid_nullability_suffix_sites = 0;
  std::size_t invalid_ownership_qualifier_sites = 0;
  bool deterministic = true;

  std::size_t total_type_annotation_sites() const {
    return generic_suffix_sites + pointer_declarator_sites + nullability_suffix_sites + ownership_qualifier_sites;
  }

  std::size_t invalid_type_annotation_sites() const {
    return invalid_generic_suffix_sites + invalid_pointer_declarator_sites + invalid_nullability_suffix_sites +
           invalid_ownership_qualifier_sites;
  }
};

struct Objc3LightweightGenericConstraintSummary {
  std::size_t generic_constraint_sites = 0;
  std::size_t generic_suffix_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t terminated_generic_suffix_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_constraint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NullabilityFlowWarningPrecisionSummary {
  std::size_t nullability_flow_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t nullability_suffix_sites = 0;
  std::size_t nullable_suffix_sites = 0;
  std::size_t nonnull_suffix_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ProtocolQualifiedObjectTypeSummary {
  std::size_t protocol_qualified_object_type_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t terminated_protocol_composition_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_protocol_composition_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3VarianceBridgeCastSummary {
  std::size_t variance_bridge_cast_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3GenericMetadataAbiSummary {
  std::size_t generic_metadata_abi_sites = 0;
  std::size_t generic_suffix_sites = 0;
  std::size_t protocol_composition_sites = 0;
  std::size_t ownership_qualifier_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ModuleImportGraphSummary {
  std::size_t module_import_graph_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NamespaceCollisionShadowingSummary {
  std::size_t namespace_collision_shadowing_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3PublicPrivateApiPartitionSummary {
  std::size_t public_private_api_partition_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3SymbolGraphScopeResolutionSummary {
  std::size_t global_symbol_nodes = 0;
  std::size_t function_symbol_nodes = 0;
  std::size_t interface_symbol_nodes = 0;
  std::size_t implementation_symbol_nodes = 0;
  std::size_t interface_property_symbol_nodes = 0;
  std::size_t implementation_property_symbol_nodes = 0;
  std::size_t interface_method_symbol_nodes = 0;
  std::size_t implementation_method_symbol_nodes = 0;
  std::size_t top_level_scope_symbols = 0;
  std::size_t nested_scope_symbols = 0;
  std::size_t scope_frames_total = 0;
  std::size_t implementation_interface_resolution_sites = 0;
  std::size_t implementation_interface_resolution_hits = 0;
  std::size_t implementation_interface_resolution_misses = 0;
  std::size_t method_resolution_sites = 0;
  std::size_t method_resolution_hits = 0;
  std::size_t method_resolution_misses = 0;
  bool deterministic = true;

  std::size_t symbol_nodes_total() const {
    return global_symbol_nodes + function_symbol_nodes + interface_symbol_nodes + implementation_symbol_nodes +
           interface_property_symbol_nodes + implementation_property_symbol_nodes + interface_method_symbol_nodes +
           implementation_method_symbol_nodes;
  }

  std::size_t resolution_sites_total() const {
    return implementation_interface_resolution_sites + method_resolution_sites;
  }

  std::size_t resolution_hits_total() const {
    return implementation_interface_resolution_hits + method_resolution_hits;
  }

  std::size_t resolution_misses_total() const {
    return implementation_interface_resolution_misses + method_resolution_misses;
  }
};

struct Objc3MethodLookupOverrideConflictSummary {
  std::size_t method_lookup_sites = 0;
  std::size_t method_lookup_hits = 0;
  std::size_t method_lookup_misses = 0;
  std::size_t override_lookup_sites = 0;
  std::size_t override_lookup_hits = 0;
  std::size_t override_lookup_misses = 0;
  std::size_t override_conflicts = 0;
  std::size_t unresolved_base_interfaces = 0;
  bool deterministic = true;

  std::size_t total_lookup_sites() const { return method_lookup_sites + override_lookup_sites; }
  std::size_t total_lookup_hits() const { return method_lookup_hits + override_lookup_hits; }
  std::size_t total_lookup_misses() const { return method_lookup_misses + override_lookup_misses; }
};

struct Objc3PropertySynthesisIvarBindingSummary {
  std::size_t property_synthesis_sites = 0;
  std::size_t property_synthesis_explicit_ivar_bindings = 0;
  std::size_t property_synthesis_default_ivar_bindings = 0;
  std::size_t ivar_binding_sites = 0;
  std::size_t ivar_binding_resolved = 0;
  std::size_t ivar_binding_missing = 0;
  std::size_t ivar_binding_conflicts = 0;
  bool deterministic = true;
};

struct Objc3IdClassSelObjectPointerTypeCheckingSummary {
  std::size_t param_type_sites = 0;
  std::size_t param_id_spelling_sites = 0;
  std::size_t param_class_spelling_sites = 0;
  std::size_t param_sel_spelling_sites = 0;
  std::size_t param_instancetype_spelling_sites = 0;
  std::size_t param_object_pointer_type_sites = 0;
  std::size_t return_type_sites = 0;
  std::size_t return_id_spelling_sites = 0;
  std::size_t return_class_spelling_sites = 0;
  std::size_t return_sel_spelling_sites = 0;
  std::size_t return_instancetype_spelling_sites = 0;
  std::size_t return_object_pointer_type_sites = 0;
  std::size_t property_type_sites = 0;
  std::size_t property_id_spelling_sites = 0;
  std::size_t property_class_spelling_sites = 0;
  std::size_t property_sel_spelling_sites = 0;
  std::size_t property_instancetype_spelling_sites = 0;
  std::size_t property_object_pointer_type_sites = 0;
  bool deterministic = true;
};

struct Objc3MessageSendSelectorLoweringSiteMetadata {
  std::string selector;
  std::string selector_lowering_symbol;
  std::size_t argument_count = 0;
  std::size_t selector_piece_count = 0;
  std::size_t selector_argument_piece_count = 0;
  bool unary_form = false;
  bool keyword_form = false;
  bool selector_lowering_is_normalized = false;
  bool receiver_is_nil_literal = false;
  bool nil_receiver_semantics_enabled = false;
  bool nil_receiver_foldable = false;
  bool nil_receiver_requires_runtime_dispatch = true;
  bool nil_receiver_semantics_is_normalized = false;
  bool runtime_shim_host_link_required = true;
  bool runtime_shim_host_link_elided = false;
  std::size_t runtime_shim_host_link_runtime_dispatch_arg_slots = 0;
  std::size_t runtime_shim_host_link_declaration_parameter_count = 0;
  std::string runtime_dispatch_bridge_symbol;
  std::string runtime_shim_host_link_symbol;
  bool runtime_shim_host_link_is_normalized = false;
  bool receiver_is_super_identifier = false;
  bool super_dispatch_enabled = false;
  bool super_dispatch_requires_class_context = false;
  bool super_dispatch_semantics_is_normalized = false;
  std::string method_family_name;
  bool method_family_returns_retained_result = false;
  bool method_family_returns_related_result = false;
  bool method_family_semantics_is_normalized = false;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3MessageSendSelectorLoweringSummary {
  std::size_t message_send_sites = 0;
  std::size_t unary_form_sites = 0;
  std::size_t keyword_form_sites = 0;
  std::size_t selector_lowering_symbol_sites = 0;
  std::size_t selector_lowering_piece_entries = 0;
  std::size_t selector_lowering_argument_piece_entries = 0;
  std::size_t selector_lowering_normalized_sites = 0;
  std::size_t selector_lowering_form_mismatch_sites = 0;
  std::size_t selector_lowering_arity_mismatch_sites = 0;
  std::size_t selector_lowering_symbol_mismatch_sites = 0;
  std::size_t selector_lowering_missing_symbol_sites = 0;
  std::size_t selector_lowering_contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3DispatchAbiMarshallingSummary {
  std::size_t message_send_sites = 0;
  std::size_t receiver_slots = 0;
  std::size_t selector_symbol_slots = 0;
  std::size_t argument_slots = 0;
  std::size_t keyword_argument_slots = 0;
  std::size_t unary_argument_slots = 0;
  std::size_t arity_mismatch_sites = 0;
  std::size_t missing_selector_symbol_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NilReceiverSemanticsFoldabilitySummary {
  std::size_t message_send_sites = 0;
  std::size_t receiver_nil_literal_sites = 0;
  std::size_t nil_receiver_semantics_enabled_sites = 0;
  std::size_t nil_receiver_foldable_sites = 0;
  std::size_t nil_receiver_runtime_dispatch_required_sites = 0;
  std::size_t non_nil_receiver_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3SuperDispatchMethodFamilySummary {
  std::size_t message_send_sites = 0;
  std::size_t receiver_super_identifier_sites = 0;
  std::size_t super_dispatch_enabled_sites = 0;
  std::size_t super_dispatch_requires_class_context_sites = 0;
  std::size_t method_family_init_sites = 0;
  std::size_t method_family_copy_sites = 0;
  std::size_t method_family_mutable_copy_sites = 0;
  std::size_t method_family_new_sites = 0;
  std::size_t method_family_none_sites = 0;
  std::size_t method_family_returns_retained_result_sites = 0;
  std::size_t method_family_returns_related_result_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3RuntimeShimHostLinkSummary {
  std::size_t message_send_sites = 0;
  std::size_t runtime_shim_required_sites = 0;
  std::size_t runtime_shim_elided_sites = 0;
  std::size_t runtime_dispatch_arg_slots = 0;
  std::size_t runtime_dispatch_declaration_parameter_count = 0;
  std::size_t contract_violation_sites = 0;
  std::string runtime_dispatch_symbol = kObjc3RuntimeShimHostLinkDefaultDispatchSymbol;
  bool default_runtime_dispatch_symbol_binding = true;
  bool deterministic = true;
};

struct Objc3RetainReleaseOperationSummary {
  std::size_t ownership_qualified_sites = 0;
  std::size_t retain_insertion_sites = 0;
  std::size_t release_insertion_sites = 0;
  std::size_t autorelease_insertion_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3WeakUnownedSemanticsSummary {
  std::size_t ownership_candidate_sites = 0;
  std::size_t weak_reference_sites = 0;
  std::size_t unowned_reference_sites = 0;
  std::size_t unowned_safe_reference_sites = 0;
  std::size_t weak_unowned_conflict_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ArcDiagnosticsFixitSummary {
  std::size_t ownership_arc_diagnostic_candidate_sites = 0;
  std::size_t ownership_arc_fixit_available_sites = 0;
  std::size_t ownership_arc_profiled_sites = 0;
  std::size_t ownership_arc_weak_unowned_conflict_diagnostic_sites = 0;
  std::size_t ownership_arc_empty_fixit_hint_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockLiteralCaptureSiteMetadata {
  std::size_t parameter_count = 0;
  std::size_t capture_count = 0;
  std::size_t body_statement_count = 0;
  bool capture_set_deterministic = false;
  bool literal_is_normalized = false;
  bool has_count_mismatch = false;
  std::string capture_profile;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3BlockLiteralCaptureSemanticsSummary {
  std::size_t block_literal_sites = 0;
  std::size_t block_parameter_entries = 0;
  std::size_t block_capture_entries = 0;
  std::size_t block_body_statement_entries = 0;
  std::size_t block_empty_capture_sites = 0;
  std::size_t block_nondeterministic_capture_sites = 0;
  std::size_t block_non_normalized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockAbiInvokeTrampolineSiteMetadata {
  std::size_t invoke_argument_slots = 0;
  std::size_t capture_word_count = 0;
  std::size_t parameter_count = 0;
  std::size_t capture_count = 0;
  std::size_t body_statement_count = 0;
  bool has_invoke_trampoline = false;
  bool layout_is_normalized = false;
  bool has_count_mismatch = false;
  std::string layout_profile;
  std::string descriptor_symbol;
  std::string invoke_trampoline_symbol;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3BlockAbiInvokeTrampolineSemanticsSummary {
  std::size_t block_literal_sites = 0;
  std::size_t invoke_argument_slots_total = 0;
  std::size_t capture_word_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t descriptor_symbolized_sites = 0;
  std::size_t invoke_trampoline_symbolized_sites = 0;
  std::size_t missing_invoke_trampoline_sites = 0;
  std::size_t non_normalized_layout_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockStorageEscapeSiteMetadata {
  std::size_t mutable_capture_count = 0;
  std::size_t byref_slot_count = 0;
  std::size_t parameter_count = 0;
  std::size_t capture_count = 0;
  std::size_t body_statement_count = 0;
  bool requires_byref_cells = false;
  bool escape_analysis_enabled = false;
  bool escape_to_heap = false;
  bool escape_profile_is_normalized = false;
  bool has_count_mismatch = false;
  std::string escape_profile;
  std::string byref_layout_symbol;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3BlockStorageEscapeSemanticsSummary {
  std::size_t block_literal_sites = 0;
  std::size_t mutable_capture_count_total = 0;
  std::size_t byref_slot_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t requires_byref_cells_sites = 0;
  std::size_t escape_analysis_enabled_sites = 0;
  std::size_t escape_to_heap_sites = 0;
  std::size_t escape_profile_normalized_sites = 0;
  std::size_t byref_layout_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockCopyDisposeSiteMetadata {
  std::size_t mutable_capture_count = 0;
  std::size_t byref_slot_count = 0;
  std::size_t parameter_count = 0;
  std::size_t capture_count = 0;
  std::size_t body_statement_count = 0;
  bool copy_helper_required = false;
  bool dispose_helper_required = false;
  bool copy_dispose_profile_is_normalized = false;
  bool has_count_mismatch = false;
  std::string copy_dispose_profile;
  std::string copy_helper_symbol;
  std::string dispose_helper_symbol;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3BlockCopyDisposeSemanticsSummary {
  std::size_t block_literal_sites = 0;
  std::size_t mutable_capture_count_total = 0;
  std::size_t byref_slot_count_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t copy_helper_required_sites = 0;
  std::size_t dispose_helper_required_sites = 0;
  std::size_t profile_normalized_sites = 0;
  std::size_t copy_helper_symbolized_sites = 0;
  std::size_t dispose_helper_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3BlockDeterminismPerfBaselineSiteMetadata {
  std::size_t parameter_count = 0;
  std::size_t capture_count = 0;
  std::size_t body_statement_count = 0;
  std::size_t baseline_weight = 0;
  bool capture_set_deterministic = false;
  bool baseline_profile_is_normalized = false;
  std::string baseline_profile;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3BlockDeterminismPerfBaselineSummary {
  std::size_t block_literal_sites = 0;
  std::size_t baseline_weight_total = 0;
  std::size_t parameter_entries_total = 0;
  std::size_t capture_entries_total = 0;
  std::size_t body_statement_entries_total = 0;
  std::size_t deterministic_capture_sites = 0;
  std::size_t heavy_tier_sites = 0;
  std::size_t normalized_profile_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AutoreleasePoolScopeSiteMetadata {
  std::string scope_symbol;
  unsigned scope_depth = 0;
  unsigned line = 1;
  unsigned column = 1;
};

struct Objc3AutoreleasePoolScopeSummary {
  std::size_t scope_sites = 0;
  std::size_t scope_symbolized_sites = 0;
  std::size_t contract_violation_sites = 0;
  unsigned max_scope_depth = 0;
  bool deterministic = true;
};

struct FunctionInfo {
  std::size_t arity = 0;
  std::vector<ValueType> param_types;
  std::vector<bool> param_is_vector;
  std::vector<std::string> param_vector_base_spelling;
  std::vector<unsigned> param_vector_lane_count;
  std::vector<bool> param_has_generic_suffix;
  std::vector<bool> param_has_pointer_declarator;
  std::vector<bool> param_has_nullability_suffix;
  std::vector<bool> param_has_ownership_qualifier;
  std::vector<bool> param_object_pointer_type_spelling;
  std::vector<bool> param_has_invalid_generic_suffix;
  std::vector<bool> param_has_invalid_pointer_declarator;
  std::vector<bool> param_has_invalid_nullability_suffix;
  std::vector<bool> param_has_invalid_ownership_qualifier;
  std::vector<bool> param_has_invalid_type_suffix;
  std::vector<bool> param_ownership_insert_retain;
  std::vector<bool> param_ownership_insert_release;
  std::vector<bool> param_ownership_insert_autorelease;
  std::vector<bool> param_ownership_is_weak_reference;
  std::vector<bool> param_ownership_is_unowned_reference;
  std::vector<bool> param_ownership_is_unowned_safe_reference;
  std::vector<bool> param_ownership_arc_diagnostic_candidate;
  std::vector<bool> param_ownership_arc_fixit_available;
  std::vector<std::string> param_ownership_arc_diagnostic_profile;
  std::vector<std::string> param_ownership_arc_fixit_hint;
  std::vector<bool> param_has_protocol_composition;
  std::vector<std::vector<std::string>> param_protocol_composition_lexicographic;
  std::vector<bool> param_has_invalid_protocol_composition;
  bool return_has_generic_suffix = false;
  bool return_has_pointer_declarator = false;
  bool return_has_nullability_suffix = false;
  bool return_has_ownership_qualifier = false;
  bool return_object_pointer_type_spelling = false;
  bool return_has_invalid_generic_suffix = false;
  bool return_has_invalid_pointer_declarator = false;
  bool return_has_invalid_nullability_suffix = false;
  bool return_has_invalid_ownership_qualifier = false;
  bool return_has_invalid_type_suffix = false;
  bool return_ownership_insert_retain = false;
  bool return_ownership_insert_release = false;
  bool return_ownership_insert_autorelease = false;
  bool return_ownership_is_weak_reference = false;
  bool return_ownership_is_unowned_reference = false;
  bool return_ownership_is_unowned_safe_reference = false;
  bool return_ownership_arc_diagnostic_candidate = false;
  bool return_ownership_arc_fixit_available = false;
  std::string return_ownership_arc_diagnostic_profile;
  std::string return_ownership_arc_fixit_hint;
  ValueType return_type = ValueType::I32;
  bool return_is_vector = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_has_protocol_composition = false;
  std::vector<std::string> return_protocol_composition_lexicographic;
  bool return_has_invalid_protocol_composition = false;
  bool has_definition = false;
  bool is_pure_annotation = false;
};

struct Objc3MethodInfo {
  std::string selector_normalized;
  std::size_t selector_piece_count = 0;
  std::size_t selector_parameter_piece_count = 0;
  bool selector_contract_normalized = false;
  bool selector_had_pieceless_form = false;
  bool selector_has_spelling_mismatch = false;
  bool selector_has_arity_mismatch = false;
  bool selector_has_parameter_linkage_mismatch = false;
  bool selector_has_normalization_flag_mismatch = false;
  bool selector_has_missing_piece_keyword = false;
  std::size_t arity = 0;
  std::vector<ValueType> param_types;
  std::vector<bool> param_is_vector;
  std::vector<std::string> param_vector_base_spelling;
  std::vector<unsigned> param_vector_lane_count;
  std::vector<bool> param_has_generic_suffix;
  std::vector<bool> param_has_pointer_declarator;
  std::vector<bool> param_has_nullability_suffix;
  std::vector<bool> param_has_ownership_qualifier;
  std::vector<bool> param_object_pointer_type_spelling;
  std::vector<bool> param_has_invalid_generic_suffix;
  std::vector<bool> param_has_invalid_pointer_declarator;
  std::vector<bool> param_has_invalid_nullability_suffix;
  std::vector<bool> param_has_invalid_ownership_qualifier;
  std::vector<bool> param_has_invalid_type_suffix;
  std::vector<bool> param_ownership_insert_retain;
  std::vector<bool> param_ownership_insert_release;
  std::vector<bool> param_ownership_insert_autorelease;
  std::vector<bool> param_ownership_arc_diagnostic_candidate;
  std::vector<bool> param_ownership_arc_fixit_available;
  std::vector<std::string> param_ownership_arc_diagnostic_profile;
  std::vector<std::string> param_ownership_arc_fixit_hint;
  std::vector<bool> param_has_protocol_composition;
  std::vector<std::vector<std::string>> param_protocol_composition_lexicographic;
  std::vector<bool> param_has_invalid_protocol_composition;
  bool return_has_generic_suffix = false;
  bool return_has_pointer_declarator = false;
  bool return_has_nullability_suffix = false;
  bool return_has_ownership_qualifier = false;
  bool return_object_pointer_type_spelling = false;
  bool return_has_invalid_generic_suffix = false;
  bool return_has_invalid_pointer_declarator = false;
  bool return_has_invalid_nullability_suffix = false;
  bool return_has_invalid_ownership_qualifier = false;
  bool return_has_invalid_type_suffix = false;
  bool return_ownership_insert_retain = false;
  bool return_ownership_insert_release = false;
  bool return_ownership_insert_autorelease = false;
  bool return_ownership_arc_diagnostic_candidate = false;
  bool return_ownership_arc_fixit_available = false;
  std::string return_ownership_arc_diagnostic_profile;
  std::string return_ownership_arc_fixit_hint;
  ValueType return_type = ValueType::I32;
  bool return_is_vector = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_has_protocol_composition = false;
  std::vector<std::string> return_protocol_composition_lexicographic;
  bool return_has_invalid_protocol_composition = false;
  bool is_class_method = false;
  bool has_definition = false;
};

struct Objc3PropertyInfo {
  ValueType type = ValueType::Unknown;
  bool is_vector = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
  bool id_spelling = false;
  bool class_spelling = false;
  bool instancetype_spelling = false;
  bool object_pointer_type_spelling = false;
  bool has_generic_suffix = false;
  bool has_pointer_declarator = false;
  bool has_nullability_suffix = false;
  bool has_ownership_qualifier = false;
  bool has_invalid_generic_suffix = false;
  bool has_invalid_pointer_declarator = false;
  bool has_invalid_nullability_suffix = false;
  bool has_invalid_ownership_qualifier = false;
  bool has_invalid_type_suffix = false;
  bool ownership_insert_retain = false;
  bool ownership_insert_release = false;
  bool ownership_insert_autorelease = false;
  bool ownership_is_weak_reference = false;
  bool ownership_is_unowned_reference = false;
  bool ownership_is_unowned_safe_reference = false;
  bool ownership_arc_diagnostic_candidate = false;
  bool ownership_arc_fixit_available = false;
  std::string ownership_arc_diagnostic_profile;
  std::string ownership_arc_fixit_hint;
  std::size_t attribute_entries = 0;
  std::vector<std::string> attribute_names_lexicographic;
  bool is_readonly = false;
  bool is_readwrite = false;
  bool is_atomic = false;
  bool is_nonatomic = false;
  bool is_copy = false;
  bool is_strong = false;
  bool is_weak = false;
  bool is_unowned = false;
  bool is_assign = false;
  bool has_getter = false;
  bool has_setter = false;
  std::string getter_selector;
  std::string setter_selector;
  std::size_t invalid_attribute_entries = 0;
  std::size_t property_contract_violations = 0;
  bool has_unknown_attribute = false;
  bool has_duplicate_attribute = false;
  bool has_readwrite_conflict = false;
  bool has_atomicity_conflict = false;
  bool has_ownership_conflict = false;
  bool has_weak_unowned_conflict = false;
  bool has_accessor_selector_contract_violation = false;
  bool has_invalid_attribute_contract = false;
};

struct Objc3InterfaceInfo {
  std::string super_name;
  std::unordered_map<std::string, Objc3PropertyInfo> properties;
  std::unordered_map<std::string, Objc3MethodInfo> methods;
};

struct Objc3ImplementationInfo {
  bool has_matching_interface = false;
  std::unordered_map<std::string, Objc3PropertyInfo> properties;
  std::unordered_map<std::string, Objc3MethodInfo> methods;
};

struct Objc3InterfaceImplementationSummary {
  std::size_t declared_interfaces = 0;
  std::size_t resolved_interfaces = 0;
  std::size_t declared_implementations = 0;
  std::size_t resolved_implementations = 0;
  std::size_t interface_method_symbols = 0;
  std::size_t implementation_method_symbols = 0;
  std::size_t linked_implementation_symbols = 0;
  bool deterministic = true;
};

struct Objc3SemanticIntegrationSurface {
  std::unordered_map<std::string, ValueType> globals;
  std::unordered_map<std::string, FunctionInfo> functions;
  std::unordered_map<std::string, Objc3InterfaceInfo> interfaces;
  std::unordered_map<std::string, Objc3ImplementationInfo> implementations;
  Objc3InterfaceImplementationSummary interface_implementation_summary;
  Objc3ProtocolCategoryCompositionSummary protocol_category_composition_summary;
  Objc3ClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;
  Objc3SelectorNormalizationSummary selector_normalization_summary;
  Objc3PropertyAttributeSummary property_attribute_summary;
  Objc3TypeAnnotationSurfaceSummary type_annotation_surface_summary;
  Objc3LightweightGenericConstraintSummary lightweight_generic_constraint_summary;
  Objc3NullabilityFlowWarningPrecisionSummary nullability_flow_warning_precision_summary;
  Objc3ProtocolQualifiedObjectTypeSummary protocol_qualified_object_type_summary;
  Objc3VarianceBridgeCastSummary variance_bridge_cast_summary;
  Objc3GenericMetadataAbiSummary generic_metadata_abi_summary;
  Objc3ModuleImportGraphSummary module_import_graph_summary;
  Objc3NamespaceCollisionShadowingSummary namespace_collision_shadowing_summary;
  Objc3PublicPrivateApiPartitionSummary public_private_api_partition_summary;
  Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;
  Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary;
  Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary;
  Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary;
  std::vector<Objc3BlockLiteralCaptureSiteMetadata> block_literal_capture_sites_lexicographic;
  Objc3BlockLiteralCaptureSemanticsSummary block_literal_capture_semantics_summary;
  std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> block_abi_invoke_trampoline_sites_lexicographic;
  Objc3BlockAbiInvokeTrampolineSemanticsSummary block_abi_invoke_trampoline_semantics_summary;
  std::vector<Objc3BlockStorageEscapeSiteMetadata> block_storage_escape_sites_lexicographic;
  Objc3BlockStorageEscapeSemanticsSummary block_storage_escape_semantics_summary;
  std::vector<Objc3BlockCopyDisposeSiteMetadata> block_copy_dispose_sites_lexicographic;
  Objc3BlockCopyDisposeSemanticsSummary block_copy_dispose_semantics_summary;
  std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> block_determinism_perf_baseline_sites_lexicographic;
  Objc3BlockDeterminismPerfBaselineSummary block_determinism_perf_baseline_summary;
  std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> message_send_selector_lowering_sites_lexicographic;
  Objc3MessageSendSelectorLoweringSummary message_send_selector_lowering_summary;
  Objc3DispatchAbiMarshallingSummary dispatch_abi_marshalling_summary;
  Objc3NilReceiverSemanticsFoldabilitySummary nil_receiver_semantics_foldability_summary;
  Objc3SuperDispatchMethodFamilySummary super_dispatch_method_family_summary;
  Objc3RuntimeShimHostLinkSummary runtime_shim_host_link_summary;
  Objc3RetainReleaseOperationSummary retain_release_operation_summary;
  Objc3WeakUnownedSemanticsSummary weak_unowned_semantics_summary;
  Objc3ArcDiagnosticsFixitSummary arc_diagnostics_fixit_summary;
  std::vector<Objc3AutoreleasePoolScopeSiteMetadata> autoreleasepool_scope_sites_lexicographic;
  Objc3AutoreleasePoolScopeSummary autoreleasepool_scope_summary;
  bool built = false;
};

struct Objc3SemanticFunctionTypeMetadata {
  std::string name;
  std::size_t arity = 0;
  std::vector<ValueType> param_types;
  std::vector<bool> param_is_vector;
  std::vector<std::string> param_vector_base_spelling;
  std::vector<unsigned> param_vector_lane_count;
  std::vector<bool> param_has_generic_suffix;
  std::vector<bool> param_has_pointer_declarator;
  std::vector<bool> param_has_nullability_suffix;
  std::vector<bool> param_has_ownership_qualifier;
  std::vector<bool> param_object_pointer_type_spelling;
  std::vector<bool> param_has_invalid_generic_suffix;
  std::vector<bool> param_has_invalid_pointer_declarator;
  std::vector<bool> param_has_invalid_nullability_suffix;
  std::vector<bool> param_has_invalid_ownership_qualifier;
  std::vector<bool> param_has_invalid_type_suffix;
  std::vector<bool> param_ownership_insert_retain;
  std::vector<bool> param_ownership_insert_release;
  std::vector<bool> param_ownership_insert_autorelease;
  std::vector<bool> param_ownership_is_weak_reference;
  std::vector<bool> param_ownership_is_unowned_reference;
  std::vector<bool> param_ownership_is_unowned_safe_reference;
  std::vector<bool> param_ownership_arc_diagnostic_candidate;
  std::vector<bool> param_ownership_arc_fixit_available;
  std::vector<std::string> param_ownership_arc_diagnostic_profile;
  std::vector<std::string> param_ownership_arc_fixit_hint;
  std::vector<bool> param_has_protocol_composition;
  std::vector<std::vector<std::string>> param_protocol_composition_lexicographic;
  std::vector<bool> param_has_invalid_protocol_composition;
  bool return_has_generic_suffix = false;
  bool return_has_pointer_declarator = false;
  bool return_has_nullability_suffix = false;
  bool return_has_ownership_qualifier = false;
  bool return_object_pointer_type_spelling = false;
  bool return_has_invalid_generic_suffix = false;
  bool return_has_invalid_pointer_declarator = false;
  bool return_has_invalid_nullability_suffix = false;
  bool return_has_invalid_ownership_qualifier = false;
  bool return_has_invalid_type_suffix = false;
  bool return_ownership_insert_retain = false;
  bool return_ownership_insert_release = false;
  bool return_ownership_insert_autorelease = false;
  bool return_ownership_is_weak_reference = false;
  bool return_ownership_is_unowned_reference = false;
  bool return_ownership_is_unowned_safe_reference = false;
  bool return_ownership_arc_diagnostic_candidate = false;
  bool return_ownership_arc_fixit_available = false;
  std::string return_ownership_arc_diagnostic_profile;
  std::string return_ownership_arc_fixit_hint;
  ValueType return_type = ValueType::I32;
  bool return_is_vector = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_has_protocol_composition = false;
  std::vector<std::string> return_protocol_composition_lexicographic;
  bool return_has_invalid_protocol_composition = false;
  bool has_definition = false;
  bool is_pure_annotation = false;
};

struct Objc3SemanticMethodTypeMetadata {
  std::string selector;
  std::string selector_normalized;
  std::size_t selector_piece_count = 0;
  std::size_t selector_parameter_piece_count = 0;
  bool selector_contract_normalized = false;
  bool selector_had_pieceless_form = false;
  bool selector_has_spelling_mismatch = false;
  bool selector_has_arity_mismatch = false;
  bool selector_has_parameter_linkage_mismatch = false;
  bool selector_has_normalization_flag_mismatch = false;
  bool selector_has_missing_piece_keyword = false;
  std::size_t arity = 0;
  std::vector<ValueType> param_types;
  std::vector<bool> param_is_vector;
  std::vector<std::string> param_vector_base_spelling;
  std::vector<unsigned> param_vector_lane_count;
  std::vector<bool> param_has_generic_suffix;
  std::vector<bool> param_has_pointer_declarator;
  std::vector<bool> param_has_nullability_suffix;
  std::vector<bool> param_has_ownership_qualifier;
  std::vector<bool> param_object_pointer_type_spelling;
  std::vector<bool> param_has_invalid_generic_suffix;
  std::vector<bool> param_has_invalid_pointer_declarator;
  std::vector<bool> param_has_invalid_nullability_suffix;
  std::vector<bool> param_has_invalid_ownership_qualifier;
  std::vector<bool> param_has_invalid_type_suffix;
  std::vector<bool> param_ownership_insert_retain;
  std::vector<bool> param_ownership_insert_release;
  std::vector<bool> param_ownership_insert_autorelease;
  std::vector<bool> param_ownership_is_weak_reference;
  std::vector<bool> param_ownership_is_unowned_reference;
  std::vector<bool> param_ownership_is_unowned_safe_reference;
  std::vector<bool> param_ownership_arc_diagnostic_candidate;
  std::vector<bool> param_ownership_arc_fixit_available;
  std::vector<std::string> param_ownership_arc_diagnostic_profile;
  std::vector<std::string> param_ownership_arc_fixit_hint;
  std::vector<bool> param_has_protocol_composition;
  std::vector<std::vector<std::string>> param_protocol_composition_lexicographic;
  std::vector<bool> param_has_invalid_protocol_composition;
  bool return_has_generic_suffix = false;
  bool return_has_pointer_declarator = false;
  bool return_has_nullability_suffix = false;
  bool return_has_ownership_qualifier = false;
  bool return_object_pointer_type_spelling = false;
  bool return_has_invalid_generic_suffix = false;
  bool return_has_invalid_pointer_declarator = false;
  bool return_has_invalid_nullability_suffix = false;
  bool return_has_invalid_ownership_qualifier = false;
  bool return_has_invalid_type_suffix = false;
  bool return_ownership_insert_retain = false;
  bool return_ownership_insert_release = false;
  bool return_ownership_insert_autorelease = false;
  bool return_ownership_is_weak_reference = false;
  bool return_ownership_is_unowned_reference = false;
  bool return_ownership_is_unowned_safe_reference = false;
  bool return_ownership_arc_diagnostic_candidate = false;
  bool return_ownership_arc_fixit_available = false;
  std::string return_ownership_arc_diagnostic_profile;
  std::string return_ownership_arc_fixit_hint;
  ValueType return_type = ValueType::I32;
  bool return_is_vector = false;
  std::string return_vector_base_spelling;
  unsigned return_vector_lane_count = 1;
  bool return_has_protocol_composition = false;
  std::vector<std::string> return_protocol_composition_lexicographic;
  bool return_has_invalid_protocol_composition = false;
  bool is_class_method = false;
  bool has_definition = false;
};

struct Objc3SemanticPropertyTypeMetadata {
  std::string name;
  ValueType type = ValueType::Unknown;
  bool is_vector = false;
  std::string vector_base_spelling;
  unsigned vector_lane_count = 1;
  bool id_spelling = false;
  bool class_spelling = false;
  bool instancetype_spelling = false;
  bool object_pointer_type_spelling = false;
  bool has_generic_suffix = false;
  bool has_pointer_declarator = false;
  bool has_nullability_suffix = false;
  bool has_ownership_qualifier = false;
  bool has_invalid_generic_suffix = false;
  bool has_invalid_pointer_declarator = false;
  bool has_invalid_nullability_suffix = false;
  bool has_invalid_ownership_qualifier = false;
  bool has_invalid_type_suffix = false;
  bool ownership_insert_retain = false;
  bool ownership_insert_release = false;
  bool ownership_insert_autorelease = false;
  bool ownership_is_weak_reference = false;
  bool ownership_is_unowned_reference = false;
  bool ownership_is_unowned_safe_reference = false;
  bool ownership_arc_diagnostic_candidate = false;
  bool ownership_arc_fixit_available = false;
  std::string ownership_arc_diagnostic_profile;
  std::string ownership_arc_fixit_hint;
  std::size_t attribute_entries = 0;
  std::vector<std::string> attribute_names_lexicographic;
  bool is_readonly = false;
  bool is_readwrite = false;
  bool is_atomic = false;
  bool is_nonatomic = false;
  bool is_copy = false;
  bool is_strong = false;
  bool is_weak = false;
  bool is_unowned = false;
  bool is_assign = false;
  bool has_getter = false;
  bool has_setter = false;
  std::string getter_selector;
  std::string setter_selector;
  std::size_t invalid_attribute_entries = 0;
  std::size_t property_contract_violations = 0;
  bool has_unknown_attribute = false;
  bool has_duplicate_attribute = false;
  bool has_readwrite_conflict = false;
  bool has_atomicity_conflict = false;
  bool has_ownership_conflict = false;
  bool has_weak_unowned_conflict = false;
  bool has_accessor_selector_contract_violation = false;
  bool has_invalid_attribute_contract = false;
};

struct Objc3SemanticInterfaceTypeMetadata {
  std::string name;
  std::string super_name;
  std::vector<Objc3SemanticPropertyTypeMetadata> properties_lexicographic;
  std::vector<Objc3SemanticMethodTypeMetadata> methods_lexicographic;
};

struct Objc3SemanticImplementationTypeMetadata {
  std::string name;
  bool has_matching_interface = false;
  std::vector<Objc3SemanticPropertyTypeMetadata> properties_lexicographic;
  std::vector<Objc3SemanticMethodTypeMetadata> methods_lexicographic;
};

struct Objc3SemanticTypeMetadataHandoff {
  std::vector<std::string> global_names_lexicographic;
  std::vector<Objc3SemanticFunctionTypeMetadata> functions_lexicographic;
  std::vector<Objc3SemanticInterfaceTypeMetadata> interfaces_lexicographic;
  std::vector<Objc3SemanticImplementationTypeMetadata> implementations_lexicographic;
  Objc3InterfaceImplementationSummary interface_implementation_summary;
  Objc3ProtocolCategoryCompositionSummary protocol_category_composition_summary;
  Objc3ClassProtocolCategoryLinkingSummary class_protocol_category_linking_summary;
  Objc3SelectorNormalizationSummary selector_normalization_summary;
  Objc3PropertyAttributeSummary property_attribute_summary;
  Objc3TypeAnnotationSurfaceSummary type_annotation_surface_summary;
  Objc3LightweightGenericConstraintSummary lightweight_generic_constraint_summary;
  Objc3NullabilityFlowWarningPrecisionSummary nullability_flow_warning_precision_summary;
  Objc3ProtocolQualifiedObjectTypeSummary protocol_qualified_object_type_summary;
  Objc3VarianceBridgeCastSummary variance_bridge_cast_summary;
  Objc3GenericMetadataAbiSummary generic_metadata_abi_summary;
  Objc3ModuleImportGraphSummary module_import_graph_summary;
  Objc3NamespaceCollisionShadowingSummary namespace_collision_shadowing_summary;
  Objc3PublicPrivateApiPartitionSummary public_private_api_partition_summary;
  Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;
  Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary;
  Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary;
  Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary;
  std::vector<Objc3BlockLiteralCaptureSiteMetadata> block_literal_capture_sites_lexicographic;
  Objc3BlockLiteralCaptureSemanticsSummary block_literal_capture_semantics_summary;
  std::vector<Objc3BlockAbiInvokeTrampolineSiteMetadata> block_abi_invoke_trampoline_sites_lexicographic;
  Objc3BlockAbiInvokeTrampolineSemanticsSummary block_abi_invoke_trampoline_semantics_summary;
  std::vector<Objc3BlockStorageEscapeSiteMetadata> block_storage_escape_sites_lexicographic;
  Objc3BlockStorageEscapeSemanticsSummary block_storage_escape_semantics_summary;
  std::vector<Objc3BlockCopyDisposeSiteMetadata> block_copy_dispose_sites_lexicographic;
  Objc3BlockCopyDisposeSemanticsSummary block_copy_dispose_semantics_summary;
  std::vector<Objc3BlockDeterminismPerfBaselineSiteMetadata> block_determinism_perf_baseline_sites_lexicographic;
  Objc3BlockDeterminismPerfBaselineSummary block_determinism_perf_baseline_summary;
  std::vector<Objc3MessageSendSelectorLoweringSiteMetadata> message_send_selector_lowering_sites_lexicographic;
  Objc3MessageSendSelectorLoweringSummary message_send_selector_lowering_summary;
  Objc3DispatchAbiMarshallingSummary dispatch_abi_marshalling_summary;
  Objc3NilReceiverSemanticsFoldabilitySummary nil_receiver_semantics_foldability_summary;
  Objc3SuperDispatchMethodFamilySummary super_dispatch_method_family_summary;
  Objc3RuntimeShimHostLinkSummary runtime_shim_host_link_summary;
  Objc3RetainReleaseOperationSummary retain_release_operation_summary;
  Objc3WeakUnownedSemanticsSummary weak_unowned_semantics_summary;
  Objc3ArcDiagnosticsFixitSummary arc_diagnostics_fixit_summary;
  std::vector<Objc3AutoreleasePoolScopeSiteMetadata> autoreleasepool_scope_sites_lexicographic;
  Objc3AutoreleasePoolScopeSummary autoreleasepool_scope_summary;
};

struct Objc3SemanticValidationOptions {
  std::size_t max_message_send_args = 4;
};

bool ResolveGlobalInitializerValues(const std::vector<Objc3ParsedGlobalDecl> &globals, std::vector<int> &values);
Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface);
bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff);
