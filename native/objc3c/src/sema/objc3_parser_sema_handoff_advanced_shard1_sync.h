#pragma once

#include <cstddef>

#include "sema/objc3_parser_sema_handoff_contract.h"

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
