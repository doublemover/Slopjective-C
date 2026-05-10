#pragma once

#include <cstddef>

#include "sema/objc3_parser_sema_handoff_contract.h"

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
