#include "artifacts/objc3_frontend_artifact_concurrency_runtime_metadata.h"

#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/contracts/concurrency_actor_contracts.h"
#include "lower/contracts/concurrency_task_lowering_contracts.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendConcurrencyRuntimeMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string
        &concurrency_task_runtime_interop_cancellation_lowering_replay_key,
    const Objc3TaskRuntimeInteropCancellationLoweringContract
        &concurrency_task_runtime_interop_cancellation_lowering_contract,
    const std::string &concurrency_replay_race_guard_lowering_replay_key,
    const Objc3ConcurrencyReplayRaceGuardLoweringContract
        &concurrency_replay_race_guard_lowering_contract) {
  ir_frontend_metadata.lowering_task_runtime_interop_cancellation_replay_key =
      concurrency_task_runtime_interop_cancellation_lowering_replay_key;
  ir_frontend_metadata.task_runtime_interop_cancellation_lowering_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .task_runtime_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_runtime_interop_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .task_runtime_interop_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_cancellation_probe_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .cancellation_probe_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_cancellation_handler_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .cancellation_handler_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_runtime_resume_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .runtime_resume_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_runtime_cancel_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .runtime_cancel_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_normalized_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .normalized_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_guard_blocked_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .guard_blocked_sites;
  ir_frontend_metadata
      .task_runtime_interop_cancellation_lowering_contract_violation_sites =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .contract_violation_sites;
  ir_frontend_metadata
      .deterministic_task_runtime_interop_cancellation_lowering_handoff =
      concurrency_task_runtime_interop_cancellation_lowering_contract
          .deterministic;

  ir_frontend_metadata.lowering_concurrency_replay_race_guard_replay_key =
      concurrency_replay_race_guard_lowering_replay_key;
  // implementation anchor: the runnable actor pipeline carries the strict
  // concurrency replay/race-guard packet beside the actor lowering contract.
  ir_frontend_metadata.concurrency_replay_race_guard_lowering_sites =
      concurrency_replay_race_guard_lowering_contract.concurrency_replay_sites;
  ir_frontend_metadata
      .concurrency_replay_race_guard_lowering_replay_proof_sites =
      concurrency_replay_race_guard_lowering_contract.replay_proof_sites;
  ir_frontend_metadata.concurrency_replay_race_guard_lowering_race_guard_sites =
      concurrency_replay_race_guard_lowering_contract.race_guard_sites;
  ir_frontend_metadata.concurrency_replay_race_guard_lowering_task_handoff_sites =
      concurrency_replay_race_guard_lowering_contract.task_handoff_sites;
  ir_frontend_metadata
      .concurrency_replay_race_guard_lowering_actor_isolation_sites =
      concurrency_replay_race_guard_lowering_contract.actor_isolation_sites;
  ir_frontend_metadata
      .concurrency_replay_race_guard_lowering_deterministic_schedule_sites =
      concurrency_replay_race_guard_lowering_contract
          .deterministic_schedule_sites;
  ir_frontend_metadata.concurrency_replay_race_guard_lowering_guard_blocked_sites =
      concurrency_replay_race_guard_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata
      .concurrency_replay_race_guard_lowering_contract_violation_sites =
      concurrency_replay_race_guard_lowering_contract.contract_violation_sites;
  ir_frontend_metadata
      .deterministic_concurrency_replay_race_guard_lowering_handoff =
      concurrency_replay_race_guard_lowering_contract.deterministic;
}

}  // namespace objc3::artifacts::frontend
