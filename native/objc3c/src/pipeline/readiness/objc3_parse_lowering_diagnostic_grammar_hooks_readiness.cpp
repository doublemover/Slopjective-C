#include "parse/objc3_diagnostic_grammar_hooks_core_feature.h"
#include "parse/objc3_diagnostic_grammar_hooks_core_feature_expansion_surface.h"
#include "parse/objc3_diagnostic_source_precision_scaffold.h"
#include "pipeline/parse_lowering_diagnostic_keys.h"
#include "pipeline/readiness/objc3_parse_lowering_conformance_keys.h"
#include "pipeline/readiness/objc3_parse_lowering_diagnostic_grammar_hooks_readiness.h"

Objc3ParseLoweringDiagnosticGrammarHooksReadinessRecord
BuildObjc3ParseLoweringDiagnosticGrammarHooksReadiness(
    Objc3ParseLoweringReadinessSurface &surface,
    const Objc3FrontendPipelineResult &pipeline_result,
    const Objc3ParserContractSnapshot &parser_snapshot) {
  Objc3ParseLoweringDiagnosticGrammarHooksReadinessRecord record;

  surface.parse_lowering_conformance_matrix_case_count =
      kObjc3ParseLoweringConformanceMatrixCaseCount;
  surface.parse_lowering_conformance_corpus_case_count =
      kObjc3ParseLoweringConformanceCorpusCaseCount;
  surface.parse_lowering_performance_quality_guardrails_case_count =
      kObjc3ParseLoweringPerformanceQualityGuardrailsCaseCount;

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
  record.core_feature_expansion =
      BuildObjc3DiagnosticGrammarHooksCoreFeatureExpansionSurface(
          parser_diagnostic_grammar_hooks_core_feature,
          parser_diagnostic_source_precision_scaffold,
          parser_diagnostic_code_coverage.unique_code_count,
          parser_diagnostic_code_coverage.unique_code_fingerprint,
          surface.parser_diagnostic_count,
          parser_snapshot.parser_diagnostic_count);

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
      record.core_feature_expansion.unique_diagnostic_code_count;
  surface.parser_diagnostic_grammar_hooks_core_feature_expansion_accounting_consistent =
      record.core_feature_expansion.accounting_consistent;
  surface.parser_diagnostic_grammar_hooks_core_feature_expansion_replay_keys_ready =
      record.core_feature_expansion.replay_keys_ready;
  surface.parser_diagnostic_grammar_hooks_core_feature_expansion_key =
      record.core_feature_expansion.expansion_key;
  surface.parser_diagnostic_grammar_hooks_core_feature_expansion_ready =
      IsObjc3DiagnosticGrammarHooksCoreFeatureExpansionReady(
          record.core_feature_expansion);
  surface.parser_diagnostic_code_count =
      parser_diagnostic_code_coverage.unique_code_count;
  surface.parser_diagnostic_code_fingerprint =
      parser_diagnostic_code_coverage.unique_code_fingerprint;
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

  return record;
}

void ApplyObjc3ParseLoweringDiagnosticGrammarHooksConformanceMatrixReadiness(
    Objc3ParseLoweringReadinessSurface &surface) {
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
}
