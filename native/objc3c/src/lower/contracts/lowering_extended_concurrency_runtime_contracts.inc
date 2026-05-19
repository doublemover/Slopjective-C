bool IsValidObjc3TaskRuntimeInteropCancellationLoweringContract(
    const Objc3TaskRuntimeInteropCancellationLoweringContract &contract) {
  if (contract.task_runtime_interop_sites > contract.task_runtime_sites ||
      contract.cancellation_probe_sites > contract.task_runtime_sites ||
      contract.cancellation_handler_sites > contract.task_runtime_sites ||
      contract.runtime_resume_sites > contract.task_runtime_sites ||
      contract.runtime_cancel_sites > contract.task_runtime_sites ||
      contract.normalized_sites > contract.task_runtime_sites ||
      contract.guard_blocked_sites > contract.task_runtime_sites ||
      contract.contract_violation_sites > contract.task_runtime_sites) {
    return false;
  }
  if (contract.runtime_resume_sites + contract.runtime_cancel_sites >
      contract.task_runtime_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.guard_blocked_sites !=
      contract.task_runtime_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3TaskRuntimeInteropCancellationLoweringReplayKey(
    const Objc3TaskRuntimeInteropCancellationLoweringContract &contract) {
  return std::string("task_runtime_sites=") +
             std::to_string(contract.task_runtime_sites) +
         ";task_runtime_interop_sites=" +
         std::to_string(contract.task_runtime_interop_sites) +
         ";cancellation_probe_sites=" +
         std::to_string(contract.cancellation_probe_sites) +
         ";cancellation_handler_sites=" +
         std::to_string(contract.cancellation_handler_sites) +
         ";runtime_resume_sites=" +
         std::to_string(contract.runtime_resume_sites) +
         ";runtime_cancel_sites=" +
         std::to_string(contract.runtime_cancel_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";guard_blocked_sites=" + std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3TaskRuntimeInteropCancellationLoweringLaneContract;
}

bool IsValidObjc3ConcurrencyReplayRaceGuardLoweringContract(
    const Objc3ConcurrencyReplayRaceGuardLoweringContract &contract) {
  if (contract.replay_proof_sites > contract.concurrency_replay_sites ||
      contract.race_guard_sites > contract.concurrency_replay_sites ||
      contract.task_handoff_sites > contract.concurrency_replay_sites ||
      contract.actor_isolation_sites > contract.concurrency_replay_sites ||
      contract.deterministic_schedule_sites > contract.concurrency_replay_sites ||
      contract.guard_blocked_sites > contract.concurrency_replay_sites ||
      contract.contract_violation_sites > contract.concurrency_replay_sites) {
    return false;
  }
  if (contract.deterministic_schedule_sites + contract.guard_blocked_sites !=
      contract.concurrency_replay_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ConcurrencyReplayRaceGuardLoweringReplayKey(
    const Objc3ConcurrencyReplayRaceGuardLoweringContract &contract) {
  return std::string("concurrency_replay_sites=") +
             std::to_string(contract.concurrency_replay_sites) +
         ";replay_proof_sites=" + std::to_string(contract.replay_proof_sites) +
         ";race_guard_sites=" + std::to_string(contract.race_guard_sites) +
         ";task_handoff_sites=" + std::to_string(contract.task_handoff_sites) +
         ";actor_isolation_sites=" +
         std::to_string(contract.actor_isolation_sites) +
         ";deterministic_schedule_sites=" +
         std::to_string(contract.deterministic_schedule_sites) +
         ";guard_blocked_sites=" +
         std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3ConcurrencyReplayRaceGuardLoweringLaneContract;
}
