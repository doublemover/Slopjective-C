#include "parse/objc3_diagnostic_grammar_hooks_edge_case_consistency_surface.h"

#include <sstream>

bool IsObjc3DiagnosticGrammarHooksPragmaCoordinateOrderConsistent(
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract) {
  if (!pragma_contract.seen) {
    return true;
  }
  const bool first_before_or_equal_last =
      pragma_contract.first_line < pragma_contract.last_line ||
      (pragma_contract.first_line == pragma_contract.last_line &&
       pragma_contract.first_column <= pragma_contract.last_column);
  const bool single_directive_coordinates_consistent =
      pragma_contract.directive_count != 1 ||
      (pragma_contract.first_line == pragma_contract.last_line &&
       pragma_contract.first_column == pragma_contract.last_column);
  return first_before_or_equal_last &&
         single_directive_coordinates_consistent;
}

std::string BuildObjc3DiagnosticGrammarHooksEdgeCaseConsistencyKey(
    const Objc3DiagnosticGrammarHooksEdgeCaseConsistencySurface &surface,
    const std::string &core_feature_expansion_key) {
  std::ostringstream key;
  key << "parser-diagnostic-grammar-hooks-edge-case-consistency:v1:"
      << "parser_diagnostic_count=" << surface.parser_diagnostic_count
      << ";parser_snapshot_diagnostic_count=" << surface.parser_snapshot_diagnostic_count
      << ";parser_token_count=" << surface.parser_token_count
      << ";language_profile_supported="
      << (surface.language_profile_supported ? "true" : "false")
      << ";core_feature_expansion_ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";parser_snapshot_accounting_consistent="
      << (surface.parser_snapshot_accounting_consistent ? "true" : "false")
      << ";parser_diagnostic_token_budget_consistent="
      << (surface.parser_diagnostic_token_budget_consistent ? "true" : "false")
      << ";language_version_pragma_coordinate_order_consistent="
      << (surface.language_version_pragma_coordinate_order_consistent ? "true" : "false")
      << ";consistency_handoff_consistent="
      << (surface.consistency_handoff_consistent ? "true" : "false")
      << ";edge_case_consistency_consistent="
      << (surface.edge_case_consistency_consistent ? "true" : "false")
      << ";edge_case_consistency_ready="
      << (surface.edge_case_consistency_ready ? "true" : "false")
      << ";core_feature_expansion_key=" << core_feature_expansion_key;
  return key.str();
}

Objc3DiagnosticGrammarHooksEdgeCaseConsistencySurface
BuildObjc3DiagnosticGrammarHooksEdgeCaseConsistencySurface(
    const Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface
        &core_feature_expansion_surface,
    const Objc3FrontendOptions &options,
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract,
    std::size_t parser_diagnostic_count,
    std::size_t parser_snapshot_diagnostic_count,
    std::size_t parser_token_count,
    bool consistency_handoff_consistent) {
  Objc3DiagnosticGrammarHooksEdgeCaseConsistencySurface surface;
  surface.parser_diagnostic_count = parser_diagnostic_count;
  surface.parser_snapshot_diagnostic_count = parser_snapshot_diagnostic_count;
  surface.parser_token_count = parser_token_count;
  surface.language_profile_supported =
      options.language_profile == Objc3FrontendLanguageProfile::kCanonical;
  surface.core_feature_expansion_ready =
      IsObjc3DiagnosticGrammarHooksCoreFeatureExpansionReady(
          core_feature_expansion_surface);
  surface.parser_snapshot_accounting_consistent =
      surface.parser_snapshot_diagnostic_count == surface.parser_diagnostic_count;
  surface.parser_diagnostic_token_budget_consistent =
      surface.parser_diagnostic_count <= surface.parser_token_count;
  surface.language_version_pragma_coordinate_order_consistent =
      IsObjc3DiagnosticGrammarHooksPragmaCoordinateOrderConsistent(pragma_contract);
  surface.consistency_handoff_consistent = consistency_handoff_consistent;
  surface.edge_case_consistency_consistent =
      surface.language_profile_supported &&
      surface.core_feature_expansion_ready &&
      surface.parser_snapshot_accounting_consistent &&
      surface.parser_diagnostic_token_budget_consistent &&
      surface.language_version_pragma_coordinate_order_consistent &&
      surface.consistency_handoff_consistent;
  surface.edge_case_consistency_ready =
      surface.edge_case_consistency_consistent &&
      !core_feature_expansion_surface.expansion_key.empty();
  surface.consistency_key =
      BuildObjc3DiagnosticGrammarHooksEdgeCaseConsistencyKey(
          surface,
          core_feature_expansion_surface.expansion_key);
  if (surface.edge_case_consistency_ready) {
    return surface;
  }

  if (!surface.language_profile_supported) {
    surface.failure_reason =
        "parser diagnostic grammar hooks require the canonical language profile";
  } else if (!surface.core_feature_expansion_ready) {
    surface.failure_reason =
        "parser diagnostic grammar hooks core feature expansion is not ready";
  } else if (!surface.parser_snapshot_accounting_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks snapshot accounting is inconsistent";
  } else if (!surface.parser_diagnostic_token_budget_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks token budget is inconsistent";
  } else if (!surface.language_version_pragma_coordinate_order_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks language-version pragma coordinate order is inconsistent";
  } else if (!surface.consistency_handoff_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks consistency handoff is inconsistent";
  } else {
    surface.failure_reason =
        "parser diagnostic grammar hooks edge-case consistency is not ready";
  }
  return surface;
}

bool IsObjc3DiagnosticGrammarHooksEdgeCaseConsistencySurfaceReady(
    const Objc3DiagnosticGrammarHooksEdgeCaseConsistencySurface &surface) {
  return surface.edge_case_consistency_ready && !surface.consistency_key.empty();
}
