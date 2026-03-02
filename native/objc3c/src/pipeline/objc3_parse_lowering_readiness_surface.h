#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

inline Objc3ParseLoweringReadinessSurface BuildObjc3ParseLoweringReadinessSurface(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  Objc3ParseLoweringReadinessSurface surface;
  surface.lexer_diagnostic_count = pipeline_result.stage_diagnostics.lexer.size();
  surface.parser_diagnostic_count = pipeline_result.stage_diagnostics.parser.size();
  surface.semantic_diagnostic_count = pipeline_result.stage_diagnostics.semantic.size();
  surface.parser_token_count = pipeline_result.parser_contract_snapshot.token_count;
  surface.parser_top_level_declaration_count = pipeline_result.parser_contract_snapshot.top_level_declaration_count;
  surface.parser_contract_snapshot_present =
      pipeline_result.parser_contract_snapshot.token_count > 0 ||
      pipeline_result.parser_contract_snapshot.top_level_declaration_count > 0 ||
      pipeline_result.parser_contract_snapshot.parser_diagnostic_count > 0;
  surface.parser_contract_deterministic = pipeline_result.parser_contract_snapshot.deterministic_handoff;
  surface.parser_recovery_replay_ready = pipeline_result.parser_contract_snapshot.parser_recovery_replay_ready;
  surface.semantic_integration_surface_built = pipeline_result.integration_surface.built;
  surface.semantic_diagnostics_deterministic = pipeline_result.sema_parity_surface.deterministic_semantic_diagnostics;
  surface.semantic_type_metadata_deterministic = pipeline_result.sema_parity_surface.deterministic_type_metadata_handoff;
  surface.symbol_graph_deterministic = pipeline_result.symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff;
  surface.scope_resolution_deterministic =
      pipeline_result.symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff;
  surface.object_pointer_type_handoff_deterministic =
      pipeline_result.object_pointer_nullability_generics_summary.deterministic_object_pointer_nullability_generics_handoff;

  Objc3LoweringIRBoundary lowering_boundary;
  std::string lowering_error;
  if (TryBuildObjc3LoweringIRBoundary(options.lowering, lowering_boundary, lowering_error)) {
    surface.lowering_boundary_ready = true;
    surface.lowering_boundary_replay_key = Objc3LoweringIRBoundaryReplayKey(lowering_boundary);
  } else {
    surface.lowering_boundary_ready = false;
    surface.failure_reason = "invalid lowering contract: " + lowering_error;
  }

  const bool diagnostics_clear =
      surface.lexer_diagnostic_count == 0 &&
      surface.parser_diagnostic_count == 0 &&
      surface.semantic_diagnostic_count == 0;
  const bool parse_snapshot_ready =
      surface.parser_contract_snapshot_present &&
      surface.parser_contract_deterministic &&
      surface.parser_recovery_replay_ready;
  const bool sema_handoff_ready =
      surface.semantic_integration_surface_built &&
      surface.semantic_diagnostics_deterministic &&
      surface.semantic_type_metadata_deterministic &&
      surface.symbol_graph_deterministic &&
      surface.scope_resolution_deterministic &&
      surface.object_pointer_type_handoff_deterministic;
  surface.ready_for_lowering = diagnostics_clear &&
                               parse_snapshot_ready &&
                               sema_handoff_ready &&
                               surface.lowering_boundary_ready;

  if (surface.ready_for_lowering || !surface.failure_reason.empty()) {
    return surface;
  }

  if (surface.lexer_diagnostic_count != 0) {
    surface.failure_reason = "lexer diagnostics present";
  } else if (surface.parser_diagnostic_count != 0) {
    surface.failure_reason = "parser diagnostics present";
  } else if (surface.semantic_diagnostic_count != 0) {
    surface.failure_reason = "semantic diagnostics present";
  } else if (!surface.parser_contract_snapshot_present) {
    surface.failure_reason = "parser contract snapshot missing";
  } else if (!surface.parser_contract_deterministic) {
    surface.failure_reason = "parser handoff is not deterministic";
  } else if (!surface.parser_recovery_replay_ready) {
    surface.failure_reason = "parser recovery handoff is not replay ready";
  } else if (!surface.semantic_integration_surface_built) {
    surface.failure_reason = "semantic integration surface not built";
  } else if (!surface.semantic_diagnostics_deterministic) {
    surface.failure_reason = "semantic diagnostics handoff is not deterministic";
  } else if (!surface.semantic_type_metadata_deterministic) {
    surface.failure_reason = "semantic type metadata handoff is not deterministic";
  } else if (!surface.symbol_graph_deterministic) {
    surface.failure_reason = "symbol graph handoff is not deterministic";
  } else if (!surface.scope_resolution_deterministic) {
    surface.failure_reason = "scope resolution handoff is not deterministic";
  } else if (!surface.object_pointer_type_handoff_deterministic) {
    surface.failure_reason = "object pointer/nullability handoff is not deterministic";
  } else if (!surface.lowering_boundary_ready) {
    surface.failure_reason = "lowering boundary is not ready";
  } else {
    surface.failure_reason = "parse-lowering readiness failed";
  }

  return surface;
}

inline bool IsObjc3ParseLoweringReadinessSurfaceReady(const Objc3ParseLoweringReadinessSurface &surface,
                                                      std::string &reason) {
  if (surface.ready_for_lowering) {
    reason.clear();
    return true;
  }
  reason = surface.failure_reason.empty() ? "parse-lowering readiness surface not ready" : surface.failure_reason;
  return false;
}
