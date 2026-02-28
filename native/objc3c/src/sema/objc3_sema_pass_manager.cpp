#include "sema/objc3_sema_pass_manager.h"

#include <algorithm>
#include <sstream>
#include <vector>

#include "diag/objc3_diag_utils.h"
#include "sema/objc3_semantic_passes.h"

namespace {

bool IsDiagnosticLess(const std::string &lhs, const std::string &rhs) {
  const DiagSortKey lhs_key = ParseDiagSortKey(lhs);
  const DiagSortKey rhs_key = ParseDiagSortKey(rhs);
  if (lhs_key.line != rhs_key.line) {
    return lhs_key.line < rhs_key.line;
  }
  if (lhs_key.column != rhs_key.column) {
    return lhs_key.column < rhs_key.column;
  }
  if (lhs_key.severity_rank != rhs_key.severity_rank) {
    return lhs_key.severity_rank < rhs_key.severity_rank;
  }
  if (lhs_key.code != rhs_key.code) {
    return lhs_key.code < rhs_key.code;
  }
  if (lhs_key.message != rhs_key.message) {
    return lhs_key.message < rhs_key.message;
  }
  return lhs_key.raw < rhs_key.raw;
}

std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {
  std::ostringstream out;
  out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";
  return out.str();
}

void AppendMigrationAssistDiagnostics(const Objc3SemaPassManagerInput &input, std::vector<std::string> &diagnostics) {
  if (!input.migration_assist || input.compatibility_mode != Objc3SemaCompatibilityMode::Canonical) {
    return;
  }

  const auto append_for_literal = [&diagnostics](std::size_t count,
                                                 unsigned column,
                                                 const char *legacy_literal,
                                                 const char *canonical_literal) {
    if (count == 0) {
      return;
    }
    diagnostics.push_back(
        MakeDiag(1u,
                 column,
                 "O3S216",
                 "migration assist requires canonical literal '" + std::string(canonical_literal) +
                     "' instead of legacy '" + legacy_literal + "' (" + std::to_string(count) +
                     " occurrence(s))"));
  };

  append_for_literal(input.migration_hints.legacy_yes_count, 1u, "YES", "true");
  append_for_literal(input.migration_hints.legacy_no_count, 2u, "NO", "false");
  append_for_literal(input.migration_hints.legacy_null_count, 3u, "NULL", "nil");
}

void CanonicalizePassDiagnostics(std::vector<std::string> &diagnostics) {
  std::stable_sort(diagnostics.begin(), diagnostics.end(), IsDiagnosticLess);
}

bool IsCanonicalPassDiagnostics(const std::vector<std::string> &diagnostics) {
  return std::is_sorted(diagnostics.begin(), diagnostics.end(), IsDiagnosticLess);
}

bool IsEquivalentSelectorNormalizationSummary(const Objc3SelectorNormalizationSummary &lhs,
                                             const Objc3SelectorNormalizationSummary &rhs) {
  return lhs.methods_total == rhs.methods_total &&
         lhs.normalized_methods == rhs.normalized_methods &&
         lhs.selector_piece_entries == rhs.selector_piece_entries &&
         lhs.selector_parameter_piece_entries == rhs.selector_parameter_piece_entries &&
         lhs.selector_pieceless_methods == rhs.selector_pieceless_methods &&
         lhs.selector_spelling_mismatches == rhs.selector_spelling_mismatches &&
         lhs.selector_arity_mismatches == rhs.selector_arity_mismatches &&
         lhs.selector_parameter_linkage_mismatches == rhs.selector_parameter_linkage_mismatches &&
         lhs.selector_normalization_flag_mismatches == rhs.selector_normalization_flag_mismatches &&
         lhs.selector_missing_keyword_pieces == rhs.selector_missing_keyword_pieces;
}

bool IsEquivalentPropertyAttributeSummary(const Objc3PropertyAttributeSummary &lhs,
                                          const Objc3PropertyAttributeSummary &rhs) {
  return lhs.properties_total == rhs.properties_total &&
         lhs.attribute_entries == rhs.attribute_entries &&
         lhs.readonly_modifiers == rhs.readonly_modifiers &&
         lhs.readwrite_modifiers == rhs.readwrite_modifiers &&
         lhs.atomic_modifiers == rhs.atomic_modifiers &&
         lhs.nonatomic_modifiers == rhs.nonatomic_modifiers &&
         lhs.copy_modifiers == rhs.copy_modifiers &&
         lhs.strong_modifiers == rhs.strong_modifiers &&
         lhs.weak_modifiers == rhs.weak_modifiers &&
         lhs.assign_modifiers == rhs.assign_modifiers &&
         lhs.getter_modifiers == rhs.getter_modifiers &&
         lhs.setter_modifiers == rhs.setter_modifiers &&
         lhs.invalid_attribute_entries == rhs.invalid_attribute_entries &&
         lhs.property_contract_violations == rhs.property_contract_violations;
}

bool IsEquivalentTypeAnnotationSurfaceSummary(const Objc3TypeAnnotationSurfaceSummary &lhs,
                                              const Objc3TypeAnnotationSurfaceSummary &rhs) {
  return lhs.generic_suffix_sites == rhs.generic_suffix_sites &&
         lhs.pointer_declarator_sites == rhs.pointer_declarator_sites &&
         lhs.nullability_suffix_sites == rhs.nullability_suffix_sites &&
         lhs.object_pointer_type_sites == rhs.object_pointer_type_sites &&
         lhs.invalid_generic_suffix_sites == rhs.invalid_generic_suffix_sites &&
         lhs.invalid_pointer_declarator_sites == rhs.invalid_pointer_declarator_sites &&
         lhs.invalid_nullability_suffix_sites == rhs.invalid_nullability_suffix_sites;
}

bool IsEquivalentSymbolGraphScopeResolutionSummary(const Objc3SymbolGraphScopeResolutionSummary &lhs,
                                                   const Objc3SymbolGraphScopeResolutionSummary &rhs) {
  return lhs.global_symbol_nodes == rhs.global_symbol_nodes &&
         lhs.function_symbol_nodes == rhs.function_symbol_nodes &&
         lhs.interface_symbol_nodes == rhs.interface_symbol_nodes &&
         lhs.implementation_symbol_nodes == rhs.implementation_symbol_nodes &&
         lhs.interface_property_symbol_nodes == rhs.interface_property_symbol_nodes &&
         lhs.implementation_property_symbol_nodes == rhs.implementation_property_symbol_nodes &&
         lhs.interface_method_symbol_nodes == rhs.interface_method_symbol_nodes &&
         lhs.implementation_method_symbol_nodes == rhs.implementation_method_symbol_nodes &&
         lhs.top_level_scope_symbols == rhs.top_level_scope_symbols &&
         lhs.nested_scope_symbols == rhs.nested_scope_symbols &&
         lhs.scope_frames_total == rhs.scope_frames_total &&
         lhs.implementation_interface_resolution_sites == rhs.implementation_interface_resolution_sites &&
         lhs.implementation_interface_resolution_hits == rhs.implementation_interface_resolution_hits &&
         lhs.implementation_interface_resolution_misses == rhs.implementation_interface_resolution_misses &&
         lhs.method_resolution_sites == rhs.method_resolution_sites &&
         lhs.method_resolution_hits == rhs.method_resolution_hits &&
         lhs.method_resolution_misses == rhs.method_resolution_misses;
}

bool IsEquivalentClassProtocolCategoryLinkingSummary(const Objc3ClassProtocolCategoryLinkingSummary &lhs,
                                                     const Objc3ClassProtocolCategoryLinkingSummary &rhs) {
  return lhs.declared_interfaces == rhs.declared_interfaces &&
         lhs.resolved_interfaces == rhs.resolved_interfaces &&
         lhs.declared_implementations == rhs.declared_implementations &&
         lhs.resolved_implementations == rhs.resolved_implementations &&
         lhs.interface_method_symbols == rhs.interface_method_symbols &&
         lhs.implementation_method_symbols == rhs.implementation_method_symbols &&
         lhs.linked_implementation_symbols == rhs.linked_implementation_symbols &&
         lhs.protocol_composition_sites == rhs.protocol_composition_sites &&
         lhs.protocol_composition_symbols == rhs.protocol_composition_symbols &&
         lhs.category_composition_sites == rhs.category_composition_sites &&
         lhs.category_composition_symbols == rhs.category_composition_symbols &&
         lhs.invalid_protocol_composition_sites == rhs.invalid_protocol_composition_sites;
}

}  // namespace

Objc3SemaPassManagerResult RunObjc3SemaPassManager(const Objc3SemaPassManagerInput &input) {
  Objc3SemaPassManagerResult result;
  if (input.program == nullptr) {
    return result;
  }

  result.executed = true;
  bool deterministic_semantic_diagnostics = true;
  for (const Objc3SemaPassId pass : kObjc3SemaPassOrder) {
    std::vector<std::string> pass_diagnostics;
    if (pass == Objc3SemaPassId::BuildIntegrationSurface) {
      result.integration_surface = BuildSemanticIntegrationSurface(*input.program, pass_diagnostics);
    } else if (pass == Objc3SemaPassId::ValidateBodies) {
      ValidateSemanticBodies(*input.program, result.integration_surface, input.validation_options, pass_diagnostics);
    } else {
      ValidatePureContractSemanticDiagnostics(*input.program, result.integration_surface.functions, pass_diagnostics);
      AppendMigrationAssistDiagnostics(input, pass_diagnostics);
    }
    CanonicalizePassDiagnostics(pass_diagnostics);
    deterministic_semantic_diagnostics = deterministic_semantic_diagnostics && IsCanonicalPassDiagnostics(pass_diagnostics);

    result.diagnostics.insert(result.diagnostics.end(), pass_diagnostics.begin(), pass_diagnostics.end());
    input.diagnostics_bus.PublishBatch(pass_diagnostics);
    result.diagnostics_after_pass[static_cast<std::size_t>(pass)] = result.diagnostics.size();
    result.diagnostics_emitted_by_pass[static_cast<std::size_t>(pass)] = pass_diagnostics.size();
  }
  result.deterministic_semantic_diagnostics = deterministic_semantic_diagnostics;
  result.type_metadata_handoff = BuildSemanticTypeMetadataHandoff(result.integration_surface);
  result.deterministic_type_metadata_handoff =
      IsDeterministicSemanticTypeMetadataHandoff(result.type_metadata_handoff);
  result.deterministic_interface_implementation_handoff =
      result.type_metadata_handoff.interface_implementation_summary.deterministic &&
      result.integration_surface.interface_implementation_summary.deterministic &&
      result.integration_surface.interface_implementation_summary.declared_interfaces ==
          result.type_metadata_handoff.interface_implementation_summary.declared_interfaces &&
      result.integration_surface.interface_implementation_summary.declared_implementations ==
          result.type_metadata_handoff.interface_implementation_summary.declared_implementations &&
      result.integration_surface.interface_implementation_summary.resolved_interfaces ==
          result.type_metadata_handoff.interface_implementation_summary.resolved_interfaces &&
      result.integration_surface.interface_implementation_summary.resolved_implementations ==
          result.type_metadata_handoff.interface_implementation_summary.resolved_implementations &&
      result.integration_surface.interface_implementation_summary.interface_method_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.interface_method_symbols &&
      result.integration_surface.interface_implementation_summary.implementation_method_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.implementation_method_symbols &&
      result.integration_surface.interface_implementation_summary.linked_implementation_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.linked_implementation_symbols;
  result.deterministic_protocol_category_composition_handoff =
      result.type_metadata_handoff.protocol_category_composition_summary.deterministic &&
      result.integration_surface.protocol_category_composition_summary.deterministic &&
      result.integration_surface.protocol_category_composition_summary.protocol_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.protocol_composition_sites &&
      result.integration_surface.protocol_category_composition_summary.protocol_composition_symbols ==
          result.type_metadata_handoff.protocol_category_composition_summary.protocol_composition_symbols &&
      result.integration_surface.protocol_category_composition_summary.category_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.category_composition_sites &&
      result.integration_surface.protocol_category_composition_summary.category_composition_symbols ==
          result.type_metadata_handoff.protocol_category_composition_summary.category_composition_symbols &&
      result.integration_surface.protocol_category_composition_summary.invalid_protocol_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.invalid_protocol_composition_sites;
  result.class_protocol_category_linking_summary = result.integration_surface.class_protocol_category_linking_summary;
  result.deterministic_class_protocol_category_linking_handoff =
      result.type_metadata_handoff.class_protocol_category_linking_summary.deterministic &&
      result.integration_surface.class_protocol_category_linking_summary.deterministic &&
      IsEquivalentClassProtocolCategoryLinkingSummary(
          result.integration_surface.class_protocol_category_linking_summary,
          result.type_metadata_handoff.class_protocol_category_linking_summary) &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.resolved_interfaces <=
          result.type_metadata_handoff.class_protocol_category_linking_summary.declared_interfaces &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.resolved_implementations <=
          result.type_metadata_handoff.class_protocol_category_linking_summary.declared_implementations &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.interface_method_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.interface_method_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.implementation_method_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.implementation_method_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.linked_implementation_symbols ==
          result.type_metadata_handoff.interface_implementation_summary.linked_implementation_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.protocol_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.protocol_composition_sites &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.protocol_composition_symbols ==
          result.type_metadata_handoff.protocol_category_composition_summary.protocol_composition_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.category_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.category_composition_sites &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.category_composition_symbols ==
          result.type_metadata_handoff.protocol_category_composition_summary.category_composition_symbols &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.invalid_protocol_composition_sites ==
          result.type_metadata_handoff.protocol_category_composition_summary.invalid_protocol_composition_sites &&
      result.type_metadata_handoff.class_protocol_category_linking_summary.invalid_protocol_composition_sites <=
          result.type_metadata_handoff.class_protocol_category_linking_summary.total_composition_sites();
  result.selector_normalization_summary = result.integration_surface.selector_normalization_summary;
  result.deterministic_selector_normalization_handoff =
      result.type_metadata_handoff.selector_normalization_summary.deterministic &&
      result.integration_surface.selector_normalization_summary.deterministic &&
      IsEquivalentSelectorNormalizationSummary(result.integration_surface.selector_normalization_summary,
                                               result.type_metadata_handoff.selector_normalization_summary) &&
      result.type_metadata_handoff.selector_normalization_summary.normalized_methods <=
          result.type_metadata_handoff.selector_normalization_summary.methods_total &&
      result.type_metadata_handoff.selector_normalization_summary.selector_parameter_piece_entries <=
          result.type_metadata_handoff.selector_normalization_summary.selector_piece_entries &&
      result.type_metadata_handoff.selector_normalization_summary.contract_violations() <=
          result.type_metadata_handoff.selector_normalization_summary.methods_total;
  result.property_attribute_summary = result.integration_surface.property_attribute_summary;
  result.deterministic_property_attribute_handoff =
      result.type_metadata_handoff.property_attribute_summary.deterministic &&
      result.integration_surface.property_attribute_summary.deterministic &&
      IsEquivalentPropertyAttributeSummary(result.integration_surface.property_attribute_summary,
                                           result.type_metadata_handoff.property_attribute_summary) &&
      result.type_metadata_handoff.property_attribute_summary.getter_modifiers <=
          result.type_metadata_handoff.property_attribute_summary.properties_total &&
      result.type_metadata_handoff.property_attribute_summary.setter_modifiers <=
          result.type_metadata_handoff.property_attribute_summary.properties_total;
  result.type_annotation_surface_summary = result.integration_surface.type_annotation_surface_summary;
  result.deterministic_type_annotation_surface_handoff =
      result.type_metadata_handoff.type_annotation_surface_summary.deterministic &&
      result.integration_surface.type_annotation_surface_summary.deterministic &&
      IsEquivalentTypeAnnotationSurfaceSummary(result.integration_surface.type_annotation_surface_summary,
                                               result.type_metadata_handoff.type_annotation_surface_summary) &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_generic_suffix_sites <=
          result.type_metadata_handoff.type_annotation_surface_summary.generic_suffix_sites &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_pointer_declarator_sites <=
          result.type_metadata_handoff.type_annotation_surface_summary.pointer_declarator_sites &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_nullability_suffix_sites <=
          result.type_metadata_handoff.type_annotation_surface_summary.nullability_suffix_sites &&
      result.type_metadata_handoff.type_annotation_surface_summary.invalid_type_annotation_sites() <=
          result.type_metadata_handoff.type_annotation_surface_summary.total_type_annotation_sites();
  result.symbol_graph_scope_resolution_summary = result.integration_surface.symbol_graph_scope_resolution_summary;
  result.deterministic_symbol_graph_scope_resolution_handoff =
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.deterministic &&
      result.integration_surface.symbol_graph_scope_resolution_summary.deterministic &&
      IsEquivalentSymbolGraphScopeResolutionSummary(result.integration_surface.symbol_graph_scope_resolution_summary,
                                                    result.type_metadata_handoff.symbol_graph_scope_resolution_summary) &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.symbol_nodes_total() ==
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.top_level_scope_symbols +
              result.type_metadata_handoff.symbol_graph_scope_resolution_summary.nested_scope_symbols &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits <=
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits +
              result.type_metadata_handoff.symbol_graph_scope_resolution_summary
                  .implementation_interface_resolution_misses ==
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_hits <=
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_hits +
              result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_misses ==
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_hits_total() <=
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_sites_total() &&
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_hits_total() +
              result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_misses_total() ==
          result.type_metadata_handoff.symbol_graph_scope_resolution_summary.resolution_sites_total();
  result.atomic_memory_order_mapping = BuildAtomicMemoryOrderMappingSummary(*input.program);
  result.deterministic_atomic_memory_order_mapping = result.atomic_memory_order_mapping.deterministic;
  result.vector_type_lowering = BuildVectorTypeLoweringSummary(result.integration_surface);
  result.deterministic_vector_type_lowering = result.vector_type_lowering.deterministic;
  result.parity_surface.diagnostics_after_pass = result.diagnostics_after_pass;
  result.parity_surface.diagnostics_emitted_by_pass = result.diagnostics_emitted_by_pass;
  result.parity_surface.diagnostics_total = result.diagnostics.size();
  result.parity_surface.globals_total = result.integration_surface.globals.size();
  result.parity_surface.functions_total = result.integration_surface.functions.size();
  result.parity_surface.interfaces_total = result.integration_surface.interfaces.size();
  result.parity_surface.implementations_total = result.integration_surface.implementations.size();
  result.parity_surface.type_metadata_global_entries = result.type_metadata_handoff.global_names_lexicographic.size();
  result.parity_surface.type_metadata_function_entries = result.type_metadata_handoff.functions_lexicographic.size();
  result.parity_surface.type_metadata_interface_entries = result.type_metadata_handoff.interfaces_lexicographic.size();
  result.parity_surface.type_metadata_implementation_entries =
      result.type_metadata_handoff.implementations_lexicographic.size();
  result.parity_surface.interface_implementation_summary = result.type_metadata_handoff.interface_implementation_summary;
  result.parity_surface.interface_method_symbols_total =
      result.parity_surface.interface_implementation_summary.interface_method_symbols;
  result.parity_surface.implementation_method_symbols_total =
      result.parity_surface.interface_implementation_summary.implementation_method_symbols;
  result.parity_surface.linked_implementation_symbols_total =
      result.parity_surface.interface_implementation_summary.linked_implementation_symbols;
  result.parity_surface.protocol_category_composition_summary =
      result.type_metadata_handoff.protocol_category_composition_summary;
  result.parity_surface.protocol_composition_sites_total =
      result.parity_surface.protocol_category_composition_summary.protocol_composition_sites;
  result.parity_surface.protocol_composition_symbols_total =
      result.parity_surface.protocol_category_composition_summary.protocol_composition_symbols;
  result.parity_surface.category_composition_sites_total =
      result.parity_surface.protocol_category_composition_summary.category_composition_sites;
  result.parity_surface.category_composition_symbols_total =
      result.parity_surface.protocol_category_composition_summary.category_composition_symbols;
  result.parity_surface.invalid_protocol_composition_sites_total =
      result.parity_surface.protocol_category_composition_summary.invalid_protocol_composition_sites;
  result.parity_surface.class_protocol_category_linking_summary =
      result.type_metadata_handoff.class_protocol_category_linking_summary;
  result.parity_surface.selector_normalization_summary = result.type_metadata_handoff.selector_normalization_summary;
  result.parity_surface.selector_normalization_methods_total =
      result.parity_surface.selector_normalization_summary.methods_total;
  result.parity_surface.selector_normalization_normalized_methods_total =
      result.parity_surface.selector_normalization_summary.normalized_methods;
  result.parity_surface.selector_normalization_piece_entries_total =
      result.parity_surface.selector_normalization_summary.selector_piece_entries;
  result.parity_surface.selector_normalization_parameter_piece_entries_total =
      result.parity_surface.selector_normalization_summary.selector_parameter_piece_entries;
  result.parity_surface.selector_normalization_pieceless_methods_total =
      result.parity_surface.selector_normalization_summary.selector_pieceless_methods;
  result.parity_surface.selector_normalization_spelling_mismatches_total =
      result.parity_surface.selector_normalization_summary.selector_spelling_mismatches;
  result.parity_surface.selector_normalization_arity_mismatches_total =
      result.parity_surface.selector_normalization_summary.selector_arity_mismatches;
  result.parity_surface.selector_normalization_parameter_linkage_mismatches_total =
      result.parity_surface.selector_normalization_summary.selector_parameter_linkage_mismatches;
  result.parity_surface.selector_normalization_flag_mismatches_total =
      result.parity_surface.selector_normalization_summary.selector_normalization_flag_mismatches;
  result.parity_surface.selector_normalization_missing_keyword_pieces_total =
      result.parity_surface.selector_normalization_summary.selector_missing_keyword_pieces;
  result.parity_surface.property_attribute_summary = result.type_metadata_handoff.property_attribute_summary;
  result.parity_surface.property_attribute_properties_total =
      result.parity_surface.property_attribute_summary.properties_total;
  result.parity_surface.property_attribute_entries_total =
      result.parity_surface.property_attribute_summary.attribute_entries;
  result.parity_surface.property_attribute_readonly_modifiers_total =
      result.parity_surface.property_attribute_summary.readonly_modifiers;
  result.parity_surface.property_attribute_readwrite_modifiers_total =
      result.parity_surface.property_attribute_summary.readwrite_modifiers;
  result.parity_surface.property_attribute_atomic_modifiers_total =
      result.parity_surface.property_attribute_summary.atomic_modifiers;
  result.parity_surface.property_attribute_nonatomic_modifiers_total =
      result.parity_surface.property_attribute_summary.nonatomic_modifiers;
  result.parity_surface.property_attribute_copy_modifiers_total =
      result.parity_surface.property_attribute_summary.copy_modifiers;
  result.parity_surface.property_attribute_strong_modifiers_total =
      result.parity_surface.property_attribute_summary.strong_modifiers;
  result.parity_surface.property_attribute_weak_modifiers_total =
      result.parity_surface.property_attribute_summary.weak_modifiers;
  result.parity_surface.property_attribute_assign_modifiers_total =
      result.parity_surface.property_attribute_summary.assign_modifiers;
  result.parity_surface.property_attribute_getter_modifiers_total =
      result.parity_surface.property_attribute_summary.getter_modifiers;
  result.parity_surface.property_attribute_setter_modifiers_total =
      result.parity_surface.property_attribute_summary.setter_modifiers;
  result.parity_surface.property_attribute_invalid_attribute_entries_total =
      result.parity_surface.property_attribute_summary.invalid_attribute_entries;
  result.parity_surface.property_attribute_contract_violations_total =
      result.parity_surface.property_attribute_summary.property_contract_violations;
  result.parity_surface.type_annotation_surface_summary = result.type_metadata_handoff.type_annotation_surface_summary;
  result.parity_surface.type_annotation_generic_suffix_sites_total =
      result.parity_surface.type_annotation_surface_summary.generic_suffix_sites;
  result.parity_surface.type_annotation_pointer_declarator_sites_total =
      result.parity_surface.type_annotation_surface_summary.pointer_declarator_sites;
  result.parity_surface.type_annotation_nullability_suffix_sites_total =
      result.parity_surface.type_annotation_surface_summary.nullability_suffix_sites;
  result.parity_surface.type_annotation_object_pointer_type_sites_total =
      result.parity_surface.type_annotation_surface_summary.object_pointer_type_sites;
  result.parity_surface.type_annotation_invalid_generic_suffix_sites_total =
      result.parity_surface.type_annotation_surface_summary.invalid_generic_suffix_sites;
  result.parity_surface.type_annotation_invalid_pointer_declarator_sites_total =
      result.parity_surface.type_annotation_surface_summary.invalid_pointer_declarator_sites;
  result.parity_surface.type_annotation_invalid_nullability_suffix_sites_total =
      result.parity_surface.type_annotation_surface_summary.invalid_nullability_suffix_sites;
  result.parity_surface.symbol_graph_scope_resolution_summary =
      result.type_metadata_handoff.symbol_graph_scope_resolution_summary;
  result.parity_surface.symbol_graph_global_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.global_symbol_nodes;
  result.parity_surface.symbol_graph_function_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.function_symbol_nodes;
  result.parity_surface.symbol_graph_interface_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_symbol_nodes;
  result.parity_surface.symbol_graph_implementation_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_symbol_nodes;
  result.parity_surface.symbol_graph_interface_property_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_property_symbol_nodes;
  result.parity_surface.symbol_graph_implementation_property_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes;
  result.parity_surface.symbol_graph_interface_method_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_method_symbol_nodes;
  result.parity_surface.symbol_graph_implementation_method_symbol_nodes_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes;
  result.parity_surface.symbol_graph_top_level_scope_symbols_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols;
  result.parity_surface.symbol_graph_nested_scope_symbols_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.nested_scope_symbols;
  result.parity_surface.symbol_graph_scope_frames_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.scope_frames_total;
  result.parity_surface.symbol_graph_implementation_interface_resolution_sites_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites;
  result.parity_surface.symbol_graph_implementation_interface_resolution_hits_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits;
  result.parity_surface.symbol_graph_implementation_interface_resolution_misses_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses;
  result.parity_surface.symbol_graph_method_resolution_sites_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_sites;
  result.parity_surface.symbol_graph_method_resolution_hits_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_hits;
  result.parity_surface.symbol_graph_method_resolution_misses_total =
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_misses;
  result.parity_surface.diagnostics_after_pass_monotonic =
      IsMonotonicObjc3SemaDiagnosticsAfterPass(result.diagnostics_after_pass);
  result.parity_surface.deterministic_semantic_diagnostics = result.deterministic_semantic_diagnostics;
  result.parity_surface.deterministic_type_metadata_handoff = result.deterministic_type_metadata_handoff;
  result.parity_surface.deterministic_interface_implementation_handoff =
      result.deterministic_interface_implementation_handoff &&
      result.parity_surface.interfaces_total == result.parity_surface.type_metadata_interface_entries &&
      result.parity_surface.implementations_total == result.parity_surface.type_metadata_implementation_entries &&
      result.parity_surface.interface_implementation_summary.resolved_interfaces ==
          result.parity_surface.type_metadata_interface_entries &&
      result.parity_surface.interface_implementation_summary.resolved_implementations ==
          result.parity_surface.type_metadata_implementation_entries;
  result.parity_surface.deterministic_protocol_category_composition_handoff =
      result.deterministic_protocol_category_composition_handoff &&
      result.parity_surface.protocol_category_composition_summary.protocol_composition_sites ==
          result.parity_surface.protocol_composition_sites_total &&
      result.parity_surface.protocol_category_composition_summary.protocol_composition_symbols ==
          result.parity_surface.protocol_composition_symbols_total &&
      result.parity_surface.protocol_category_composition_summary.category_composition_sites ==
          result.parity_surface.category_composition_sites_total &&
      result.parity_surface.protocol_category_composition_summary.category_composition_symbols ==
          result.parity_surface.category_composition_symbols_total &&
      result.parity_surface.protocol_category_composition_summary.invalid_protocol_composition_sites ==
          result.parity_surface.invalid_protocol_composition_sites_total &&
      result.parity_surface.protocol_category_composition_summary.invalid_protocol_composition_sites <=
          result.parity_surface.protocol_category_composition_summary.total_composition_sites();
  result.parity_surface.deterministic_class_protocol_category_linking_handoff =
      result.deterministic_class_protocol_category_linking_handoff &&
      result.parity_surface.class_protocol_category_linking_summary.declared_interfaces ==
          result.parity_surface.interface_implementation_summary.declared_interfaces &&
      result.parity_surface.class_protocol_category_linking_summary.resolved_interfaces ==
          result.parity_surface.interface_implementation_summary.resolved_interfaces &&
      result.parity_surface.class_protocol_category_linking_summary.declared_implementations ==
          result.parity_surface.interface_implementation_summary.declared_implementations &&
      result.parity_surface.class_protocol_category_linking_summary.resolved_implementations ==
          result.parity_surface.interface_implementation_summary.resolved_implementations &&
      result.parity_surface.class_protocol_category_linking_summary.interface_method_symbols ==
          result.parity_surface.interface_method_symbols_total &&
      result.parity_surface.class_protocol_category_linking_summary.implementation_method_symbols ==
          result.parity_surface.implementation_method_symbols_total &&
      result.parity_surface.class_protocol_category_linking_summary.linked_implementation_symbols ==
          result.parity_surface.linked_implementation_symbols_total &&
      result.parity_surface.class_protocol_category_linking_summary.protocol_composition_sites ==
          result.parity_surface.protocol_composition_sites_total &&
      result.parity_surface.class_protocol_category_linking_summary.protocol_composition_symbols ==
          result.parity_surface.protocol_composition_symbols_total &&
      result.parity_surface.class_protocol_category_linking_summary.category_composition_sites ==
          result.parity_surface.category_composition_sites_total &&
      result.parity_surface.class_protocol_category_linking_summary.category_composition_symbols ==
          result.parity_surface.category_composition_symbols_total &&
      result.parity_surface.class_protocol_category_linking_summary.invalid_protocol_composition_sites ==
          result.parity_surface.invalid_protocol_composition_sites_total &&
      result.parity_surface.class_protocol_category_linking_summary.invalid_protocol_composition_sites <=
          result.parity_surface.class_protocol_category_linking_summary.total_composition_sites() &&
      result.parity_surface.class_protocol_category_linking_summary.deterministic;
  result.parity_surface.deterministic_selector_normalization_handoff =
      result.deterministic_selector_normalization_handoff &&
      result.parity_surface.selector_normalization_summary.methods_total ==
          result.parity_surface.selector_normalization_methods_total &&
      result.parity_surface.selector_normalization_summary.normalized_methods ==
          result.parity_surface.selector_normalization_normalized_methods_total &&
      result.parity_surface.selector_normalization_summary.selector_piece_entries ==
          result.parity_surface.selector_normalization_piece_entries_total &&
      result.parity_surface.selector_normalization_summary.selector_parameter_piece_entries ==
          result.parity_surface.selector_normalization_parameter_piece_entries_total &&
      result.parity_surface.selector_normalization_summary.selector_pieceless_methods ==
          result.parity_surface.selector_normalization_pieceless_methods_total &&
      result.parity_surface.selector_normalization_summary.selector_spelling_mismatches ==
          result.parity_surface.selector_normalization_spelling_mismatches_total &&
      result.parity_surface.selector_normalization_summary.selector_arity_mismatches ==
          result.parity_surface.selector_normalization_arity_mismatches_total &&
      result.parity_surface.selector_normalization_summary.selector_parameter_linkage_mismatches ==
          result.parity_surface.selector_normalization_parameter_linkage_mismatches_total &&
      result.parity_surface.selector_normalization_summary.selector_normalization_flag_mismatches ==
          result.parity_surface.selector_normalization_flag_mismatches_total &&
      result.parity_surface.selector_normalization_summary.selector_missing_keyword_pieces ==
          result.parity_surface.selector_normalization_missing_keyword_pieces_total &&
      result.parity_surface.selector_normalization_summary.normalized_methods <=
          result.parity_surface.selector_normalization_summary.methods_total &&
      result.parity_surface.selector_normalization_summary.selector_parameter_piece_entries <=
          result.parity_surface.selector_normalization_summary.selector_piece_entries &&
      result.parity_surface.selector_normalization_summary.contract_violations() <=
          result.parity_surface.selector_normalization_summary.methods_total &&
      result.parity_surface.selector_normalization_summary.deterministic;
  result.parity_surface.deterministic_property_attribute_handoff =
      result.deterministic_property_attribute_handoff &&
      result.parity_surface.property_attribute_summary.properties_total ==
          result.parity_surface.property_attribute_properties_total &&
      result.parity_surface.property_attribute_summary.attribute_entries ==
          result.parity_surface.property_attribute_entries_total &&
      result.parity_surface.property_attribute_summary.readonly_modifiers ==
          result.parity_surface.property_attribute_readonly_modifiers_total &&
      result.parity_surface.property_attribute_summary.readwrite_modifiers ==
          result.parity_surface.property_attribute_readwrite_modifiers_total &&
      result.parity_surface.property_attribute_summary.atomic_modifiers ==
          result.parity_surface.property_attribute_atomic_modifiers_total &&
      result.parity_surface.property_attribute_summary.nonatomic_modifiers ==
          result.parity_surface.property_attribute_nonatomic_modifiers_total &&
      result.parity_surface.property_attribute_summary.copy_modifiers ==
          result.parity_surface.property_attribute_copy_modifiers_total &&
      result.parity_surface.property_attribute_summary.strong_modifiers ==
          result.parity_surface.property_attribute_strong_modifiers_total &&
      result.parity_surface.property_attribute_summary.weak_modifiers ==
          result.parity_surface.property_attribute_weak_modifiers_total &&
      result.parity_surface.property_attribute_summary.assign_modifiers ==
          result.parity_surface.property_attribute_assign_modifiers_total &&
      result.parity_surface.property_attribute_summary.getter_modifiers ==
          result.parity_surface.property_attribute_getter_modifiers_total &&
      result.parity_surface.property_attribute_summary.setter_modifiers ==
          result.parity_surface.property_attribute_setter_modifiers_total &&
      result.parity_surface.property_attribute_summary.invalid_attribute_entries ==
          result.parity_surface.property_attribute_invalid_attribute_entries_total &&
      result.parity_surface.property_attribute_summary.property_contract_violations ==
          result.parity_surface.property_attribute_contract_violations_total &&
      result.parity_surface.property_attribute_summary.getter_modifiers <=
          result.parity_surface.property_attribute_summary.properties_total &&
      result.parity_surface.property_attribute_summary.setter_modifiers <=
          result.parity_surface.property_attribute_summary.properties_total &&
      result.parity_surface.property_attribute_summary.deterministic;
  result.parity_surface.deterministic_type_annotation_surface_handoff =
      result.deterministic_type_annotation_surface_handoff &&
      result.parity_surface.type_annotation_surface_summary.generic_suffix_sites ==
          result.parity_surface.type_annotation_generic_suffix_sites_total &&
      result.parity_surface.type_annotation_surface_summary.pointer_declarator_sites ==
          result.parity_surface.type_annotation_pointer_declarator_sites_total &&
      result.parity_surface.type_annotation_surface_summary.nullability_suffix_sites ==
          result.parity_surface.type_annotation_nullability_suffix_sites_total &&
      result.parity_surface.type_annotation_surface_summary.object_pointer_type_sites ==
          result.parity_surface.type_annotation_object_pointer_type_sites_total &&
      result.parity_surface.type_annotation_surface_summary.invalid_generic_suffix_sites ==
          result.parity_surface.type_annotation_invalid_generic_suffix_sites_total &&
      result.parity_surface.type_annotation_surface_summary.invalid_pointer_declarator_sites ==
          result.parity_surface.type_annotation_invalid_pointer_declarator_sites_total &&
      result.parity_surface.type_annotation_surface_summary.invalid_nullability_suffix_sites ==
          result.parity_surface.type_annotation_invalid_nullability_suffix_sites_total &&
      result.parity_surface.type_annotation_surface_summary.invalid_generic_suffix_sites <=
          result.parity_surface.type_annotation_surface_summary.generic_suffix_sites &&
      result.parity_surface.type_annotation_surface_summary.invalid_pointer_declarator_sites <=
          result.parity_surface.type_annotation_surface_summary.pointer_declarator_sites &&
      result.parity_surface.type_annotation_surface_summary.invalid_nullability_suffix_sites <=
          result.parity_surface.type_annotation_surface_summary.nullability_suffix_sites &&
      result.parity_surface.type_annotation_surface_summary.invalid_type_annotation_sites() <=
          result.parity_surface.type_annotation_surface_summary.total_type_annotation_sites() &&
      result.parity_surface.type_annotation_surface_summary.deterministic;
  result.parity_surface.deterministic_symbol_graph_scope_resolution_handoff =
      result.deterministic_symbol_graph_scope_resolution_handoff &&
      result.parity_surface.symbol_graph_scope_resolution_summary.global_symbol_nodes ==
          result.parity_surface.symbol_graph_global_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.function_symbol_nodes ==
          result.parity_surface.symbol_graph_function_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_symbol_nodes ==
          result.parity_surface.symbol_graph_interface_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_symbol_nodes ==
          result.parity_surface.symbol_graph_implementation_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_property_symbol_nodes ==
          result.parity_surface.symbol_graph_interface_property_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_property_symbol_nodes ==
          result.parity_surface.symbol_graph_implementation_property_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.interface_method_symbol_nodes ==
          result.parity_surface.symbol_graph_interface_method_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_method_symbol_nodes ==
          result.parity_surface.symbol_graph_implementation_method_symbol_nodes_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols ==
          result.parity_surface.symbol_graph_top_level_scope_symbols_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.nested_scope_symbols ==
          result.parity_surface.symbol_graph_nested_scope_symbols_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.scope_frames_total ==
          result.parity_surface.symbol_graph_scope_frames_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites ==
          result.parity_surface.symbol_graph_implementation_interface_resolution_sites_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits ==
          result.parity_surface.symbol_graph_implementation_interface_resolution_hits_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_misses ==
          result.parity_surface.symbol_graph_implementation_interface_resolution_misses_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_sites ==
          result.parity_surface.symbol_graph_method_resolution_sites_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_hits ==
          result.parity_surface.symbol_graph_method_resolution_hits_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_misses ==
          result.parity_surface.symbol_graph_method_resolution_misses_total &&
      result.parity_surface.symbol_graph_scope_resolution_summary.symbol_nodes_total() ==
          result.parity_surface.symbol_graph_scope_resolution_summary.top_level_scope_symbols +
              result.parity_surface.symbol_graph_scope_resolution_summary.nested_scope_symbols &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits <=
          result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
      result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_hits +
              result.parity_surface.symbol_graph_scope_resolution_summary
                  .implementation_interface_resolution_misses ==
          result.parity_surface.symbol_graph_scope_resolution_summary.implementation_interface_resolution_sites &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_hits <=
          result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_hits +
              result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_misses ==
          result.parity_surface.symbol_graph_scope_resolution_summary.method_resolution_sites &&
      result.parity_surface.symbol_graph_scope_resolution_summary.resolution_hits_total() <=
          result.parity_surface.symbol_graph_scope_resolution_summary.resolution_sites_total() &&
      result.parity_surface.symbol_graph_scope_resolution_summary.resolution_hits_total() +
              result.parity_surface.symbol_graph_scope_resolution_summary.resolution_misses_total() ==
          result.parity_surface.symbol_graph_scope_resolution_summary.resolution_sites_total() &&
      result.parity_surface.symbol_graph_scope_resolution_summary.deterministic;
  result.parity_surface.atomic_memory_order_mapping = result.atomic_memory_order_mapping;
  result.parity_surface.deterministic_atomic_memory_order_mapping = result.deterministic_atomic_memory_order_mapping;
  result.parity_surface.vector_type_lowering = result.vector_type_lowering;
  result.parity_surface.deterministic_vector_type_lowering = result.deterministic_vector_type_lowering;
  result.parity_surface.ready =
      result.executed && result.parity_surface.diagnostics_after_pass_monotonic &&
      result.parity_surface.deterministic_semantic_diagnostics &&
      result.parity_surface.deterministic_type_metadata_handoff &&
      result.parity_surface.deterministic_atomic_memory_order_mapping &&
      result.parity_surface.deterministic_vector_type_lowering &&
      result.parity_surface.atomic_memory_order_mapping.deterministic &&
      result.parity_surface.vector_type_lowering.deterministic &&
      result.parity_surface.globals_total == result.parity_surface.type_metadata_global_entries &&
      result.parity_surface.functions_total == result.parity_surface.type_metadata_function_entries &&
      result.parity_surface.interfaces_total == result.parity_surface.type_metadata_interface_entries &&
      result.parity_surface.implementations_total == result.parity_surface.type_metadata_implementation_entries &&
      result.parity_surface.interface_implementation_summary.deterministic &&
      result.parity_surface.deterministic_interface_implementation_handoff &&
      result.parity_surface.protocol_category_composition_summary.deterministic &&
      result.parity_surface.deterministic_protocol_category_composition_handoff &&
      result.parity_surface.class_protocol_category_linking_summary.deterministic &&
      result.parity_surface.deterministic_class_protocol_category_linking_handoff &&
      result.parity_surface.selector_normalization_summary.deterministic &&
      result.parity_surface.deterministic_selector_normalization_handoff &&
      result.parity_surface.property_attribute_summary.deterministic &&
      result.parity_surface.deterministic_property_attribute_handoff &&
      result.parity_surface.type_annotation_surface_summary.deterministic &&
      result.parity_surface.deterministic_type_annotation_surface_handoff &&
      result.parity_surface.symbol_graph_scope_resolution_summary.deterministic &&
      result.parity_surface.deterministic_symbol_graph_scope_resolution_handoff;
  return result;
}
