#pragma once

#include <cstddef>

#include "parse/objc3_diagnostic_grammar_hooks_core_feature.h"
#include "parse/objc3_diagnostic_grammar_hooks_core_feature_expansion_surface.h"
#include "parse/objc3_diagnostic_grammar_hooks_edge_case_compatibility_surface.h"
#include "parse/objc3_diagnostic_source_precision_scaffold.h"
#include "pipeline/parse_lowering_artifact_keys.h"
#include "pipeline/parse_lowering_diagnostic_keys.h"
#include "pipeline/objc3_frontend_types.h"
#include "pipeline/readiness/objc3_long_tail_grammar_readiness_keys.h"
#include "pipeline/readiness/objc3_parse_lowering_conformance_keys.h"

inline void BuildObjc3ParseLoweringParserBehaviorReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3FrontendOptions &options) {
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
  const bool canonical_literal_rejection_counts_consistent =
      IsObjc3FrontendCanonicalLiteralRejectionCountsConsistent(
          pipeline_result.canonical_literal_rejection_counts,
          surface.parser_token_count);
  const bool language_version_pragma_contract_consistent =
      IsObjc3LanguageVersionPragmaContractConsistent(
          pipeline_result.language_version_pragma_contract);
  surface.language_version_pragma_coordinate_order_consistent =
      IsObjc3LanguageVersionPragmaCoordinateOrderConsistent(
          pipeline_result.language_version_pragma_contract);
  surface.compatibility_handoff_consistent =
      canonical_literal_rejection_counts_consistent &&
      language_version_pragma_contract_consistent;
  surface.compatibility_handoff_key = BuildObjc3CompatibilityHandoffKey(
      options,
      pipeline_result.canonical_literal_rejection_counts,
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
}
