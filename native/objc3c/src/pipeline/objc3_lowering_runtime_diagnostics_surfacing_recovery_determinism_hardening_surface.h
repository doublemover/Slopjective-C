#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline std::string
BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
        &surface) {
  std::ostringstream key;
  key << "lowering-runtime-diagnostics-surfacing-recovery-determinism-hardening:v1:"
      << "diagnostics_hardening_consistent="
      << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";diagnostics_hardening_ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";parse_recovery_determinism_consistent="
      << (surface.parse_recovery_determinism_consistent ? "true" : "false")
      << ";parse_recovery_determinism_ready="
      << (surface.parse_recovery_determinism_ready ? "true" : "false")
      << ";semantic_recovery_determinism_consistent="
      << (surface.semantic_recovery_determinism_consistent ? "true" : "false")
      << ";semantic_recovery_determinism_ready="
      << (surface.semantic_recovery_determinism_ready ? "true" : "false")
      << ";lowering_pipeline_recovery_determinism_ready="
      << (surface.lowering_pipeline_recovery_determinism_ready ? "true"
                                                                : "false")
      << ";recovery_determinism_consistent="
      << (surface.recovery_determinism_consistent ? "true" : "false")
      << ";recovery_determinism_ready="
      << (surface.recovery_determinism_ready ? "true" : "false")
      << ";diag-hardening-key-ready="
      << (!surface.diagnostics_hardening_key.empty() ? "true" : "false")
      << ";parse-recovery-key-ready="
      << (!surface.parse_recovery_determinism_hardening_key.empty() ? "true"
                                                                     : "false")
      << ";long-tail-recovery-key-ready="
      << (!surface.long_tail_grammar_recovery_determinism_key.empty() ? "true"
                                                                       : "false")
      << ";parser-hooks-recovery-key-ready="
      << (!surface
               .parser_diagnostic_grammar_hooks_recovery_determinism_key.empty()
              ? "true"
              : "false")
      << ";semantic-recovery-key-ready="
      << (!surface.semantic_recovery_determinism_key.empty() ? "true"
                                                              : "false")
      << ";lowering-pipeline-recovery-key-ready="
      << (!surface.lowering_pipeline_recovery_determinism_key.empty() ? "true"
                                                                       : "false")
      << ";diagnostics_hardening_key=" << surface.diagnostics_hardening_key
      << ";parse_recovery_determinism_hardening_key="
      << surface.parse_recovery_determinism_hardening_key
      << ";long_tail_grammar_recovery_determinism_key="
      << surface.long_tail_grammar_recovery_determinism_key
      << ";parser_diagnostic_grammar_hooks_recovery_determinism_key="
      << surface.parser_diagnostic_grammar_hooks_recovery_determinism_key
      << ";semantic_recovery_determinism_key="
      << surface.semantic_recovery_determinism_key
      << ";lowering_pipeline_recovery_determinism_key="
      << surface.lowering_pipeline_recovery_determinism_key;
  return key.str();
}

inline Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
      surface;
  const Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface
      &diagnostics_surface =
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_diagnostics_hardening_surface;
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;
  const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisRecoveryDeterminismHardeningSurface
      &semantic_surface =
          pipeline_result
              .semantic_diagnostic_taxonomy_and_fixit_recovery_determinism_hardening_surface;
  const Objc3LoweringPipelinePassGraphCoreFeatureSurface &pass_graph_surface =
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface;

  surface.diagnostics_hardening_consistent =
      diagnostics_surface.diagnostics_hardening_consistent;
  surface.diagnostics_hardening_ready = diagnostics_surface.diagnostics_hardening_ready;
  surface.parse_recovery_determinism_consistent =
      parse_surface.parse_recovery_determinism_hardening_consistent &&
      parse_surface.long_tail_grammar_recovery_determinism_consistent &&
      parse_surface
          .parser_diagnostic_grammar_hooks_recovery_determinism_consistent;
  surface.parse_recovery_determinism_ready =
      parse_surface.long_tail_grammar_recovery_determinism_ready &&
      parse_surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready;
  surface.semantic_recovery_determinism_consistent =
      semantic_surface.recovery_determinism_consistent;
  surface.semantic_recovery_determinism_ready =
      semantic_surface.recovery_determinism_ready;
  surface.lowering_pipeline_recovery_determinism_ready =
      pass_graph_surface.recovery_determinism_ready;

  surface.diagnostics_hardening_key = diagnostics_surface.diagnostics_hardening_key;
  surface.parse_recovery_determinism_hardening_key =
      parse_surface.parse_recovery_determinism_hardening_key;
  surface.long_tail_grammar_recovery_determinism_key =
      parse_surface.long_tail_grammar_recovery_determinism_key;
  surface.parser_diagnostic_grammar_hooks_recovery_determinism_key =
      parse_surface.parser_diagnostic_grammar_hooks_recovery_determinism_key;
  surface.semantic_recovery_determinism_key =
      semantic_surface.recovery_determinism_key;
  surface.lowering_pipeline_recovery_determinism_key =
      pass_graph_surface.recovery_determinism_key;

  surface.recovery_determinism_consistent =
      surface.diagnostics_hardening_consistent &&
      surface.parse_recovery_determinism_consistent &&
      surface.semantic_recovery_determinism_consistent &&
      pass_graph_surface.recovery_determinism_consistent;
  const bool recovery_determinism_replay_keys_ready =
      !surface.diagnostics_hardening_key.empty() &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.long_tail_grammar_recovery_determinism_key.empty() &&
      !surface
           .parser_diagnostic_grammar_hooks_recovery_determinism_key.empty() &&
      !surface.semantic_recovery_determinism_key.empty() &&
      !surface.lowering_pipeline_recovery_determinism_key.empty();
  surface.recovery_determinism_key =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningKey(
          surface);
  surface.recovery_determinism_ready =
      surface.diagnostics_hardening_ready &&
      surface.parse_recovery_determinism_ready &&
      surface.semantic_recovery_determinism_ready &&
      surface.lowering_pipeline_recovery_determinism_ready &&
      surface.recovery_determinism_consistent &&
      recovery_determinism_replay_keys_ready &&
      !surface.recovery_determinism_key.empty();

  if (surface.recovery_determinism_ready) {
    return surface;
  }

  if (!surface.diagnostics_hardening_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics hardening is not ready";
  } else if (!surface.parse_recovery_determinism_consistent) {
    surface.failure_reason = "parse recovery/determinism surfaces are inconsistent";
  } else if (!surface.parse_recovery_determinism_ready) {
    surface.failure_reason = "parse recovery/determinism surfaces are not ready";
  } else if (!surface.semantic_recovery_determinism_consistent) {
    surface.failure_reason =
        "semantic recovery/determinism surfaces are inconsistent";
  } else if (!surface.semantic_recovery_determinism_ready) {
    surface.failure_reason = "semantic recovery/determinism surfaces are not ready";
  } else if (!surface.lowering_pipeline_recovery_determinism_ready) {
    surface.failure_reason =
        "lowering pipeline recovery/determinism prerequisites are not ready";
  } else if (!surface.recovery_determinism_consistent) {
    surface.failure_reason =
        "lowering/runtime diagnostics recovery/determinism hardening is inconsistent";
  } else if (!recovery_determinism_replay_keys_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics recovery/determinism replay keys are not ready";
  } else {
    surface.failure_reason =
        "lowering/runtime diagnostics recovery/determinism hardening is not ready";
  }

  return surface;
}

inline bool
IsObjc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurfaceReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingRecoveryDeterminismHardeningSurface
        &surface,
    std::string &reason) {
  if (surface.recovery_determinism_ready) {
    reason.clear();
    return true;
  }

  reason =
      surface.failure_reason.empty()
          ? "lowering/runtime diagnostics recovery/determinism hardening is not ready"
          : surface.failure_reason;
  return false;
}
