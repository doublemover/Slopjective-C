#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

// The C005 surface type is declared in objc3_frontend_types.h; this header
// owns deterministic key/derivation/readiness helpers for that type.
inline std::string
BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilityKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurface
        &surface) {
  std::ostringstream key;
  key << "lowering-runtime-diagnostics-surfacing-edge-case-compatibility:v1:"
      << "core_feature_expansion_ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";compatibility_handoff_consistent="
      << (surface.compatibility_handoff_consistent ? "true" : "false")
      << ";language_version_pragma_coordinate_order_consistent="
      << (surface.language_version_pragma_coordinate_order_consistent ? "true"
                                                                      : "false")
      << ";parse_artifact_edge_case_robustness_consistent="
      << (surface.parse_artifact_edge_case_robustness_consistent ? "true"
                                                                  : "false")
      << ";parse_artifact_replay_key_deterministic="
      << (surface.parse_artifact_replay_key_deterministic ? "true" : "false")
      << ";parse_recovery_determinism_hardening_consistent="
      << (surface.parse_recovery_determinism_hardening_consistent ? "true"
                                                                   : "false")
      << ";parse_edge_case_surfaces_consistent="
      << (surface.parse_edge_case_surfaces_consistent ? "true" : "false")
      << ";parse_edge_case_surfaces_ready="
      << (surface.parse_edge_case_surfaces_ready ? "true" : "false")
      << ";lowering_pipeline_edge_case_compatibility_ready="
      << (surface.lowering_pipeline_edge_case_compatibility_ready ? "true"
                                                                   : "false")
      << ";edge_case_compatibility_consistent="
      << (surface.edge_case_compatibility_consistent ? "true" : "false")
      << ";edge_case_compatibility_replay_keys_ready="
      << (surface.edge_case_compatibility_replay_keys_ready ? "true" : "false")
      << ";edge_case_compatibility_ready="
      << (surface.edge_case_compatibility_ready ? "true" : "false")
      << ";compatibility_handoff_key=" << surface.compatibility_handoff_key
      << ";parse_artifact_edge_robustness_key="
      << surface.parse_artifact_edge_robustness_key
      << ";parse_recovery_determinism_hardening_key="
      << surface.parse_recovery_determinism_hardening_key
      << ";long_tail_grammar_edge_case_compatibility_key="
      << surface.long_tail_grammar_edge_case_compatibility_key
      << ";parser_diagnostic_grammar_hooks_edge_case_compatibility_key="
      << surface
             .parser_diagnostic_grammar_hooks_edge_case_compatibility_key
      << ";core_feature_expansion_key=" << surface.core_feature_expansion_key;
  return key.str();
}

inline Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurface
BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurface(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurface surface;
  const Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface
      &expansion_surface =
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_core_feature_expansion_surface;
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;

  surface.core_feature_expansion_ready =
      expansion_surface.core_feature_expansion_ready;
  surface.compatibility_handoff_consistent =
      parse_surface.compatibility_handoff_consistent;
  surface.language_version_pragma_coordinate_order_consistent =
      parse_surface.language_version_pragma_coordinate_order_consistent;
  surface.parse_artifact_edge_case_robustness_consistent =
      parse_surface.parse_artifact_edge_case_robustness_consistent;
  surface.parse_artifact_replay_key_deterministic =
      parse_surface.parse_artifact_replay_key_deterministic;
  surface.parse_recovery_determinism_hardening_consistent =
      parse_surface.parse_recovery_determinism_hardening_consistent;
  surface.parse_edge_case_surfaces_consistent =
      parse_surface.long_tail_grammar_edge_case_compatibility_consistent &&
      parse_surface
          .parser_diagnostic_grammar_hooks_edge_case_compatibility_consistent;
  surface.parse_edge_case_surfaces_ready =
      parse_surface.long_tail_grammar_edge_case_compatibility_ready &&
      parse_surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_ready;
  surface.lowering_pipeline_edge_case_compatibility_ready =
      pipeline_result.ir_emission_completeness_scaffold
          .edge_case_compatibility_ready &&
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .edge_case_compatibility_ready;
  surface.compatibility_handoff_key = parse_surface.compatibility_handoff_key;
  surface.parse_artifact_edge_robustness_key =
      parse_surface.parse_artifact_edge_robustness_key;
  surface.parse_recovery_determinism_hardening_key =
      parse_surface.parse_recovery_determinism_hardening_key;
  surface.long_tail_grammar_edge_case_compatibility_key =
      parse_surface.long_tail_grammar_edge_case_compatibility_key;
  surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_key =
      parse_surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_key;
  surface.core_feature_expansion_key = expansion_surface.expansion_key;

  surface.edge_case_compatibility_consistent =
      surface.core_feature_expansion_ready &&
      surface.compatibility_handoff_consistent &&
      surface.language_version_pragma_coordinate_order_consistent &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.parse_edge_case_surfaces_consistent;
  surface.edge_case_compatibility_replay_keys_ready =
      !surface.compatibility_handoff_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty() &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.long_tail_grammar_edge_case_compatibility_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_key
           .empty() &&
      !surface.core_feature_expansion_key.empty();
  surface.edge_case_compatibility_ready =
      surface.edge_case_compatibility_consistent &&
      surface.parse_recovery_determinism_hardening_consistent &&
      surface.parse_edge_case_surfaces_ready &&
      surface.lowering_pipeline_edge_case_compatibility_ready &&
      surface.edge_case_compatibility_replay_keys_ready;
  surface.edge_case_compatibility_key =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilityKey(
          surface);

  if (surface.edge_case_compatibility_ready) {
    return surface;
  }

  if (!surface.core_feature_expansion_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics core feature expansion is not ready";
  } else if (!surface.compatibility_handoff_consistent) {
    surface.failure_reason = "compatibility handoff is inconsistent";
  } else if (!surface.language_version_pragma_coordinate_order_consistent) {
    surface.failure_reason = "language-version pragma coordinate order is inconsistent";
  } else if (!surface.parse_artifact_edge_case_robustness_consistent) {
    surface.failure_reason = "parse artifact edge-case robustness is inconsistent";
  } else if (!surface.parse_artifact_replay_key_deterministic) {
    surface.failure_reason = "parse artifact replay key determinism is inconsistent";
  } else if (!surface.parse_recovery_determinism_hardening_consistent) {
    surface.failure_reason = "parse recovery determinism hardening is inconsistent";
  } else if (!surface.parse_edge_case_surfaces_consistent) {
    surface.failure_reason =
        "parse edge-case compatibility surfaces are inconsistent";
  } else if (!surface.parse_edge_case_surfaces_ready) {
    surface.failure_reason = "parse edge-case compatibility surfaces are not ready";
  } else if (!surface.lowering_pipeline_edge_case_compatibility_ready) {
    surface.failure_reason =
        "lowering pipeline edge-case compatibility prerequisites are not ready";
  } else if (!surface.edge_case_compatibility_consistent) {
    surface.failure_reason =
        "lowering/runtime diagnostics edge-case compatibility is inconsistent";
  } else if (!surface.edge_case_compatibility_replay_keys_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics edge-case compatibility replay keys are not ready";
  } else {
    surface.failure_reason =
        "lowering/runtime diagnostics edge-case compatibility is not ready";
  }

  return surface;
}

inline bool
IsObjc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurfaceReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingEdgeCaseCompatibilitySurface
        &surface,
    std::string &reason) {
  if (surface.edge_case_compatibility_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "lowering/runtime diagnostics edge-case compatibility is not ready"
               : surface.failure_reason;
  return false;
}
