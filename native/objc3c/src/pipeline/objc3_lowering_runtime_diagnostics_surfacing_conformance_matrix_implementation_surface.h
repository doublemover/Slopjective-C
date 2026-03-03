#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline std::string
BuildObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
        &surface) {
  std::ostringstream key;
  key << "lowering-runtime-diagnostics-surfacing-conformance-matrix-implementation:v1:"
      << "recovery-determinism-consistent="
      << (surface.recovery_determinism_consistent ? "true" : "false")
      << ";recovery-determinism-ready="
      << (surface.recovery_determinism_ready ? "true" : "false")
      << ";parse-conformance-matrix-consistent="
      << (surface.parse_conformance_matrix_consistent ? "true" : "false")
      << ";parse-conformance-matrix-ready="
      << (surface.parse_conformance_matrix_ready ? "true" : "false")
      << ";semantic-conformance-matrix-consistent="
      << (surface.semantic_conformance_matrix_consistent ? "true" : "false")
      << ";semantic-conformance-matrix-ready="
      << (surface.semantic_conformance_matrix_ready ? "true" : "false")
      << ";lowering-pipeline-conformance-matrix-ready="
      << (surface.lowering_pipeline_conformance_matrix_ready ? "true" : "false")
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
      << (!surface.parse_lowering_conformance_matrix_key.empty() ? "true"
                                                                  : "false")
      << ";long-tail-conformance-matrix-key-ready="
      << (!surface.long_tail_grammar_conformance_matrix_key.empty() ? "true"
                                                                     : "false")
      << ";parser-hooks-conformance-matrix-key-ready="
      << (!surface
               .parser_diagnostic_grammar_hooks_conformance_matrix_key.empty()
              ? "true"
              : "false")
      << ";semantic-conformance-matrix-key-ready="
      << (!surface.semantic_conformance_matrix_key.empty() ? "true" : "false")
      << ";lowering-pipeline-conformance-matrix-key-ready="
      << (!surface.lowering_pipeline_conformance_matrix_key.empty() ? "true"
                                                                     : "false");
  return key.str();
}

inline Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
BuildObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
      surface;
  const Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
      &recovery_surface =
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_recovery_determinism_hardening_surface;
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;
  const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisConformanceMatrixImplementationSurface
      &semantic_surface =
          pipeline_result
              .semantic_diagnostic_taxonomy_and_fixit_conformance_matrix_implementation_surface;
  const Objc3LoweringPipelinePassGraphCoreFeatureSurface &pass_graph_surface =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface;

  surface.recovery_determinism_consistent =
      recovery_surface.recovery_determinism_consistent;
  surface.recovery_determinism_ready = recovery_surface.recovery_determinism_ready;
  surface.parse_conformance_matrix_consistent =
      parse_surface.parse_lowering_conformance_matrix_consistent &&
      parse_surface.long_tail_grammar_conformance_matrix_consistent &&
      parse_surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent;
  surface.parse_conformance_matrix_ready =
      surface.parse_conformance_matrix_consistent &&
      parse_surface.long_tail_grammar_conformance_matrix_ready &&
      parse_surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready &&
      parse_surface.parse_lowering_conformance_matrix_case_count > 0;
  surface.semantic_conformance_matrix_consistent =
      semantic_surface.conformance_matrix_consistent;
  surface.semantic_conformance_matrix_ready =
      semantic_surface.conformance_matrix_ready;
  surface.lowering_pipeline_conformance_matrix_ready =
      pass_graph_surface.conformance_matrix_ready;

  surface.parse_lowering_conformance_matrix_case_count =
      parse_surface.parse_lowering_conformance_matrix_case_count;

  surface.recovery_determinism_key = recovery_surface.recovery_determinism_key;
  surface.parse_lowering_conformance_matrix_key =
      parse_surface.parse_lowering_conformance_matrix_key;
  surface.long_tail_grammar_conformance_matrix_key =
      parse_surface.long_tail_grammar_conformance_matrix_key;
  surface.parser_diagnostic_grammar_hooks_conformance_matrix_key =
      parse_surface.parser_diagnostic_grammar_hooks_conformance_matrix_key;
  surface.semantic_conformance_matrix_key = semantic_surface.conformance_matrix_key;
  surface.lowering_pipeline_conformance_matrix_key =
      pass_graph_surface.conformance_matrix_key;

  surface.conformance_matrix_consistent =
      surface.recovery_determinism_consistent &&
      surface.parse_conformance_matrix_consistent &&
      surface.semantic_conformance_matrix_consistent;
  const bool conformance_matrix_replay_keys_ready =
      !surface.recovery_determinism_key.empty() &&
      !surface.parse_lowering_conformance_matrix_key.empty() &&
      !surface.long_tail_grammar_conformance_matrix_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_conformance_matrix_key.empty() &&
      !surface.semantic_conformance_matrix_key.empty() &&
      !surface.lowering_pipeline_conformance_matrix_key.empty();

  surface.conformance_matrix_key =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationKey(
          surface);
  surface.conformance_matrix_ready =
      surface.recovery_determinism_ready &&
      surface.parse_conformance_matrix_ready &&
      surface.semantic_conformance_matrix_ready &&
      surface.lowering_pipeline_conformance_matrix_ready &&
      surface.conformance_matrix_consistent &&
      conformance_matrix_replay_keys_ready &&
      !surface.conformance_matrix_key.empty();

  if (surface.conformance_matrix_ready) {
    return surface;
  }

  if (!surface.recovery_determinism_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics recovery/determinism hardening is not ready";
  } else if (!surface.parse_conformance_matrix_consistent) {
    surface.failure_reason = "parse conformance-matrix surfaces are inconsistent";
  } else if (!surface.parse_conformance_matrix_ready) {
    surface.failure_reason = "parse conformance-matrix surfaces are not ready";
  } else if (!surface.semantic_conformance_matrix_consistent) {
    surface.failure_reason = "semantic conformance-matrix surface is inconsistent";
  } else if (!surface.semantic_conformance_matrix_ready) {
    surface.failure_reason = "semantic conformance-matrix surface is not ready";
  } else if (!surface.lowering_pipeline_conformance_matrix_ready) {
    surface.failure_reason =
        "lowering pipeline conformance-matrix prerequisite is not ready";
  } else if (!surface.conformance_matrix_consistent) {
    surface.failure_reason =
        "lowering/runtime diagnostics surfacing conformance matrix is inconsistent";
  } else if (!conformance_matrix_replay_keys_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics surfacing conformance matrix replay keys are not ready";
  } else {
    surface.failure_reason =
        "lowering/runtime diagnostics surfacing conformance matrix is not ready";
  }

  return surface;
}

inline bool
IsObjc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurfaceReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingConformanceMatrixImplementationSurface
        &surface,
    std::string &reason) {
  if (surface.conformance_matrix_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "lowering/runtime diagnostics surfacing conformance matrix is not ready"
               : surface.failure_reason;
  return false;
}
