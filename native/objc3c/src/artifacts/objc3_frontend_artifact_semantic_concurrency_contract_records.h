#pragma once

#include <cstddef>
#include <string>

inline constexpr const char *kObjc3ActorIsolationSendabilityLoweringLaneContract =
    "objc3c.actor.isolation.sendability.lowering.v1";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataLaneContract =
    "objc3c.actor.lowering.metadata.contract.v1";
inline constexpr const char *kObjc3ConcurrencyReplayRaceGuardLoweringLaneContract =
    "objc3c.concurrency.replay.race.guard.lowering.v1";

struct Objc3ActorIsolationSendabilityLoweringContract {
  std::size_t actor_isolation_sites = 0;
  std::size_t sendability_check_sites = 0;
  std::size_t cross_actor_hop_sites = 0;
  std::size_t non_sendable_capture_sites = 0;
  std::size_t sendable_transfer_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ActorLoweringMetadataContract {
  std::size_t actor_interface_sites = 0;
  std::size_t actor_method_sites = 0;
  std::size_t actor_metadata_record_sites = 0;
  std::size_t nonisolated_entry_sites = 0;
  std::size_t executor_affinity_sites = 0;
  std::size_t actor_hop_artifact_sites = 0;
  std::size_t actor_isolation_thunk_sites = 0;
  std::size_t replay_proof_dependency_sites = 0;
  std::size_t race_guard_dependency_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3ConcurrencyReplayRaceGuardLoweringContract {
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char *kObjc3AsyncContinuationLoweringLaneContract =
    "objc3c.async.continuation.lowering.v1";
inline constexpr const char
    *kObjc3AwaitLoweringSuspensionStateLoweringLaneContract =
        "objc3c.await.lowering.suspension.state.lowering.v1";

struct Objc3AsyncContinuationLoweringContract {
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3AwaitLoweringSuspensionStateLoweringContract {
  std::size_t await_suspension_sites = 0;
  std::size_t await_keyword_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t await_state_machine_sites = 0;
  std::size_t await_continuation_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t gate_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

inline constexpr const char *kObjc3TaskRuntimeInteropCancellationLoweringLaneContract =
    "objc3c.task.runtime.interop.cancellation.lowering.v1";

struct Objc3TaskRuntimeInteropCancellationLoweringContract {
  std::size_t task_runtime_sites = 0;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t cancellation_probe_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t runtime_resume_sites = 0;
  std::size_t runtime_cancel_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

bool IsValidObjc3ActorIsolationSendabilityLoweringContract(
    const Objc3ActorIsolationSendabilityLoweringContract &contract);
std::string Objc3ActorIsolationSendabilityLoweringReplayKey(
    const Objc3ActorIsolationSendabilityLoweringContract &contract);
bool IsValidObjc3ActorLoweringMetadataContract(
    const Objc3ActorLoweringMetadataContract &contract);
std::string Objc3ActorLoweringMetadataReplayKey(
    const Objc3ActorLoweringMetadataContract &contract);
bool IsValidObjc3ConcurrencyReplayRaceGuardLoweringContract(
    const Objc3ConcurrencyReplayRaceGuardLoweringContract &contract);
std::string Objc3ConcurrencyReplayRaceGuardLoweringReplayKey(
    const Objc3ConcurrencyReplayRaceGuardLoweringContract &contract);
bool IsValidObjc3AsyncContinuationLoweringContract(
    const Objc3AsyncContinuationLoweringContract &contract);
std::string Objc3AsyncContinuationLoweringReplayKey(
    const Objc3AsyncContinuationLoweringContract &contract);
bool IsValidObjc3AwaitLoweringSuspensionStateLoweringContract(
    const Objc3AwaitLoweringSuspensionStateLoweringContract &contract);
std::string Objc3AwaitLoweringSuspensionStateLoweringReplayKey(
    const Objc3AwaitLoweringSuspensionStateLoweringContract &contract);
bool IsValidObjc3TaskRuntimeInteropCancellationLoweringContract(
    const Objc3TaskRuntimeInteropCancellationLoweringContract &contract);
std::string Objc3TaskRuntimeInteropCancellationLoweringReplayKey(
    const Objc3TaskRuntimeInteropCancellationLoweringContract &contract);
