#pragma once

#include <cstddef>
#include <string>

#include "parse/objc3_diagnostic_grammar_hooks_core_feature_expansion_surface.h"
#include "pipeline/objc3_frontend_types.h"

struct Objc3DiagnosticGrammarHooksEdgeCaseConsistencySurface {
  std::size_t parser_diagnostic_count = 0;
  std::size_t parser_snapshot_diagnostic_count = 0;
  std::size_t parser_token_count = 0;
  bool language_profile_supported = false;
  bool core_feature_expansion_ready = false;
  bool parser_snapshot_accounting_consistent = false;
  bool parser_diagnostic_token_budget_consistent = false;
  bool language_version_pragma_coordinate_order_consistent = false;
  bool consistency_handoff_consistent = false;
  bool edge_case_consistency_consistent = false;
  bool edge_case_consistency_ready = false;
  std::string consistency_key;
  std::string failure_reason;
};

bool IsObjc3DiagnosticGrammarHooksPragmaCoordinateOrderConsistent(
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract);

std::string BuildObjc3DiagnosticGrammarHooksEdgeCaseConsistencyKey(
    const Objc3DiagnosticGrammarHooksEdgeCaseConsistencySurface &surface,
    const std::string &core_feature_expansion_key);

Objc3DiagnosticGrammarHooksEdgeCaseConsistencySurface
BuildObjc3DiagnosticGrammarHooksEdgeCaseConsistencySurface(
    const Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface
        &core_feature_expansion_surface,
    const Objc3FrontendOptions &options,
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract,
    std::size_t parser_diagnostic_count,
    std::size_t parser_snapshot_diagnostic_count,
    std::size_t parser_token_count,
    bool consistency_handoff_consistent);

bool IsObjc3DiagnosticGrammarHooksEdgeCaseConsistencySurfaceReady(
    const Objc3DiagnosticGrammarHooksEdgeCaseConsistencySurface &surface);
