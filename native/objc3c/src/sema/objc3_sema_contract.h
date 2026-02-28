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
  std::size_t object_pointer_type_sites = 0;
  std::size_t invalid_generic_suffix_sites = 0;
  std::size_t invalid_pointer_declarator_sites = 0;
  std::size_t invalid_nullability_suffix_sites = 0;
  bool deterministic = true;

  std::size_t total_type_annotation_sites() const {
    return generic_suffix_sites + pointer_declarator_sites + nullability_suffix_sites;
  }

  std::size_t invalid_type_annotation_sites() const {
    return invalid_generic_suffix_sites + invalid_pointer_declarator_sites + invalid_nullability_suffix_sites;
  }
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

struct FunctionInfo {
  std::size_t arity = 0;
  std::vector<ValueType> param_types;
  std::vector<bool> param_is_vector;
  std::vector<std::string> param_vector_base_spelling;
  std::vector<unsigned> param_vector_lane_count;
  std::vector<bool> param_has_generic_suffix;
  std::vector<bool> param_has_pointer_declarator;
  std::vector<bool> param_has_nullability_suffix;
  std::vector<bool> param_object_pointer_type_spelling;
  std::vector<bool> param_has_invalid_generic_suffix;
  std::vector<bool> param_has_invalid_pointer_declarator;
  std::vector<bool> param_has_invalid_nullability_suffix;
  std::vector<bool> param_has_invalid_type_suffix;
  std::vector<bool> param_has_protocol_composition;
  std::vector<std::vector<std::string>> param_protocol_composition_lexicographic;
  std::vector<bool> param_has_invalid_protocol_composition;
  bool return_has_generic_suffix = false;
  bool return_has_pointer_declarator = false;
  bool return_has_nullability_suffix = false;
  bool return_object_pointer_type_spelling = false;
  bool return_has_invalid_generic_suffix = false;
  bool return_has_invalid_pointer_declarator = false;
  bool return_has_invalid_nullability_suffix = false;
  bool return_has_invalid_type_suffix = false;
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
  std::vector<bool> param_object_pointer_type_spelling;
  std::vector<bool> param_has_invalid_generic_suffix;
  std::vector<bool> param_has_invalid_pointer_declarator;
  std::vector<bool> param_has_invalid_nullability_suffix;
  std::vector<bool> param_has_invalid_type_suffix;
  std::vector<bool> param_has_protocol_composition;
  std::vector<std::vector<std::string>> param_protocol_composition_lexicographic;
  std::vector<bool> param_has_invalid_protocol_composition;
  bool return_has_generic_suffix = false;
  bool return_has_pointer_declarator = false;
  bool return_has_nullability_suffix = false;
  bool return_object_pointer_type_spelling = false;
  bool return_has_invalid_generic_suffix = false;
  bool return_has_invalid_pointer_declarator = false;
  bool return_has_invalid_nullability_suffix = false;
  bool return_has_invalid_type_suffix = false;
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
  bool has_invalid_generic_suffix = false;
  bool has_invalid_pointer_declarator = false;
  bool has_invalid_nullability_suffix = false;
  bool has_invalid_type_suffix = false;
  std::size_t attribute_entries = 0;
  std::vector<std::string> attribute_names_lexicographic;
  bool is_readonly = false;
  bool is_readwrite = false;
  bool is_atomic = false;
  bool is_nonatomic = false;
  bool is_copy = false;
  bool is_strong = false;
  bool is_weak = false;
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
  Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;
  Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary;
  Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary;
  Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary;
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
  std::vector<bool> param_object_pointer_type_spelling;
  std::vector<bool> param_has_invalid_generic_suffix;
  std::vector<bool> param_has_invalid_pointer_declarator;
  std::vector<bool> param_has_invalid_nullability_suffix;
  std::vector<bool> param_has_invalid_type_suffix;
  std::vector<bool> param_has_protocol_composition;
  std::vector<std::vector<std::string>> param_protocol_composition_lexicographic;
  std::vector<bool> param_has_invalid_protocol_composition;
  bool return_has_generic_suffix = false;
  bool return_has_pointer_declarator = false;
  bool return_has_nullability_suffix = false;
  bool return_object_pointer_type_spelling = false;
  bool return_has_invalid_generic_suffix = false;
  bool return_has_invalid_pointer_declarator = false;
  bool return_has_invalid_nullability_suffix = false;
  bool return_has_invalid_type_suffix = false;
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
  std::vector<bool> param_object_pointer_type_spelling;
  std::vector<bool> param_has_invalid_generic_suffix;
  std::vector<bool> param_has_invalid_pointer_declarator;
  std::vector<bool> param_has_invalid_nullability_suffix;
  std::vector<bool> param_has_invalid_type_suffix;
  std::vector<bool> param_has_protocol_composition;
  std::vector<std::vector<std::string>> param_protocol_composition_lexicographic;
  std::vector<bool> param_has_invalid_protocol_composition;
  bool return_has_generic_suffix = false;
  bool return_has_pointer_declarator = false;
  bool return_has_nullability_suffix = false;
  bool return_object_pointer_type_spelling = false;
  bool return_has_invalid_generic_suffix = false;
  bool return_has_invalid_pointer_declarator = false;
  bool return_has_invalid_nullability_suffix = false;
  bool return_has_invalid_type_suffix = false;
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
  bool has_invalid_generic_suffix = false;
  bool has_invalid_pointer_declarator = false;
  bool has_invalid_nullability_suffix = false;
  bool has_invalid_type_suffix = false;
  std::size_t attribute_entries = 0;
  std::vector<std::string> attribute_names_lexicographic;
  bool is_readonly = false;
  bool is_readwrite = false;
  bool is_atomic = false;
  bool is_nonatomic = false;
  bool is_copy = false;
  bool is_strong = false;
  bool is_weak = false;
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
  Objc3SymbolGraphScopeResolutionSummary symbol_graph_scope_resolution_summary;
  Objc3MethodLookupOverrideConflictSummary method_lookup_override_conflict_summary;
  Objc3PropertySynthesisIvarBindingSummary property_synthesis_ivar_binding_summary;
  Objc3IdClassSelObjectPointerTypeCheckingSummary id_class_sel_object_pointer_type_checking_summary;
};

struct Objc3SemanticValidationOptions {
  std::size_t max_message_send_args = 4;
};

bool ResolveGlobalInitializerValues(const std::vector<Objc3ParsedGlobalDecl> &globals, std::vector<int> &values);
Objc3SemanticTypeMetadataHandoff BuildSemanticTypeMetadataHandoff(const Objc3SemanticIntegrationSurface &surface);
bool IsDeterministicSemanticTypeMetadataHandoff(const Objc3SemanticTypeMetadataHandoff &handoff);
