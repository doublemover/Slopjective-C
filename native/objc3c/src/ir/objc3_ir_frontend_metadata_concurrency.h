#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendConcurrencyMetadata {
  std::string lowering_async_continuation_replay_key;
  std::size_t async_continuation_lowering_sites = 0;
  std::size_t async_continuation_lowering_async_keyword_sites = 0;
  std::size_t async_continuation_lowering_async_function_sites = 0;
  std::size_t async_continuation_lowering_continuation_allocation_sites = 0;
  std::size_t async_continuation_lowering_continuation_resume_sites = 0;
  std::size_t async_continuation_lowering_continuation_suspend_sites = 0;
  std::size_t async_continuation_lowering_async_state_machine_sites = 0;
  std::size_t async_continuation_lowering_normalized_sites = 0;
  std::size_t async_continuation_lowering_gate_blocked_sites = 0;
  std::size_t async_continuation_lowering_contract_violation_sites = 0;
  bool deterministic_async_continuation_lowering_handoff = false;
  std::string lowering_await_lowering_suspension_state_replay_key;
  std::size_t await_lowering_suspension_state_lowering_sites = 0;
  std::size_t await_lowering_suspension_state_lowering_await_keyword_sites = 0;
  std::size_t await_lowering_suspension_state_lowering_await_suspension_point_sites = 0;
  std::size_t await_lowering_suspension_state_lowering_await_resume_sites = 0;
  std::size_t await_lowering_suspension_state_lowering_await_state_machine_sites = 0;
  std::size_t await_lowering_suspension_state_lowering_await_continuation_sites = 0;
  std::size_t await_lowering_suspension_state_lowering_normalized_sites = 0;
  std::size_t await_lowering_suspension_state_lowering_gate_blocked_sites = 0;
  std::size_t await_lowering_suspension_state_lowering_contract_violation_sites = 0;
  bool deterministic_await_lowering_suspension_state_lowering_handoff = false;
  std::string lowering_actor_isolation_sendability_replay_key;
  std::size_t actor_isolation_sendability_lowering_sites = 0;
  std::size_t actor_isolation_sendability_lowering_sendability_check_sites = 0;
  std::size_t actor_isolation_sendability_lowering_cross_actor_hop_sites = 0;
  std::size_t actor_isolation_sendability_lowering_non_sendable_capture_sites = 0;
  std::size_t actor_isolation_sendability_lowering_sendable_transfer_sites = 0;
  std::size_t actor_isolation_sendability_lowering_isolation_boundary_sites = 0;
  std::size_t actor_isolation_sendability_lowering_guard_blocked_sites = 0;
  std::size_t actor_isolation_sendability_lowering_contract_violation_sites = 0;
  bool deterministic_actor_isolation_sendability_lowering_handoff = false;
  std::string lowering_actor_lowering_metadata_replay_key;
  std::size_t actor_lowering_metadata_actor_interface_sites = 0;
  std::size_t actor_lowering_metadata_actor_method_sites = 0;
  std::size_t actor_lowering_metadata_actor_metadata_record_sites = 0;
  std::size_t actor_lowering_metadata_nonisolated_entry_sites = 0;
  std::size_t actor_lowering_metadata_executor_affinity_sites = 0;
  std::size_t actor_lowering_metadata_actor_hop_artifact_sites = 0;
  std::size_t actor_lowering_metadata_actor_isolation_thunk_sites = 0;
  std::size_t actor_lowering_metadata_replay_proof_dependency_sites = 0;
  std::size_t actor_lowering_metadata_race_guard_dependency_sites = 0;
  std::size_t actor_lowering_metadata_task_handoff_sites = 0;
  std::size_t actor_lowering_metadata_guard_blocked_sites = 0;
  std::size_t actor_lowering_metadata_contract_violation_sites = 0;
  bool deterministic_actor_lowering_metadata_handoff = false;
};
