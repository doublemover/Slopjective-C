             "toolchain_runtime_ga_operations_advanced_conformance_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsAdvancedPerformanceKey(
    bool toolchain_runtime_ga_operations_advanced_integration_consistent,
    bool toolchain_runtime_ga_operations_advanced_integration_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_integration_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_advanced_performance_consistent,
    bool toolchain_runtime_ga_operations_advanced_performance_ready) {
  const bool advanced_integration_key_shape_deterministic =
      objc3c::support::StartsWith(
          toolchain_runtime_ga_operations_advanced_integration_key,
          "toolchain_runtime_ga_operations_advanced_conformance_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("toolchain_runtime_ga_operations_advanced_integration_consistent=") +
         (toolchain_runtime_ga_operations_advanced_integration_consistent ? "true" : "false") +
         ";toolchain_runtime_ga_operations_advanced_integration_ready=" +
         (toolchain_runtime_ga_operations_advanced_integration_ready ? "true" : "false") +
         ";advanced_integration_key_shape_deterministic=" +
         (advanced_integration_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_integration_closeout_key_shape_deterministic=" +
         (long_tail_grammar_integration_closeout_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_advanced_performance_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_advanced_performance_ready ? "true" : "false");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Consistent(
    bool toolchain_runtime_ga_operations_advanced_performance_consistent,
    bool toolchain_runtime_ga_operations_advanced_performance_ready,
    bool long_tail_grammar_integration_closeout_consistent,
    bool long_tail_grammar_gate_signoff_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_performance_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_advanced_performance_consistent &&
         toolchain_runtime_ga_operations_advanced_performance_ready &&
         long_tail_grammar_integration_closeout_consistent &&
         long_tail_grammar_gate_signoff_ready &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_performance_key,
             "toolchain_runtime_ga_operations_advanced_integration_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         objc3c::support::StartsWith(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Ready(
    bool toolchain_runtime_ga_operations_advanced_core_shard2_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_performance_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_advanced_core_shard2_consistent &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_performance_key,
             "toolchain_runtime_ga_operations_advanced_integration_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsAdvancedCoreShard2Key(
    bool toolchain_runtime_ga_operations_advanced_performance_consistent,
    bool toolchain_runtime_ga_operations_advanced_performance_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_performance_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_advanced_core_shard2_consistent,
    bool toolchain_runtime_ga_operations_advanced_core_shard2_ready) {
  const bool advanced_performance_key_shape_deterministic =
      objc3c::support::StartsWith(
          toolchain_runtime_ga_operations_advanced_performance_key,
          "toolchain_runtime_ga_operations_advanced_integration_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("toolchain_runtime_ga_operations_advanced_performance_consistent=") +
         (toolchain_runtime_ga_operations_advanced_performance_consistent ? "true" : "false") +
         ";toolchain_runtime_ga_operations_advanced_performance_ready=" +
         (toolchain_runtime_ga_operations_advanced_performance_ready ? "true" : "false") +
         ";advanced_performance_key_shape_deterministic=" +
         (advanced_performance_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_integration_closeout_key_shape_deterministic=" +
         (long_tail_grammar_integration_closeout_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_advanced_core_shard2_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_advanced_core_shard2_ready ? "true" : "false");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffConsistent(
    bool toolchain_runtime_ga_operations_advanced_core_shard2_consistent,
    bool toolchain_runtime_ga_operations_advanced_core_shard2_ready,
    bool long_tail_grammar_integration_closeout_consistent,
    bool long_tail_grammar_gate_signoff_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_core_shard2_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key) {
  return toolchain_runtime_ga_operations_advanced_core_shard2_consistent &&
         toolchain_runtime_ga_operations_advanced_core_shard2_ready &&
         long_tail_grammar_integration_closeout_consistent &&
         long_tail_grammar_gate_signoff_ready &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_core_shard2_key,
             "toolchain_runtime_ga_operations_advanced_performance_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=") &&
         objc3c::support::StartsWith(
             parse_lowering_performance_quality_guardrails_key,
             "case_count=");
}

inline bool IsObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffReady(
    bool toolchain_runtime_ga_operations_integration_closeout_signoff_consistent,
    const std::string &toolchain_runtime_ga_operations_advanced_core_shard2_key,
    const std::string &long_tail_grammar_integration_closeout_key) {
  return toolchain_runtime_ga_operations_integration_closeout_signoff_consistent &&
         objc3c::support::StartsWith(
             toolchain_runtime_ga_operations_advanced_core_shard2_key,
             "toolchain_runtime_ga_operations_advanced_performance_consistent=") &&
         objc3c::support::StartsWith(
             long_tail_grammar_integration_closeout_key,
             "conformance_matrix_ready=");
}

inline std::string BuildObjc3ToolchainRuntimeGaOperationsIntegrationCloseoutSignoffKey(
    bool toolchain_runtime_ga_operations_advanced_core_shard2_consistent,
    bool toolchain_runtime_ga_operations_advanced_core_shard2_ready,
    const std::string &toolchain_runtime_ga_operations_advanced_core_shard2_key,
    const std::string &long_tail_grammar_integration_closeout_key,
    const std::string &parse_lowering_performance_quality_guardrails_key,
    bool toolchain_runtime_ga_operations_integration_closeout_signoff_consistent,
    bool toolchain_runtime_ga_operations_integration_closeout_signoff_ready) {
  const bool advanced_core_shard2_key_shape_deterministic =
      objc3c::support::StartsWith(
          toolchain_runtime_ga_operations_advanced_core_shard2_key,
          "toolchain_runtime_ga_operations_advanced_performance_consistent=");
  const bool long_tail_grammar_integration_closeout_key_shape_deterministic =
      objc3c::support::StartsWith(
          long_tail_grammar_integration_closeout_key,
          "conformance_matrix_ready=");
  const bool parse_lowering_performance_quality_guardrails_key_shape_deterministic =
      objc3c::support::StartsWith(
          parse_lowering_performance_quality_guardrails_key,
          "case_count=");
  return std::string("toolchain_runtime_ga_operations_advanced_core_shard2_consistent=") +
         (toolchain_runtime_ga_operations_advanced_core_shard2_consistent ? "true" : "false") +
         ";toolchain_runtime_ga_operations_advanced_core_shard2_ready=" +
         (toolchain_runtime_ga_operations_advanced_core_shard2_ready ? "true" : "false") +
         ";advanced_core_shard2_key_shape_deterministic=" +
         (advanced_core_shard2_key_shape_deterministic ? "true" : "false") +
         ";long_tail_grammar_integration_closeout_key_shape_deterministic=" +
         (long_tail_grammar_integration_closeout_key_shape_deterministic ? "true" : "false") +
         ";parse_lowering_performance_quality_guardrails_key_shape_deterministic=" +
         (parse_lowering_performance_quality_guardrails_key_shape_deterministic ? "true" : "false") +
         ";consistent=" +
         (toolchain_runtime_ga_operations_integration_closeout_signoff_consistent ? "true" : "false") +
         ";ready=" + (toolchain_runtime_ga_operations_integration_closeout_signoff_ready ? "true" : "false");
}

inline std::string BuildObjc3LongTailGrammarIntegrationCloseoutKey(
    bool conformance_matrix_ready,
    bool conformance_corpus_consistent,
    bool performance_guardrails_consistent,
    bool cross_lane_integration_consistent,
    bool cross_lane_integration_ready,
    bool recovery_determinism_ready,
    bool semantic_handoff_ready,
    bool lowering_boundary_ready,
    bool integration_closeout_consistent,
    bool gate_signoff_ready) {
  return std::string("conformance_matrix_ready=") +
         (conformance_matrix_ready ? "true" : "false") +
         ";conformance_corpus_consistent=" +
         (conformance_corpus_consistent ? "true" : "false") +
         ";performance_guardrails_consistent=" +
         (performance_guardrails_consistent ? "true" : "false") +
         ";cross_lane_integration_consistent=" +
         (cross_lane_integration_consistent ? "true" : "false") +
         ";cross_lane_integration_ready=" +
         (cross_lane_integration_ready ? "true" : "false") +
         ";recovery_determinism_ready=" +
         (recovery_determinism_ready ? "true" : "false") +
         ";semantic_handoff_ready=" +
         (semantic_handoff_ready ? "true" : "false") +
         ";lowering_boundary_ready=" +
         (lowering_boundary_ready ? "true" : "false") +
         ";integration_closeout_consistent=" +
         (integration_closeout_consistent ? "true" : "false") +
         ";gate_signoff_ready=" + (gate_signoff_ready ? "true" : "false");
}

inline constexpr std::size_t kObjc3ParseLoweringConformanceMatrixCaseCount = 8u;
inline constexpr std::size_t kObjc3ParseLoweringConformanceCorpusCaseCount = 8u;
inline constexpr std::size_t kObjc3ParseLoweringPerformanceQualityGuardrailsCaseCount = 6u;

inline std::string BuildObjc3ParseLoweringConformanceMatrixKey(
    std::size_t case_count,
    bool parser_contract_snapshot_present,
    bool parse_artifact_handoff_deterministic,
    bool parse_artifact_replay_key_deterministic,
    bool parse_artifact_diagnostics_hardening_consistent,
    bool parse_artifact_edge_case_robustness_consistent,
    bool parse_recovery_determinism_hardening_consistent,
    bool semantic_integration_surface_built,
    bool semantic_handoff_deterministic,
    bool lowering_boundary_ready,
    bool parse_lowering_conformance_matrix_consistent) {
  return "case_count=" + std::to_string(case_count) +
         ";parser_contract_snapshot_present=" + (parser_contract_snapshot_present ? "true" : "false") +
         ";parse_artifact_handoff_deterministic=" + (parse_artifact_handoff_deterministic ? "true" : "false") +
         ";parse_artifact_replay_key_deterministic=" + (parse_artifact_replay_key_deterministic ? "true" : "false") +
         ";parse_artifact_diagnostics_hardening_consistent=" +
         (parse_artifact_diagnostics_hardening_consistent ? "true" : "false") +
         ";parse_artifact_edge_case_robustness_consistent=" +
         (parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
         ";parse_recovery_determinism_hardening_consistent=" +
         (parse_recovery_determinism_hardening_consistent ? "true" : "false") +
         ";semantic_integration_surface_built=" + (semantic_integration_surface_built ? "true" : "false") +
         ";semantic_handoff_deterministic=" + (semantic_handoff_deterministic ? "true" : "false") +
         ";lowering_boundary_ready=" + (lowering_boundary_ready ? "true" : "false") +
         ";consistent=" + (parse_lowering_conformance_matrix_consistent ? "true" : "false");
}

inline std::string BuildObjc3ParseLoweringConformanceCorpusKey(
    std::size_t case_count,
    std::size_t passed_case_count,
    std::size_t failed_case_count,
    bool parser_contract_snapshot_case_passed,
    bool parse_artifact_handoff_case_passed,
    bool parse_artifact_replay_case_passed,
    bool parse_artifact_diagnostics_hardening_case_passed,
    bool parse_artifact_edge_robustness_case_passed,
    bool parse_recovery_determinism_hardening_case_passed,
    bool semantic_handoff_case_passed,
    bool lowering_boundary_case_passed,
    bool parse_lowering_conformance_matrix_consistent,
    bool parse_lowering_conformance_corpus_consistent) {
  return "case_count=" + std::to_string(case_count) +
         ";passed_case_count=" + std::to_string(passed_case_count) +
         ";failed_case_count=" + std::to_string(failed_case_count) +
         ";parser_contract_snapshot_case_passed=" + (parser_contract_snapshot_case_passed ? "true" : "false") +
         ";parse_artifact_handoff_case_passed=" + (parse_artifact_handoff_case_passed ? "true" : "false") +
         ";parse_artifact_replay_case_passed=" + (parse_artifact_replay_case_passed ? "true" : "false") +
         ";parse_artifact_diagnostics_hardening_case_passed=" +
         (parse_artifact_diagnostics_hardening_case_passed ? "true" : "false") +
         ";parse_artifact_edge_robustness_case_passed=" +
         (parse_artifact_edge_robustness_case_passed ? "true" : "false") +
         ";parse_recovery_determinism_hardening_case_passed=" +
         (parse_recovery_determinism_hardening_case_passed ? "true" : "false") +
         ";semantic_handoff_case_passed=" + (semantic_handoff_case_passed ? "true" : "false") +
         ";lowering_boundary_case_passed=" + (lowering_boundary_case_passed ? "true" : "false") +
         ";parse_lowering_conformance_matrix_consistent=" +
         (parse_lowering_conformance_matrix_consistent ? "true" : "false") +
         ";consistent=" + (parse_lowering_conformance_corpus_consistent ? "true" : "false");
}

inline std::string BuildObjc3ParseLoweringPerformanceQualityGuardrailsKey(
    std::size_t case_count,
    std::size_t passed_case_count,
    std::size_t failed_case_count,
    bool parser_token_count_budget_consistent,
    bool parser_diagnostic_code_surface_deterministic,
    bool parse_artifact_diagnostics_hardening_consistent,
    bool parse_artifact_edge_case_robustness_consistent,
    bool parse_recovery_determinism_hardening_consistent,
    bool parse_lowering_conformance_corpus_consistent,
    bool parse_lowering_performance_quality_guardrails_consistent) {
  return "case_count=" + std::to_string(case_count) +
         ";passed_case_count=" + std::to_string(passed_case_count) +
         ";failed_case_count=" + std::to_string(failed_case_count) +
         ";parser_token_count_budget_consistent=" +
         (parser_token_count_budget_consistent ? "true" : "false") +
         ";parser_diagnostic_code_surface_deterministic=" +
         (parser_diagnostic_code_surface_deterministic ? "true" : "false") +
         ";parse_artifact_diagnostics_hardening_consistent=" +
         (parse_artifact_diagnostics_hardening_consistent ? "true" : "false") +
         ";parse_artifact_edge_case_robustness_consistent=" +
         (parse_artifact_edge_case_robustness_consistent ? "true" : "false") +
         ";parse_recovery_determinism_hardening_consistent=" +
         (parse_recovery_determinism_hardening_consistent ? "true" : "false") +
         ";parse_lowering_conformance_corpus_consistent=" +
         (parse_lowering_conformance_corpus_consistent ? "true" : "false") +
         ";consistent=" +
         (parse_lowering_performance_quality_guardrails_consistent ? "true" : "false");
}

inline bool HasObjc3TypedSemaToLoweringCoreFeatureSurface(
    const Objc3TypedSemaToLoweringContractSurface &surface) {
  return surface.typed_core_feature_case_count > 0 ||
         surface.typed_core_feature_expansion_case_count > 0 ||
         surface.typed_core_feature_edge_case_compatibility_ready ||
         !surface.typed_handoff_key.empty() ||
         !surface.typed_core_feature_key.empty() ||
         !surface.typed_core_feature_expansion_key.empty() ||
         !surface.typed_core_feature_edge_case_compatibility_key.empty() ||
         surface.ready_for_lowering ||
         !surface.failure_reason.empty();
}

inline Objc3ParseLoweringReadinessSurface BuildObjc3ParseLoweringReadinessSurface(
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
  Objc3ParseLoweringReadinessSurface surface;
  surface.parse_lowering_conformance_matrix_case_count =
      kObjc3ParseLoweringConformanceMatrixCaseCount;
  surface.parse_lowering_conformance_corpus_case_count =
      kObjc3ParseLoweringConformanceCorpusCaseCount;
  surface.parse_lowering_performance_quality_guardrails_case_count =
      kObjc3ParseLoweringPerformanceQualityGuardrailsCaseCount;
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
  surface.long_tail_grammar_construct_count = parser_snapshot.long_tail_grammar_construct_count;
  surface.long_tail_grammar_covered_construct_count = parser_snapshot.long_tail_grammar_covered_construct_count;
  surface.long_tail_grammar_fingerprint = parser_snapshot.long_tail_grammar_fingerprint;
  surface.long_tail_grammar_handoff_key = parser_snapshot.long_tail_grammar_handoff_key;
  surface.long_tail_grammar_core_feature_consistent =
      parser_snapshot.long_tail_grammar_covered_construct_count <=
          parser_snapshot.long_tail_grammar_construct_count &&
      parser_snapshot.long_tail_grammar_fingerprint != 0 &&
      !parser_snapshot.long_tail_grammar_handoff_key.empty() &&
      parser_snapshot.long_tail_grammar_handoff_deterministic;
  surface.long_tail_grammar_handoff_key_deterministic =
      parser_snapshot.long_tail_grammar_handoff_deterministic &&
      surface.long_tail_grammar_core_feature_consistent;
  surface.long_tail_grammar_expansion_accounting_consistent =
      surface.long_tail_grammar_covered_construct_count <=
          surface.long_tail_grammar_construct_count &&
      surface.long_tail_grammar_fingerprint != 0 &&
      surface.long_tail_grammar_core_feature_consistent;
  const std::size_t parser_snapshot_breakdown_count =
      Objc3ParserSnapshotDeclarationBreakdownCount(parser_snapshot);
  const std::size_t ast_top_level_declaration_count =
      Objc3ParsedProgramTopLevelDeclarationCount(pipeline_result.program);
  const Objc3ParseLoweringDiagnosticCodeCoverage parser_diagnostic_code_coverage =
      BuildObjc3ParseLoweringDiagnosticCodeCoverage(pipeline_result.stage_diagnostics.parser);
  const Objc3ParserDiagnosticSourcePrecisionScaffold parser_diagnostic_source_precision_scaffold =
      BuildObjc3ParserDiagnosticSourcePrecisionScaffold(
          pipeline_result.stage_diagnostics.parser,
          parser_snapshot);
  const Objc3DiagnosticGrammarHooksCoreFeatureSurface parser_diagnostic_grammar_hooks_core_feature =
      BuildObjc3DiagnosticGrammarHooksCoreFeatureSurface(
          pipeline_result.stage_diagnostics.parser,
          parser_snapshot,
          parser_diagnostic_source_precision_scaffold);
  const Objc3DiagnosticGrammarHooksCoreFeatureExpansionSurface
      parser_diagnostic_grammar_hooks_core_feature_expansion =
          BuildObjc3DiagnosticGrammarHooksCoreFeatureExpansionSurface(
              parser_diagnostic_grammar_hooks_core_feature,
              parser_diagnostic_source_precision_scaffold,
              parser_diagnostic_code_coverage.unique_code_count,
              parser_diagnostic_code_coverage.unique_code_fingerprint,
              surface.parser_diagnostic_count,
              parser_snapshot.parser_diagnostic_count);
  const bool parser_snapshot_breakdown_consistent =
      parser_snapshot_breakdown_count == parser_snapshot.top_level_declaration_count;
  surface.parser_diagnostic_surface_consistent =
      parser_snapshot.parser_diagnostic_count == surface.parser_diagnostic_count;
  surface.parser_diagnostic_coordinate_tagged_count =
      parser_diagnostic_source_precision_scaffold.coordinate_tagged_diagnostic_count;
  surface.parser_diagnostic_source_precision_fingerprint =
      parser_diagnostic_source_precision_scaffold.coordinate_fingerprint;
  surface.parser_diagnostic_source_precision_scaffold_key =
      parser_diagnostic_source_precision_scaffold.scaffold_key;
  surface.parser_diagnostic_source_precision_scaffold_consistent =
      parser_diagnostic_source_precision_scaffold.scaffold_consistent;
  surface.parser_diagnostic_source_precision_scaffold_ready =
      IsObjc3ParserDiagnosticSourcePrecisionScaffoldReady(
          parser_diagnostic_source_precision_scaffold);
  surface.parser_diagnostic_grammar_hook_code_count =
      parser_diagnostic_grammar_hooks_core_feature.grammar_hook_code_count;
  surface.parser_diagnostic_grammar_hooks_core_feature_consistent =
      parser_diagnostic_grammar_hooks_core_feature.core_feature_consistent;
  surface.parser_diagnostic_grammar_hooks_core_feature_key =
      parser_diagnostic_grammar_hooks_core_feature.core_feature_key;
  surface.parser_diagnostic_grammar_hooks_core_feature_ready =
      IsObjc3DiagnosticGrammarHooksCoreFeatureReady(
          parser_diagnostic_grammar_hooks_core_feature);
  surface.parser_diagnostic_grammar_hook_unique_code_count =
      parser_diagnostic_grammar_hooks_core_feature_expansion.unique_diagnostic_code_count;
  surface.parser_diagnostic_grammar_hooks_core_feature_expansion_accounting_consistent =
      parser_diagnostic_grammar_hooks_core_feature_expansion.accounting_consistent;
  surface.parser_diagnostic_grammar_hooks_core_feature_expansion_replay_keys_ready =
      parser_diagnostic_grammar_hooks_core_feature_expansion.replay_keys_ready;
  surface.parser_diagnostic_grammar_hooks_core_feature_expansion_key =
      parser_diagnostic_grammar_hooks_core_feature_expansion.expansion_key;
  surface.parser_diagnostic_grammar_hooks_core_feature_expansion_ready =
      IsObjc3DiagnosticGrammarHooksCoreFeatureExpansionReady(
          parser_diagnostic_grammar_hooks_core_feature_expansion);
  surface.parser_diagnostic_code_count = parser_diagnostic_code_coverage.unique_code_count;
  surface.parser_diagnostic_code_fingerprint = parser_diagnostic_code_coverage.unique_code_fingerprint;
  surface.parser_diagnostic_code_surface_deterministic =
      parser_diagnostic_code_coverage.deterministic_surface &&
      surface.parser_diagnostic_code_count <= surface.parser_diagnostic_count;
  surface.parse_artifact_diagnostics_hardening_consistent =
      surface.parser_diagnostic_surface_consistent &&
      surface.parser_diagnostic_code_surface_deterministic &&
      surface.parser_diagnostic_source_precision_scaffold_consistent &&
      surface.parser_diagnostic_source_precision_scaffold_ready &&
      surface.parser_diagnostic_grammar_hooks_core_feature_consistent &&
      surface.parser_diagnostic_grammar_hooks_core_feature_ready &&
      surface.parser_diagnostic_grammar_hooks_core_feature_expansion_accounting_consistent &&
      surface.parser_diagnostic_grammar_hooks_core_feature_expansion_replay_keys_ready &&
      surface.parser_diagnostic_grammar_hooks_core_feature_expansion_ready &&
      !surface.parser_diagnostic_source_precision_scaffold_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_core_feature_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_core_feature_expansion_key.empty();
  surface.parser_token_count_budget_consistent =
      surface.parser_token_count >= parser_snapshot_breakdown_count &&
      surface.parser_token_count >= parser_snapshot.top_level_declaration_count &&
      surface.parser_token_count >= ast_top_level_declaration_count;
  surface.parse_artifact_handoff_consistent =
      parser_snapshot_breakdown_consistent &&
      ast_top_level_declaration_count == parser_snapshot.top_level_declaration_count;
  surface.parse_artifact_handoff_deterministic =
      surface.parse_artifact_handoff_consistent &&
      surface.parser_diagnostic_surface_consistent &&
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
      (options.legacy_literal_diagnostics || legacy_literal_total == 0);
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
  const Objc3DiagnosticGrammarHooksEdgeCaseCompatibilitySurface
      parser_diagnostic_grammar_hooks_edge_case_compatibility =
          BuildObjc3DiagnosticGrammarHooksEdgeCaseCompatibilitySurface(
              parser_diagnostic_grammar_hooks_core_feature_expansion,
              options,
              pipeline_result.language_version_pragma_contract,
              surface.parser_diagnostic_count,
              parser_snapshot.parser_diagnostic_count,
              surface.parser_token_count,
              surface.compatibility_handoff_consistent);
  surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_consistent =
      parser_diagnostic_grammar_hooks_edge_case_compatibility
          .edge_case_compatibility_consistent;
  surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_key =
      parser_diagnostic_grammar_hooks_edge_case_compatibility.compatibility_key;
  surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_ready =
      IsObjc3DiagnosticGrammarHooksEdgeCaseCompatibilitySurfaceReady(
          parser_diagnostic_grammar_hooks_edge_case_compatibility);
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
  surface.long_tail_grammar_replay_keys_ready =
      !surface.long_tail_grammar_handoff_key.empty() &&
      !surface.parse_artifact_handoff_key.empty() &&
      !surface.parse_artifact_replay_key.empty();
  surface.long_tail_grammar_expansion_ready =
      surface.long_tail_grammar_expansion_accounting_consistent &&
      surface.long_tail_grammar_handoff_key_deterministic &&
      surface.long_tail_grammar_replay_keys_ready;
  surface.long_tail_grammar_expansion_key = BuildObjc3LongTailGrammarExpansionKey(
      surface.long_tail_grammar_construct_count,
      surface.long_tail_grammar_covered_construct_count,
      surface.long_tail_grammar_fingerprint,
      surface.long_tail_grammar_core_feature_consistent,
      surface.long_tail_grammar_handoff_key_deterministic,
      surface.long_tail_grammar_expansion_accounting_consistent,
      surface.long_tail_grammar_replay_keys_ready,
      surface.long_tail_grammar_expansion_ready);
  surface.long_tail_grammar_compatibility_handoff_ready =
      surface.long_tail_grammar_expansion_ready &&
      (surface.compatibility_handoff_consistent) &&
      !surface.compatibility_handoff_key.empty();
  surface.parse_artifact_diagnostics_hardening_key =
      BuildObjc3ParseArtifactDiagnosticsHardeningKey(
          surface.parser_diagnostic_count,
          parser_snapshot.parser_diagnostic_count,
          surface.parser_diagnostic_code_count,
          surface.parser_diagnostic_code_fingerprint,
          surface.parser_diagnostic_surface_consistent,
          surface.parser_diagnostic_code_surface_deterministic,
          surface.parser_diagnostic_source_precision_scaffold_consistent,
          surface.parser_diagnostic_grammar_hooks_core_feature_consistent,
          surface.parser_diagnostic_grammar_hooks_core_feature_expansion_accounting_consistent,
          surface.parser_diagnostic_grammar_hooks_core_feature_expansion_replay_keys_ready,
          surface.parser_diagnostic_grammar_hooks_core_feature_expansion_ready,
          surface.parser_diagnostic_source_precision_scaffold_key,
          surface.parser_diagnostic_grammar_hooks_core_feature_key,
          surface.parser_diagnostic_grammar_hooks_core_feature_expansion_key,
          surface.parse_artifact_diagnostics_hardening_consistent);
  surface.parse_artifact_edge_case_robustness_consistent =
      surface.parser_token_count_budget_consistent &&
      surface.language_version_pragma_coordinate_order_consistent &&
      surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_consistent &&
      surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_ready &&
      surface.parse_artifact_diagnostics_hardening_consistent &&
      !surface.parse_artifact_handoff_key.empty() &&
      !surface.compatibility_handoff_key.empty() &&
      !surface.parse_artifact_replay_key.empty() &&
      !surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_key.empty();
  surface.parse_artifact_edge_robustness_key = BuildObjc3ParseArtifactEdgeRobustnessKey(
      surface.parser_token_count,
      parser_snapshot_breakdown_count,
      ast_top_level_declaration_count,
      surface.parser_token_count_budget_consistent,
      surface.language_version_pragma_coordinate_order_consistent,
      surface.parse_artifact_edge_case_robustness_consistent);
  surface.parser_diagnostic_grammar_hooks_edge_case_expansion_consistent =
      surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_consistent &&
      surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_ready &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      surface.parser_diagnostic_code_surface_deterministic &&
      surface.parser_diagnostic_source_precision_scaffold_ready &&
      !surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_key.empty() &&
      !surface.parse_artifact_diagnostics_hardening_key.empty();
  surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready =
      surface.parser_diagnostic_grammar_hooks_edge_case_expansion_consistent &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.language_version_pragma_coordinate_order_consistent;
  surface.parser_diagnostic_grammar_hooks_edge_case_robustness_key =
      BuildObjc3DiagnosticGrammarHooksEdgeCaseRobustnessKey(
          surface.parser_diagnostic_count,
          surface.parser_diagnostic_code_count,
          surface.parser_diagnostic_code_fingerprint,
          surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_consistent,
          surface.parser_diagnostic_grammar_hooks_edge_case_compatibility_ready,
          surface.parse_artifact_edge_case_robustness_consistent,
          surface.parser_diagnostic_grammar_hooks_edge_case_expansion_consistent,
          surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready);
  surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent =
      surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready &&
      surface.parse_artifact_diagnostics_hardening_consistent &&
      surface.parser_diagnostic_surface_consistent &&
      surface.parser_diagnostic_code_surface_deterministic &&
      !surface.parser_diagnostic_grammar_hooks_edge_case_robustness_key.empty() &&
      !surface.parse_artifact_diagnostics_hardening_key.empty();
  surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready =
      surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent &&
      surface.parse_artifact_replay_key_deterministic;
  surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_key =
      BuildObjc3DiagnosticGrammarHooksDiagnosticsHardeningKey(
          surface.parser_diagnostic_count,
          surface.parser_diagnostic_code_count,
          surface.parser_diagnostic_code_fingerprint,
          surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready,
          surface.parse_artifact_diagnostics_hardening_consistent,
          surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent,
          surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready);
  surface.long_tail_grammar_edge_case_compatibility_consistent =
      surface.long_tail_grammar_expansion_ready &&
      surface.long_tail_grammar_compatibility_handoff_ready &&
      surface.language_version_pragma_coordinate_order_consistent &&
      surface.parse_artifact_edge_case_robustness_consistent;
  surface.long_tail_grammar_edge_case_compatibility_ready =
      surface.long_tail_grammar_edge_case_compatibility_consistent &&
      (surface.parse_artifact_replay_key_deterministic);
  surface.long_tail_grammar_edge_case_compatibility_key =
      BuildObjc3LongTailGrammarEdgeCaseCompatibilityKey(
          surface.compatibility_handoff_consistent,
          surface.long_tail_grammar_compatibility_handoff_ready,
          surface.language_version_pragma_coordinate_order_consistent,
          surface.parse_artifact_edge_case_robustness_consistent,
          surface.long_tail_grammar_edge_case_compatibility_consistent,
          surface.long_tail_grammar_edge_case_compatibility_ready);
  surface.long_tail_grammar_edge_case_expansion_consistent =
      surface.long_tail_grammar_expansion_ready &&
      surface.long_tail_grammar_edge_case_compatibility_ready &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      surface.long_tail_grammar_covered_construct_count <=
          surface.long_tail_grammar_construct_count &&
      surface.long_tail_grammar_fingerprint != 0 &&
      !surface.long_tail_grammar_expansion_key.empty() &&
      !surface.long_tail_grammar_edge_case_compatibility_key.empty();
  surface.long_tail_grammar_edge_case_robustness_ready =
      surface.long_tail_grammar_edge_case_expansion_consistent &&
      (surface.parse_artifact_replay_key_deterministic) &&
      surface.language_version_pragma_coordinate_order_consistent;
  surface.long_tail_grammar_edge_case_robustness_key =
      BuildObjc3LongTailGrammarEdgeCaseRobustnessKey(
          surface.long_tail_grammar_construct_count,
          surface.long_tail_grammar_covered_construct_count,
          surface.long_tail_grammar_fingerprint,
          surface.long_tail_grammar_edge_case_compatibility_ready,
          surface.long_tail_grammar_edge_case_expansion_consistent,
          surface.parse_artifact_edge_case_robustness_consistent,
          surface.long_tail_grammar_edge_case_robustness_ready);
  surface.long_tail_grammar_diagnostics_hardening_consistent =
      surface.long_tail_grammar_edge_case_robustness_ready &&
      surface.parse_artifact_diagnostics_hardening_consistent &&
      surface.parser_diagnostic_surface_consistent &&
      surface.parser_diagnostic_code_surface_deterministic &&
      surface.parser_diagnostic_code_fingerprint != 0 &&
      !surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.long_tail_grammar_edge_case_robustness_key.empty();
  surface.long_tail_grammar_diagnostics_hardening_ready =
      surface.long_tail_grammar_diagnostics_hardening_consistent &&
      (surface.parse_artifact_replay_key_deterministic);
  surface.long_tail_grammar_diagnostics_hardening_key =
      BuildObjc3LongTailGrammarDiagnosticsHardeningKey(
          surface.parser_diagnostic_count,
          surface.parser_diagnostic_code_count,
          surface.parser_diagnostic_code_fingerprint,
          surface.parser_diagnostic_surface_consistent,
          surface.parser_diagnostic_code_surface_deterministic,
          surface.parse_artifact_diagnostics_hardening_consistent,
          surface.long_tail_grammar_edge_case_robustness_ready,
          surface.long_tail_grammar_diagnostics_hardening_consistent,
          surface.long_tail_grammar_diagnostics_hardening_ready);
  surface.parse_recovery_determinism_hardening_consistent =
      surface.parser_contract_snapshot_present &&
      surface.parser_contract_deterministic &&
      surface.parser_recovery_replay_ready &&
      surface.long_tail_grammar_core_feature_consistent &&
      surface.long_tail_grammar_handoff_key_deterministic &&
      surface.long_tail_grammar_expansion_accounting_consistent &&
      surface.long_tail_grammar_replay_keys_ready &&
      surface.long_tail_grammar_expansion_ready &&
      surface.long_tail_grammar_compatibility_handoff_ready &&
      surface.long_tail_grammar_edge_case_compatibility_consistent &&
      surface.long_tail_grammar_edge_case_compatibility_ready &&
      surface.long_tail_grammar_edge_case_expansion_consistent &&
      surface.long_tail_grammar_edge_case_robustness_ready &&
      surface.long_tail_grammar_diagnostics_hardening_ready &&
      surface.parse_artifact_handoff_deterministic &&
      surface.parse_artifact_replay_key_deterministic &&
      surface.parse_artifact_diagnostics_hardening_consistent &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      surface.parser_diagnostic_grammar_hooks_edge_case_expansion_consistent &&
      surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready &&
      surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_consistent &&
      surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready &&
      !surface.long_tail_grammar_handoff_key.empty() &&
      !surface.long_tail_grammar_expansion_key.empty() &&
      !surface.long_tail_grammar_edge_case_compatibility_key.empty() &&
      !surface.long_tail_grammar_edge_case_robustness_key.empty() &&
      !surface.long_tail_grammar_diagnostics_hardening_key.empty() &&
      !surface.parse_artifact_handoff_key.empty() &&
      !surface.parse_artifact_replay_key.empty() &&
      !surface.parse_artifact_diagnostics_hardening_key.empty() &&
      !surface.parse_artifact_edge_robustness_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_edge_case_robustness_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_key.empty();
  surface.parse_recovery_determinism_hardening_key = BuildObjc3ParseRecoveryDeterminismHardeningKey(
      surface.parser_contract_snapshot_present,
      surface.parser_contract_deterministic,
      surface.parser_recovery_replay_ready,
      surface.long_tail_grammar_core_feature_consistent,
      surface.long_tail_grammar_handoff_key_deterministic,
      surface.long_tail_grammar_expansion_accounting_consistent,
      surface.long_tail_grammar_replay_keys_ready,
      surface.long_tail_grammar_expansion_ready,
      surface.long_tail_grammar_compatibility_handoff_ready,
      surface.long_tail_grammar_edge_case_compatibility_consistent,
      surface.long_tail_grammar_edge_case_compatibility_ready,
      surface.long_tail_grammar_edge_case_expansion_consistent,
      surface.long_tail_grammar_edge_case_robustness_ready,
      surface.long_tail_grammar_diagnostics_hardening_ready,
      surface.parse_artifact_handoff_deterministic,
      surface.parse_artifact_replay_key_deterministic,
      surface.parse_artifact_diagnostics_hardening_consistent,
      surface.parse_artifact_edge_case_robustness_consistent,
      surface.parse_recovery_determinism_hardening_consistent);
  surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent =
      surface.parse_recovery_determinism_hardening_consistent &&
      surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready &&
      surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready &&
      surface.parser_recovery_replay_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_edge_case_robustness_key.empty();
  surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready =
      surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent &&
      surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready;
  surface.parser_diagnostic_grammar_hooks_recovery_determinism_key =
      BuildObjc3DiagnosticGrammarHooksRecoveryDeterminismKey(
          surface.parser_recovery_replay_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.parser_diagnostic_grammar_hooks_diagnostics_hardening_ready,
          surface.parser_diagnostic_grammar_hooks_edge_case_robustness_ready,
          surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent,
          surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready);
  surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent =
      surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent &&
      surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready &&
      surface.parse_lowering_conformance_matrix_case_count ==
          kObjc3ParseLoweringConformanceMatrixCaseCount &&
      surface.parse_lowering_conformance_matrix_case_count > 0 &&
      surface.parse_artifact_replay_key_deterministic &&
      !surface.parser_diagnostic_grammar_hooks_recovery_determinism_key.empty() &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.parse_artifact_replay_key.empty();
  surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready =
      surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent &&
      surface.parse_lowering_conformance_corpus_case_count > 0 &&
      surface.parse_lowering_performance_quality_guardrails_case_count > 0;
  surface.parser_diagnostic_grammar_hooks_conformance_matrix_key =
      BuildObjc3DiagnosticGrammarHooksConformanceMatrixKey(
          surface.parse_lowering_conformance_matrix_case_count,
          surface.parse_lowering_conformance_corpus_case_count,
          surface.parse_lowering_performance_quality_guardrails_case_count,
          surface.parse_artifact_replay_key_deterministic,
          surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready,
          surface.parser_diagnostic_grammar_hooks_conformance_matrix_consistent,
          surface.parser_diagnostic_grammar_hooks_conformance_matrix_ready);
  surface.long_tail_grammar_recovery_determinism_consistent =
      surface.parse_recovery_determinism_hardening_consistent &&
      surface.parser_diagnostic_grammar_hooks_recovery_determinism_consistent &&
      surface.parser_diagnostic_grammar_hooks_recovery_determinism_ready &&
      surface.long_tail_grammar_diagnostics_hardening_ready &&
      surface.parser_recovery_replay_ready &&
      surface.parse_artifact_replay_key_deterministic &&
      !surface.parse_recovery_determinism_hardening_key.empty() &&
      !surface.parser_diagnostic_grammar_hooks_recovery_determinism_key.empty() &&
      !surface.long_tail_grammar_diagnostics_hardening_key.empty();
  surface.long_tail_grammar_recovery_determinism_ready =
      surface.long_tail_grammar_recovery_determinism_consistent &&
      surface.long_tail_grammar_expansion_ready &&
      surface.long_tail_grammar_edge_case_robustness_ready;
  surface.long_tail_grammar_recovery_determinism_key =
      BuildObjc3LongTailGrammarRecoveryDeterminismKey(
          surface.parser_recovery_replay_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.long_tail_grammar_diagnostics_hardening_ready,
          surface.long_tail_grammar_edge_case_robustness_ready,
          surface.long_tail_grammar_expansion_ready,
          surface.long_tail_grammar_recovery_determinism_consistent,
          surface.long_tail_grammar_recovery_determinism_ready);
  const bool toolchain_runtime_ga_operations_recovery_determinism_consistent =
      IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningConsistent(
          surface.parser_recovery_replay_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.long_tail_grammar_replay_keys_ready,
          surface.long_tail_grammar_diagnostics_hardening_ready,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.parse_artifact_handoff_key,
          surface.parse_artifact_replay_key,
          surface.parse_artifact_diagnostics_hardening_key,
          surface.parse_artifact_edge_robustness_key,
          surface.long_tail_grammar_handoff_key,
          surface.long_tail_grammar_diagnostics_hardening_key,
          surface.parse_recovery_determinism_hardening_key);
  const bool toolchain_runtime_ga_operations_recovery_determinism_ready =
      IsObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningReady(
          toolchain_runtime_ga_operations_recovery_determinism_consistent,
          surface.long_tail_grammar_recovery_determinism_consistent,
          surface.long_tail_grammar_recovery_determinism_ready,
          surface.long_tail_grammar_recovery_determinism_key);
  surface.parse_recovery_determinism_hardening_consistent &=
      toolchain_runtime_ga_operations_recovery_determinism_consistent;
  surface.long_tail_grammar_recovery_determinism_consistent =
      surface.long_tail_grammar_recovery_determinism_consistent &&
      toolchain_runtime_ga_operations_recovery_determinism_consistent;
  surface.long_tail_grammar_recovery_determinism_ready =
      surface.long_tail_grammar_recovery_determinism_ready &&
      toolchain_runtime_ga_operations_recovery_determinism_ready;
  surface.parse_recovery_determinism_hardening_key =
      BuildObjc3ParseRecoveryDeterminismHardeningKey(
          surface.parser_contract_snapshot_present,
          surface.parser_contract_deterministic,
          surface.parser_recovery_replay_ready,
          surface.long_tail_grammar_core_feature_consistent,
          surface.long_tail_grammar_handoff_key_deterministic,
          surface.long_tail_grammar_expansion_accounting_consistent,
          surface.long_tail_grammar_replay_keys_ready,
          surface.long_tail_grammar_expansion_ready,
          surface.long_tail_grammar_compatibility_handoff_ready,
          surface.long_tail_grammar_edge_case_compatibility_consistent,
          surface.long_tail_grammar_edge_case_compatibility_ready,
          surface.long_tail_grammar_edge_case_expansion_consistent,
          surface.long_tail_grammar_edge_case_robustness_ready,
          surface.long_tail_grammar_diagnostics_hardening_ready,
          surface.parse_artifact_handoff_deterministic,
          surface.parse_artifact_replay_key_deterministic,
          surface.parse_artifact_diagnostics_hardening_consistent,
          surface.parse_artifact_edge_case_robustness_consistent,
          surface.parse_recovery_determinism_hardening_consistent);
  surface.long_tail_grammar_recovery_determinism_key =
      BuildObjc3LongTailGrammarRecoveryDeterminismKey(
          surface.parser_recovery_replay_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.parse_recovery_determinism_hardening_consistent,
          surface.long_tail_grammar_diagnostics_hardening_ready,
          surface.long_tail_grammar_edge_case_robustness_ready,
          surface.long_tail_grammar_expansion_ready,
          surface.long_tail_grammar_recovery_determinism_consistent,
          surface.long_tail_grammar_recovery_determinism_ready);
  const std::string toolchain_runtime_ga_operations_recovery_determinism_key =
      BuildObjc3ToolchainRuntimeGaOperationsRecoveryDeterminismHardeningKey(
          surface.parser_recovery_replay_ready,
          surface.parse_artifact_replay_key_deterministic,
          surface.long_tail_grammar_replay_keys_ready,
          surface.long_tail_grammar_diagnostics_hardening_ready,
          surface.parse_artifact_handoff_key,
          surface.parse_artifact_replay_key,
          surface.parse_artifact_diagnostics_hardening_key,
          surface.parse_artifact_edge_robustness_key,
          surface.long_tail_grammar_handoff_key,
          surface.long_tail_grammar_diagnostics_hardening_key,
          surface.parse_recovery_determinism_hardening_key,
          surface.long_tail_grammar_recovery_determinism_key,
          toolchain_runtime_ga_operations_recovery_determinism_consistent,
          toolchain_runtime_ga_operations_recovery_determinism_ready);
  surface.parse_recovery_determinism_hardening_key +=
      ";toolchain_runtime_ga_operations_recovery_determinism_key=" +
      toolchain_runtime_ga_operations_recovery_determinism_key;
  surface.long_tail_grammar_recovery_determinism_key +=
      ";toolchain_runtime_ga_operations_recovery_determinism_key=" +
      toolchain_runtime_ga_operations_recovery_determinism_key;
  const Objc3TypedSemaToLoweringContractSurface typed_sema_to_lowering_contract_surface =
      HasObjc3TypedSemaToLoweringCoreFeatureSurface(
          pipeline_result.typed_sema_to_lowering_contract_surface)
          ? pipeline_result.typed_sema_to_lowering_contract_surface
          : BuildObjc3TypedSemaToLoweringContractSurface(pipeline_result, options);
  surface.semantic_integration_surface_built =
      typed_sema_to_lowering_contract_surface.semantic_integration_surface_built;
  surface.semantic_diagnostics_deterministic =
      pipeline_result.sema_parity_surface.deterministic_semantic_diagnostics;
  surface.semantic_type_metadata_deterministic =
      typed_sema_to_lowering_contract_surface.semantic_type_metadata_handoff_deterministic;
  surface.executable_metadata_lowering_handoff_ready =
      typed_sema_to_lowering_contract_surface
          .executable_metadata_lowering_handoff_ready;
  surface.executable_metadata_lowering_handoff_deterministic =
      typed_sema_to_lowering_contract_surface
          .executable_metadata_lowering_handoff_deterministic;
  surface.executable_metadata_typed_lowering_handoff_ready =
      typed_sema_to_lowering_contract_surface
          .executable_metadata_typed_lowering_handoff_ready;
  surface.executable_metadata_typed_lowering_handoff_deterministic =
      typed_sema_to_lowering_contract_surface
          .executable_metadata_typed_lowering_handoff_deterministic;
  surface.protocol_category_deterministic =
      typed_sema_to_lowering_contract_surface.protocol_category_handoff_deterministic;
  surface.class_protocol_category_linking_deterministic =
      typed_sema_to_lowering_contract_surface.class_protocol_category_linking_handoff_deterministic;
  surface.selector_normalization_deterministic =
      typed_sema_to_lowering_contract_surface.selector_normalization_handoff_deterministic;
  surface.property_attribute_deterministic =
      typed_sema_to_lowering_contract_surface.property_attribute_handoff_deterministic;
  surface.symbol_graph_deterministic =
      typed_sema_to_lowering_contract_surface.symbol_graph_handoff_deterministic;
  surface.scope_resolution_deterministic =
      typed_sema_to_lowering_contract_surface.scope_resolution_handoff_deterministic;
  surface.object_pointer_type_handoff_deterministic =
      typed_sema_to_lowering_contract_surface.object_pointer_type_handoff_deterministic;
  surface.typed_handoff_key_deterministic =
      typed_sema_to_lowering_contract_surface.typed_handoff_key_deterministic;
  surface.typed_sema_core_feature_consistent =
      typed_sema_to_lowering_contract_surface.typed_core_feature_consistent;
  surface.typed_sema_core_feature_expansion_consistent =
      typed_sema_to_lowering_contract_surface.typed_core_feature_expansion_consistent;
  surface.typed_sema_core_feature_case_count =
      typed_sema_to_lowering_contract_surface.typed_core_feature_case_count;
  surface.typed_sema_core_feature_passed_case_count =
      typed_sema_to_lowering_contract_surface.typed_core_feature_passed_case_count;
  surface.typed_sema_core_feature_failed_case_count =
      typed_sema_to_lowering_contract_surface.typed_core_feature_failed_case_count;
  surface.typed_sema_core_feature_expansion_case_count =
      typed_sema_to_lowering_contract_surface.typed_core_feature_expansion_case_count;
  surface.typed_sema_core_feature_expansion_passed_case_count =
      typed_sema_to_lowering_contract_surface.typed_core_feature_expansion_passed_case_count;
  surface.typed_sema_core_feature_expansion_failed_case_count =
      typed_sema_to_lowering_contract_surface.typed_core_feature_expansion_failed_case_count;
  surface.typed_sema_core_feature_key =
      typed_sema_to_lowering_contract_surface.typed_core_feature_key;
  surface.executable_metadata_lowering_handoff_key =
      typed_sema_to_lowering_contract_surface
          .executable_metadata_lowering_handoff_key;
  surface.executable_metadata_typed_lowering_handoff_key =
      typed_sema_to_lowering_contract_surface
          .executable_metadata_typed_lowering_handoff_key;
  surface.typed_sema_core_feature_expansion_key =
      typed_sema_to_lowering_contract_surface.typed_core_feature_expansion_key;
  surface.typed_sema_edge_case_compatibility_consistent =
      typed_sema_to_lowering_contract_surface.typed_core_feature_edge_case_compatibility_ready;
  surface.typed_sema_edge_case_compatibility_key =
      typed_sema_to_lowering_contract_surface.typed_core_feature_edge_case_compatibility_key;
  surface.typed_sema_edge_case_compatibility_ready =
      surface.typed_sema_edge_case_compatibility_consistent &&
      !surface.typed_sema_edge_case_compatibility_key.empty();
  surface.typed_sema_edge_case_expansion_consistent =
      typed_sema_to_lowering_contract_surface.typed_core_feature_edge_case_expansion_consistent;
  surface.typed_sema_edge_case_robustness_key =
      typed_sema_to_lowering_contract_surface.typed_core_feature_edge_case_robustness_key;
  surface.typed_sema_edge_case_robustness_ready =
      surface.typed_sema_edge_case_compatibility_ready &&
      surface.typed_sema_edge_case_expansion_consistent &&
      !surface.typed_sema_edge_case_robustness_key.empty();
  surface.typed_sema_diagnostics_hardening_consistent =
      typed_sema_to_lowering_contract_surface.typed_diagnostics_hardening_consistent;
  surface.typed_sema_diagnostics_hardening_key =
      typed_sema_to_lowering_contract_surface.typed_diagnostics_hardening_key;
  surface.typed_sema_diagnostics_hardening_ready =
      surface.typed_sema_diagnostics_hardening_consistent &&
      surface.typed_sema_edge_case_robustness_ready &&
      !surface.typed_sema_diagnostics_hardening_key.empty();
  surface.typed_sema_recovery_determinism_consistent =
      typed_sema_to_lowering_contract_surface.typed_recovery_determinism_consistent;
  surface.typed_sema_recovery_determinism_key =
      typed_sema_to_lowering_contract_surface.typed_recovery_determinism_key;
  surface.typed_sema_recovery_determinism_ready =
      surface.typed_sema_recovery_determinism_consistent &&
      surface.typed_sema_diagnostics_hardening_ready &&
      !surface.typed_sema_recovery_determinism_key.empty();
  surface.typed_sema_conformance_matrix_consistent =
      typed_sema_to_lowering_contract_surface.typed_conformance_matrix_consistent;
  surface.typed_sema_conformance_matrix_key =
      typed_sema_to_lowering_contract_surface.typed_conformance_matrix_key;
  surface.typed_sema_conformance_matrix_ready =
      surface.typed_sema_conformance_matrix_consistent &&
      surface.typed_sema_recovery_determinism_ready &&
      !surface.typed_sema_conformance_matrix_key.empty();
  surface.typed_sema_conformance_corpus_consistent =
      typed_sema_to_lowering_contract_surface.typed_conformance_corpus_consistent;
  surface.typed_sema_conformance_corpus_key =
      typed_sema_to_lowering_contract_surface.typed_conformance_corpus_key;
  surface.typed_sema_conformance_corpus_ready =
      surface.typed_sema_conformance_corpus_consistent &&
      surface.typed_sema_conformance_matrix_ready &&
      !surface.typed_sema_conformance_corpus_key.empty();
  surface.typed_sema_performance_quality_guardrails_consistent =
      typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_consistent;
  surface.typed_sema_performance_quality_guardrails_ready =
      typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_ready;
  surface.typed_sema_performance_quality_guardrails_case_count =
      typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_case_count;
  surface.typed_sema_performance_quality_guardrails_passed_case_count =
      typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_passed_case_count;
  surface.typed_sema_performance_quality_guardrails_failed_case_count =
      typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_failed_case_count;
  surface.typed_sema_performance_quality_guardrails_key =
      typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_key;
  surface.typed_sema_cross_lane_integration_consistent =
      typed_sema_to_lowering_contract_surface.typed_cross_lane_integration_consistent;
  surface.typed_sema_cross_lane_integration_ready =
      typed_sema_to_lowering_contract_surface.typed_cross_lane_integration_ready;
  surface.typed_sema_cross_lane_integration_key =
      typed_sema_to_lowering_contract_surface.typed_cross_lane_integration_key;
  surface.typed_sema_docs_runbook_sync_consistent =
      typed_sema_to_lowering_contract_surface.typed_docs_runbook_sync_consistent;
  surface.typed_sema_docs_runbook_sync_ready =
      typed_sema_to_lowering_contract_surface.typed_docs_runbook_sync_ready;
  surface.typed_sema_docs_runbook_sync_key =
      typed_sema_to_lowering_contract_surface.typed_docs_runbook_sync_key;
  surface.typed_sema_release_candidate_replay_dry_run_consistent =
      typed_sema_to_lowering_contract_surface.typed_release_candidate_replay_dry_run_consistent;
  surface.typed_sema_release_candidate_replay_dry_run_ready =
      typed_sema_to_lowering_contract_surface.typed_release_candidate_replay_dry_run_ready;
  surface.typed_sema_release_candidate_replay_dry_run_key =
      typed_sema_to_lowering_contract_surface.typed_release_candidate_replay_dry_run_key;
  surface.typed_sema_advanced_core_shard1_consistent =
      typed_sema_to_lowering_contract_surface.typed_advanced_core_shard1_consistent;
  surface.typed_sema_advanced_core_shard1_ready =
      typed_sema_to_lowering_contract_surface.typed_advanced_core_shard1_ready;
  surface.typed_sema_advanced_core_shard1_key =
      typed_sema_to_lowering_contract_surface.typed_advanced_core_shard1_key;
  surface.typed_sema_advanced_edge_compatibility_shard1_consistent =
      typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard1_consistent;
  surface.typed_sema_advanced_edge_compatibility_shard1_ready =
      typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard1_ready;
  surface.typed_sema_advanced_edge_compatibility_shard1_key =
      typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard1_key;
  surface.typed_sema_advanced_diagnostics_shard1_consistent =
      typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard1_consistent;
  surface.typed_sema_advanced_diagnostics_shard1_ready =
      typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard1_ready;
  surface.typed_sema_advanced_diagnostics_shard1_key =
      typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard1_key;
  surface.typed_sema_advanced_conformance_shard1_consistent =
      typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard1_consistent;
  surface.typed_sema_advanced_conformance_shard1_ready =
      typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard1_ready;
  surface.typed_sema_advanced_conformance_shard1_key =
      typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard1_key;
  surface.typed_sema_advanced_integration_shard1_consistent =
      typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard1_consistent;
  surface.typed_sema_advanced_integration_shard1_ready =
      typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard1_ready;
  surface.typed_sema_advanced_integration_shard1_key =
      typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard1_key;
  surface.typed_sema_advanced_performance_shard1_consistent =
      typed_sema_to_lowering_contract_surface.typed_advanced_performance_shard1_consistent;
  surface.typed_sema_advanced_performance_shard1_ready =
      typed_sema_to_lowering_contract_surface.typed_advanced_performance_shard1_ready;
  surface.typed_sema_advanced_performance_shard1_key =
      typed_sema_to_lowering_contract_surface.typed_advanced_performance_shard1_key;
  surface.typed_sema_advanced_core_shard2_consistent =
      typed_sema_to_lowering_contract_surface.typed_advanced_core_shard2_consistent;
  surface.typed_sema_advanced_core_shard2_ready =
      typed_sema_to_lowering_contract_surface.typed_advanced_core_shard2_ready;
  surface.typed_sema_advanced_core_shard2_key =
      typed_sema_to_lowering_contract_surface.typed_advanced_core_shard2_key;
  surface.typed_sema_advanced_edge_compatibility_shard2_consistent =
      typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard2_consistent;
  surface.typed_sema_advanced_edge_compatibility_shard2_ready =
      typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard2_ready;
  surface.typed_sema_advanced_edge_compatibility_shard2_key =
      typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard2_key;
  surface.typed_sema_advanced_diagnostics_shard2_consistent =
      typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard2_consistent;
  surface.typed_sema_advanced_diagnostics_shard2_ready =
      typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard2_ready;
  surface.typed_sema_advanced_diagnostics_shard2_key =
      typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard2_key;
  surface.typed_sema_advanced_conformance_shard2_consistent =
      typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard2_consistent;
  surface.typed_sema_advanced_conformance_shard2_ready =
      typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard2_ready;
  surface.typed_sema_advanced_conformance_shard2_key =
      typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard2_key;
  surface.typed_sema_advanced_integration_shard2_consistent =
      typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard2_consistent;
  surface.typed_sema_advanced_integration_shard2_ready =
      typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard2_ready;
  surface.typed_sema_advanced_integration_shard2_key =
      typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard2_key;
  surface.typed_sema_integration_closeout_signoff_consistent =
      typed_sema_to_lowering_contract_surface.typed_integration_closeout_signoff_consistent;
  surface.typed_sema_integration_closeout_signoff_ready =
      typed_sema_to_lowering_contract_surface.typed_integration_closeout_signoff_ready;
  surface.typed_sema_integration_closeout_signoff_key =
      typed_sema_to_lowering_contract_surface.typed_integration_closeout_signoff_key;
  Objc3LoweringIRBoundary lowering_boundary;
  std::string lowering_error;
  const bool lowering_boundary_from_options_ready =
      TryBuildObjc3LoweringIRBoundary(options.lowering, lowering_boundary, lowering_error);
  const std::string lowering_boundary_replay_key_from_options =
      lowering_boundary_from_options_ready ? Objc3LoweringIRBoundaryReplayKey(lowering_boundary)
                                           : std::string();
  const bool lowering_boundary_replay_key_matches_typed_surface =
      !lowering_boundary_from_options_ready ||
      lowering_boundary_replay_key_from_options ==
          typed_sema_to_lowering_contract_surface.lowering_boundary_replay_key;
  surface.lowering_boundary_ready =
      typed_sema_to_lowering_contract_surface.lowering_boundary_ready &&
      lowering_boundary_from_options_ready &&
      lowering_boundary_replay_key_matches_typed_surface;
  surface.lowering_boundary_replay_key =
      typed_sema_to_lowering_contract_surface.lowering_boundary_replay_key;
  if (surface.lowering_boundary_replay_key.empty() && lowering_boundary_from_options_ready) {
    surface.lowering_boundary_replay_key = lowering_boundary_replay_key_from_options;
  }
  if (!typed_sema_to_lowering_contract_surface.failure_reason.empty()) {
    surface.failure_reason = typed_sema_to_lowering_contract_surface.failure_reason;
  } else if (!lowering_boundary_from_options_ready && !lowering_error.empty()) {
    surface.failure_reason = lowering_error;
  } else if (!lowering_boundary_replay_key_matches_typed_surface) {
    surface.failure_reason = "typed sema/lowering boundary replay key does not match lowering contract";
  }

  const bool diagnostics_clear =
      surface.lexer_diagnostic_count == 0 &&
      surface.parser_diagnostic_count == 0 &&
      surface.semantic_diagnostic_count == 0;
  const bool parse_snapshot_ready =
      surface.parser_contract_snapshot_present &&
      surface.parser_contract_deterministic &&
      surface.parser_recovery_replay_ready &&
      surface.long_tail_grammar_core_feature_consistent &&
      surface.long_tail_grammar_handoff_key_deterministic &&
      surface.long_tail_grammar_expansion_ready &&
      surface.long_tail_grammar_compatibility_handoff_ready &&
      surface.long_tail_grammar_edge_case_compatibility_ready &&
      surface.long_tail_grammar_edge_case_expansion_consistent &&
      surface.long_tail_grammar_edge_case_robustness_ready &&
      surface.long_tail_grammar_diagnostics_hardening_ready &&
      surface.long_tail_grammar_recovery_determinism_ready &&
      surface.parse_artifact_handoff_deterministic;
  const bool parse_artifact_replay_key_ready =
      surface.parse_artifact_replay_key_deterministic;
  const bool parse_artifact_diagnostics_hardening_ready =
      surface.parse_artifact_diagnostics_hardening_consistent;
  const bool parse_recovery_determinism_hardening_ready =
      surface.parse_recovery_determinism_hardening_consistent;
  const bool parse_snapshot_replay_ready =
      parse_snapshot_ready &&
      parse_artifact_replay_key_ready &&
      parse_artifact_diagnostics_hardening_ready &&
      surface.parse_artifact_edge_case_robustness_consistent &&
      surface.long_tail_grammar_edge_case_robustness_ready &&
      surface.long_tail_grammar_diagnostics_hardening_ready &&
      surface.long_tail_grammar_recovery_determinism_ready &&
      parse_recovery_determinism_hardening_ready;
  const bool typed_core_feature_expansion_case_accounting_consistent =
      surface.typed_sema_core_feature_expansion_case_count ==
          kObjc3TypedSemaToLoweringCoreFeatureExpansionCaseCount &&
      surface.typed_sema_core_feature_expansion_case_count > 0 &&
      surface.typed_sema_core_feature_expansion_case_count >=
          surface.typed_sema_core_feature_expansion_passed_case_count &&
      surface.typed_sema_core_feature_expansion_failed_case_count ==
          (surface.typed_sema_core_feature_expansion_case_count -
           surface.typed_sema_core_feature_expansion_passed_case_count);
  const bool typed_core_feature_expansion_ready =
      typed_core_feature_expansion_case_accounting_consistent &&
      surface.typed_sema_core_feature_expansion_consistent &&
      !surface.typed_sema_core_feature_expansion_key.empty();
  const bool typed_edge_case_compatibility_alignment =
      surface.compatibility_handoff_consistent ==
          typed_sema_to_lowering_contract_surface.compatibility_handoff_consistent &&
      surface.language_version_pragma_coordinate_order_consistent ==
          typed_sema_to_lowering_contract_surface.language_version_pragma_coordinate_order_consistent &&
      surface.parse_artifact_replay_key_deterministic ==
          typed_sema_to_lowering_contract_surface.parse_artifact_replay_key_deterministic &&
      surface.parse_artifact_edge_case_robustness_consistent ==
          typed_sema_to_lowering_contract_surface.parse_artifact_edge_case_robustness_consistent &&
      surface.compatibility_handoff_key ==
          typed_sema_to_lowering_contract_surface.compatibility_handoff_key &&
      surface.parse_artifact_edge_robustness_key ==
          typed_sema_to_lowering_contract_surface.parse_artifact_edge_robustness_key;
  const bool typed_edge_case_robustness_alignment =
      surface.typed_sema_edge_case_expansion_consistent ==
          typed_sema_to_lowering_contract_surface.typed_core_feature_edge_case_expansion_consistent &&
      surface.typed_sema_edge_case_robustness_ready ==
          typed_sema_to_lowering_contract_surface.typed_core_feature_edge_case_robustness_ready &&
      surface.typed_sema_edge_case_robustness_key ==
          typed_sema_to_lowering_contract_surface.typed_core_feature_edge_case_robustness_key;
  const bool typed_diagnostics_hardening_alignment =
      surface.typed_sema_diagnostics_hardening_consistent ==
          typed_sema_to_lowering_contract_surface.typed_diagnostics_hardening_consistent &&
      surface.typed_sema_diagnostics_hardening_ready ==
          typed_sema_to_lowering_contract_surface.typed_diagnostics_hardening_ready &&
      surface.typed_sema_diagnostics_hardening_key ==
          typed_sema_to_lowering_contract_surface.typed_diagnostics_hardening_key;
  const bool typed_recovery_determinism_alignment =
      surface.typed_sema_recovery_determinism_consistent ==
          typed_sema_to_lowering_contract_surface.typed_recovery_determinism_consistent &&
      surface.typed_sema_recovery_determinism_ready ==
          typed_sema_to_lowering_contract_surface.typed_recovery_determinism_ready &&
      surface.typed_sema_recovery_determinism_key ==
          typed_sema_to_lowering_contract_surface.typed_recovery_determinism_key;
  const bool typed_conformance_matrix_alignment =
      surface.typed_sema_conformance_matrix_consistent ==
          typed_sema_to_lowering_contract_surface.typed_conformance_matrix_consistent &&
      surface.typed_sema_conformance_matrix_ready ==
          typed_sema_to_lowering_contract_surface.typed_conformance_matrix_ready &&
      surface.typed_sema_conformance_matrix_key ==
          typed_sema_to_lowering_contract_surface.typed_conformance_matrix_key;
  const bool typed_conformance_corpus_alignment =
      surface.typed_sema_conformance_corpus_consistent ==
          typed_sema_to_lowering_contract_surface.typed_conformance_corpus_consistent &&
      surface.typed_sema_conformance_corpus_ready ==
          typed_sema_to_lowering_contract_surface.typed_conformance_corpus_ready &&
      surface.typed_sema_conformance_corpus_key ==
          typed_sema_to_lowering_contract_surface.typed_conformance_corpus_key;
  const bool typed_performance_quality_guardrails_alignment =
      surface.typed_sema_performance_quality_guardrails_consistent ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_consistent &&
      surface.typed_sema_performance_quality_guardrails_ready ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_ready &&
      surface.typed_sema_performance_quality_guardrails_case_count ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_case_count &&
      surface.typed_sema_performance_quality_guardrails_passed_case_count ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_passed_case_count &&
      surface.typed_sema_performance_quality_guardrails_failed_case_count ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_failed_case_count &&
      surface.typed_sema_performance_quality_guardrails_key ==
          typed_sema_to_lowering_contract_surface.typed_performance_quality_guardrails_key;
  const bool typed_cross_lane_integration_alignment =
      surface.typed_sema_cross_lane_integration_consistent ==
          typed_sema_to_lowering_contract_surface.typed_cross_lane_integration_consistent &&
      surface.typed_sema_cross_lane_integration_ready ==
          typed_sema_to_lowering_contract_surface.typed_cross_lane_integration_ready &&
      surface.typed_sema_cross_lane_integration_key ==
          typed_sema_to_lowering_contract_surface.typed_cross_lane_integration_key;
  const bool typed_docs_runbook_sync_alignment =
      surface.typed_sema_docs_runbook_sync_consistent ==
          typed_sema_to_lowering_contract_surface.typed_docs_runbook_sync_consistent &&
      surface.typed_sema_docs_runbook_sync_ready ==
          typed_sema_to_lowering_contract_surface.typed_docs_runbook_sync_ready &&
      surface.typed_sema_docs_runbook_sync_key ==
          typed_sema_to_lowering_contract_surface.typed_docs_runbook_sync_key;
  const bool typed_release_candidate_replay_dry_run_alignment =
      surface.typed_sema_release_candidate_replay_dry_run_consistent ==
          typed_sema_to_lowering_contract_surface.typed_release_candidate_replay_dry_run_consistent &&
      surface.typed_sema_release_candidate_replay_dry_run_ready ==
          typed_sema_to_lowering_contract_surface.typed_release_candidate_replay_dry_run_ready &&
      surface.typed_sema_release_candidate_replay_dry_run_key ==
          typed_sema_to_lowering_contract_surface.typed_release_candidate_replay_dry_run_key;
  const bool typed_advanced_core_shard1_alignment =
      surface.typed_sema_advanced_core_shard1_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_core_shard1_consistent &&
      surface.typed_sema_advanced_core_shard1_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_core_shard1_ready &&
      surface.typed_sema_advanced_core_shard1_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_core_shard1_key;
  const bool typed_advanced_edge_compatibility_shard1_alignment =
      surface.typed_sema_advanced_edge_compatibility_shard1_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard1_consistent &&
      surface.typed_sema_advanced_edge_compatibility_shard1_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard1_ready &&
      surface.typed_sema_advanced_edge_compatibility_shard1_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard1_key;
  const bool typed_advanced_diagnostics_shard1_alignment =
      surface.typed_sema_advanced_diagnostics_shard1_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard1_consistent &&
      surface.typed_sema_advanced_diagnostics_shard1_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard1_ready &&
      surface.typed_sema_advanced_diagnostics_shard1_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard1_key;
  const bool typed_advanced_conformance_shard1_alignment =
      surface.typed_sema_advanced_conformance_shard1_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard1_consistent &&
      surface.typed_sema_advanced_conformance_shard1_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard1_ready &&
      surface.typed_sema_advanced_conformance_shard1_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard1_key;
  const bool typed_advanced_integration_shard1_alignment =
      surface.typed_sema_advanced_integration_shard1_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard1_consistent &&
      surface.typed_sema_advanced_integration_shard1_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard1_ready &&
      surface.typed_sema_advanced_integration_shard1_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard1_key;
  const bool typed_advanced_performance_shard1_alignment =
      surface.typed_sema_advanced_performance_shard1_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_performance_shard1_consistent &&
      surface.typed_sema_advanced_performance_shard1_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_performance_shard1_ready &&
      surface.typed_sema_advanced_performance_shard1_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_performance_shard1_key;
  const bool typed_advanced_core_shard2_alignment =
      surface.typed_sema_advanced_core_shard2_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_core_shard2_consistent &&
      surface.typed_sema_advanced_core_shard2_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_core_shard2_ready &&
      surface.typed_sema_advanced_core_shard2_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_core_shard2_key;
  const bool typed_advanced_edge_compatibility_shard2_alignment =
      surface.typed_sema_advanced_edge_compatibility_shard2_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard2_consistent &&
      surface.typed_sema_advanced_edge_compatibility_shard2_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard2_ready &&
      surface.typed_sema_advanced_edge_compatibility_shard2_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_edge_compatibility_shard2_key;
  const bool typed_advanced_diagnostics_shard2_alignment =
      surface.typed_sema_advanced_diagnostics_shard2_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard2_consistent &&
      surface.typed_sema_advanced_diagnostics_shard2_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard2_ready &&
      surface.typed_sema_advanced_diagnostics_shard2_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_diagnostics_shard2_key;
  const bool typed_advanced_conformance_shard2_alignment =
      surface.typed_sema_advanced_conformance_shard2_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard2_consistent &&
      surface.typed_sema_advanced_conformance_shard2_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard2_ready &&
      surface.typed_sema_advanced_conformance_shard2_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_conformance_shard2_key;
  const bool typed_advanced_integration_shard2_alignment =
      surface.typed_sema_advanced_integration_shard2_consistent ==
          typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard2_consistent &&
      surface.typed_sema_advanced_integration_shard2_ready ==
          typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard2_ready &&
      surface.typed_sema_advanced_integration_shard2_key ==
          typed_sema_to_lowering_contract_surface.typed_advanced_integration_shard2_key;
  const bool typed_integration_closeout_signoff_alignment =
      surface.typed_sema_integration_closeout_signoff_consistent ==
          typed_sema_to_lowering_contract_surface.typed_integration_closeout_signoff_consistent &&
      surface.typed_sema_integration_closeout_signoff_ready ==
          typed_sema_to_lowering_contract_surface.typed_integration_closeout_signoff_ready &&
      surface.typed_sema_integration_closeout_signoff_key ==
          typed_sema_to_lowering_contract_surface.typed_integration_closeout_signoff_key;
  const bool typed_core_feature_ready =
      surface.typed_handoff_key_deterministic &&
      surface.typed_sema_core_feature_consistent &&
      typed_core_feature_expansion_ready &&
      surface.typed_sema_edge_case_compatibility_ready &&
      surface.typed_sema_edge_case_expansion_consistent &&
      surface.typed_sema_edge_case_robustness_ready &&
      surface.typed_sema_diagnostics_hardening_consistent &&
      surface.typed_sema_diagnostics_hardening_ready &&
      surface.typed_sema_recovery_determinism_consistent &&
      surface.typed_sema_recovery_determinism_ready &&
      surface.typed_sema_conformance_matrix_consistent &&
      surface.typed_sema_conformance_matrix_ready &&
      surface.typed_sema_conformance_corpus_consistent &&
      surface.typed_sema_conformance_corpus_ready &&
      surface.typed_sema_performance_quality_guardrails_consistent &&
      surface.typed_sema_performance_quality_guardrails_ready &&
      surface.typed_sema_cross_lane_integration_consistent &&
      surface.typed_sema_cross_lane_integration_ready &&
      surface.typed_sema_docs_runbook_sync_consistent &&
      surface.typed_sema_docs_runbook_sync_ready &&
      surface.typed_sema_release_candidate_replay_dry_run_consistent &&
      surface.typed_sema_release_candidate_replay_dry_run_ready &&
      surface.typed_sema_advanced_core_shard1_consistent &&
      surface.typed_sema_advanced_core_shard1_ready &&
      surface.typed_sema_advanced_edge_compatibility_shard1_consistent &&
      surface.typed_sema_advanced_edge_compatibility_shard1_ready &&
      surface.typed_sema_advanced_diagnostics_shard1_consistent &&
      surface.typed_sema_advanced_diagnostics_shard1_ready &&
      surface.typed_sema_advanced_conformance_shard1_consistent &&
      surface.typed_sema_advanced_conformance_shard1_ready &&
      surface.typed_sema_advanced_integration_shard1_consistent &&
      surface.typed_sema_advanced_integration_shard1_ready &&
      surface.typed_sema_advanced_performance_shard1_consistent &&
      surface.typed_sema_advanced_performance_shard1_ready &&
      surface.typed_sema_advanced_core_shard2_consistent &&
      surface.typed_sema_advanced_core_shard2_ready &&
      surface.typed_sema_advanced_edge_compatibility_shard2_consistent &&
      surface.typed_sema_advanced_edge_compatibility_shard2_ready &&
      surface.typed_sema_advanced_diagnostics_shard2_consistent &&
      surface.typed_sema_advanced_diagnostics_shard2_ready &&
      surface.typed_sema_advanced_conformance_shard2_consistent &&
      surface.typed_sema_advanced_conformance_shard2_ready &&
      surface.typed_sema_advanced_integration_shard2_consistent &&
      surface.typed_sema_advanced_integration_shard2_ready &&
      surface.typed_sema_integration_closeout_signoff_consistent &&
      surface.typed_sema_integration_closeout_signoff_ready &&
      typed_edge_case_compatibility_alignment &&
      typed_edge_case_robustness_alignment &&
      !surface.typed_sema_edge_case_compatibility_key.empty() &&
      !surface.typed_sema_edge_case_robustness_key.empty() &&
      typed_diagnostics_hardening_alignment &&
      !surface.typed_sema_diagnostics_hardening_key.empty() &&
      typed_recovery_determinism_alignment &&
      !surface.typed_sema_recovery_determinism_key.empty() &&
      typed_conformance_matrix_alignment &&
      !surface.typed_sema_conformance_matrix_key.empty() &&
      typed_conformance_corpus_alignment &&
      !surface.typed_sema_conformance_corpus_key.empty() &&
      typed_performance_quality_guardrails_alignment &&
      !surface.typed_sema_performance_quality_guardrails_key.empty() &&
      typed_cross_lane_integration_alignment &&
      !surface.typed_sema_cross_lane_integration_key.empty() &&
      typed_docs_runbook_sync_alignment &&
      !surface.typed_sema_docs_runbook_sync_key.empty() &&
