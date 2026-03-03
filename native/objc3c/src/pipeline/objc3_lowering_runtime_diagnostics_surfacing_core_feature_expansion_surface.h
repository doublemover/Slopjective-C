#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

// The C004 surface type is declared in objc3_frontend_types.h; this header
// owns deterministic key/derivation/readiness helpers for that type.
inline std::string
BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface
        &surface) {
  std::ostringstream key;
  key << "lowering-runtime-diagnostics-surfacing-core-feature-expansion:v1:"
      << "core_feature_impl_ready="
      << (surface.core_feature_impl_ready ? "true" : "false")
      << ";diagnostics_surfacing_scaffold_ready="
      << (surface.diagnostics_surfacing_scaffold_ready ? "true" : "false")
      << ";diagnostics_hardening_key_consistent="
      << (surface.diagnostics_hardening_key_consistent ? "true" : "false")
      << ";diagnostics_payload_accounting_consistent="
      << (surface.diagnostics_payload_accounting_consistent ? "true" : "false")
      << ";expansion_replay_keys_ready="
      << (surface.expansion_replay_keys_ready ? "true" : "false")
      << ";lowering_pipeline_expansion_ready="
      << (surface.lowering_pipeline_expansion_ready ? "true" : "false")
      << ";core_feature_expansion_ready="
      << (surface.core_feature_expansion_ready ? "true" : "false")
      << ";lexer_diagnostic_count=" << surface.lexer_diagnostic_count
      << ";parser_diagnostic_count=" << surface.parser_diagnostic_count
      << ";semantic_diagnostic_count=" << surface.semantic_diagnostic_count
      << ";parser_diagnostic_code_count=" << surface.parser_diagnostic_code_count
      << ";parse_artifact_replay_key=" << surface.parse_artifact_replay_key
      << ";lowering_boundary_replay_key=" << surface.lowering_boundary_replay_key
      << ";diagnostics_hardening_key=" << surface.diagnostics_hardening_key
      << ";core_feature_key=" << surface.core_feature_key;
  return key.str();
}

inline Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface
BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface surface;
  const Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureImplementationSurface
      &core_surface =
          pipeline_result
              .lowering_runtime_diagnostics_surfacing_core_feature_implementation_surface;
  const Objc3LoweringRuntimeDiagnosticsSurfacingScaffold &scaffold =
      pipeline_result.lowering_runtime_diagnostics_surfacing_scaffold;
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;

  surface.core_feature_impl_ready = core_surface.core_feature_impl_ready;
  surface.diagnostics_surfacing_scaffold_ready = scaffold.modular_split_ready;
  surface.lexer_diagnostic_count = core_surface.lexer_diagnostic_count;
  surface.parser_diagnostic_count = core_surface.parser_diagnostic_count;
  surface.semantic_diagnostic_count = core_surface.semantic_diagnostic_count;
  surface.parser_diagnostic_code_count = parse_surface.parser_diagnostic_code_count;
  surface.parse_artifact_replay_key = core_surface.parse_artifact_replay_key;
  surface.lowering_boundary_replay_key = core_surface.lowering_boundary_replay_key;
  surface.diagnostics_hardening_key = core_surface.diagnostics_hardening_key;
  surface.core_feature_key = core_surface.core_feature_key;

  const bool stage_diagnostic_count_consistent =
      surface.lexer_diagnostic_count == parse_surface.lexer_diagnostic_count &&
      surface.parser_diagnostic_count == parse_surface.parser_diagnostic_count &&
      surface.semantic_diagnostic_count ==
          parse_surface.semantic_diagnostic_count;
  const bool parser_code_accounting_consistent =
      surface.parser_diagnostic_code_count <= surface.parser_diagnostic_count &&
      parse_surface.parser_diagnostic_code_surface_deterministic;
  surface.diagnostics_hardening_key_consistent =
      !surface.diagnostics_hardening_key.empty() &&
      surface.diagnostics_hardening_key ==
          parse_surface.parse_artifact_diagnostics_hardening_key &&
      !parse_surface.long_tail_grammar_diagnostics_hardening_key.empty();
  surface.diagnostics_payload_accounting_consistent =
      stage_diagnostic_count_consistent &&
      parser_code_accounting_consistent &&
      core_surface.stage_diagnostics_bus_consistent &&
      core_surface.parser_diagnostic_surface_consistent &&
      core_surface.parser_diagnostic_code_surface_deterministic &&
      core_surface.semantic_diagnostics_deterministic;
  surface.expansion_replay_keys_ready =
      !surface.parse_artifact_replay_key.empty() &&
      !surface.lowering_boundary_replay_key.empty() &&
      !surface.diagnostics_hardening_key.empty() &&
      !surface.core_feature_key.empty() &&
      !parse_surface.parser_diagnostic_source_precision_scaffold_key.empty();
  surface.lowering_pipeline_expansion_ready =
      pipeline_result.ir_emission_completeness_scaffold.expansion_ready &&
      pipeline_result.lowering_pipeline_pass_graph_core_feature_surface
          .expansion_ready;
  surface.core_feature_expansion_ready =
      surface.core_feature_impl_ready &&
      surface.diagnostics_surfacing_scaffold_ready &&
      surface.diagnostics_hardening_key_consistent &&
      surface.diagnostics_payload_accounting_consistent &&
      surface.expansion_replay_keys_ready &&
      surface.lowering_pipeline_expansion_ready;
  surface.expansion_key =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionKey(
          surface);

  if (surface.core_feature_expansion_ready) {
    return surface;
  }

  if (!surface.core_feature_impl_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics core feature implementation is not ready";
  } else if (!surface.diagnostics_surfacing_scaffold_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics surfacing scaffold is not ready";
  } else if (!surface.diagnostics_hardening_key_consistent) {
    surface.failure_reason =
        "lowering/runtime diagnostics hardening key continuity is inconsistent";
  } else if (!surface.diagnostics_payload_accounting_consistent) {
    surface.failure_reason =
        "lowering/runtime diagnostics payload accounting is inconsistent";
  } else if (!surface.expansion_replay_keys_ready) {
    surface.failure_reason =
        "lowering/runtime diagnostics expansion replay keys are not ready";
  } else if (!surface.lowering_pipeline_expansion_ready) {
    surface.failure_reason =
        "lowering pipeline expansion prerequisites are not ready";
  } else {
    surface.failure_reason =
        "lowering/runtime diagnostics core feature expansion is not ready";
  }

  return surface;
}

inline bool
IsObjc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurfaceReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingCoreFeatureExpansionSurface
        &surface,
    std::string &reason) {
  if (surface.core_feature_expansion_ready) {
    reason.clear();
    return true;
  }

  reason = surface.failure_reason.empty()
               ? "lowering/runtime diagnostics core feature expansion is not ready"
               : surface.failure_reason;
  return false;
}
