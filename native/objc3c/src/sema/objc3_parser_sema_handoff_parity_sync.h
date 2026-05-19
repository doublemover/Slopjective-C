#pragma once

#include <cstddef>

#include "sema/objc3_parser_sema_handoff_contract.h"

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
