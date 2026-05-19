#pragma once

#include <cstddef>
#include <string>

struct Objc3IRFrontendTaskRuntimeSupportMetadata {
  std::string lowering_task_runtime_interop_cancellation_replay_key;
  std::size_t task_runtime_interop_cancellation_lowering_sites = 0;
  std::size_t task_runtime_interop_cancellation_lowering_runtime_interop_sites = 0;
  std::size_t task_runtime_interop_cancellation_lowering_cancellation_probe_sites = 0;
  std::size_t task_runtime_interop_cancellation_lowering_cancellation_handler_sites = 0;
  std::size_t task_runtime_interop_cancellation_lowering_runtime_resume_sites = 0;
  std::size_t task_runtime_interop_cancellation_lowering_runtime_cancel_sites = 0;
  std::size_t task_runtime_interop_cancellation_lowering_normalized_sites = 0;
  std::size_t task_runtime_interop_cancellation_lowering_guard_blocked_sites = 0;
  std::size_t task_runtime_interop_cancellation_lowering_contract_violation_sites = 0;
  bool deterministic_task_runtime_interop_cancellation_lowering_handoff = false;
  std::string lowering_concurrency_replay_race_guard_replay_key;
  std::size_t concurrency_replay_race_guard_lowering_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_replay_proof_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_race_guard_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_task_handoff_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_actor_isolation_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_deterministic_schedule_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_guard_blocked_sites = 0;
  std::size_t concurrency_replay_race_guard_lowering_contract_violation_sites = 0;
  bool deterministic_concurrency_replay_race_guard_lowering_handoff = false;
};
