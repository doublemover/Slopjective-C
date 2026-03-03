#pragma once

#include <cstddef>
#include <sstream>
#include <string>

#include "parse/objc3_diagnostic_grammar_hooks_core_feature_expansion_surface.h"
#include "pipeline/objc3_frontend_types.h"

struct Objc3DiagnosticGrammarHooksEdgeCaseCompatibilitySurface {
  std::size_t parser_diagnostic_count = 0;
  std::size_t parser_snapshot_diagnostic_count = 0;
  std::size_t parser_token_count = 0;
  bool compatibility_mode_supported = false;
  bool core_feature_expansion_ready = false;
  bool parser_snapshot_accounting_consistent = false;
  bool parser_diagnostic_token_budget_consistent = false;
  bool language_version_pragma_coordinate_order_consistent = false;
  bool compatibility_handoff_consistent = false;
  bool edge_case_compatibility_consistent = false;
  bool edge_case_compatibility_ready = false;
  std::string compatibility_key;
  std::string failure_reason;
};

inline bool IsObjc3DiagnosticGrammarHooksPragmaCoordinateOrderConsistent(
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

inline std::string BuildObjc3DiagnosticGrammarHooksEdgeCaseCompatibilityKey(
    const Objc3DiagnosticGrammarHooksEdgeCaseCompatibilitySurface &surface,
    const std::string &core_feature_expansion_key) {
  std::ostringstream key;
  key << "parser-diagnostic-grammar-hooks-edge-case-compatibility:v1:"
      << "parser_diagnostic_count=" << surface.parser_diagnostic_count
      << ";parser_snapshot_diagnostic_count=" << surface.parser_snapshot_diagnostic_count
      << ";parser_token_count=" << surface.parser_token_count
      << ";compatibility_mode_supported="
      << (surface.compatibility_mode_supported ? "true" : "false")
      << ";core_feature_expansion_ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";parser_snapshot_accounting_consistent="
      << (surface.parser_snapshot_accounting_consistent ? "true" : "false")
      << ";parser_diagnostic_token_budget_consistent="
      << (surface.parser_diagnostic_token_budget_consistent ? "true" : "false")
      << ";language_version_pragma_coordinate_order_consistent="
      << (surface.language_version_pragma_coordinate_order_consistent ? "true" : "false")
      << ";compatibility_handoff_consistent="
      << (surface.compatibility_handoff_consistent ? "true" : "false")
      << ";edge_case_compatibility_consistent="
      << (surface.edge_case_compatibility_consistent ? "true" : "false")
      << ";edge_case_compatibility_ready="
      << (surface.edge_case_compatibility_ready ? "true" : "false")
      << ";core_feature_expansion_key=" << core_feature_expansion_key;
  return key.str();
}

inline Objc3DiagnosticGrammarHooksEdgeCaseCompatibilitySurface
BuildObjc3DiagnosticGrammarHooksEdgeCaseCompatibilitySurface(
    const Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface
        &core_feature_expansion_surface,
    const Objc3FrontendOptions &options,
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract,
    std::size_t parser_diagnostic_count,
    std::size_t parser_snapshot_diagnostic_count,
    std::size_t parser_token_count,
    bool compatibility_handoff_consistent) {
  Objc3DiagnosticGrammarHooksEdgeCaseCompatibilitySurface surface;
  surface.parser_diagnostic_count = parser_diagnostic_count;
  surface.parser_snapshot_diagnostic_count = parser_snapshot_diagnostic_count;
  surface.parser_token_count = parser_token_count;
  surface.compatibility_mode_supported =
      options.compatibility_mode == Objc3FrontendCompatibilityMode::kCanonical ||
      options.compatibility_mode == Objc3FrontendCompatibilityMode::kLegacy;
  surface.core_feature_expansion_ready =
      IsObjc3DiagnosticGrammarHooksCoreFeatureExpansionReady(
          core_feature_expansion_surface);
  surface.parser_snapshot_accounting_consistent =
      surface.parser_snapshot_diagnostic_count == surface.parser_diagnostic_count;
  surface.parser_diagnostic_token_budget_consistent =
      surface.parser_diagnostic_count <= surface.parser_token_count;
  surface.language_version_pragma_coordinate_order_consistent =
      IsObjc3DiagnosticGrammarHooksPragmaCoordinateOrderConsistent(pragma_contract);
  surface.compatibility_handoff_consistent = compatibility_handoff_consistent;
  surface.edge_case_compatibility_consistent =
      surface.compatibility_mode_supported &&
      surface.core_feature_expansion_ready &&
      surface.parser_snapshot_accounting_consistent &&
      surface.parser_diagnostic_token_budget_consistent &&
      surface.language_version_pragma_coordinate_order_consistent &&
      surface.compatibility_handoff_consistent;
  surface.edge_case_compatibility_ready =
      surface.edge_case_compatibility_consistent &&
      !core_feature_expansion_surface.expansion_key.empty();
  surface.compatibility_key =
      BuildObjc3DiagnosticGrammarHooksEdgeCaseCompatibilityKey(
          surface,
          core_feature_expansion_surface.expansion_key);
  if (surface.edge_case_compatibility_ready) {
    return surface;
  }

  if (!surface.compatibility_mode_supported) {
    surface.failure_reason =
        "parser diagnostic grammar hooks compatibility mode is not supported";
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
  } else if (!surface.compatibility_handoff_consistent) {
    surface.failure_reason =
        "parser diagnostic grammar hooks compatibility handoff is inconsistent";
  } else {
    surface.failure_reason =
        "parser diagnostic grammar hooks edge-case compatibility is not ready";
  }
  return surface;
}

inline bool IsObjc3DiagnosticGrammarHooksEdgeCaseCompatibilitySurfaceReady(
    const Objc3DiagnosticGrammarHooksEdgeCaseCompatibilitySurface &surface) {
  return surface.edge_case_compatibility_ready && !surface.compatibility_key.empty();
}
