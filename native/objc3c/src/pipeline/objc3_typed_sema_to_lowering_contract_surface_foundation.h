#pragma once

#include "pipeline/objc3_typed_sema_to_lowering_contract_surface_core.h"

struct Objc3TypedSemaToLoweringContractSurfaceFoundationState {
  std::string semantic_type_metadata_handoff_failure_reason;
  bool typed_core_feature_consistent = false;
  bool typed_core_feature_expansion_key_ready = false;
  bool typed_core_feature_edge_case_compatibility_key_ready = false;
  bool typed_core_feature_edge_case_robustness_key_ready = false;
};

inline Objc3TypedSemaToLoweringContractSurfaceFoundationState
PopulateObjc3TypedSemaToLoweringContractSurfaceFoundation(
    Objc3TypedSemaToLoweringContractSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  Objc3TypedSemaToLoweringContractSurfaceFoundationState state;

  surface.semantic_integration_surface_built = pipeline_result.integration_surface.built;
  surface.semantic_type_metadata_handoff_deterministic =
      IsDeterministicSemanticTypeMetadataHandoff(pipeline_result.sema_type_metadata_handoff);
  state.semantic_type_metadata_handoff_failure_reason =
      surface.semantic_type_metadata_handoff_deterministic
          ? std::string()
          : ExplainNonDeterministicSemanticTypeMetadataHandoff(
                pipeline_result.sema_type_metadata_handoff);
  surface.sema_parity_surface_ready =
      IsReadyObjc3SemaParityContractSurface(pipeline_result.sema_parity_surface);
  surface.sema_parity_surface_deterministic =
      pipeline_result.sema_parity_surface.deterministic_semantic_diagnostics &&
      pipeline_result.sema_parity_surface.deterministic_type_metadata_handoff;
  surface.executable_metadata_lowering_handoff_ready =
      IsReadyObjc3ExecutableMetadataLoweringHandoffSurface(
          pipeline_result.executable_metadata_lowering_handoff_surface);
  surface.executable_metadata_lowering_handoff_deterministic =
      pipeline_result.executable_metadata_lowering_handoff_surface
              .lowering_schema_frozen &&
      pipeline_result.executable_metadata_lowering_handoff_surface.fail_closed &&
      !pipeline_result.executable_metadata_lowering_handoff_surface.replay_key
           .empty();
  surface.executable_metadata_lowering_handoff_key =
      pipeline_result.executable_metadata_lowering_handoff_surface.replay_key;
  surface.executable_metadata_typed_lowering_handoff_ready =
      IsReadyObjc3ExecutableMetadataTypedLoweringHandoff(
          pipeline_result.executable_metadata_typed_lowering_handoff);
  surface.executable_metadata_typed_lowering_handoff_deterministic =
      pipeline_result.executable_metadata_typed_lowering_handoff.deterministic &&
      pipeline_result.executable_metadata_typed_lowering_handoff
          .manifest_schema_frozen &&
      !pipeline_result.executable_metadata_typed_lowering_handoff.replay_key
           .empty();
  surface.executable_metadata_typed_lowering_handoff_key =
      pipeline_result.executable_metadata_typed_lowering_handoff.replay_key;
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
  const bool canonical_literal_rejection_counts_consistent =
      IsObjc3FrontendCanonicalLiteralRejectionCountsConsistent(
          pipeline_result.canonical_literal_rejection_counts,
          parser_snapshot.token_count);
  const bool language_version_pragma_contract_consistent =
      IsObjc3TypedSemaToLoweringLanguageVersionPragmaContractConsistent(
          pipeline_result.language_version_pragma_contract);
  surface.language_version_pragma_coordinate_order_consistent =
      IsObjc3TypedSemaToLoweringLanguageVersionPragmaCoordinateOrderConsistent(
          pipeline_result.language_version_pragma_contract);
  surface.compatibility_handoff_consistent =
      canonical_literal_rejection_counts_consistent &&
      language_version_pragma_contract_consistent;
  surface.compatibility_handoff_key = BuildObjc3TypedSemaToLoweringCompatibilityHandoffKey(
      options,
      pipeline_result.canonical_literal_rejection_counts,
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
      surface.sema_parity_surface_ready &&
      surface.executable_metadata_typed_lowering_handoff_ready;
  surface.semantic_handoff_deterministic =
      surface.semantic_type_metadata_handoff_deterministic &&
      surface.sema_parity_surface_deterministic &&
      surface.executable_metadata_typed_lowering_handoff_deterministic &&
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
  state.typed_core_feature_consistent =
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
  state.typed_core_feature_expansion_key_ready =
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
  state.typed_core_feature_edge_case_compatibility_key_ready =
      !surface.typed_core_feature_edge_case_compatibility_key.empty();
  surface.typed_core_feature_edge_case_expansion_consistent =
      surface.typed_core_feature_edge_case_compatibility_ready &&
      surface.language_version_pragma_coordinate_order_consistent &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      state.typed_core_feature_edge_case_compatibility_key_ready;
  surface.typed_core_feature_edge_case_robustness_ready =
      surface.typed_core_feature_edge_case_expansion_consistent &&
      surface.parse_artifact_replay_key_deterministic &&
      !surface.compatibility_handoff_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty();
  surface.typed_core_feature_edge_case_robustness_key =
      BuildObjc3TypedSemaToLoweringCoreFeatureEdgeRobustnessKey(surface);
  state.typed_core_feature_edge_case_robustness_key_ready =
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

  return state;
}
