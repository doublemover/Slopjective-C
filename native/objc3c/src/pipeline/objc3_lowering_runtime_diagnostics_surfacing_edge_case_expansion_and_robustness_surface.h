#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline std::string
BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseRobustnessKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface
        &surface) {
  std::ostringstream key;
  key << "lowering-runtime-diagnostics-surfacing-edge-case-robustness:v1:"
      << "edge_case_compatibility_ready="
      << (surface.edge_case_compatibility_ready ? "true" : "false")
      << ";parse_edge_case_expansion_consistent="
      << (surface.parse_edge_case_expansion_consistent ? "true" : "false")
      << ";parse_edge_case_robustness_ready="
      << (surface.parse_edge_case_robustness_ready ? "true" : "false")
      << ";lowering_pipeline_edge_case_robustness_ready="
      << (surface.lowering_pipeline_edge_case_robustness_ready ? "true"
                                                                : "false")
      << ";edge_case_expansion_consistent="
      << (surface.edge_case_expansion_consistent ? "true" : "false")
      << ";edge_case_robustness_ready="
      << (surface.edge_case_robustness_ready ? "true" : "false")
      << ";compatibility_handoff_key_ready="
      << (!surface.compatibility_handoff_key.empty() ? "true" : "false")
      << ";parse_artifact_edge_robustness_key_ready="
      << (!surface.parse_artifact_edge_robustness_key.empty() ? "true"
                                                               : "false")
      << ";parse_recovery_determinism_hardening_key_ready="
      << (!surface.parse_recovery_determinism_hardening_key.empty() ? "true"
                                                                     : "false")
      << ";long_tail_grammar_edge_case_robustness_key_ready="
      << (!surface.long_tail_grammar_edge_case_robustness_key.empty() ? "true"
                                                                       : "false")
      << ";parser_diagnostic_grammar_hooks_edge_case_robustness_key_ready="
      << (!surface.parser_diagnostic_grammar_hooks_edge_case_robustness_key
               .empty()
              ? "true"
              : "false")
      << ";lowering_pipeline_edge_case_robustness_key_ready="
      << (!surface.lowering_pipeline_edge_case_robustness_key.empty() ? "true"
                                                                       : "false")
      << ";edge_case_compatibility_key_ready="
      << (!surface.edge_case_compatibility_key.empty() ? "true" : "false")
      << ";compatibility_handoff_key=" << surface.compatibility_handoff_key
      << ";parse_artifact_edge_robustness_key="
      << surface.parse_artifact_edge_robustness_key
      << ";parse_recovery_determinism_hardening_key="
      << surface.parse_recovery_determinism_hardening_key
      << ";long_tail_grammar_edge_case_robustness_key="
      << surface.long_tail_grammar_edge_case_robustness_key
      << ";parser_diagnostic_grammar_hooks_edge_case_robustness_key="
      << surface.parser_diagnostic_grammar_hooks_edge_case_robustness_key
      << ";lowering_pipeline_edge_case_robustness_key="
      << surface.lowering_pipeline_edge_case_robustness_key
      << ";edge_case_compatibility_key=" << surface.edge_case_compatibility_key;
  return key.str();
}

inline Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface
BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface
      surface;
  const Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurface
      &compatibility_surface =
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_edge_case_compatibility_surface;
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;
  const Objc3LoweringPipelinePassGraphCoreFeatureSurface
      &pass_graph_core_feature_surface =
          pipeline_result.lowering_pipeline_pass_graph_core_feature_surface;

  surface.edge_case_compatibility_consistent =
      compatibility_surface.edge_case_compatibility_consistent;
  surface.edge_case_compatibility_ready =
      compatibility_surface.edge_case_compatibility_ready;
  surface.parse_edge_case_expansion_consistent =
      parse_surface.long_tail_grammar_edge_case_expansion_consistent &&
      parse_surface
          .parser_diagnostic_grammar_hooks_edge_case_expansion_consistent;
  surface.parse_edge_case_robustness_ready =
      parse_surface.long_tail_grammar_edge_case_robustness_ready &&
      parse_surface
          .parser_diagnostic_grammar_hooks_edge_case_robustness_ready;
  surface.lowering_pipeline_edge_case_robustness_ready =
      pass_graph_core_feature_surface.edge_case_robustness_ready;
  surface.compatibility_handoff_key =
      compatibility_surface.compatibility_handoff_key;
  surface.parse_artifact_edge_robustness_key =
      compatibility_surface.parse_artifact_edge_robustness_key;
  surface.parse_recovery_determinism_hardening_key =
      compatibility_surface.parse_recovery_determinism_hardening_key;
  surface.long_tail_grammar_edge_case_robustness_key =
      parse_surface.long_tail_grammar_edge_case_robustness_key;
  surface.parser_diagnostic_grammar_hooks_edge_case_robustness_key =
      parse_surface.parser_diagnostic_grammar_hooks_edge_case_robustness_key;
  surface.lowering_pipeline_edge_case_robustness_key =
      pass_graph_core_feature_surface.edge_case_robustness_key;
  surface.edge_case_compatibility_key =
      compatibility_surface.edge_case_compatibility_key;

  surface.edge_case_expansion_consistent =
      surface.edge_case_compatibility_consistent &&
      surface.parse_edge_case_expansion_consistent &&
      parse_surface.parse_artifact_edge_case_robustness_consistent &&
      parse_surface.parse_recovery_determinism_hardening_consistent;
  const bool edge_case_replay_keys_ready =
      !surface.compatibility_handoff_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty() &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.long_tail_grammar_edge_case_robustness_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_edge_case_robustness_key
           .empty() &&
      !surface.lowering_pipeline_edge_case_robustness_key.empty() &&
      !surface.edge_case_compatibility_key.empty();
  surface.edge_case_robustness_key =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseRobustnessKey(
          surface);
  surface.edge_case_robustness_ready =
      surface.edge_case_compatibility_ready &&
      surface.edge_case_expansion_consistent &&
      surface.parse_edge_case_robustness_ready &&
      surface.lowering_pipeline_edge_case_robustness_ready &&
      edge_case_replay_keys_ready && !surface.edge_case_robustness_key.empty();

  if (surface.edge_case_robustness_ready) {
    return surface;
  }

  if (!surface.edge_case_compatibility_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics edge-case compatibility is not ready";
  } else if (!surface.parse_edge_case_expansion_consistent) {
    surface.failure_reason = "parse edge-case expansion surfaces are inconsistent";
  } else if (!surface.edge_case_expansion_consistent) {
    surface.failure_reason =
        "lowering/runtime diagnostics edge-case expansion is inconsistent";
  } else if (!surface.parse_edge_case_robustness_ready) {
    surface.failure_reason = "parse edge-case robustness surfaces are not ready";
  } else if (!surface.lowering_pipeline_edge_case_robustness_ready) {
    surface.failure_reason =
        "lowering pipeline edge-case robustness prerequisites are not ready";
  } else if (!edge_case_replay_keys_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics edge-case robustness replay keys are not ready";
  } else {
    surface.failure_reason =
        "lowering/runtime diagnostics edge-case robustness is not ready";
  }

  return surface;
}

inline bool
IsObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurfaceReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseExpansionAndRobustnessSurface
        &surface,
    std::string &reason) {
  if (surface.edge_case_robustness_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "lowering/runtime diagnostics edge-case robustness is not ready"
               : surface.failure_reason;
  return false;
}
