#pragma once

#include <cstddef>

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
