#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline std::string
BuildObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface
        &surface) {
  std::ostringstream key;
  key << "lowering-runtime-diagnostics-surfacing-diagnostics-hardening:v1:"
      << "edge_case_robustness_consistent="
      << (surface.edge_case_robustness_consistent ? "true" : "false")
      << ";edge_case_robustness_ready="
      << (surface.edge_case_robustness_ready ? "true" : "false")
      << ";parse_diagnostics_hardening_consistent="
      << (surface.parse_diagnostics_hardening_consistent ? "true" : "false")
      << ";parse_diagnostics_hardening_ready="
      << (surface.parse_diagnostics_hardening_ready ? "true" : "false")
      << ";semantic_diagnostics_hardening_consistent="
      << (surface.semantic_diagnostics_hardening_consistent ? "true" : "false")
      << ";semantic_diagnostics_hardening_ready="
      << (surface.semantic_diagnostics_hardening_ready ? "true" : "false")
      << ";lowering_pipeline_diagnostics_hardening_ready="
      << (surface.lowering_pipeline_diagnostics_hardening_ready ? "true"
                                                                 : "false")
      << ";diagnostics_hardening_consistent="
      << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";diagnostics_hardening_ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";diag-hardening-key-ready="
      << (!surface.diagnostics_hardening_key.empty() ? "true" : "false")
      << ";edge-case-robustness-key-ready="
      << (!surface.edge_case_robustness_key.empty() ? "true" : "false")
      << ";parse-artifact-diag-hardening-key-ready="
      << (!surface.parse_artifact_diagnostics_hardening_key.empty() ? "true"
                                                                     : "false")
      << ";long-tail-diag-hardening-key-ready="
      << (!surface.long_tail_grammar_diagnostics_hardening_key.empty() ? "true"
                                                                        : "false")
      << ";parser-hooks-diag-hardening-key-ready="
      << (!surface
               .parser_diagnostic_grammar_hooks_diagnostics_hardening_key.empty()
              ? "true"
              : "false")
      << ";semantic-diag-hardening-key-ready="
      << (!surface.semantic_diagnostics_hardening_key.empty() ? "true"
                                                               : "false")
      << ";lowering-pipeline-diag-hardening-key-ready="
      << (!surface.lowering_pipeline_diagnostics_hardening_key.empty() ? "true"
                                                                        : "false")
      << ";edge_case_robustness_key=" << surface.edge_case_robustness_key
      << ";parse_artifact_diagnostics_hardening_key="
      << surface.parse_artifact_diagnostics_hardening_key
      << ";long_tail_grammar_diagnostics_hardening_key="
      << surface.long_tail_grammar_diagnostics_hardening_key
      << ";parser_diagnostic_grammar_hooks_diagnostics_hardening_key="
      << surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_key
      << ";semantic_diagnostics_hardening_key="
      << surface.semantic_diagnostics_hardening_key
      << ";lowering_pipeline_diagnostics_hardening_key="
      << surface.lowering_pipeline_diagnostics_hardening_key;
  return key.str();
}

inline Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface
BuildObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface surface;
  const Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface
      &edge_case_surface =
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_edge_case_expansion_and_robustness_surface;
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;
  const Objc3SemanticDiagnosticTaxonomyAndFixitSynthesisDiagnosticsHardeningSurface
      &semantic_surface =
          pipeline_result
              .semantic_diagnostic_taxonomy_and_fixit_diagnostics_hardening_surface;
  const Objc3LoweringPipelinePassGraphCoreFeatureSurface
      &pass_graph_core_feature_surface =
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface;

  surface.edge_case_robustness_consistent =
      edge_case_surface.edge_case_expansion_consistent;
  surface.edge_case_robustness_ready = edge_case_surface.edge_case_robustness_ready;
  surface.parse_diagnostics_hardening_consistent =
      parse_surface.parse_artifact_diagnostics_hardening_consistent &&
      parse_surface.long_tail_grammar_diagnostics_hardening_consistent &&
      parse_surface
          .parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent;
  surface.parse_diagnostics_hardening_ready =
      parse_surface.long_tail_grammar_diagnostics_hardening_ready &&
      parse_surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready;
  surface.semantic_diagnostics_hardening_consistent =
      semantic_surface.diagnostics_hardening_consistent;
  surface.semantic_diagnostics_hardening_ready =
      semantic_surface.diagnostics_hardening_ready;
  surface.lowering_pipeline_diagnostics_hardening_ready =
      pass_graph_core_feature_surface.diagnostics_hardening_ready;

  surface.edge_case_robustness_key = edge_case_surface.edge_case_robustness_key;
  surface.parse_artifact_diagnostics_hardening_key =
      parse_surface.parse_artifact_diagnostics_hardening_key;
  surface.long_tail_grammar_diagnostics_hardening_key =
      parse_surface.long_tail_grammar_diagnostics_hardening_key;
  surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_key =
      parse_surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_key;
  surface.semantic_diagnostics_hardening_key =
      semantic_surface.diagnostics_hardening_key;
  surface.lowering_pipeline_diagnostics_hardening_key =
      pass_graph_core_feature_surface.diagnostics_hardening_key;

  surface.diagnostics_hardening_consistent =
      surface.edge_case_robustness_consistent &&
      surface.parse_diagnostics_hardening_consistent &&
      surface.semantic_diagnostics_hardening_consistent &&
      pass_graph_core_feature_surface.diagnostics_hardening_consistent;
  const bool diagnostics_hardening_replay_keys_ready =
      !surface.edge_case_robustness_key.empty() &&
      !surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.long_tail_grammar_diagnostics_hardening_key.empty() &&
      !surface
           .parser_diagnostic_grammar_hooks_diagnostics_hardening_key.empty() &&
      !surface.semantic_diagnostics_hardening_key.empty() &&
      !surface.lowering_pipeline_diagnostics_hardening_key.empty();
  surface.diagnostics_hardening_key =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningKey(
          surface);
  surface.diagnostics_hardening_ready =
      surface.edge_case_robustness_ready &&
      surface.parse_diagnostics_hardening_ready &&
      surface.semantic_diagnostics_hardening_ready &&
      surface.lowering_pipeline_diagnostics_hardening_ready &&
      surface.diagnostics_hardening_consistent &&
      diagnostics_hardening_replay_keys_ready &&
      !surface.diagnostics_hardening_key.empty();

  if (surface.diagnostics_hardening_ready) {
    return surface;
  }

  if (!surface.edge_case_robustness_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics edge-case robustness is not ready";
  } else if (!surface.parse_diagnostics_hardening_consistent) {
    surface.failure_reason =
        "parse diagnostics hardening surfaces are inconsistent";
  } else if (!surface.parse_diagnostics_hardening_ready) {
    surface.failure_reason = "parse diagnostics hardening surfaces are not ready";
  } else if (!surface.semantic_diagnostics_hardening_consistent) {
    surface.failure_reason =
        "semantic diagnostics hardening surfaces are inconsistent";
  } else if (!surface.semantic_diagnostics_hardening_ready) {
    surface.failure_reason =
        "semantic diagnostics hardening surfaces are not ready";
  } else if (!surface.lowering_pipeline_diagnostics_hardening_ready) {
    surface.failure_reason =
        "lowering pipeline diagnostics hardening prerequisites are not ready";
  } else if (!surface.diagnostics_hardening_consistent) {
    surface.failure_reason =
        "lowering/runtime diagnostics hardening is inconsistent";
  } else if (!diagnostics_hardening_replay_keys_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics hardening replay keys are not ready";
  } else {
    surface.failure_reason =
        "lowering/runtime diagnostics hardening is not ready";
  }

  return surface;
}

inline bool
IsObjc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurfaceReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingDiagnosticsHardeningSurface
        &surface,
    std::string &reason) {
  if (surface.diagnostics_hardening_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "lowering/runtime diagnostics hardening is not ready"
               : surface.failure_reason;
  return false;
}
