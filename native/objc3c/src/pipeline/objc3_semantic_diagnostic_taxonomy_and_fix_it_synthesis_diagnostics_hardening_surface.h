#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_edge_case_expansion_and_robustness_surface.h"

inline std::string BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningKey(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface &surface) {
  std::ostringstream key;
  key << "semantic-diagnostic-taxonomy-fixit-diagnostics-hardening:v1:"
      << "edge_case_robustness_ready=" << (surface.edge_case_robustness_ready ? "true" : "false")
      << ";diag-hardening-consistent=" << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";diag-hardening-ready=" << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";diag-hardening-key-ready=" << (!surface.diagnostics_hardening_key.empty() ? "true" : "false")
      << ";edge-case-robustness-key-ready=" << (!surface.edge_case_robustness_key.empty() ? "true" : "false")
      << ";parse-diag-hardening-key-ready=" << (!surface.parse_artifact_diagnostics_hardening_key.empty() ? "true" : "false")
      << ";long-tail-diag-hardening-key-ready="
      << (!surface.long_tail_grammar_diagnostics_hardening_key.empty() ? "true" : "false")
      << ";typed-handoff-key-ready=" << (!surface.typed_handoff_key.empty() ? "true" : "false")
      << ";parser_diagnostic_code_count=" << surface.parser_diagnostic_code_count
      << ";parser_diagnostic_grammar_hook_code_count="
      << surface.parser_diagnostic_grammar_hook_code_count
      << ";parser_diagnostic_coordinate_tagged_count="
      << surface.parser_diagnostic_coordinate_tagged_count
      << ";edge_case_robustness_key=" << surface.edge_case_robustness_key
      << ";parse_artifact_diagnostics_hardening_key="
      << surface.parse_artifact_diagnostics_hardening_key
      << ";long_tail_grammar_diagnostics_hardening_key="
      << surface.long_tail_grammar_diagnostics_hardening_key
      << ";typed_handoff_key=" << surface.typed_handoff_key;
  return key.str();
}

inline Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface
BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisEdgeCaseExpansionAndRobustnessSurface &edge_surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface) {
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface surface;
  surface.edge_case_robustness_ready = edge_surface.edge_case_robustness_ready;
  surface.edge_case_expansion_consistent = edge_surface.edge_case_expansion_consistent;
  surface.parser_diagnostic_surface_consistent = parse_surface.parser_diagnostic_surface_consistent;
  surface.parser_diagnostic_code_surface_deterministic =
      parse_surface.parser_diagnostic_code_surface_deterministic;
  surface.parse_artifact_diagnostics_hardening_consistent =
      parse_surface.parse_artifact_diagnostics_hardening_consistent;
  surface.long_tail_grammar_diagnostics_hardening_consistent =
      parse_surface.long_tail_grammar_diagnostics_hardening_consistent;
  surface.long_tail_grammar_diagnostics_hardening_ready =
      parse_surface.long_tail_grammar_diagnostics_hardening_ready;
  surface.semantic_diagnostics_deterministic = parse_surface.semantic_diagnostics_deterministic;
  surface.parser_diagnostic_code_count = parse_surface.parser_diagnostic_code_count;
  surface.parser_diagnostic_grammar_hook_code_count =
      parse_surface.parser_diagnostic_grammar_hook_code_count;
  surface.parser_diagnostic_coordinate_tagged_count =
      parse_surface.parser_diagnostic_coordinate_tagged_count;
  surface.edge_case_robustness_key = edge_surface.edge_case_robustness_key;
  surface.parse_artifact_diagnostics_hardening_key =
      parse_surface.parse_artifact_diagnostics_hardening_key;
  surface.long_tail_grammar_diagnostics_hardening_key =
      parse_surface.long_tail_grammar_diagnostics_hardening_key;
  surface.typed_handoff_key = edge_surface.typed_handoff_key;

  const bool diagnostics_hardening_consistent =
      surface.edge_case_expansion_consistent &&
      surface.parser_diagnostic_surface_consistent &&
      surface.parser_diagnostic_code_surface_deterministic &&
      surface.parse_artifact_diagnostics_hardening_consistent &&
      surface.long_tail_grammar_diagnostics_hardening_consistent &&
      surface.long_tail_grammar_diagnostics_hardening_ready &&
      surface.semantic_diagnostics_deterministic &&
      surface.parser_diagnostic_grammar_hook_code_count <= surface.parser_diagnostic_code_count &&
      surface.parser_diagnostic_coordinate_tagged_count <= surface.parser_diagnostic_code_count;
  const bool diagnostics_hardening_ready =
      surface.edge_case_robustness_ready &&
      diagnostics_hardening_consistent &&
      !surface.edge_case_robustness_key.empty() &&
      !surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.long_tail_grammar_diagnostics_hardening_key.empty() &&
      !surface.typed_handoff_key.empty();

  surface.diagnostics_hardening_consistent = diagnostics_hardening_consistent;
  surface.diagnostics_hardening_ready = diagnostics_hardening_ready;
  surface.diagnostics_hardening_key =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningKey(surface);
  surface.diagnostics_hardening_ready =
      surface.diagnostics_hardening_ready &&
      !surface.diagnostics_hardening_key.empty();

  if (surface.diagnostics_hardening_ready) {
    return surface;
  }

  if (!surface.edge_case_robustness_ready) {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it edge-case robustness is not ready";
  } else if (!surface.diagnostics_hardening_consistent) {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it diagnostics hardening is inconsistent";
  } else {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it diagnostics hardening is not ready";
  }

  return surface;
}

inline bool IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurfaceReady(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface &surface,
    std::string &reason) {
  if (surface.diagnostics_hardening_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "semantic diagnostic taxonomy/fix-it diagnostics hardening is not ready"
               : surface.failure_reason;
  return false;
}
