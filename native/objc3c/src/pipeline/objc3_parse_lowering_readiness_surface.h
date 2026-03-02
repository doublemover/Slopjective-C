#pragma once

#include <cstdint>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline std::size_t Objc3ParserSnapshotDeclarationBreakdownCount(const Objc3ParserContractSnapshot &snapshot) {
  return snapshot.global_decl_count + snapshot.protocol_decl_count +
         snapshot.interface_decl_count + snapshot.implementation_decl_count +
         snapshot.function_decl_count;
}

inline std::size_t Objc3ParsedProgramTopLevelDeclarationCount(const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  return ast.globals.size() + ast.protocols.size() + ast.interfaces.size() +
         ast.implementations.size() + ast.functions.size();
}

inline std::string BuildObjc3ParseArtifactHandoffKey(
    const Objc3ParserContractSnapshot &snapshot,
    std::size_t ast_top_level_declaration_count,
    std::size_t parser_diagnostic_count,
    bool handoff_deterministic) {
  return "parser_snapshot=" + std::to_string(snapshot.top_level_declaration_count) + ":" +
         std::to_string(snapshot.global_decl_count) + ":" +
         std::to_string(snapshot.protocol_decl_count) + ":" +
         std::to_string(snapshot.interface_decl_count) + ":" +
         std::to_string(snapshot.implementation_decl_count) + ":" +
         std::to_string(snapshot.function_decl_count) + ";ast_top_level=" +
         std::to_string(ast_top_level_declaration_count) + ";parser_diagnostics=" +
         std::to_string(parser_diagnostic_count) + ";deterministic=" +
         (handoff_deterministic ? "true" : "false");
}

inline std::string BuildObjc3ParseArtifactReplayKey(
    const Objc3ParserContractSnapshot &snapshot,
    std::uint64_t parser_contract_snapshot_fingerprint,
    std::uint64_t parser_ast_top_level_layout_fingerprint,
    std::uint64_t ast_shape_fingerprint,
    std::uint64_t ast_top_level_layout_fingerprint,
    const std::string &compatibility_handoff_key,
    bool fingerprint_consistent,
    bool replay_key_deterministic) {
  return "parser_snapshot_fingerprint=" + std::to_string(parser_contract_snapshot_fingerprint) +
         ";snapshot_ast_shape_fingerprint=" + std::to_string(snapshot.ast_shape_fingerprint) +
         ";ast_shape_fingerprint=" + std::to_string(ast_shape_fingerprint) +
         ";snapshot_ast_top_level_layout_fingerprint=" +
         std::to_string(parser_ast_top_level_layout_fingerprint) +
         ";ast_top_level_layout_fingerprint=" + std::to_string(ast_top_level_layout_fingerprint) +
         ";compatibility_handoff_key=" + compatibility_handoff_key +
         ";fingerprint_consistent=" + (fingerprint_consistent ? "true" : "false") +
         ";deterministic=" + (replay_key_deterministic ? "true" : "false");
}

inline const char *Objc3FrontendCompatibilityModeName(const Objc3FrontendCompatibilityMode mode) {
  return mode == Objc3FrontendCompatibilityMode::kLegacy ? "legacy" : "canonical";
}

inline bool IsObjc3LanguageVersionPragmaContractConsistent(
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract) {
  if (!pragma_contract.seen) {
    return pragma_contract.directive_count == 0 &&
           !pragma_contract.duplicate &&
           !pragma_contract.non_leading &&
           pragma_contract.first_line == 0 &&
           pragma_contract.first_column == 0 &&
           pragma_contract.last_line == 0 &&
           pragma_contract.last_column == 0;
  }

  const bool coordinates_present =
      pragma_contract.first_line > 0 &&
      pragma_contract.first_column > 0 &&
      pragma_contract.last_line > 0 &&
      pragma_contract.last_column > 0;
  const bool duplicate_consistent =
      !pragma_contract.duplicate || pragma_contract.directive_count > 1;
  return pragma_contract.directive_count > 0 &&
         coordinates_present &&
         duplicate_consistent;
}

inline bool IsObjc3LanguageVersionPragmaCoordinateOrderConsistent(
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract) {
  if (!pragma_contract.seen) {
    return true;
  }

  const bool first_before_or_equal_last =
      pragma_contract.first_line < pragma_contract.last_line ||
      (pragma_contract.first_line == pragma_contract.last_line &&
       pragma_contract.first_column <= pragma_contract.last_column);
  const bool single_directive_coordinates_consistent =
      pragma_contract.directive_count != 1 ||
      (pragma_contract.first_line == pragma_contract.last_line &&
       pragma_contract.first_column == pragma_contract.last_column);
  return first_before_or_equal_last &&
         single_directive_coordinates_consistent;
}

inline std::string BuildObjc3CompatibilityHandoffKey(
    const Objc3FrontendOptions &options,
    const Objc3FrontendMigrationHints &migration_hints,
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract,
    bool compatibility_handoff_consistent) {
  return "compatibility_mode=" +
         std::string(Objc3FrontendCompatibilityModeName(options.compatibility_mode)) +
         ";migration_assist=" + (options.migration_assist ? "true" : "false") +
         ";legacy_literals=" + std::to_string(migration_hints.legacy_yes_count) + ":" +
         std::to_string(migration_hints.legacy_no_count) + ":" +
         std::to_string(migration_hints.legacy_null_count) +
         ";language_version_pragma=" + (pragma_contract.seen ? "seen" : "none") + ":" +
         std::to_string(pragma_contract.directive_count) + ":" +
         (pragma_contract.duplicate ? "duplicate" : "single") + ":" +
         (pragma_contract.non_leading ? "non-leading" : "leading") +
         ";consistent=" + (compatibility_handoff_consistent ? "true" : "false");
}

inline std::string BuildObjc3ParseArtifactEdgeRobustnessKey(
    std::size_t parser_token_count,
    std::size_t parser_snapshot_breakdown_count,
    std::size_t ast_top_level_declaration_count,
    bool parser_token_count_budget_consistent,
    bool language_version_pragma_coordinate_order_consistent,
    bool parse_artifact_edge_case_robustness_consistent) {
  return "parser_tokens=" + std::to_string(parser_token_count) +
         ";snapshot_breakdown=" + std::to_string(parser_snapshot_breakdown_count) +
         ";ast_top_level=" + std::to_string(ast_top_level_declaration_count) +
         ";token_budget_consistent=" + (parser_token_count_budget_consistent ? "true" : "false") +
         ";pragma_coordinate_order_consistent=" +
         (language_version_pragma_coordinate_order_consistent ? "true" : "false") +
         ";consistent=" + (parse_artifact_edge_case_robustness_consistent ? "true" : "false");
}

inline Objc3ParseLoweringReadinessSurface BuildObjc3ParseLoweringReadinessSurface(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  Objc3ParseLoweringReadinessSurface surface;
  const Objc3ParserContractSnapshot &parser_snapshot = pipeline_result.parser_contract_snapshot;
  surface.lexer_diagnostic_count = pipeline_result.stage_diagnostics.lexer.size();
  surface.parser_diagnostic_count = pipeline_result.stage_diagnostics.parser.size();
  surface.semantic_diagnostic_count = pipeline_result.stage_diagnostics.semantic.size();
  surface.parser_token_count = parser_snapshot.token_count;
  surface.parser_top_level_declaration_count = parser_snapshot.top_level_declaration_count;
  surface.parser_contract_snapshot_fingerprint = BuildObjc3ParserContractSnapshotFingerprint(parser_snapshot);
  surface.parser_ast_shape_fingerprint = parser_snapshot.ast_shape_fingerprint;
  surface.parser_ast_top_level_layout_fingerprint = parser_snapshot.ast_top_level_layout_fingerprint;
  surface.ast_shape_fingerprint = BuildObjc3ParsedProgramAstShapeFingerprint(pipeline_result.program);
  surface.ast_top_level_layout_fingerprint =
      BuildObjc3ParsedProgramTopLevelLayoutFingerprint(pipeline_result.program);
  surface.parser_contract_snapshot_present =
      parser_snapshot.token_count > 0 ||
      parser_snapshot.top_level_declaration_count > 0 ||
      parser_snapshot.parser_diagnostic_count > 0;
  surface.parser_contract_deterministic = parser_snapshot.deterministic_handoff;
  surface.parser_recovery_replay_ready = parser_snapshot.parser_recovery_replay_ready;
  const std::size_t parser_snapshot_breakdown_count =
      Objc3ParserSnapshotDeclarationBreakdownCount(parser_snapshot);
  const std::size_t ast_top_level_declaration_count =
      Objc3ParsedProgramTopLevelDeclarationCount(pipeline_result.program);
  const bool parser_snapshot_breakdown_consistent =
      parser_snapshot_breakdown_count == parser_snapshot.top_level_declaration_count;
  const bool parser_diagnostic_surface_consistent =
      parser_snapshot.parser_diagnostic_count == surface.parser_diagnostic_count;
  surface.parser_token_count_budget_consistent =
      surface.parser_token_count >= parser_snapshot_breakdown_count &&
      surface.parser_token_count >= parser_snapshot.top_level_declaration_count &&
      surface.parser_token_count >= ast_top_level_declaration_count;
  surface.parse_artifact_handoff_consistent =
      parser_snapshot_breakdown_consistent &&
      ast_top_level_declaration_count == parser_snapshot.top_level_declaration_count;
  surface.parse_artifact_handoff_deterministic =
      surface.parse_artifact_handoff_consistent &&
      parser_diagnostic_surface_consistent &&
      surface.parser_contract_deterministic;
  surface.parse_artifact_layout_fingerprint_consistent =
      surface.parser_ast_top_level_layout_fingerprint == surface.ast_top_level_layout_fingerprint;
  surface.parse_artifact_fingerprint_consistent =
      surface.parser_ast_shape_fingerprint == surface.ast_shape_fingerprint &&
      surface.parse_artifact_layout_fingerprint_consistent;
  const std::size_t legacy_literal_total = pipeline_result.migration_hints.legacy_total();
  const bool migration_hints_consistent =
      legacy_literal_total ==
          pipeline_result.migration_hints.legacy_yes_count +
              pipeline_result.migration_hints.legacy_no_count +
              pipeline_result.migration_hints.legacy_null_count &&
      legacy_literal_total <= surface.parser_token_count &&
      (options.migration_assist || legacy_literal_total == 0);
  const bool language_version_pragma_contract_consistent =
      IsObjc3LanguageVersionPragmaContractConsistent(
          pipeline_result.language_version_pragma_contract);
  surface.language_version_pragma_coordinate_order_consistent =
      IsObjc3LanguageVersionPragmaCoordinateOrderConsistent(
          pipeline_result.language_version_pragma_contract);
  surface.compatibility_handoff_consistent =
      migration_hints_consistent &&
      language_version_pragma_contract_consistent;
  surface.compatibility_handoff_key = BuildObjc3CompatibilityHandoffKey(
      options,
      pipeline_result.migration_hints,
      pipeline_result.language_version_pragma_contract,
      surface.compatibility_handoff_consistent);
  surface.parse_artifact_replay_key_deterministic =
      surface.parse_artifact_handoff_deterministic &&
      surface.parse_artifact_fingerprint_consistent &&
      surface.compatibility_handoff_consistent &&
      surface.parser_contract_snapshot_fingerprint != 0;
  surface.parse_artifact_handoff_key = BuildObjc3ParseArtifactHandoffKey(
      parser_snapshot,
      ast_top_level_declaration_count,
      surface.parser_diagnostic_count,
      surface.parse_artifact_handoff_deterministic);
  surface.parse_artifact_replay_key = BuildObjc3ParseArtifactReplayKey(
      parser_snapshot,
      surface.parser_contract_snapshot_fingerprint,
      surface.parser_ast_top_level_layout_fingerprint,
      surface.ast_shape_fingerprint,
      surface.ast_top_level_layout_fingerprint,
      surface.compatibility_handoff_key,
      surface.parse_artifact_fingerprint_consistent,
      surface.parse_artifact_replay_key_deterministic);
  surface.parse_artifact_edge_case_robustness_consistent =
      surface.parser_token_count_budget_consistent &&
      surface.language_version_pragma_coordinate_order_consistent &&
      !surface.parse_artifact_handoff_key.empty() &&
      !surface.compatibility_handoff_key.empty() &&
      !surface.parse_artifact_replay_key.empty();
  surface.parse_artifact_edge_robustness_key = BuildObjc3ParseArtifactEdgeRobustnessKey(
      surface.parser_token_count,
      parser_snapshot_breakdown_count,
      ast_top_level_declaration_count,
      surface.parser_token_count_budget_consistent,
      surface.language_version_pragma_coordinate_order_consistent,
      surface.parse_artifact_edge_case_robustness_consistent);
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
      surface.parser_recovery_replay_ready &&
      surface.parse_artifact_handoff_deterministic;
  const bool parse_artifact_replay_key_ready =
      surface.parse_artifact_replay_key_deterministic;
  const bool parse_snapshot_replay_ready =
      parse_snapshot_ready &&
      parse_artifact_replay_key_ready &&
      surface.parse_artifact_edge_case_robustness_consistent;
  const bool sema_handoff_ready =
      surface.semantic_integration_surface_built &&
      surface.semantic_diagnostics_deterministic &&
      surface.semantic_type_metadata_deterministic &&
      surface.symbol_graph_deterministic &&
      surface.scope_resolution_deterministic &&
      surface.object_pointer_type_handoff_deterministic;
  surface.ready_for_lowering = diagnostics_clear &&
                               parse_snapshot_replay_ready &&
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
  } else if (!surface.parse_artifact_handoff_consistent) {
    surface.failure_reason = "parse artifact handoff is inconsistent";
  } else if (!surface.parse_artifact_handoff_deterministic) {
    surface.failure_reason = "parse artifact handoff is not deterministic";
  } else if (!surface.parse_artifact_layout_fingerprint_consistent) {
    surface.failure_reason = "parse artifact layout fingerprint is inconsistent";
  } else if (!surface.parse_artifact_fingerprint_consistent) {
    surface.failure_reason = "parse artifact fingerprint is inconsistent";
  } else if (!surface.compatibility_handoff_consistent) {
    surface.failure_reason = "compatibility handoff is inconsistent";
  } else if (!surface.parse_artifact_replay_key_deterministic) {
    surface.failure_reason = "parse artifact replay key is not deterministic";
  } else if (!surface.parser_token_count_budget_consistent) {
    surface.failure_reason = "parser token count budget is inconsistent";
  } else if (!surface.language_version_pragma_coordinate_order_consistent) {
    surface.failure_reason = "language-version pragma coordinate order is inconsistent";
  } else if (!surface.parse_artifact_edge_case_robustness_consistent) {
    surface.failure_reason = "parse artifact edge-case robustness is inconsistent";
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
