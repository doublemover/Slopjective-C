#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"
#include "pipeline/objc3_semantic_diagnostic_taxonomy_and_fix_it_synthesis_recovery_determinism_hardening_surface.h"

inline std::string BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationKey(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface &surface) {
  std::ostringstream key;
  key << "semantic-diagnostic-taxonomy-fixit-conformance-matrix-implementation:v1:"
      << "recovery-determinism-consistent=" << (surface.recovery_determinism_consistent ? "true" : "false")
      << ";recovery-determinism-ready=" << (surface.recovery_determinism_ready ? "true" : "false")
      << ";parse-conformance-matrix-consistent="
      << (surface.parse_lowering_conformance_matrix_consistent ? "true" : "false")
      << ";long-tail-conformance-matrix-consistent="
      << (surface.long_tail_grammar_conformance_matrix_consistent ? "true" : "false")
      << ";long-tail-conformance-matrix-ready="
      << (surface.long_tail_grammar_conformance_matrix_ready ? "true" : "false")
      << ";conformance-matrix-consistent="
      << (surface.conformance_matrix_consistent ? "true" : "false")
      << ";conformance-matrix-ready="
      << (surface.conformance_matrix_ready ? "true" : "false")
      << ";conformance-matrix-key-ready="
      << (!surface.conformance_matrix_key.empty() ? "true" : "false")
      << ";parse-conformance-matrix-case-count="
      << surface.parse_lowering_conformance_matrix_case_count
      << ";recovery-determinism-key-ready="
      << (!surface.recovery_determinism_key.empty() ? "true" : "false")
      << ";parse-conformance-matrix-key-ready="
      << (!surface.parse_lowering_conformance_matrix_key.empty() ? "true" : "false")
      << ";long-tail-conformance-matrix-key-ready="
      << (!surface.long_tail_grammar_conformance_matrix_key.empty() ? "true" : "false");
  return key.str();
}

inline Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface
BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurface &recovery_surface,
    const Objc3ParseLoweringReadinessSurface &parse_surface) {
  Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface surface;
  surface.recovery_determinism_consistent = recovery_surface.recovery_determinism_consistent;
  surface.recovery_determinism_ready = recovery_surface.recovery_determinism_ready;
  surface.parse_lowering_conformance_matrix_consistent =
      parse_surface.parse_lowering_conformance_matrix_consistent;
  surface.long_tail_grammar_conformance_matrix_consistent =
      parse_surface.long_tail_grammar_conformance_matrix_consistent;
  surface.long_tail_grammar_conformance_matrix_ready =
      parse_surface.long_tail_grammar_conformance_matrix_ready;
  surface.parse_lowering_conformance_matrix_case_count =
      parse_surface.parse_lowering_conformance_matrix_case_count;
  surface.recovery_determinism_key = recovery_surface.recovery_determinism_key;
  surface.parse_lowering_conformance_matrix_key =
      parse_surface.parse_lowering_conformance_matrix_key;
  surface.long_tail_grammar_conformance_matrix_key =
      parse_surface.long_tail_grammar_conformance_matrix_key;

  const bool conformance_matrix_consistent =
      surface.recovery_determinism_consistent &&
      surface.recovery_determinism_ready &&
      surface.parse_lowering_conformance_matrix_consistent &&
      surface.long_tail_grammar_conformance_matrix_consistent &&
      surface.long_tail_grammar_conformance_matrix_ready;
  const bool conformance_matrix_ready =
      conformance_matrix_consistent &&
      surface.parse_lowering_conformance_matrix_case_count > 0 &&
      !surface.recovery_determinism_key.empty() &&
      !surface.parse_lowering_conformance_matrix_key.empty() &&
      !surface.long_tail_grammar_conformance_matrix_key.empty();

  surface.conformance_matrix_consistent = conformance_matrix_consistent;
  surface.conformance_matrix_ready = conformance_matrix_ready;
  surface.conformance_matrix_key =
      BuildObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationKey(surface);
  surface.conformance_matrix_ready =
      surface.conformance_matrix_ready &&
      !surface.conformance_matrix_key.empty();

  if (surface.conformance_matrix_ready) {
    return surface;
  }

  if (!surface.recovery_determinism_ready) {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it recovery determinism is not ready";
  } else if (!surface.conformance_matrix_consistent) {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it conformance matrix is inconsistent";
  } else {
    surface.failure_reason =
        "semantic diagnostic taxonomy/fix-it conformance matrix is not ready";
  }

  return surface;
}

inline bool IsObjc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurfaceReady(
    const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface &surface,
    std::string &reason) {
  if (surface.conformance_matrix_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "semantic diagnostic taxonomy/fix-it conformance matrix is not ready"
               : surface.failure_reason;
  return false;
}
