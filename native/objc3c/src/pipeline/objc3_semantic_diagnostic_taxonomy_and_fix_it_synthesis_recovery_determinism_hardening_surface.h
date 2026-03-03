#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_diagnostics_hardening_surface.h"

inline std::string BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningKey(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurface &surface) {
  std::ostringstream key;
  key << "semantic-diagnostic-taxonomy-fixit-recovery-determinism-hardening:v1:"
      << "diag-hardening-consistent=" << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";diag-hardening-ready=" << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";parse-recovery-consistent="
      << (surface.parse_recovery_determinism_hardening_consistent ? "true" : "false")
      << ";long-tail-recovery-consistent="
      << (surface.long_tail_grammar_recovery_determinism_consistent ? "true" : "false")
      << ";long-tail-recovery-ready="
      << (surface.long_tail_grammar_recovery_determinism_ready ? "true" : "false")
      << ";parser-recovery-replay-ready="
      << (surface.parser_recovery_replay_ready ? "true" : "false")
      << ";recovery-determinism-consistent="
      << (surface.recovery_determinism_consistent ? "true" : "false")
      << ";recovery-determinism-ready="
      << (surface.recovery_determinism_ready ? "true" : "false")
      << ";recovery-determinism-key-ready="
      << (!surface.recovery_determinism_key.empty() ? "true" : "false")
      << ";diagnostics-hardening-key-ready="
      << (!surface.diagnostics_hardening_key.empty() ? "true" : "false")
      << ";parse-recovery-key-ready="
      << (!surface.parse_recovery_determinism_hardening_key.empty() ? "true" : "false")
      << ";long-tail-recovery-key-ready="
      << (!surface.long_tail_grammar_recovery_determinism_key.empty() ? "true" : "false")
      << ";edge-case-robustness-key-ready="
      << (!surface.edge_case_robustness_key.empty() ? "true" : "false")
      << ";parse-diag-hardening-key-ready="
      << (!surface.parse_artifact_diagnostics_hardening_key.empty() ? "true" : "false")
      << ";typed-handoff-key-ready="
      << (!surface.typed_handoff_key.empty() ? "true" : "false")
      << ";parser_diagnostic_code_count=" << surface.parser_diagnostic_code_count
      << ";parser_diagnostic_grammar_hook_code_count="
      << surface.parser_diagnostic_grammar_hook_code_count
      << ";parser_diagnostic_coordinate_tagged_count="
      << surface.parser_diagnostic_coordinate_tagged_count
      << ";edge_case_robustness_key=" << surface.edge_case_robustness_key
      << ";parse_artifact_diagnostics_hardening_key="
      << surface.parse_artifact_diagnostics_hardening_key
      << ";typed_handoff_key=" << surface.typed_handoff_key
      << ";diagnostics_hardening_key=" << surface.diagnostics_hardening_key
      << ";parse_recovery_determinism_hardening_key="
      << surface.parse_recovery_determinism_hardening_key
      << ";long_tail_grammar_recovery_determinism_key="
      << surface.long_tail_grammar_recovery_determinism_key;
  return key.str();
}

inline Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurface
BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurface(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface &diagnostics_surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface) {
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurface surface;
  surface.diagnostics_hardening_consistent = diagnostics_surface.diagnostics_hardening_consistent;
  surface.diagnostics_hardening_ready = diagnostics_surface.diagnostics_hardening_ready;
  surface.parse_recovery_determinism_hardening_consistent =
      parse_surface.parse_recovery_determinism_hardening_consistent;
  surface.long_tail_grammar_recovery_determinism_consistent =
      parse_surface.long_tail_grammar_recovery_determinism_consistent;
  surface.long_tail_grammar_recovery_determinism_ready =
      parse_surface.long_tail_grammar_recovery_determinism_ready;
  surface.parser_recovery_replay_ready = parse_surface.parser_recovery_replay_ready;
  surface.parser_diagnostic_code_count = diagnostics_surface.parser_diagnostic_code_count;
  surface.parser_diagnostic_grammar_hook_code_count =
      diagnostics_surface.parser_diagnostic_grammar_hook_code_count;
  surface.parser_diagnostic_coordinate_tagged_count =
      diagnostics_surface.parser_diagnostic_coordinate_tagged_count;
  surface.edge_case_robustness_key = diagnostics_surface.edge_case_robustness_key;
  surface.parse_artifact_diagnostics_hardening_key =
      diagnostics_surface.parse_artifact_diagnostics_hardening_key;
  surface.typed_handoff_key = diagnostics_surface.typed_handoff_key;
  surface.diagnostics_hardening_key = diagnostics_surface.diagnostics_hardening_key;
  surface.parse_recovery_determinism_hardening_key =
      parse_surface.parse_recovery_determinism_hardening_key;
  surface.long_tail_grammar_recovery_determinism_key =
      parse_surface.long_tail_grammar_recovery_determinism_key;

  const bool recovery_determinism_consistent =
      surface.diagnostics_hardening_consistent &&
      surface.diagnostics_hardening_ready &&
      surface.parse_recovery_determinism_hardening_consistent &&
      surface.long_tail_grammar_recovery_determinism_consistent &&
      surface.long_tail_grammar_recovery_determinism_ready &&
      surface.parser_recovery_replay_ready &&
      surface.parser_diagnostic_grammar_hook_code_count <= surface.parser_diagnostic_code_count &&
      surface.parser_diagnostic_coordinate_tagged_count <= surface.parser_diagnostic_code_count;
  const bool recovery_determinism_ready =
      surface.diagnostics_hardening_ready &&
      recovery_determinism_consistent &&
      !surface.edge_case_robustness_key.empty() &&
      !surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.typed_handoff_key.empty() &&
      !surface.diagnostics_hardening_key.empty() &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.long_tail_grammar_recovery_determinism_key.empty();

  surface.recovery_determinism_consistent = recovery_determinism_consistent;
  surface.recovery_determinism_ready = recovery_determinism_ready;
  surface.recovery_determinism_key =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningKey(surface);
  surface.recovery_determinism_ready =
      surface.recovery_determinism_ready &&
      !surface.recovery_determinism_key.empty();

  if (surface.recovery_determinism_ready) {
    return surface;
  }

  if (!surface.diagnostics_hardening_ready) {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it diagnostics hardening is not ready";
  } else if (!surface.recovery_determinism_consistent) {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it recovery determinism is inconsistent";
  } else {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it recovery determinism is not ready";
  }

  return surface;
}

inline bool IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurfaceReady(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurface &surface,
    std::string &reason) {
  if (surface.recovery_determinism_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "semantic diagnostic taxonomy/fix-it recovery determinism is not ready"
               : surface.failure_reason;
  return false;
}
