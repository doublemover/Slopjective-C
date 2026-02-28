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
      result.parity_surface.selector_normalization_summary.deterministic &&
      result.parity_surface.deterministic_selector_normalization_handoff;
  return result;
}
