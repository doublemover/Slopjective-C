#pragma once

#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline std::string BuildObjc3LoweringRuntimeDiagnosticsSurfacingScaffoldKey(
    const Objc3LoweringRuntimeDiagnosticsSurfacingScaffold &scaffold) {
  std::ostringstream key;
  key << "lowering-runtime-diagnostics-surfacing-modular-split-scaffold:v1:"
      << "stage_diagnostics_bus_consistent="
      << (scaffold.stage_diagnostics_bus_consistent ? "true" : "false")
      << ";parse_readiness_surface_present="
      << (scaffold.parse_readiness_surface_present ? "true" : "false")
      << ";parse_readiness_surface_ready="
      << (scaffold.parse_readiness_surface_ready ? "true" : "false")
      << ";parse_diagnostics_hardening_consistent="
      << (scaffold.parse_diagnostics_hardening_consistent ? "true" : "false")
      << ";parser_source_precision_scaffold_ready="
      << (scaffold.parser_source_precision_scaffold_ready ? "true" : "false")
      << ";typed_handoff_key_deterministic="
      << (scaffold.typed_handoff_key_deterministic ? "true" : "false")
      << ";diagnostics_replay_key_ready="
      << (scaffold.diagnostics_replay_key_ready ? "true" : "false")
      << ";modular_split_ready="
      << (scaffold.modular_split_ready ? "true" : "false");
  return key.str();
}

inline Objc3LoweringRuntimeDiagnosticsSurfacingScaffold
BuildObjc3LoweringRuntimeDiagnosticsSurfacingScaffold(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3LoweringRuntimeDiagnosticsSurfacingScaffold scaffold;
  const Objc3ParseLoweringReadinessSurface &parse_surface =
      pipeline_result.parse_lowering_readiness_surface;
  const std::size_t stage_diagnostics_total =
      pipeline_result.stage_diagnostics.lexer.size() +
      pipeline_result.stage_diagnostics.parser.size() +
      pipeline_result.stage_diagnostics.semantic.size();
  scaffold.stage_diagnostics_bus_consistent =
      stage_diagnostics_total == pipeline_result.stage_diagnostics.size();
  scaffold.parse_readiness_surface_present =
      parse_surface.ready_for_lowering ||
      !parse_surface.parse_artifact_replay_key.empty() ||
      !parse_surface.failure_reason.empty();
  scaffold.parse_readiness_surface_ready =
      parse_surface.ready_for_lowering &&
      parse_surface.lowering_boundary_ready;
  scaffold.parse_diagnostics_hardening_consistent =
      parse_surface.parse_artifact_diagnostics_hardening_consistent;
  scaffold.parser_source_precision_scaffold_ready =
      parse_surface.parser_diagnostic_source_precision_scaffold_ready;
  scaffold.typed_handoff_key_deterministic =
      parse_surface.typed_handoff_key_deterministic;
  scaffold.parse_artifact_replay_key = parse_surface.parse_artifact_replay_key;
  scaffold.lowering_boundary_replay_key =
      parse_surface.lowering_boundary_replay_key;
  scaffold.diagnostics_replay_key_ready =
      !scaffold.parse_artifact_replay_key.empty() &&
      !scaffold.lowering_boundary_replay_key.empty() &&
      !parse_surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !parse_surface.parser_diagnostic_source_precision_scaffold_key.empty();
  scaffold.modular_split_ready =
      scaffold.stage_diagnostics_bus_consistent &&
      scaffold.parse_readiness_surface_present &&
      scaffold.parse_readiness_surface_ready &&
      scaffold.parse_diagnostics_hardening_consistent &&
      scaffold.parser_source_precision_scaffold_ready &&
      scaffold.typed_handoff_key_deterministic &&
      scaffold.diagnostics_replay_key_ready;
  scaffold.scaffold_key =
      BuildObjc3LoweringRuntimeDiagnosticsSurfacingScaffoldKey(scaffold);

  if (scaffold.modular_split_ready) {
    return scaffold;
  }

  if (!scaffold.stage_diagnostics_bus_consistent) {
    scaffold.failure_reason = "stage diagnostics bus accounting is inconsistent";
  } else if (!scaffold.parse_readiness_surface_present) {
    scaffold.failure_reason = "parse-lowering readiness surface missing";
  } else if (!scaffold.parse_readiness_surface_ready) {
    scaffold.failure_reason = "parse-lowering readiness surface is not ready";
  } else if (!scaffold.parse_diagnostics_hardening_consistent) {
    scaffold.failure_reason =
        "parse diagnostics hardening surface is inconsistent";
  } else if (!scaffold.parser_source_precision_scaffold_ready) {
    scaffold.failure_reason =
        "parser diagnostic source-precision scaffold is not ready";
  } else if (!scaffold.typed_handoff_key_deterministic) {
    scaffold.failure_reason = "typed handoff key is not deterministic";
  } else if (!scaffold.diagnostics_replay_key_ready) {
    scaffold.failure_reason = "diagnostics replay keys are not ready";
  } else {
    scaffold.failure_reason = "lowering/runtime diagnostics surfacing scaffold not ready";
  }

  return scaffold;
}

inline bool IsObjc3LoweringRuntimeDiagnosticsSurfacingScaffoldReady(
    const Objc3LoweringRuntimeDiagnosticsSurfacingScaffold &scaffold,
    std::string &reason) {
  if (scaffold.modular_split_ready) {
    reason.clear();
    return true;
  }

  reason = scaffold.failure_reason.empty()
               ? "lowering/runtime diagnostics surfacing scaffold not ready"
               : scaffold.failure_reason;
  return false;
}
