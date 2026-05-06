#pragma once

struct Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary {
  std::string contract_id =
      kObjc3CompatibilityStrictnessClaimSemanticsContractId;
  std::string runnable_feature_claim_inventory_contract_id =
      kObjc3RunnableFeatureClaimInventoryContractId;
  std::string feature_claim_truth_surface_contract_id =
      kObjc3FeatureClaimStrictnessTruthSurfaceContractId;
  std::string frontend_surface_path =
      kObjc3CompatibilityStrictnessClaimSemanticsSurfacePath;
  std::string semantic_model =
      kObjc3CompatibilityStrictnessClaimSemanticModel;
  std::string downgrade_model =
      kObjc3CompatibilityStrictnessClaimDowngradeModel;
  std::string rejection_model =
      kObjc3CompatibilityStrictnessClaimRejectionModel;
  std::string canonical_interface_truth_model =
      kObjc3CompatibilityStrictnessClaimCanonicalInterfaceTruthModel;
  std::string separate_compilation_macro_truth_model =
      kObjc3CompatibilityStrictnessClaimSeparateCompilationMacroTruthModel;
  std::string canonical_interface_payload_mode =
      kObjc3CompatibilityStrictnessClaimCanonicalInterfacePayloadMode;
  std::string effective_language_profile = "canonical";
  std::vector<std::string> suppressed_macro_claim_ids;
  bool canonical_literal_rejection_diagnostics_enabled = false;
  std::size_t valid_language_profile_count = 0;
  std::size_t live_selection_surface_count = 0;
  std::size_t valid_selection_combination_count = 0;
  std::size_t runnable_feature_claim_count = 0;
  std::size_t downgraded_source_only_claim_count = 0;
  std::size_t rejected_unsupported_feature_claim_count = 0;
  std::size_t rejected_selection_surface_count = 0;
  std::size_t suppressed_macro_claim_count = 0;
  // unsupported-feature enforcement anchor: the frontend
  // mirror of the sema packet must preserve zero-site runnable proofs and
  // deterministic early rejection accounting for accepted advanced surfaces.
  std::size_t live_unsupported_feature_family_count = 0;
  std::size_t live_unsupported_feature_site_count = 0;
  std::size_t live_unsupported_feature_diagnostic_count = 0;
  std::size_t throws_source_rejection_site_count = 0;
  std::size_t blocks_source_rejection_site_count = 0;
  std::size_t arc_source_rejection_site_count = 0;
  bool fail_closed = false;
  bool semantic_boundary_ready = false;
  bool language_profile_semantics_landed = false;
  bool canonical_literal_rejection_semantics_landed = false;
  bool source_only_claim_downgrade_semantics_landed = false;
  bool unsupported_feature_claim_rejection_semantics_landed = false;
  bool live_unsupported_feature_source_rejection_landed = false;
  bool strictness_selection_rejection_semantics_landed = false;
  bool feature_macro_claim_suppression_semantics_landed = false;
  bool canonical_interface_truth_semantics_landed = false;
  bool separate_compilation_macro_truth_semantics_landed = false;
  bool selected_configuration_valid = false;
  bool selected_configuration_downgraded = false;
  bool selected_configuration_rejected = false;
  bool ready_for_lowering_and_runtime = false;
  std::string semantic_boundary_replay_key;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3FrontendCompatibilityStrictnessClaimSemanticsSummary(
    const Objc3FrontendCompatibilityStrictnessClaimSemanticsSummary &summary) {
  const bool language_profile_valid =
      summary.effective_language_profile == "canonical";
  return !summary.contract_id.empty() &&
         !summary.runnable_feature_claim_inventory_contract_id.empty() &&
         !summary.feature_claim_truth_surface_contract_id.empty() &&
         !summary.frontend_surface_path.empty() &&
         !summary.semantic_model.empty() && !summary.downgrade_model.empty() &&
         !summary.rejection_model.empty() &&
         !summary.canonical_interface_truth_model.empty() &&
         !summary.separate_compilation_macro_truth_model.empty() &&
         summary.canonical_interface_payload_mode ==
             kObjc3CompatibilityStrictnessClaimCanonicalInterfacePayloadMode &&
         language_profile_valid && summary.fail_closed && summary.semantic_boundary_ready &&
         summary.suppressed_macro_claim_ids.size() ==
             kObjc3CompatibilityStrictnessClaimSuppressedMacroClaimCount &&
         summary.suppressed_macro_claim_ids[0] ==
             kObjc3SuppressedMacroClaimStrictnessLevel &&
         summary.suppressed_macro_claim_ids[1] ==
             kObjc3SuppressedMacroClaimConcurrencyMode &&
         summary.suppressed_macro_claim_ids[2] ==
             kObjc3SuppressedMacroClaimConcurrencyStrict &&
         summary.valid_language_profile_count ==
             kObjc3CompatibilityStrictnessClaimValidLanguageProfileCount &&
         summary.live_selection_surface_count ==
             kObjc3CompatibilityStrictnessClaimLiveSelectionSurfaceCount &&
         summary.valid_selection_combination_count ==
             kObjc3CompatibilityStrictnessClaimValidSelectionCombinationCount &&
         summary.runnable_feature_claim_count ==
             kObjc3CompatibilityStrictnessClaimRunnableFeatureCount &&
         summary.downgraded_source_only_claim_count ==
             kObjc3CompatibilityStrictnessClaimSourceOnlyFeatureCount &&
         summary.rejected_unsupported_feature_claim_count ==
             kObjc3CompatibilityStrictnessClaimRejectedFeatureCount &&
         summary.live_unsupported_feature_family_count <=
             summary.rejected_unsupported_feature_claim_count &&
         summary.throws_source_rejection_site_count <=
             summary.live_unsupported_feature_site_count &&
         summary.blocks_source_rejection_site_count <=
             summary.live_unsupported_feature_site_count &&
         summary.arc_source_rejection_site_count <=
             summary.live_unsupported_feature_site_count &&
         summary.live_unsupported_feature_diagnostic_count ==
             summary.live_unsupported_feature_site_count &&
         summary.throws_source_rejection_site_count +
                 summary.blocks_source_rejection_site_count +
                 summary.arc_source_rejection_site_count ==
             summary.live_unsupported_feature_site_count &&
         summary.rejected_selection_surface_count ==
             kObjc3CompatibilityStrictnessClaimRejectedSelectionSurfaceCount &&
         summary.suppressed_macro_claim_count ==
             kObjc3CompatibilityStrictnessClaimSuppressedMacroClaimCount &&
         summary.language_profile_semantics_landed &&
         summary.canonical_literal_rejection_semantics_landed &&
         summary.source_only_claim_downgrade_semantics_landed &&
         summary.unsupported_feature_claim_rejection_semantics_landed &&
         summary.live_unsupported_feature_source_rejection_landed &&
         summary.strictness_selection_rejection_semantics_landed &&
         summary.feature_macro_claim_suppression_semantics_landed &&
         summary.canonical_interface_truth_semantics_landed &&
         summary.separate_compilation_macro_truth_semantics_landed &&
         summary.selected_configuration_valid &&
         !summary.selected_configuration_downgraded &&
         !summary.selected_configuration_rejected &&
         summary.ready_for_lowering_and_runtime &&
         !summary.semantic_boundary_replay_key.empty() &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline bool IsReadyObjc3FrontendTypeSystemTypeSourceClosureSummary(
    const Objc3FrontendTypeSystemTypeSourceClosureSummary &summary) {
  return summary.contract_id == kObjc3TypeSystemTypeSourceClosureContractId &&
         summary.frontend_surface_path == kObjc3TypeSystemTypeSourceClosureSurfacePath &&
         summary.source_model == kObjc3TypeSystemTypeSourceClosureSourceModel &&
         summary.failure_model == kObjc3TypeSystemTypeSourceClosureFailureModel &&
         summary.source_only_claim_ids.size() == 7 &&
         summary.source_only_claim_ids[0] ==
             kObjc3SourceOnlyFeatureClaimProtocolOptionalPartitions &&
         summary.source_only_claim_ids[1] ==
             kObjc3SourceOnlyFeatureClaimObjectPointerNullabilitySuffixes &&
         summary.source_only_claim_ids[2] ==
             kObjc3SourceOnlyFeatureClaimPragmaticGenericSuffixes &&
         summary.source_only_claim_ids[3] ==
             kObjc3SourceOnlyFeatureClaimOptionalBindings &&
         summary.source_only_claim_ids[4] ==
             kObjc3SourceOnlyFeatureClaimOptionalSends &&
         summary.source_only_claim_ids[5] ==
             kObjc3SourceOnlyFeatureClaimNilCoalescing &&
         summary.source_only_claim_ids[6] ==
             kObjc3SourceOnlyFeatureClaimTypedKeyPathLiterals &&
         summary.unsupported_claim_ids.empty() &&
         summary.protocol_optional_partition_source_supported &&
         summary.object_pointer_nullability_source_supported &&
         summary.pragmatic_generic_suffix_source_supported &&
         summary.optional_binding_source_supported &&
         summary.optional_send_source_supported &&
         summary.nil_coalescing_source_supported &&
         summary.typed_keypath_literal_source_supported &&
         !summary.optional_member_access_fail_closed &&
         summary.deterministic_handoff &&
         summary.ready_for_semantic_expansion &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline bool IsReadyObjc3FrontendControlFlowControlFlowSourceClosureSummary(
    const Objc3FrontendControlFlowControlFlowSourceClosureSummary &summary) {
  return summary.contract_id == kObjc3ControlFlowControlFlowSourceClosureContractId &&
         summary.frontend_surface_path ==
             kObjc3ControlFlowControlFlowSourceClosureSurfacePath &&
         summary.source_model == kObjc3ControlFlowControlFlowSourceClosureSourceModel &&
         summary.failure_model ==
             kObjc3ControlFlowControlFlowSourceClosureFailureModel &&
         summary.supported_construct_ids.size() == 8 &&
         summary.supported_construct_ids[0] ==
             kObjc3ControlFlowSourceSurfaceGuardBindings &&
         summary.supported_construct_ids[1] ==
             kObjc3ControlFlowSourceSurfaceGuardConditionLists &&
         summary.supported_construct_ids[2] ==
             kObjc3ControlFlowSourceSurfaceSwitchCasePatterns &&
         summary.supported_construct_ids[3] ==
             kObjc3ControlFlowSourceSurfaceDeferStatements &&
         summary.supported_construct_ids[4] ==
             kObjc3ControlFlowSourceSurfaceMatchStatement &&
         summary.supported_construct_ids[5] ==
             kObjc3ControlFlowSourceSurfaceMatchWildcardPatterns &&
         summary.supported_construct_ids[6] ==
             kObjc3ControlFlowSourceSurfaceMatchLiteralPatterns &&
         summary.supported_construct_ids[7] ==
             kObjc3ControlFlowSourceSurfaceMatchBindingPatterns &&
         summary.supported_construct_ids[8] ==
             kObjc3ControlFlowSourceSurfaceMatchResultCasePatterns &&
         summary.fail_closed_construct_ids.size() == 3 &&
         summary.fail_closed_construct_ids[0] ==
             kObjc3ControlFlowFailClosedConstructMatchExpression &&
         summary.fail_closed_construct_ids[1] ==
             kObjc3ControlFlowFailClosedConstructGuardedPatterns &&
         summary.fail_closed_construct_ids[2] ==
             kObjc3ControlFlowFailClosedConstructMatchTypeTestPatterns &&
         summary.guard_binding_source_supported &&
         summary.guard_condition_list_source_supported &&
         summary.switch_case_pattern_source_supported &&
         summary.defer_statement_source_supported &&
         summary.match_statement_source_supported &&
         summary.match_wildcard_pattern_source_supported &&
         summary.match_literal_pattern_source_supported &&
         summary.match_binding_pattern_source_supported &&
         summary.match_result_case_pattern_source_supported &&
         summary.defer_keyword_reserved && !summary.defer_fail_closed &&
         summary.match_expression_fail_closed &&
         summary.guarded_pattern_fail_closed &&
         summary.type_test_pattern_fail_closed &&
         summary.deterministic_handoff &&
         summary.ready_for_semantic_expansion &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}

inline bool IsReadyObjc3FrontendErrorHandlingErrorSourceClosureSummary(
    const Objc3FrontendErrorHandlingErrorSourceClosureSummary &summary) {
  return summary.contract_id == kObjc3ErrorHandlingErrorSourceClosureContractId &&
         summary.frontend_surface_path ==
             kObjc3ErrorHandlingErrorSourceClosureSurfacePath &&
         summary.source_model == kObjc3ErrorHandlingErrorSourceClosureSourceModel &&
         summary.failure_model == kObjc3ErrorHandlingErrorSourceClosureFailureModel &&
         summary.source_only_claim_ids.size() == 3 &&
         summary.source_only_claim_ids[0] ==
             kObjc3SourceOnlyFeatureClaimThrowsDeclarations &&
         summary.source_only_claim_ids[1] ==
             kObjc3SourceOnlyFeatureClaimResultCarrierProfiles &&
         summary.source_only_claim_ids[2] ==
             kObjc3SourceOnlyFeatureClaimNSErrorBridgingProfiles &&
         summary.fail_closed_construct_ids.size() == 3 &&
         summary.fail_closed_construct_ids[0] ==
             kObjc3ErrorHandlingFailClosedConstructTryExpressions &&
         summary.fail_closed_construct_ids[1] ==
             kObjc3ErrorHandlingFailClosedConstructThrowStatements &&
         summary.fail_closed_construct_ids[2] ==
             kObjc3ErrorHandlingFailClosedConstructDoCatchStatements &&
         summary.throws_declaration_source_supported &&
         summary.result_carrier_source_supported &&
         summary.ns_error_bridging_source_supported &&
         summary.error_bridge_marker_source_supported &&
         summary.try_keyword_reserved && summary.throw_keyword_reserved &&
         summary.catch_keyword_reserved && summary.try_fail_closed &&
         summary.throw_fail_closed && summary.do_catch_fail_closed &&
         summary.deterministic_handoff &&
         summary.ready_for_semantic_expansion &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
