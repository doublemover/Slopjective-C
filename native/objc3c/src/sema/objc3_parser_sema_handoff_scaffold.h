#pragma once

#include "parser/contracts/canonical_literal_handoff.h"
#include "sema/objc3_parser_sema_handoff_contract.h"

inline Objc3ParserSemaPerformanceQualityGuardrails BuildObjc3ParserSemaPerformanceQualityGuardrails(
    const Objc3ParserSemaConformanceMatrix &matrix,
    const Objc3ParserSemaConformanceCorpus &corpus) {
  Objc3ParserSemaPerformanceQualityGuardrails guardrails;
  guardrails.conformance_matrix_builder_max_lines = kObjc3ParserSemaConformanceMatrixBuilderMaxLines;
  guardrails.conformance_corpus_builder_max_lines = kObjc3ParserSemaConformanceCorpusBuilderMaxLines;
  guardrails.handoff_scaffold_builder_max_lines = kObjc3ParserSemaHandoffScaffoldBuilderMaxLines;
  guardrails.conformance_matrix_builder_budget_guarded =
      guardrails.conformance_matrix_builder_max_lines >= 150u &&
      guardrails.conformance_matrix_builder_max_lines <= 220u;
  guardrails.conformance_corpus_builder_budget_guarded =
      guardrails.conformance_corpus_builder_max_lines >= 40u &&
      guardrails.conformance_corpus_builder_max_lines <= 100u;
  guardrails.handoff_scaffold_builder_budget_guarded =
      guardrails.handoff_scaffold_builder_max_lines >= 40u &&
      guardrails.handoff_scaffold_builder_max_lines <= 120u;
  guardrails.matrix_diagnostic_budget_consistent =
      matrix.parser_diagnostic_budget_consistent;
  guardrails.matrix_token_top_level_budget_consistent =
      matrix.parser_token_top_level_budget_consistent;
  guardrails.matrix_subset_budget_consistent = matrix.parser_subset_count_consistent;
  guardrails.corpus_case_budget_consistent =
      corpus.required_case_count == 5u &&
      corpus.passed_case_count == corpus.required_case_count &&
      corpus.failed_case_count == 0u;
  guardrails.required_guardrail_count = 7u;
  guardrails.passed_guardrail_count =
      static_cast<std::size_t>(guardrails.conformance_matrix_builder_budget_guarded) +
      static_cast<std::size_t>(guardrails.conformance_corpus_builder_budget_guarded) +
      static_cast<std::size_t>(guardrails.handoff_scaffold_builder_budget_guarded) +
      static_cast<std::size_t>(guardrails.matrix_diagnostic_budget_consistent) +
      static_cast<std::size_t>(guardrails.matrix_token_top_level_budget_consistent) +
      static_cast<std::size_t>(guardrails.matrix_subset_budget_consistent) +
      static_cast<std::size_t>(guardrails.corpus_case_budget_consistent);
  guardrails.failed_guardrail_count =
      guardrails.required_guardrail_count >= guardrails.passed_guardrail_count
          ? (guardrails.required_guardrail_count - guardrails.passed_guardrail_count)
          : guardrails.required_guardrail_count;
  guardrails.deterministic =
      matrix.deterministic &&
      corpus.deterministic &&
      guardrails.required_guardrail_count == 7u &&
      guardrails.passed_guardrail_count == guardrails.required_guardrail_count &&
      guardrails.failed_guardrail_count == 0u;
  return guardrails;
}

inline Objc3ParserSemaCrossLaneIntegrationSync BuildObjc3ParserSemaCrossLaneIntegrationSync(
    const Objc3ParserSemaConformanceMatrix &matrix,
    const Objc3ParserSemaConformanceCorpus &corpus,
    const Objc3ParserSemaPerformanceQualityGuardrails &guardrails) {
  Objc3ParserSemaCrossLaneIntegrationSync sync;
  sync.matrix_consistent = matrix.deterministic;
  sync.corpus_consistent = corpus.deterministic;
  sync.performance_quality_guardrails_consistent = guardrails.deterministic;
  sync.pass_manager_contract_surface_sync =
      matrix.deterministic &&
      corpus.deterministic &&
      guardrails.deterministic &&
      guardrails.required_guardrail_count == 7u &&
      guardrails.passed_guardrail_count == guardrails.required_guardrail_count &&
      guardrails.failed_guardrail_count == 0u;
  sync.required_sync_count = 4u;
  sync.passed_sync_count =
      static_cast<std::size_t>(sync.matrix_consistent) +
      static_cast<std::size_t>(sync.corpus_consistent) +
      static_cast<std::size_t>(sync.performance_quality_guardrails_consistent) +
      static_cast<std::size_t>(sync.pass_manager_contract_surface_sync);
  sync.failed_sync_count =
      sync.required_sync_count >= sync.passed_sync_count
          ? (sync.required_sync_count - sync.passed_sync_count)
          : sync.required_sync_count;
  sync.deterministic =
      sync.required_sync_count == 4u &&
      sync.passed_sync_count == sync.required_sync_count &&
      sync.failed_sync_count == 0u;
  return sync;
}

inline Objc3ParserSemaDocsRunbookSync BuildObjc3ParserSemaDocsRunbookSync(
    const Objc3ParserSemaCrossLaneIntegrationSync &cross_lane_sync) {
  Objc3ParserSemaDocsRunbookSync sync;
  sync.cross_lane_integration_sync_ready = cross_lane_sync.deterministic;
  sync.pass_manager_contract_surface_sync =
      cross_lane_sync.required_sync_count == 4u &&
      cross_lane_sync.passed_sync_count == cross_lane_sync.required_sync_count &&
      cross_lane_sync.failed_sync_count == 0u;
  sync.parity_surface_sync =
      sync.cross_lane_integration_sync_ready &&
      sync.pass_manager_contract_surface_sync;
  sync.required_sync_count = 3u;
  sync.passed_sync_count =
      static_cast<std::size_t>(sync.cross_lane_integration_sync_ready) +
      static_cast<std::size_t>(sync.pass_manager_contract_surface_sync) +
      static_cast<std::size_t>(sync.parity_surface_sync);
  sync.failed_sync_count =
      sync.required_sync_count >= sync.passed_sync_count
          ? (sync.required_sync_count - sync.passed_sync_count)
          : sync.required_sync_count;
  sync.deterministic =
      sync.required_sync_count == 3u &&
      sync.passed_sync_count == sync.required_sync_count &&
      sync.failed_sync_count == 0u;
  return sync;
}

inline Objc3ParserSemaReleaseCandidateReplayDryRun
BuildObjc3ParserSemaReleaseCandidateReplayDryRun(
    const Objc3ParserSemaDocsRunbookSync &docs_sync) {
  Objc3ParserSemaReleaseCandidateReplayDryRun sync;
  sync.docs_runbook_sync_ready = docs_sync.deterministic;
  sync.pass_manager_contract_surface_sync =
      docs_sync.required_sync_count == 3u &&
      docs_sync.passed_sync_count == docs_sync.required_sync_count &&
      docs_sync.failed_sync_count == 0u;
  sync.replay_surface_sync =
      sync.docs_runbook_sync_ready && sync.pass_manager_contract_surface_sync;
  sync.required_sync_count = 3u;
  sync.passed_sync_count =
      static_cast<std::size_t>(sync.docs_runbook_sync_ready) +
      static_cast<std::size_t>(sync.pass_manager_contract_surface_sync) +
      static_cast<std::size_t>(sync.replay_surface_sync);
  sync.failed_sync_count =
      sync.required_sync_count >= sync.passed_sync_count
          ? (sync.required_sync_count - sync.passed_sync_count)
          : sync.required_sync_count;
  sync.deterministic =
      sync.required_sync_count == 3u &&
      sync.passed_sync_count == sync.required_sync_count &&
      sync.failed_sync_count == 0u;
  return sync;
}

inline Objc3ParserSemaAdvancedCoreShard1 BuildObjc3ParserSemaAdvancedCoreShard1(
    const Objc3ParserSemaReleaseCandidateReplayDryRun &release_sync) {
  Objc3ParserSemaAdvancedCoreShard1 sync;
  sync.release_candidate_replay_dry_run_ready = release_sync.deterministic;
  sync.pass_manager_contract_surface_sync =
      release_sync.required_sync_count == 3u &&
      release_sync.passed_sync_count == release_sync.required_sync_count &&
      release_sync.failed_sync_count == 0u;
  sync.shard_surface_sync =
      sync.release_candidate_replay_dry_run_ready &&
      sync.pass_manager_contract_surface_sync;
  sync.required_sync_count = 3u;
  sync.passed_sync_count =
      static_cast<std::size_t>(sync.release_candidate_replay_dry_run_ready) +
      static_cast<std::size_t>(sync.pass_manager_contract_surface_sync) +
      static_cast<std::size_t>(sync.shard_surface_sync);
  sync.failed_sync_count =
      sync.required_sync_count >= sync.passed_sync_count
          ? (sync.required_sync_count - sync.passed_sync_count)
          : sync.required_sync_count;
  sync.deterministic =
      sync.required_sync_count == 3u &&
      sync.passed_sync_count == sync.required_sync_count &&
      sync.failed_sync_count == 0u;
  return sync;
}

inline Objc3ParserSemaAdvancedContractRejectionShard1
BuildObjc3ParserSemaAdvancedContractRejectionShard1(
    const Objc3ParserSemaAdvancedCoreShard1 &core_shard1_sync) {
  Objc3ParserSemaAdvancedContractRejectionShard1 sync;
  sync.advanced_core_shard1_ready = core_shard1_sync.deterministic;
  sync.pass_manager_contract_surface_sync =
      core_shard1_sync.required_sync_count == 3u &&
      core_shard1_sync.passed_sync_count == core_shard1_sync.required_sync_count &&
      core_shard1_sync.failed_sync_count == 0u;
  sync.shard_surface_sync =
      sync.advanced_core_shard1_ready &&
      sync.pass_manager_contract_surface_sync;
  sync.required_sync_count = 3u;
  sync.passed_sync_count =
      static_cast<std::size_t>(sync.advanced_core_shard1_ready) +
      static_cast<std::size_t>(sync.pass_manager_contract_surface_sync) +
      static_cast<std::size_t>(sync.shard_surface_sync);
  sync.failed_sync_count =
      sync.required_sync_count >= sync.passed_sync_count
          ? (sync.required_sync_count - sync.passed_sync_count)
          : sync.required_sync_count;
  sync.deterministic =
      sync.required_sync_count == 3u &&
      sync.passed_sync_count == sync.required_sync_count &&
      sync.failed_sync_count == 0u;
  return sync;
}

inline Objc3ParserSemaAdvancedDiagnosticsShard1
BuildObjc3ParserSemaAdvancedDiagnosticsShard1(
    const Objc3ParserSemaAdvancedContractRejectionShard1 &contract_rejection_shard1_sync) {
  Objc3ParserSemaAdvancedDiagnosticsShard1 sync;
  sync.advanced_contract_rejection_shard1_ready = contract_rejection_shard1_sync.deterministic;
  sync.pass_manager_contract_surface_sync =
      contract_rejection_shard1_sync.required_sync_count == 3u &&
      contract_rejection_shard1_sync.passed_sync_count ==
          contract_rejection_shard1_sync.required_sync_count &&
      contract_rejection_shard1_sync.failed_sync_count == 0u;
  sync.shard_surface_sync =
      sync.advanced_contract_rejection_shard1_ready &&
      sync.pass_manager_contract_surface_sync;
  sync.required_sync_count = 3u;
  sync.passed_sync_count =
      static_cast<std::size_t>(sync.advanced_contract_rejection_shard1_ready) +
      static_cast<std::size_t>(sync.pass_manager_contract_surface_sync) +
      static_cast<std::size_t>(sync.shard_surface_sync);
  sync.failed_sync_count =
      sync.required_sync_count >= sync.passed_sync_count
          ? (sync.required_sync_count - sync.passed_sync_count)
          : sync.required_sync_count;
  sync.deterministic =
      sync.required_sync_count == 3u &&
      sync.passed_sync_count == sync.required_sync_count &&
      sync.failed_sync_count == 0u;
  return sync;
}

inline Objc3ParserSemaAdvancedConformanceShard1
BuildObjc3ParserSemaAdvancedConformanceShard1(
    const Objc3ParserSemaAdvancedDiagnosticsShard1 &diagnostics_shard1_sync) {
  Objc3ParserSemaAdvancedConformanceShard1 sync;
  sync.advanced_diagnostics_shard1_ready = diagnostics_shard1_sync.deterministic;
  sync.pass_manager_contract_surface_sync =
      diagnostics_shard1_sync.required_sync_count == 3u &&
      diagnostics_shard1_sync.passed_sync_count ==
          diagnostics_shard1_sync.required_sync_count &&
      diagnostics_shard1_sync.failed_sync_count == 0u;
  sync.shard_surface_sync =
      sync.advanced_diagnostics_shard1_ready &&
      sync.pass_manager_contract_surface_sync;
  sync.required_sync_count = 3u;
  sync.passed_sync_count =
      static_cast<std::size_t>(sync.advanced_diagnostics_shard1_ready) +
      static_cast<std::size_t>(sync.pass_manager_contract_surface_sync) +
      static_cast<std::size_t>(sync.shard_surface_sync);
  sync.failed_sync_count =
      sync.required_sync_count >= sync.passed_sync_count
          ? (sync.required_sync_count - sync.passed_sync_count)
          : sync.required_sync_count;
  sync.deterministic =
      sync.required_sync_count == 3u &&
      sync.passed_sync_count == sync.required_sync_count &&
      sync.failed_sync_count == 0u;
  return sync;
}

inline Objc3ParserSemaAdvancedIntegrationShard1
BuildObjc3ParserSemaAdvancedIntegrationShard1(
    const Objc3ParserSemaAdvancedConformanceShard1 &conformance_shard1_sync) {
  Objc3ParserSemaAdvancedIntegrationShard1 sync;
  sync.advanced_conformance_shard1_ready = conformance_shard1_sync.deterministic;
  sync.pass_manager_contract_surface_sync =
      conformance_shard1_sync.required_sync_count == 3u &&
      conformance_shard1_sync.passed_sync_count ==
          conformance_shard1_sync.required_sync_count &&
      conformance_shard1_sync.failed_sync_count == 0u;
  sync.shard_surface_sync =
      sync.advanced_conformance_shard1_ready &&
      sync.pass_manager_contract_surface_sync;
  sync.required_sync_count = 3u;
  sync.passed_sync_count =
      static_cast<std::size_t>(sync.advanced_conformance_shard1_ready) +
      static_cast<std::size_t>(sync.pass_manager_contract_surface_sync) +
      static_cast<std::size_t>(sync.shard_surface_sync);
  sync.failed_sync_count =
      sync.required_sync_count >= sync.passed_sync_count
          ? (sync.required_sync_count - sync.passed_sync_count)
          : sync.required_sync_count;
  sync.deterministic =
      sync.required_sync_count == 3u &&
      sync.passed_sync_count == sync.required_sync_count &&
      sync.failed_sync_count == 0u;
  return sync;
}

inline Objc3ParserSemaAdvancedPerformanceShard1
BuildObjc3ParserSemaAdvancedPerformanceShard1(
    const Objc3ParserSemaAdvancedIntegrationShard1 &integration_shard1_sync) {
  Objc3ParserSemaAdvancedPerformanceShard1 sync;
  sync.advanced_integration_shard1_ready = integration_shard1_sync.deterministic;
  sync.pass_manager_contract_surface_sync =
      integration_shard1_sync.required_sync_count == 3u &&
      integration_shard1_sync.passed_sync_count ==
          integration_shard1_sync.required_sync_count &&
      integration_shard1_sync.failed_sync_count == 0u;
  sync.shard_surface_sync =
      sync.advanced_integration_shard1_ready &&
      sync.pass_manager_contract_surface_sync;
  sync.required_sync_count = 3u;
  sync.passed_sync_count =
      static_cast<std::size_t>(sync.advanced_integration_shard1_ready) +
      static_cast<std::size_t>(sync.pass_manager_contract_surface_sync) +
      static_cast<std::size_t>(sync.shard_surface_sync);
  sync.failed_sync_count =
      sync.required_sync_count >= sync.passed_sync_count
          ? (sync.required_sync_count - sync.passed_sync_count)
          : sync.required_sync_count;
  sync.deterministic =
      sync.required_sync_count == 3u &&
      sync.passed_sync_count == sync.required_sync_count &&
      sync.failed_sync_count == 0u;
  return sync;
}

inline Objc3ParserSemaAdvancedCoreShard2
BuildObjc3ParserSemaAdvancedCoreShard2(
    const Objc3ParserSemaAdvancedPerformanceShard1 &performance_shard1_sync) {
  Objc3ParserSemaAdvancedCoreShard2 sync;
  sync.advanced_performance_shard1_ready = performance_shard1_sync.deterministic;
  sync.pass_manager_contract_surface_sync =
      performance_shard1_sync.required_sync_count == 3u &&
      performance_shard1_sync.passed_sync_count ==
          performance_shard1_sync.required_sync_count &&
      performance_shard1_sync.failed_sync_count == 0u;
  sync.shard_surface_sync =
      sync.advanced_performance_shard1_ready &&
      sync.pass_manager_contract_surface_sync;
  sync.required_sync_count = 3u;
  sync.passed_sync_count =
      static_cast<std::size_t>(sync.advanced_performance_shard1_ready) +
      static_cast<std::size_t>(sync.pass_manager_contract_surface_sync) +
      static_cast<std::size_t>(sync.shard_surface_sync);
  sync.failed_sync_count =
      sync.required_sync_count >= sync.passed_sync_count
          ? (sync.required_sync_count - sync.passed_sync_count)
          : sync.required_sync_count;
  sync.deterministic =
      sync.required_sync_count == 3u &&
      sync.passed_sync_count == sync.required_sync_count &&
      sync.failed_sync_count == 0u;
  return sync;
}

inline Objc3ParserSemaAdvancedContractRejectionShard2
BuildObjc3ParserSemaAdvancedContractRejectionShard2(
    const Objc3ParserSemaAdvancedCoreShard2 &core_shard2_sync) {
  Objc3ParserSemaAdvancedContractRejectionShard2 sync;
  sync.advanced_core_shard2_ready = core_shard2_sync.deterministic;
  sync.pass_manager_contract_surface_sync =
      core_shard2_sync.required_sync_count == 3u &&
      core_shard2_sync.passed_sync_count ==
          core_shard2_sync.required_sync_count &&
      core_shard2_sync.failed_sync_count == 0u;
  sync.shard_surface_sync =
      sync.advanced_core_shard2_ready &&
      sync.pass_manager_contract_surface_sync;
  sync.required_sync_count = 3u;
  sync.passed_sync_count =
      static_cast<std::size_t>(sync.advanced_core_shard2_ready) +
      static_cast<std::size_t>(sync.pass_manager_contract_surface_sync) +
      static_cast<std::size_t>(sync.shard_surface_sync);
  sync.failed_sync_count =
      sync.required_sync_count >= sync.passed_sync_count
          ? (sync.required_sync_count - sync.passed_sync_count)
          : sync.required_sync_count;
  sync.deterministic =
      sync.required_sync_count == 3u &&
      sync.passed_sync_count == sync.required_sync_count &&
      sync.failed_sync_count == 0u;
  return sync;
}

inline Objc3ParserSemaAdvancedDiagnosticsShard2
BuildObjc3ParserSemaAdvancedDiagnosticsShard2(
    const Objc3ParserSemaAdvancedContractRejectionShard2 &contract_rejection_shard2_sync) {
  Objc3ParserSemaAdvancedDiagnosticsShard2 sync;
  sync.advanced_contract_rejection_shard2_ready = contract_rejection_shard2_sync.deterministic;
  sync.pass_manager_contract_surface_sync =
      contract_rejection_shard2_sync.required_sync_count == 3u &&
      contract_rejection_shard2_sync.passed_sync_count ==
          contract_rejection_shard2_sync.required_sync_count &&
      contract_rejection_shard2_sync.failed_sync_count == 0u;
  sync.shard_surface_sync =
      sync.advanced_contract_rejection_shard2_ready &&
      sync.pass_manager_contract_surface_sync;
  sync.required_sync_count = 3u;
  sync.passed_sync_count =
      static_cast<std::size_t>(sync.advanced_contract_rejection_shard2_ready) +
      static_cast<std::size_t>(sync.pass_manager_contract_surface_sync) +
      static_cast<std::size_t>(sync.shard_surface_sync);
  sync.failed_sync_count =
      sync.required_sync_count >= sync.passed_sync_count
          ? (sync.required_sync_count - sync.passed_sync_count)
          : sync.required_sync_count;
  sync.deterministic =
      sync.required_sync_count == 3u &&
      sync.passed_sync_count == sync.required_sync_count &&
      sync.failed_sync_count == 0u;
  return sync;
}

inline Objc3ParserSemaIntegrationCloseoutSignoff
BuildObjc3ParserSemaIntegrationCloseoutSignoff(
    const Objc3ParserSemaAdvancedDiagnosticsShard2 &diagnostics_shard2_sync) {
  Objc3ParserSemaIntegrationCloseoutSignoff sync;
  sync.advanced_diagnostics_shard2_ready = diagnostics_shard2_sync.deterministic;
  sync.pass_manager_contract_surface_sync =
      diagnostics_shard2_sync.required_sync_count == 3u &&
      diagnostics_shard2_sync.passed_sync_count ==
          diagnostics_shard2_sync.required_sync_count &&
      diagnostics_shard2_sync.failed_sync_count == 0u;
  sync.gate_signoff_surface_sync =
      sync.advanced_diagnostics_shard2_ready &&
      sync.pass_manager_contract_surface_sync;
  sync.required_sync_count = 3u;
  sync.passed_sync_count =
      static_cast<std::size_t>(sync.advanced_diagnostics_shard2_ready) +
      static_cast<std::size_t>(sync.pass_manager_contract_surface_sync) +
      static_cast<std::size_t>(sync.gate_signoff_surface_sync);
  sync.failed_sync_count =
      sync.required_sync_count >= sync.passed_sync_count
          ? (sync.required_sync_count - sync.passed_sync_count)
          : sync.required_sync_count;
  sync.deterministic =
      sync.required_sync_count == 3u &&
      sync.passed_sync_count == sync.required_sync_count &&
      sync.failed_sync_count == 0u;
  return sync;
}

struct Objc3ParserSemaHandoffScaffold {
  const Objc3ParsedProgram *program = nullptr;
  Objc3ParserSemaHandoffOwnerRecord owner_record;
  Objc3SemanticValidationOptions validation_options;
  Objc3SemaLanguageProfile language_profile = Objc3SemaLanguageProfile::Canonical;
  Objc3SemaCanonicalLiteralRejectionCounts
      canonical_literal_rejection_counts;
  Objc3ParserCanonicalLiteralRejectionHandoff
      canonical_literal_rejection_handoff;
  Objc3SemaDiagnosticsBus diagnostics_bus;
  Objc3ParserContractSnapshot parser_contract_snapshot;
  bool parser_contract_snapshot_normalization_candidate_detected = false;
  bool parser_contract_snapshot_normalization_applied = false;
  bool parser_contract_snapshot_normalization_rejected = false;
  std::uint64_t expected_ast_shape_fingerprint = 0;
  bool parser_contract_ast_shape_fingerprint_matches = false;
  std::uint64_t expected_ast_top_level_layout_fingerprint = 0;
  bool parser_contract_ast_top_level_layout_fingerprint_matches = false;
  std::uint64_t expected_parser_contract_snapshot_fingerprint = 0;
  std::uint64_t parser_contract_snapshot_fingerprint = 0;
  bool parser_contract_snapshot_fingerprint_matches = false;
  Objc3ParserSemaConformanceMatrix parser_sema_conformance_matrix;
  Objc3ParserSemaConformanceCorpus parser_sema_conformance_corpus;
  Objc3ParserSemaConformanceEvidenceRecord
      parser_sema_conformance_evidence_record;
  bool deterministic_parser_sema_conformance_evidence_record = false;
  Objc3ParserSemaPerformanceQualityGuardrails parser_sema_performance_quality_guardrails;
  Objc3ParserSemaCrossLaneIntegrationSync parser_sema_cross_lane_integration_sync;
  Objc3ParserSemaDocsRunbookSync parser_sema_docs_runbook_sync;
  Objc3ParserSemaReleaseCandidateReplayDryRun parser_sema_release_candidate_replay_dry_run;
  Objc3ParserSemaAdvancedCoreShard1 parser_sema_advanced_core_shard1;
  Objc3ParserSemaAdvancedContractRejectionShard1 parser_sema_advanced_contract_rejection_shard1;
  Objc3ParserSemaAdvancedDiagnosticsShard1 parser_sema_advanced_diagnostics_shard1;
  Objc3ParserSemaAdvancedConformanceShard1 parser_sema_advanced_conformance_shard1;
  Objc3ParserSemaAdvancedIntegrationShard1 parser_sema_advanced_integration_shard1;
  Objc3ParserSemaAdvancedPerformanceShard1 parser_sema_advanced_performance_shard1;
  Objc3ParserSemaAdvancedCoreShard2 parser_sema_advanced_core_shard2;
  Objc3ParserSemaAdvancedContractRejectionShard2 parser_sema_advanced_contract_rejection_shard2;
  Objc3ParserSemaAdvancedDiagnosticsShard2 parser_sema_advanced_diagnostics_shard2;
  Objc3ParserSemaIntegrationCloseoutSignoff parser_sema_integration_closeout_signoff;
  Objc3ParserSemaContractReadinessRecord parser_sema_contract_readiness_record;
  bool deterministic_parser_sema_contract_readiness_record = false;
  Objc3ParserSemaHandoffScaffoldReadinessRecord readiness_record;
  bool readiness_record_deterministic = false;
  bool parser_contract_snapshot_matches_program = false;
  bool deterministic = false;
};

inline Objc3ParserSemaConformanceEvidenceRecord
BuildObjc3ParserSemaHandoffScaffoldConformanceEvidenceRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3ParserSemaHandoffScaffold &scaffold) {
  Objc3SemaParityContractSurface surface;
  surface.parser_sema_conformance_matrix =
      scaffold.parser_sema_conformance_matrix;
  surface.parser_sema_conformance_corpus =
      scaffold.parser_sema_conformance_corpus;
  surface.deterministic_parser_sema_conformance_matrix =
      scaffold.parser_sema_conformance_matrix.deterministic;
  surface.deterministic_parser_sema_conformance_corpus =
      scaffold.parser_sema_conformance_corpus.deterministic;
  return BuildObjc3ParserSemaConformanceEvidenceRecord(input, surface);
}

inline Objc3SemaParityContractSurface
BuildObjc3ParserSemaHandoffScaffoldContractSurface(
    const Objc3ParserSemaHandoffScaffold &scaffold) {
  Objc3SemaParityContractSurface surface;
  surface.parser_sema_conformance_matrix =
      scaffold.parser_sema_conformance_matrix;
  surface.parser_sema_conformance_corpus =
      scaffold.parser_sema_conformance_corpus;
  surface.parser_sema_conformance_evidence_record =
      scaffold.parser_sema_conformance_evidence_record;
  surface.parser_sema_performance_quality_guardrails =
      scaffold.parser_sema_performance_quality_guardrails;
  surface.parser_sema_cross_lane_integration_sync =
      scaffold.parser_sema_cross_lane_integration_sync;
  surface.parser_sema_docs_runbook_sync =
      scaffold.parser_sema_docs_runbook_sync;
  surface.parser_sema_release_candidate_replay_dry_run =
      scaffold.parser_sema_release_candidate_replay_dry_run;
  surface.parser_sema_advanced_core_shard1 =
      scaffold.parser_sema_advanced_core_shard1;
  surface.parser_sema_advanced_contract_rejection_shard1 =
      scaffold.parser_sema_advanced_contract_rejection_shard1;
  surface.parser_sema_advanced_diagnostics_shard1 =
      scaffold.parser_sema_advanced_diagnostics_shard1;
  surface.parser_sema_advanced_conformance_shard1 =
      scaffold.parser_sema_advanced_conformance_shard1;
  surface.parser_sema_advanced_integration_shard1 =
      scaffold.parser_sema_advanced_integration_shard1;
  surface.parser_sema_advanced_performance_shard1 =
      scaffold.parser_sema_advanced_performance_shard1;
  surface.parser_sema_advanced_core_shard2 =
      scaffold.parser_sema_advanced_core_shard2;
  surface.parser_sema_advanced_contract_rejection_shard2 =
      scaffold.parser_sema_advanced_contract_rejection_shard2;
  surface.parser_sema_advanced_diagnostics_shard2 =
      scaffold.parser_sema_advanced_diagnostics_shard2;
  surface.parser_sema_integration_closeout_signoff =
      scaffold.parser_sema_integration_closeout_signoff;
  surface.deterministic_parser_sema_conformance_matrix =
      scaffold.parser_sema_conformance_matrix.deterministic;
  surface.deterministic_parser_sema_conformance_corpus =
      scaffold.parser_sema_conformance_corpus.deterministic;
  surface.deterministic_parser_sema_conformance_evidence_record =
      scaffold.deterministic_parser_sema_conformance_evidence_record;
  surface.deterministic_parser_sema_performance_quality_guardrails =
      scaffold.parser_sema_performance_quality_guardrails.deterministic;
  surface.deterministic_parser_sema_cross_lane_integration_sync =
      scaffold.parser_sema_cross_lane_integration_sync.deterministic;
  surface.deterministic_parser_sema_docs_runbook_sync =
      scaffold.parser_sema_docs_runbook_sync.deterministic;
  surface.deterministic_parser_sema_release_candidate_replay_dry_run =
      scaffold.parser_sema_release_candidate_replay_dry_run.deterministic;
  surface.deterministic_parser_sema_advanced_core_shard1 =
      scaffold.parser_sema_advanced_core_shard1.deterministic;
  surface.deterministic_parser_sema_advanced_contract_rejection_shard1 =
      scaffold.parser_sema_advanced_contract_rejection_shard1.deterministic;
  surface.deterministic_parser_sema_advanced_diagnostics_shard1 =
      scaffold.parser_sema_advanced_diagnostics_shard1.deterministic;
  surface.deterministic_parser_sema_advanced_conformance_shard1 =
      scaffold.parser_sema_advanced_conformance_shard1.deterministic;
  surface.deterministic_parser_sema_advanced_integration_shard1 =
      scaffold.parser_sema_advanced_integration_shard1.deterministic;
  surface.deterministic_parser_sema_advanced_performance_shard1 =
      scaffold.parser_sema_advanced_performance_shard1.deterministic;
  surface.deterministic_parser_sema_advanced_core_shard2 =
      scaffold.parser_sema_advanced_core_shard2.deterministic;
  surface.deterministic_parser_sema_advanced_contract_rejection_shard2 =
      scaffold.parser_sema_advanced_contract_rejection_shard2.deterministic;
  surface.deterministic_parser_sema_advanced_diagnostics_shard2 =
      scaffold.parser_sema_advanced_diagnostics_shard2.deterministic;
  surface.deterministic_parser_sema_integration_closeout_signoff =
      scaffold.parser_sema_integration_closeout_signoff.deterministic;
  return surface;
}

inline Objc3ParserSemaHandoffScaffoldReadinessRecord
BuildObjc3ParserSemaHandoffScaffoldReadinessRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3ParserSemaHandoffScaffold &scaffold) {
  Objc3ParserSemaHandoffScaffoldReadinessRecord record;
  record.stage_input_owner = input.stage_input_owner;
  record.parser_sema_contract_handoff_owner =
      scaffold.owner_record.parser_sema_contract_handoff_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.owner_record_ready =
      IsReadyObjc3ParserSemaHandoffOwnerRecord(scaffold.owner_record);
  record.snapshot_evidence_ready =
      scaffold.parser_contract_ast_shape_fingerprint_matches &&
      scaffold.parser_contract_ast_top_level_layout_fingerprint_matches &&
      scaffold.parser_contract_snapshot_fingerprint_matches &&
      scaffold.parser_contract_snapshot_matches_program;
  record.snapshot_normalization_ready =
      !scaffold.parser_contract_snapshot_normalization_rejected;
  record.canonical_rejection_ready =
      scaffold.canonical_literal_rejection_handoff.deterministic;
  record.conformance_evidence_ready =
      scaffold.deterministic_parser_sema_conformance_evidence_record &&
      IsReadyObjc3ParserSemaConformanceEvidenceRecord(
          scaffold.parser_sema_conformance_evidence_record);
  record.parser_contract_readiness_ready =
      scaffold.deterministic_parser_sema_contract_readiness_record &&
      IsReadyObjc3ParserSemaContractReadinessRecord(
          scaffold.parser_sema_contract_readiness_record);
  record.deterministic =
      Objc3SemaOwnerIsExplicit(record.handoff_scaffold_readiness_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_conformance_evidence_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_readiness_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.owner_record_ready && record.snapshot_evidence_ready &&
      record.snapshot_normalization_ready &&
      record.canonical_rejection_ready &&
      record.conformance_evidence_ready &&
      record.parser_contract_readiness_ready;
  return record;
}

inline Objc3ParserSemaHandoffScaffold BuildObjc3ParserSemaHandoffScaffold(const Objc3SemaPassManagerInput &input) {
  Objc3ParserSemaHandoffScaffold scaffold;
  scaffold.program = input.program;
  scaffold.owner_record = BuildObjc3ParserSemaHandoffOwnerRecord(input);
  scaffold.validation_options = input.validation_options;
  scaffold.language_profile = input.language_profile;
  scaffold.canonical_literal_rejection_counts =
      input.canonical_literal_rejection_counts;
  scaffold.diagnostics_bus = input.diagnostics_bus;
  if (input.program == nullptr) {
    return scaffold;
  }

  const Objc3ParserContractSnapshot resolved_snapshot = ResolveObjc3ParserContractSnapshotForSemaHandoff(input);
  scaffold.parser_contract_snapshot_normalization_candidate_detected =
      IsObjc3ParserContractSnapshotNormalizationCandidate(
          resolved_snapshot, *input.program);
  scaffold.parser_contract_snapshot_normalization_rejected =
      IsObjc3ParserContractSnapshotNormalizationRejectedForSemaHandoff(
          resolved_snapshot, *input.program, input.language_profile);
  scaffold.parser_contract_snapshot = NormalizeObjc3ParserContractSnapshotForSemaHandoff(
      resolved_snapshot,
      *input.program,
      input.language_profile,
      scaffold.parser_contract_snapshot_normalization_applied);
  scaffold.canonical_literal_rejection_handoff =
      BuildObjc3ParserCanonicalLiteralRejectionHandoff(
          scaffold.parser_contract_snapshot,
          scaffold.canonical_literal_rejection_counts);
  scaffold.expected_ast_shape_fingerprint = BuildObjc3ParsedProgramAstShapeFingerprint(*input.program);
  scaffold.parser_contract_ast_shape_fingerprint_matches =
      scaffold.parser_contract_snapshot.ast_shape_fingerprint == scaffold.expected_ast_shape_fingerprint;
  scaffold.expected_ast_top_level_layout_fingerprint = BuildObjc3ParsedProgramTopLevelLayoutFingerprint(*input.program);
  scaffold.parser_contract_ast_top_level_layout_fingerprint_matches =
      scaffold.parser_contract_snapshot.ast_top_level_layout_fingerprint ==
      scaffold.expected_ast_top_level_layout_fingerprint;
  const Objc3ParserContractSnapshot expected_snapshot = BuildObjc3ParserContractSnapshot(
      *input.program,
      scaffold.parser_contract_snapshot.parser_diagnostic_count,
      scaffold.parser_contract_snapshot.token_count);
  scaffold.parser_contract_snapshot_fingerprint =
      BuildObjc3ParserContractSnapshotFingerprint(scaffold.parser_contract_snapshot);
  scaffold.expected_parser_contract_snapshot_fingerprint =
      BuildObjc3ParserContractSnapshotFingerprint(expected_snapshot);
  scaffold.parser_contract_snapshot_fingerprint_matches =
      scaffold.parser_contract_snapshot_fingerprint == scaffold.expected_parser_contract_snapshot_fingerprint;
  scaffold.parser_sema_conformance_matrix = BuildObjc3ParserSemaConformanceMatrix(
      scaffold.parser_contract_snapshot, *input.program);
  scaffold.parser_sema_conformance_corpus =
      BuildObjc3ParserSemaConformanceCorpus(scaffold.parser_sema_conformance_matrix);
  scaffold.parser_sema_performance_quality_guardrails =
      BuildObjc3ParserSemaPerformanceQualityGuardrails(
          scaffold.parser_sema_conformance_matrix,
          scaffold.parser_sema_conformance_corpus);
  scaffold.parser_sema_cross_lane_integration_sync =
      BuildObjc3ParserSemaCrossLaneIntegrationSync(
          scaffold.parser_sema_conformance_matrix,
          scaffold.parser_sema_conformance_corpus,
          scaffold.parser_sema_performance_quality_guardrails);
  scaffold.parser_sema_docs_runbook_sync =
      BuildObjc3ParserSemaDocsRunbookSync(
          scaffold.parser_sema_cross_lane_integration_sync);
  scaffold.parser_sema_release_candidate_replay_dry_run =
      BuildObjc3ParserSemaReleaseCandidateReplayDryRun(
          scaffold.parser_sema_docs_runbook_sync);
  scaffold.parser_sema_advanced_core_shard1 =
      BuildObjc3ParserSemaAdvancedCoreShard1(
          scaffold.parser_sema_release_candidate_replay_dry_run);
  scaffold.parser_sema_advanced_contract_rejection_shard1 =
      BuildObjc3ParserSemaAdvancedContractRejectionShard1(
          scaffold.parser_sema_advanced_core_shard1);
  scaffold.parser_sema_advanced_diagnostics_shard1 =
      BuildObjc3ParserSemaAdvancedDiagnosticsShard1(
          scaffold.parser_sema_advanced_contract_rejection_shard1);
  scaffold.parser_sema_advanced_conformance_shard1 =
      BuildObjc3ParserSemaAdvancedConformanceShard1(
          scaffold.parser_sema_advanced_diagnostics_shard1);
  scaffold.parser_sema_advanced_integration_shard1 =
      BuildObjc3ParserSemaAdvancedIntegrationShard1(
          scaffold.parser_sema_advanced_conformance_shard1);
  scaffold.parser_sema_advanced_performance_shard1 =
      BuildObjc3ParserSemaAdvancedPerformanceShard1(
          scaffold.parser_sema_advanced_integration_shard1);
  scaffold.parser_sema_advanced_core_shard2 =
      BuildObjc3ParserSemaAdvancedCoreShard2(
          scaffold.parser_sema_advanced_performance_shard1);
  scaffold.parser_sema_advanced_contract_rejection_shard2 =
      BuildObjc3ParserSemaAdvancedContractRejectionShard2(
          scaffold.parser_sema_advanced_core_shard2);
  scaffold.parser_sema_advanced_diagnostics_shard2 =
      BuildObjc3ParserSemaAdvancedDiagnosticsShard2(
          scaffold.parser_sema_advanced_contract_rejection_shard2);
  scaffold.parser_sema_integration_closeout_signoff =
      BuildObjc3ParserSemaIntegrationCloseoutSignoff(
          scaffold.parser_sema_advanced_diagnostics_shard2);
  scaffold.parser_sema_conformance_evidence_record =
      BuildObjc3ParserSemaHandoffScaffoldConformanceEvidenceRecord(
          input, scaffold);
  scaffold.deterministic_parser_sema_conformance_evidence_record =
      IsReadyObjc3ParserSemaConformanceEvidenceRecord(
          scaffold.parser_sema_conformance_evidence_record);
  scaffold.parser_sema_contract_readiness_record =
      BuildObjc3ParserSemaContractReadinessRecord(
          input,
          BuildObjc3ParserSemaHandoffScaffoldContractSurface(scaffold));
  scaffold.deterministic_parser_sema_contract_readiness_record =
      IsReadyObjc3ParserSemaContractReadinessRecord(
          scaffold.parser_sema_contract_readiness_record);
  scaffold.parser_contract_snapshot_matches_program =
      scaffold.deterministic_parser_sema_conformance_evidence_record &&
      scaffold.parser_sema_conformance_evidence_record
          .conformance_matrix_deterministic;
  scaffold.readiness_record =
      BuildObjc3ParserSemaHandoffScaffoldReadinessRecord(input, scaffold);
  scaffold.readiness_record_deterministic =
      IsReadyObjc3ParserSemaHandoffScaffoldReadinessRecord(
          scaffold.readiness_record);
  scaffold.deterministic = scaffold.readiness_record_deterministic;
  return scaffold;
}
