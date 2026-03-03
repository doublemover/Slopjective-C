#pragma once

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>

#include "pipeline/objc3_frontend_types.h"

inline constexpr std::size_t kObjc3TypedSemaToLoweringCoreFeatureCaseCount = 6u;
inline constexpr std::size_t kObjc3TypedSemaToLoweringCoreFeatureExpansionCaseCount = 4u;

inline std::size_t Objc3TypedSemaToLoweringParserSnapshotDeclarationBreakdownCount(
    const Objc3ParserContractSnapshot &snapshot) {
  return snapshot.global_decl_count + snapshot.protocol_decl_count +
         snapshot.interface_decl_count + snapshot.implementation_decl_count +
         snapshot.function_decl_count;
}

inline std::size_t Objc3TypedSemaToLoweringParsedProgramTopLevelDeclarationCount(
    const Objc3ParsedProgram &program) {
  const Objc3Program &ast = Objc3ParsedProgramAst(program);
  return ast.globals.size() + ast.protocols.size() + ast.interfaces.size() +
         ast.implementations.size() + ast.functions.size();
}

inline bool IsObjc3TypedSemaToLoweringLanguageVersionPragmaContractConsistent(
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

inline bool IsObjc3TypedSemaToLoweringLanguageVersionPragmaCoordinateOrderConsistent(
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

inline const char *Objc3TypedSemaToLoweringCompatibilityModeName(
    const Objc3FrontendCompatibilityMode mode) {
  return mode == Objc3FrontendCompatibilityMode::kLegacy ? "legacy" : "canonical";
}

inline std::string BuildObjc3TypedSemaToLoweringCompatibilityHandoffKey(
    const Objc3FrontendOptions &options,
    const Objc3FrontendMigrationHints &migration_hints,
    const Objc3FrontendLanguageVersionPragmaContract &pragma_contract,
    bool compatibility_handoff_consistent) {
  return "compatibility_mode=" +
         std::string(Objc3TypedSemaToLoweringCompatibilityModeName(options.compatibility_mode)) +
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

inline std::string BuildObjc3TypedSemaToLoweringParseArtifactEdgeRobustnessKey(
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

inline std::string BuildObjc3TypedSemaToLoweringCoreFeatureEdgeCaseCompatibilityKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-core-edge-compat:v1:compatibility_handoff_consistent=" +
         std::string(surface.compatibility_handoff_consistent ? "true" : "false") +
         ";language_version_pragma_coordinate_order_consistent=" +
         std::string(surface.language_version_pragma_coordinate_order_consistent ? "true" : "false") +
         ";parse_artifact_replay_key_deterministic=" +
         std::string(surface.parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";parse_artifact_edge_case_robustness_consistent=" +
         std::string(surface.parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
         ";compatibility_handoff_key=" + surface.compatibility_handoff_key +
         ";parse_artifact_edge_robustness_key=" + surface.parse_artifact_edge_robustness_key +
         ";ready=" + (surface.typed_core_feature_edge_case_compatibility_ready ? "true" : "false");
}

inline std::string BuildObjc3TypedSemaToLoweringCoreFeatureEdgeRobustnessKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-core-edge-robustness:v1:edge_case_compatibility_ready=" +
         std::string(surface.typed_core_feature_edge_case_compatibility_ready ? "true" : "false") +
         ";edge_case_expansion_consistent=" +
         std::string(surface.typed_core_feature_edge_case_expansion_consistent ? "true" : "false") +
         ";edge_case_robustness_ready=" +
         std::string(surface.typed_core_feature_edge_case_robustness_ready ? "true" : "false") +
         ";parse_artifact_replay_key_deterministic=" +
         std::string(surface.parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";compatibility_handoff_key=" + surface.compatibility_handoff_key +
         ";edge_case_compatibility_key=" + surface.typed_core_feature_edge_case_compatibility_key +
         ";parse_artifact_edge_robustness_key=" + surface.parse_artifact_edge_robustness_key;
}

inline std::string BuildObjc3TypedSemaToLoweringDiagnosticsHardeningKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-diagnostics-hardening:v1:edge_case_robustness_ready=" +
         std::string(surface.typed_core_feature_edge_case_robustness_ready ? "true" : "false") +
         ";typed_diagnostics_hardening_consistent=" +
         std::string(surface.typed_diagnostics_hardening_consistent ? "true" : "false") +
         ";typed_diagnostics_hardening_ready=" +
         std::string(surface.typed_diagnostics_hardening_ready ? "true" : "false") +
         ";typed_handoff_key_deterministic=" +
         std::string(surface.typed_handoff_key_deterministic ? "true" : "false") +
         ";edge_case_robustness_key=" + surface.typed_core_feature_edge_case_robustness_key;
}

inline std::string BuildObjc3TypedSemaToLoweringRecoveryDeterminismKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return "typed-sema-lowering-recovery-determinism:v1:typed_diagnostics_hardening_ready=" +
         std::string(surface.typed_diagnostics_hardening_ready ? "true" : "false") +
         ";typed_recovery_determinism_consistent=" +
         std::string(surface.typed_recovery_determinism_consistent ? "true" : "false") +
         ";typed_recovery_determinism_ready=" +
         std::string(surface.typed_recovery_determinism_ready ? "true" : "false") +
         ";parse_artifact_replay_key_deterministic=" +
         std::string(surface.parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";typed_diagnostics_hardening_key=" + surface.typed_diagnostics_hardening_key;
}

inline std::string BuildObjc3TypedSemaToLoweringContractHandoffKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  std::ostringstream key;
  key << "typed-sema-lowering:v1:"
      << "semantic_surface=" << (surface.semantic_integration_surface_built ? "true" : "false")
      << ";type_metadata=" << (surface.semantic_type_metadata_handoff_deterministic ? "true" : "false")
      << ";sema_parity=" << (surface.sema_parity_surface_ready ? "true" : "false")
      << ";sema_parity_deterministic=" << (surface.sema_parity_surface_deterministic ? "true" : "false")
      << ";protocol_category=" << (surface.protocol_category_handoff_deterministic ? "true" : "false")
      << ";class_protocol_category_linking="
      << (surface.class_protocol_category_linking_handoff_deterministic ? "true" : "false")
      << ";selector_normalization="
      << (surface.selector_normalization_handoff_deterministic ? "true" : "false")
      << ";property_attribute="
      << (surface.property_attribute_handoff_deterministic ? "true" : "false")
      << ";object_pointer=" << (surface.object_pointer_type_handoff_deterministic ? "true" : "false")
      << ";symbol_graph=" << (surface.symbol_graph_handoff_deterministic ? "true" : "false")
      << ";scope_resolution=" << (surface.scope_resolution_handoff_deterministic ? "true" : "false")
      << ";semantic_handoff_consistent=" << (surface.semantic_handoff_consistent ? "true" : "false")
      << ";semantic_handoff_deterministic=" << (surface.semantic_handoff_deterministic ? "true" : "false")
      << ";runtime_dispatch=" << (surface.runtime_dispatch_contract_consistent ? "true" : "false")
      << ";core_feature_passed_case_count=" << surface.typed_core_feature_passed_case_count
      << ";core_feature_failed_case_count=" << surface.typed_core_feature_failed_case_count
      << ";core_feature_consistent=" << (surface.typed_core_feature_consistent ? "true" : "false")
      << ";core_feature_expansion_passed_case_count=" << surface.typed_core_feature_expansion_passed_case_count
      << ";core_feature_expansion_failed_case_count=" << surface.typed_core_feature_expansion_failed_case_count
      << ";core_feature_expansion_consistent="
      << (surface.typed_core_feature_expansion_consistent ? "true" : "false")
      << ";compatibility_handoff_consistent="
      << (surface.compatibility_handoff_consistent ? "true" : "false")
      << ";language_version_pragma_coordinate_order_consistent="
      << (surface.language_version_pragma_coordinate_order_consistent ? "true" : "false")
      << ";parse_artifact_replay_key_deterministic="
      << (surface.parse_artifact_replay_key_deterministic ? "true" : "false")
      << ";parse_artifact_edge_case_robustness_consistent="
      << (surface.parse_artifact_edge_case_robustness_consistent ? "true" : "false")
      << ";core_feature_edge_case_compatibility_ready="
      << (surface.typed_core_feature_edge_case_compatibility_ready ? "true" : "false")
      << ";core_feature_edge_case_expansion_consistent="
      << (surface.typed_core_feature_edge_case_expansion_consistent ? "true" : "false")
      << ";core_feature_edge_case_robustness_ready="
      << (surface.typed_core_feature_edge_case_robustness_ready ? "true" : "false")
      << ";typed_diagnostics_hardening_consistent="
      << (surface.typed_diagnostics_hardening_consistent ? "true" : "false")
      << ";typed_diagnostics_hardening_ready="
      << (surface.typed_diagnostics_hardening_ready ? "true" : "false")
      << ";typed_recovery_determinism_consistent="
      << (surface.typed_recovery_determinism_consistent ? "true" : "false")
      << ";typed_recovery_determinism_ready="
      << (surface.typed_recovery_determinism_ready ? "true" : "false")
      << ";lowering_boundary=" << (surface.lowering_boundary_ready ? "true" : "false")
      << ";ready_for_lowering=" << (surface.ready_for_lowering ? "true" : "false");
  return key.str();
}

inline std::string BuildObjc3TypedSemaToLoweringCoreFeatureKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  std::ostringstream key;
  key << "typed-sema-lowering-core:v1:"
      << "case_count=" << surface.typed_core_feature_case_count
      << ";passed_case_count=" << surface.typed_core_feature_passed_case_count
      << ";failed_case_count=" << surface.typed_core_feature_failed_case_count
      << ";semantic_handoff_consistent=" << (surface.semantic_handoff_consistent ? "true" : "false")
      << ";semantic_handoff_deterministic=" << (surface.semantic_handoff_deterministic ? "true" : "false")
      << ";typed_handoff_key_deterministic=" << (surface.typed_handoff_key_deterministic ? "true" : "false")
      << ";runtime_dispatch_contract_consistent="
      << (surface.runtime_dispatch_contract_consistent ? "true" : "false")
      << ";lowering_boundary_ready=" << (surface.lowering_boundary_ready ? "true" : "false")
      << ";core_feature_expansion_consistent="
      << (surface.typed_core_feature_expansion_consistent ? "true" : "false")
      << ";core_feature_edge_case_compatibility_ready="
      << (surface.typed_core_feature_edge_case_compatibility_ready ? "true" : "false")
      << ";core_feature_edge_case_expansion_consistent="
      << (surface.typed_core_feature_edge_case_expansion_consistent ? "true" : "false")
      << ";core_feature_edge_case_robustness_ready="
      << (surface.typed_core_feature_edge_case_robustness_ready ? "true" : "false")
      << ";typed_diagnostics_hardening_consistent="
      << (surface.typed_diagnostics_hardening_consistent ? "true" : "false")
      << ";typed_diagnostics_hardening_ready="
      << (surface.typed_diagnostics_hardening_ready ? "true" : "false")
      << ";typed_recovery_determinism_consistent="
      << (surface.typed_recovery_determinism_consistent ? "true" : "false")
      << ";typed_recovery_determinism_ready="
      << (surface.typed_recovery_determinism_ready ? "true" : "false")
      << ";consistent=" << (surface.typed_core_feature_consistent ? "true" : "false");
  return key.str();
}

inline std::string BuildObjc3TypedSemaToLoweringCoreFeatureExpansionKey(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  std::ostringstream key;
  key << "typed-sema-lowering-core-expansion:v1:"
      << "case_count=" << surface.typed_core_feature_expansion_case_count
      << ";passed_case_count=" << surface.typed_core_feature_expansion_passed_case_count
      << ";failed_case_count=" << surface.typed_core_feature_expansion_failed_case_count
      << ";protocol_category_handoff_deterministic="
      << (surface.protocol_category_handoff_deterministic ? "true" : "false")
      << ";class_protocol_category_linking_handoff_deterministic="
      << (surface.class_protocol_category_linking_handoff_deterministic ? "true" : "false")
      << ";selector_normalization_handoff_deterministic="
      << (surface.selector_normalization_handoff_deterministic ? "true" : "false")
      << ";property_attribute_handoff_deterministic="
      << (surface.property_attribute_handoff_deterministic ? "true" : "false")
      << ";consistent=" << (surface.typed_core_feature_expansion_consistent ? "true" : "false");
  return key.str();
}

inline Objc3TypedSemaToLoweringContractSurface BuildObjc3TypedSemaToLoweringContractSurface(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  Objc3TypedSemaToLoweringContractSurface surface;
  surface.semantic_integration_surface_built = pipeline_result.integration_surface.built;
  surface.semantic_type_metadata_handoff_deterministic =
      IsDeterministicSemanticTypeMetadataHandoff(pipeline_result.sema_type_metadata_handoff);
  surface.sema_parity_surface_ready =
      IsReadyObjc3SemaParityContractSurface(pipeline_result.sema_parity_surface);
  surface.sema_parity_surface_deterministic =
      pipeline_result.sema_parity_surface.deterministic_semantic_diagnostics &&
      pipeline_result.sema_parity_surface.deterministic_type_metadata_handoff;
  surface.protocol_category_handoff_deterministic =
      pipeline_result.protocol_category_summary.deterministic_protocol_category_handoff;
  surface.class_protocol_category_linking_handoff_deterministic =
      pipeline_result.class_protocol_category_linking_summary
          .deterministic_class_protocol_category_linking_handoff;
  surface.selector_normalization_handoff_deterministic =
      pipeline_result.selector_normalization_summary.deterministic_selector_normalization_handoff;
  surface.property_attribute_handoff_deterministic =
      pipeline_result.property_attribute_summary.deterministic_property_attribute_handoff;
  surface.object_pointer_type_handoff_deterministic =
      pipeline_result.object_pointer_nullability_generics_summary
          .deterministic_object_pointer_nullability_generics_handoff;
  surface.symbol_graph_handoff_deterministic =
      pipeline_result.symbol_graph_scope_resolution_summary.deterministic_symbol_graph_handoff;
  surface.scope_resolution_handoff_deterministic =
      pipeline_result.symbol_graph_scope_resolution_summary.deterministic_scope_resolution_handoff;
  const Objc3ParserContractSnapshot &parser_snapshot = pipeline_result.parser_contract_snapshot;
  const std::size_t parser_snapshot_breakdown_count =
      Objc3TypedSemaToLoweringParserSnapshotDeclarationBreakdownCount(parser_snapshot);
  const bool parser_snapshot_breakdown_consistent =
      parser_snapshot_breakdown_count == parser_snapshot.top_level_declaration_count;
  const std::size_t ast_top_level_declaration_count =
      Objc3TypedSemaToLoweringParsedProgramTopLevelDeclarationCount(pipeline_result.program);
  const bool parse_artifact_handoff_consistent =
      parser_snapshot_breakdown_consistent &&
      ast_top_level_declaration_count == parser_snapshot.top_level_declaration_count;
  const bool parse_artifact_handoff_deterministic =
      parse_artifact_handoff_consistent && parser_snapshot.deterministic_handoff;
  const bool parse_artifact_layout_fingerprint_consistent =
      parser_snapshot.ast_top_level_layout_fingerprint ==
      BuildObjc3ParsedProgramTopLevelLayoutFingerprint(pipeline_result.program);
  const bool parse_artifact_fingerprint_consistent =
      parser_snapshot.ast_shape_fingerprint ==
          BuildObjc3ParsedProgramAstShapeFingerprint(pipeline_result.program) &&
      parse_artifact_layout_fingerprint_consistent;
  const std::size_t legacy_literal_total = pipeline_result.migration_hints.legacy_total();
  const bool migration_hints_consistent =
      legacy_literal_total ==
          pipeline_result.migration_hints.legacy_yes_count +
              pipeline_result.migration_hints.legacy_no_count +
              pipeline_result.migration_hints.legacy_null_count &&
      legacy_literal_total <= parser_snapshot.token_count &&
      (options.migration_assist || legacy_literal_total == 0);
  const bool language_version_pragma_contract_consistent =
      IsObjc3TypedSemaToLoweringLanguageVersionPragmaContractConsistent(
          pipeline_result.language_version_pragma_contract);
  surface.language_version_pragma_coordinate_order_consistent =
      IsObjc3TypedSemaToLoweringLanguageVersionPragmaCoordinateOrderConsistent(
          pipeline_result.language_version_pragma_contract);
  surface.compatibility_handoff_consistent =
      migration_hints_consistent &&
      language_version_pragma_contract_consistent;
  surface.compatibility_handoff_key = BuildObjc3TypedSemaToLoweringCompatibilityHandoffKey(
      options,
      pipeline_result.migration_hints,
      pipeline_result.language_version_pragma_contract,
      surface.compatibility_handoff_consistent);
  const std::uint64_t parser_contract_snapshot_fingerprint =
      BuildObjc3ParserContractSnapshotFingerprint(parser_snapshot);
  surface.parse_artifact_replay_key_deterministic =
      parse_artifact_handoff_deterministic &&
      parse_artifact_fingerprint_consistent &&
      surface.compatibility_handoff_consistent &&
      parser_contract_snapshot_fingerprint != 0;
  const bool parser_token_count_budget_consistent =
      parser_snapshot.token_count >= parser_snapshot_breakdown_count &&
      parser_snapshot.token_count >= parser_snapshot.top_level_declaration_count &&
      parser_snapshot.token_count >= ast_top_level_declaration_count;
  surface.parse_artifact_edge_case_robustness_consistent =
      parser_token_count_budget_consistent &&
      surface.language_version_pragma_coordinate_order_consistent &&
      surface.parse_artifact_replay_key_deterministic &&
      !surface.compatibility_handoff_key.empty();
  surface.parse_artifact_edge_robustness_key =
      BuildObjc3TypedSemaToLoweringParseArtifactEdgeRobustnessKey(
          parser_snapshot.token_count,
          parser_snapshot_breakdown_count,
          ast_top_level_declaration_count,
          parser_token_count_budget_consistent,
          surface.language_version_pragma_coordinate_order_consistent,
          surface.parse_artifact_edge_case_robustness_consistent);

  Objc3LoweringIRBoundary lowering_boundary;
  std::string lowering_error;
  if (TryBuildObjc3LoweringIRBoundary(options.lowering, lowering_boundary, lowering_error)) {
    surface.lowering_boundary_ready = true;
    surface.lowering_boundary_replay_key = Objc3LoweringIRBoundaryReplayKey(lowering_boundary);
  } else {
    surface.lowering_boundary_ready = false;
    surface.failure_reason = "invalid lowering contract: " + lowering_error;
  }

  surface.runtime_dispatch_contract_consistent =
      surface.lowering_boundary_ready &&
      lowering_boundary.runtime_dispatch_arg_slots >= kObjc3RuntimeDispatchDefaultArgs &&
      lowering_boundary.runtime_dispatch_arg_slots <= kObjc3RuntimeDispatchMaxArgs &&
      !lowering_boundary.runtime_dispatch_symbol.empty() &&
      lowering_boundary.selector_global_ordering == kObjc3SelectorGlobalOrdering;
  surface.semantic_handoff_consistent =
      surface.semantic_integration_surface_built &&
      surface.sema_parity_surface_ready;
  surface.semantic_handoff_deterministic =
      surface.semantic_type_metadata_handoff_deterministic &&
      surface.sema_parity_surface_deterministic &&
      surface.protocol_category_handoff_deterministic &&
      surface.class_protocol_category_linking_handoff_deterministic &&
      surface.selector_normalization_handoff_deterministic &&
      surface.property_attribute_handoff_deterministic &&
      surface.object_pointer_type_handoff_deterministic &&
      surface.symbol_graph_handoff_deterministic &&
      surface.scope_resolution_handoff_deterministic;

  const bool semantic_parity_feature_case_passed =
      surface.sema_parity_surface_ready &&
      surface.sema_parity_surface_deterministic;
  const bool symbol_graph_scope_resolution_feature_case_passed =
      surface.symbol_graph_handoff_deterministic &&
      surface.scope_resolution_handoff_deterministic;
  const bool lowering_runtime_boundary_feature_case_passed =
      surface.runtime_dispatch_contract_consistent &&
      surface.lowering_boundary_ready;
  surface.typed_core_feature_case_count = kObjc3TypedSemaToLoweringCoreFeatureCaseCount;
  surface.typed_core_feature_passed_case_count =
      static_cast<std::size_t>(surface.semantic_integration_surface_built) +
      static_cast<std::size_t>(surface.semantic_type_metadata_handoff_deterministic) +
      static_cast<std::size_t>(semantic_parity_feature_case_passed) +
      static_cast<std::size_t>(surface.object_pointer_type_handoff_deterministic) +
      static_cast<std::size_t>(symbol_graph_scope_resolution_feature_case_passed) +
      static_cast<std::size_t>(lowering_runtime_boundary_feature_case_passed);
  surface.typed_core_feature_failed_case_count =
      surface.typed_core_feature_case_count >= surface.typed_core_feature_passed_case_count
          ? (surface.typed_core_feature_case_count - surface.typed_core_feature_passed_case_count)
          : surface.typed_core_feature_case_count;
  const bool typed_core_feature_case_accounting_consistent =
      surface.typed_core_feature_case_count == kObjc3TypedSemaToLoweringCoreFeatureCaseCount &&
      surface.typed_core_feature_case_count > 0 &&
      surface.typed_core_feature_passed_case_count <= surface.typed_core_feature_case_count &&
      surface.typed_core_feature_failed_case_count ==
          (surface.typed_core_feature_case_count - surface.typed_core_feature_passed_case_count);
  const bool typed_core_feature_cases_passed =
      surface.typed_core_feature_passed_case_count == surface.typed_core_feature_case_count &&
      surface.typed_core_feature_failed_case_count == 0;
  const bool typed_core_feature_consistent =
      typed_core_feature_case_accounting_consistent &&
      typed_core_feature_cases_passed &&
      surface.semantic_handoff_consistent &&
      surface.semantic_handoff_deterministic &&
      lowering_runtime_boundary_feature_case_passed;
  surface.typed_core_feature_expansion_case_count =
      kObjc3TypedSemaToLoweringCoreFeatureExpansionCaseCount;
  surface.typed_core_feature_expansion_passed_case_count =
      static_cast<std::size_t>(surface.protocol_category_handoff_deterministic) +
      static_cast<std::size_t>(surface.class_protocol_category_linking_handoff_deterministic) +
      static_cast<std::size_t>(surface.selector_normalization_handoff_deterministic) +
      static_cast<std::size_t>(surface.property_attribute_handoff_deterministic);
  surface.typed_core_feature_expansion_failed_case_count =
      surface.typed_core_feature_expansion_case_count >=
              surface.typed_core_feature_expansion_passed_case_count
          ? (surface.typed_core_feature_expansion_case_count -
             surface.typed_core_feature_expansion_passed_case_count)
          : surface.typed_core_feature_expansion_case_count;
  const bool typed_core_feature_expansion_case_accounting_consistent =
      surface.typed_core_feature_expansion_case_count ==
          kObjc3TypedSemaToLoweringCoreFeatureExpansionCaseCount &&
      surface.typed_core_feature_expansion_case_count > 0 &&
      surface.typed_core_feature_expansion_passed_case_count <=
          surface.typed_core_feature_expansion_case_count &&
      surface.typed_core_feature_expansion_failed_case_count ==
          (surface.typed_core_feature_expansion_case_count -
           surface.typed_core_feature_expansion_passed_case_count);
  const bool typed_core_feature_expansion_cases_passed =
      surface.typed_core_feature_expansion_passed_case_count ==
          surface.typed_core_feature_expansion_case_count &&
      surface.typed_core_feature_expansion_failed_case_count == 0;
  surface.typed_core_feature_expansion_consistent =
      typed_core_feature_expansion_case_accounting_consistent &&
      typed_core_feature_expansion_cases_passed;
  surface.typed_core_feature_expansion_key =
      BuildObjc3TypedSemaToLoweringCoreFeatureExpansionKey(surface);
  const bool typed_core_feature_expansion_key_ready =
      !surface.typed_core_feature_expansion_key.empty();
  surface.typed_core_feature_edge_case_compatibility_ready =
      surface.typed_core_feature_expansion_consistent &&
      surface.compatibility_handoff_consistent &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      !surface.compatibility_handoff_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty();
  surface.typed_core_feature_edge_case_compatibility_key =
      BuildObjc3TypedSemaToLoweringCoreFeatureEdgeCaseCompatibilityKey(surface);
  const bool typed_core_feature_edge_case_compatibility_key_ready =
      !surface.typed_core_feature_edge_case_compatibility_key.empty();
  surface.typed_core_feature_edge_case_expansion_consistent =
      surface.typed_core_feature_edge_case_compatibility_ready &&
      surface.language_version_pragma_coordinate_order_consistent &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      typed_core_feature_edge_case_compatibility_key_ready;
  surface.typed_core_feature_edge_case_robustness_ready =
      surface.typed_core_feature_edge_case_expansion_consistent &&
      surface.parse_artifact_replay_key_deterministic &&
      !surface.compatibility_handoff_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty();
  surface.typed_core_feature_edge_case_robustness_key =
      BuildObjc3TypedSemaToLoweringCoreFeatureEdgeRobustnessKey(surface);
  const bool typed_core_feature_edge_case_robustness_key_ready =
      !surface.typed_core_feature_edge_case_robustness_key.empty();
  surface.typed_diagnostics_hardening_consistent =
      surface.typed_core_feature_edge_case_robustness_ready &&
      surface.semantic_handoff_deterministic &&
      surface.sema_parity_surface_deterministic &&
      surface.parse_artifact_replay_key_deterministic;
  surface.typed_diagnostics_hardening_ready =
      surface.typed_diagnostics_hardening_consistent &&
      !surface.typed_core_feature_edge_case_robustness_key.empty() &&
      !surface.compatibility_handoff_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty();
  surface.typed_diagnostics_hardening_key =
      BuildObjc3TypedSemaToLoweringDiagnosticsHardeningKey(surface);
  const bool typed_diagnostics_hardening_key_ready =
      !surface.typed_diagnostics_hardening_key.empty();
  surface.typed_recovery_determinism_consistent =
      surface.typed_diagnostics_hardening_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.semantic_handoff_deterministic;
  surface.typed_recovery_determinism_ready =
      surface.typed_recovery_determinism_consistent &&
      !surface.typed_diagnostics_hardening_key.empty() &&
      !surface.typed_core_feature_edge_case_robustness_key.empty();
  surface.typed_recovery_determinism_key =
      BuildObjc3TypedSemaToLoweringRecoveryDeterminismKey(surface);
  const bool typed_recovery_determinism_key_ready =
      !surface.typed_recovery_determinism_key.empty();
  surface.typed_core_feature_consistent =
      typed_core_feature_consistent &&
      surface.typed_core_feature_expansion_consistent &&
      typed_core_feature_expansion_key_ready &&
      surface.typed_core_feature_edge_case_compatibility_ready &&
      typed_core_feature_edge_case_compatibility_key_ready &&
      surface.typed_core_feature_edge_case_expansion_consistent &&
      surface.typed_core_feature_edge_case_robustness_ready &&
      typed_core_feature_edge_case_robustness_key_ready &&
      surface.typed_diagnostics_hardening_consistent &&
      surface.typed_diagnostics_hardening_ready &&
      typed_diagnostics_hardening_key_ready &&
      surface.typed_recovery_determinism_consistent &&
      surface.typed_recovery_determinism_ready &&
      typed_recovery_determinism_key_ready;

  surface.ready_for_lowering = surface.typed_core_feature_consistent;
  surface.typed_handoff_key = BuildObjc3TypedSemaToLoweringContractHandoffKey(surface);
  surface.typed_handoff_key_deterministic =
      surface.semantic_handoff_deterministic &&
      surface.runtime_dispatch_contract_consistent &&
      surface.lowering_boundary_ready &&
      !surface.typed_handoff_key.empty();
  surface.typed_core_feature_consistent =
      surface.typed_core_feature_consistent &&
      surface.typed_handoff_key_deterministic;
  surface.typed_core_feature_key = BuildObjc3TypedSemaToLoweringCoreFeatureKey(surface);
  surface.ready_for_lowering = surface.typed_core_feature_consistent;

  if (surface.ready_for_lowering || !surface.failure_reason.empty()) {
    return surface;
  }

  if (!surface.semantic_integration_surface_built) {
    surface.failure_reason = "semantic integration surface not built";
  } else if (!surface.semantic_type_metadata_handoff_deterministic) {
    surface.failure_reason = "semantic type metadata handoff is not deterministic";
  } else if (!surface.sema_parity_surface_ready) {
    surface.failure_reason = "semantic parity surface is not ready";
  } else if (!surface.sema_parity_surface_deterministic) {
    surface.failure_reason = "semantic parity surface is not deterministic";
  } else if (!surface.protocol_category_handoff_deterministic) {
    surface.failure_reason = "protocol/category handoff is not deterministic";
  } else if (!surface.class_protocol_category_linking_handoff_deterministic) {
    surface.failure_reason = "class/protocol/category linking handoff is not deterministic";
  } else if (!surface.selector_normalization_handoff_deterministic) {
    surface.failure_reason = "selector normalization handoff is not deterministic";
  } else if (!surface.property_attribute_handoff_deterministic) {
    surface.failure_reason = "property attribute handoff is not deterministic";
  } else if (!surface.object_pointer_type_handoff_deterministic) {
    surface.failure_reason = "object pointer/nullability handoff is not deterministic";
  } else if (!surface.symbol_graph_handoff_deterministic) {
    surface.failure_reason = "symbol graph handoff is not deterministic";
  } else if (!surface.scope_resolution_handoff_deterministic) {
    surface.failure_reason = "scope resolution handoff is not deterministic";
  } else if (!surface.semantic_handoff_consistent) {
    surface.failure_reason = "semantic handoff is inconsistent";
  } else if (!surface.semantic_handoff_deterministic) {
    surface.failure_reason = "semantic handoff is not deterministic";
  } else if (!surface.runtime_dispatch_contract_consistent) {
    surface.failure_reason = "runtime dispatch contract is inconsistent";
  } else if (!surface.lowering_boundary_ready) {
    surface.failure_reason = "lowering boundary is not ready";
  } else if (!surface.typed_core_feature_expansion_consistent) {
    surface.failure_reason = "typed sema-to-lowering core feature expansion is inconsistent";
  } else if (surface.typed_core_feature_expansion_key.empty()) {
    surface.failure_reason = "typed core feature expansion key is empty";
  } else if (!surface.compatibility_handoff_consistent) {
    surface.failure_reason = "typed sema-to-lowering compatibility handoff is inconsistent";
  } else if (!surface.parse_artifact_replay_key_deterministic) {
    surface.failure_reason = "typed sema-to-lowering parse artifact replay key is not deterministic";
  } else if (!surface.language_version_pragma_coordinate_order_consistent) {
    surface.failure_reason =
        "typed sema-to-lowering language version pragma coordinate order is inconsistent";
  } else if (!surface.parse_artifact_edge_case_robustness_consistent) {
    surface.failure_reason = "typed sema-to-lowering parse artifact edge-case robustness is inconsistent";
  } else if (!surface.typed_core_feature_edge_case_compatibility_ready) {
    surface.failure_reason = "typed sema-to-lowering edge-case compatibility is not ready";
  } else if (surface.typed_core_feature_edge_case_compatibility_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering edge-case compatibility key is empty";
  } else if (!surface.typed_core_feature_edge_case_expansion_consistent) {
    surface.failure_reason = "typed sema-to-lowering edge-case expansion is inconsistent";
  } else if (!surface.typed_core_feature_edge_case_robustness_ready) {
    surface.failure_reason = "typed sema-to-lowering edge-case robustness is not ready";
  } else if (surface.typed_core_feature_edge_case_robustness_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering edge-case robustness key is empty";
  } else if (!surface.typed_diagnostics_hardening_consistent) {
    surface.failure_reason = "typed sema-to-lowering diagnostics hardening is inconsistent";
  } else if (!surface.typed_diagnostics_hardening_ready) {
    surface.failure_reason = "typed sema-to-lowering diagnostics hardening is not ready";
  } else if (surface.typed_diagnostics_hardening_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering diagnostics hardening key is empty";
  } else if (!surface.typed_recovery_determinism_consistent) {
    surface.failure_reason = "typed sema-to-lowering recovery/determinism is inconsistent";
  } else if (!surface.typed_recovery_determinism_ready) {
    surface.failure_reason = "typed sema-to-lowering recovery/determinism is not ready";
  } else if (surface.typed_recovery_determinism_key.empty()) {
    surface.failure_reason = "typed sema-to-lowering recovery/determinism key is empty";
  } else if (!surface.typed_handoff_key_deterministic) {
    surface.failure_reason = "typed handoff key is not deterministic";
  } else if (!surface.typed_core_feature_consistent) {
    surface.failure_reason = "typed sema-to-lowering core feature contract is inconsistent";
  } else {
    surface.failure_reason = "typed sema-to-lowering contract readiness failed";
  }

  return surface;
}

inline bool IsObjc3TypedSemaToLoweringContractSurfaceReady(
    const Objc3TypedSemaToLoweringContractSurface &surface,
    std::string &failure_reason) {
  if (surface.ready_for_lowering) {
    failure_reason.clear();
    return true;
  }
  failure_reason = surface.failure_reason.empty() ? "typed sema-to-lowering readiness failed"
                                                  : surface.failure_reason;
  return false;
}
