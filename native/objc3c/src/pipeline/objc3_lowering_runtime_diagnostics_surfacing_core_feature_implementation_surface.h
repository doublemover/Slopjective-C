#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline std::string
BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
        &surface) {
  std::ostringstream key;
  key << "lowering-runtime-diagnostics-surfacing-core-feature-impl:v1:"
      << "stage_diagnostics_bus_consistent="
      << (surface.stage_diagnostics_bus_consistent ? "true" : "false")
      << ";parse_readiness_surface_ready="
      << (surface.parse_readiness_surface_ready ? "true" : "false")
      << ";diagnostics_surfacing_scaffold_ready="
      << (surface.diagnostics_surfacing_scaffold_ready ? "true" : "false")
      << ";parser_diagnostic_surface_consistent="
      << (surface.parser_diagnostic_surface_consistent ? "true" : "false")
      << ";parser_diagnostic_code_surface_deterministic="
      << (surface.parser_diagnostic_code_surface_deterministic ? "true"
                                                               : "false")
      << ";semantic_diagnostics_deterministic="
      << (surface.semantic_diagnostics_deterministic ? "true" : "false")
      << ";diagnostics_hardening_consistent="
      << (surface.diagnostics_hardening_consistent ? "true" : "false")
      << ";diagnostics_hardening_ready="
      << (surface.diagnostics_hardening_ready ? "true" : "false")
      << ";replay_keys_ready="
      << (surface.replay_keys_ready ? "true" : "false")
      << ";lowering_pipeline_ready="
      << (surface.lowering_pipeline_ready ? "true" : "false")
      << ";lexer_diagnostic_count=" << surface.lexer_diagnostic_count
      << ";parser_diagnostic_count=" << surface.parser_diagnostic_count
      << ";semantic_diagnostic_count=" << surface.semantic_diagnostic_count
      << ";parse_artifact_replay_key=" << surface.parse_artifact_replay_key
      << ";lowering_boundary_replay_key=" << surface.lowering_boundary_replay_key
      << ";diagnostics_hardening_key=" << surface.diagnostics_hardening_key
      << ";core_feature_impl_ready="
      << (surface.core_feature_impl_ready ? "true" : "false");
  return key.str();
}

inline Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
      surface;
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;
  const Objc3LoweringRuntimeDiagnosticsSurfacingScaffold &scaffold =
      pipeline_result.lowering_runtime_diagnostics_surfacing_scaffold;

  surface.lexer_diagnostic_count = pipeline_result.stage_diagnostics.lexer.size();
  surface.parser_diagnostic_count = pipeline_result.stage_diagnostics.parser.size();
  surface.semantic_diagnostic_count =
      pipeline_result.stage_diagnostics.semantic.size();

  const std::size_t stage_diagnostics_total =
      surface.lexer_diagnostic_count + surface.parser_diagnostic_count +
      surface.semantic_diagnostic_count;
  surface.stage_diagnostics_bus_consistent =
      stage_diagnostics_total == pipeline_result.stage_diagnostics.size();
  surface.parse_readiness_surface_ready =
      parse_surface.ready_for_lowering && parse_surface.lowering_boundary_ready;
  surface.diagnostics_surfacing_scaffold_ready = scaffold.modular_split_ready;
  surface.parser_diagnostic_surface_consistent =
      parse_surface.parser_diagnostic_surface_consistent;
  surface.parser_diagnostic_code_surface_deterministic =
      parse_surface.parser_diagnostic_code_surface_deterministic;
  surface.semantic_diagnostics_deterministic =
      parse_surface.semantic_diagnostics_deterministic;

  surface.diagnostics_hardening_consistent =
      scaffold.parse_diagnostics_hardening_consistent &&
      parse_surface.parse_artifact_diagnostics_hardening_consistent &&
      parse_surface.long_tail_grammar_diagnostics_hardening_consistent &&
      surface.parser_diagnostic_surface_consistent &&
      surface.parser_diagnostic_code_surface_deterministic &&
      surface.semantic_diagnostics_deterministic;
  surface.diagnostics_hardening_ready =
      surface.diagnostics_hardening_consistent &&
      parse_surface.long_tail_grammar_diagnostics_hardening_ready &&
      !parse_surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !parse_surface.long_tail_grammar_diagnostics_hardening_key.empty();

  surface.parse_artifact_replay_key = scaffold.parse_artifact_replay_key;
  surface.lowering_boundary_replay_key = scaffold.lowering_boundary_replay_key;
  surface.diagnostics_hardening_key =
      parse_surface.parse_artifact_diagnostics_hardening_key;
  surface.replay_keys_ready =
      !surface.parse_artifact_replay_key.empty() &&
      !surface.lowering_boundary_replay_key.empty() &&
      !surface.diagnostics_hardening_key.empty() &&
      !parse_surface.parser_diagnostic_source_precision_scaffold_key.empty();
  surface.lowering_pipeline_ready =
      pipeline_result.lowering_pipeline_pass_graph_scaffold.pass_graph_ready &&
      pipeline_result.ir_emission_completeness_scaffold.modular_split_ready &&
      pipeline_result.ir_emission_completeness_scaffold.core_feature_ready;

  surface.core_feature_impl_ready =
      surface.stage_diagnostics_bus_consistent &&
      surface.parse_readiness_surface_ready &&
      surface.diagnostics_surfacing_scaffold_ready &&
      surface.parser_diagnostic_surface_consistent &&
      surface.parser_diagnostic_code_surface_deterministic &&
      surface.semantic_diagnostics_deterministic &&
      surface.diagnostics_hardening_consistent &&
      surface.diagnostics_hardening_ready &&
      surface.replay_keys_ready &&
      surface.lowering_pipeline_ready;
  surface.core_feature_key =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationKey(
          surface);

  if (surface.core_feature_impl_ready) {
    return surface;
  }

  if (!surface.stage_diagnostics_bus_consistent) {
    surface.failure_reason = "stage diagnostics bus accounting is inconsistent";
  } else if (!surface.parse_readiness_surface_ready) {
    surface.failure_reason = "parse-lowering readiness surface is not ready";
  } else if (!surface.diagnostics_surfacing_scaffold_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics surfacing scaffold is not ready";
  } else if (!surface.parser_diagnostic_surface_consistent) {
    surface.failure_reason = "parser diagnostic surface is inconsistent";
  } else if (!surface.parser_diagnostic_code_surface_deterministic) {
    surface.failure_reason =
        "parser diagnostic code surface is not deterministic";
  } else if (!surface.semantic_diagnostics_deterministic) {
    surface.failure_reason = "semantic diagnostics are not deterministic";
  } else if (!surface.diagnostics_hardening_consistent) {
    surface.failure_reason =
        "lowering/runtime diagnostics hardening is inconsistent";
  } else if (!surface.diagnostics_hardening_ready) {
    surface.failure_reason = "lowering/runtime diagnostics hardening is not ready";
  } else if (!surface.replay_keys_ready) {
    surface.failure_reason = "lowering/runtime diagnostics replay keys are not ready";
  } else if (!surface.lowering_pipeline_ready) {
    surface.failure_reason = "lowering pipeline prerequisites are not ready";
  } else {
    surface.failure_reason = "lowering/runtime diagnostics core feature implementation is not ready";
  }

  return surface;
}

inline bool
IsObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurfaceReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
        &surface,
    std::string &reason) {
  if (surface.core_feature_impl_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "lowering/runtime diagnostics core feature implementation is not ready"
               : surface.failure_reason;
  return false;
}
