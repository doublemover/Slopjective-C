#include "lower/objc3_lowering_contract.h"

#include "lower/metadata/lowering_metadata_helpers.h"

#include <string>

bool IsValidObjc3AsyncContinuationLoweringContract(
    const Objc3AsyncContinuationLoweringContract &contract) {
  if (contract.async_keyword_sites > contract.async_continuation_sites ||
      contract.async_function_sites > contract.async_continuation_sites ||
      contract.continuation_allocation_sites > contract.async_continuation_sites ||
      contract.continuation_resume_sites > contract.async_continuation_sites ||
      contract.continuation_suspend_sites > contract.async_continuation_sites ||
      contract.async_state_machine_sites > contract.async_continuation_sites ||
      contract.normalized_sites > contract.async_continuation_sites ||
      contract.gate_blocked_sites > contract.async_continuation_sites ||
      contract.contract_violation_sites > contract.async_continuation_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.gate_blocked_sites !=
      contract.async_continuation_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3AsyncContinuationLoweringReplayKey(
    const Objc3AsyncContinuationLoweringContract &contract) {
  return std::string("async_continuation_sites=") +
             std::to_string(contract.async_continuation_sites) +
         ";async_keyword_sites=" + std::to_string(contract.async_keyword_sites) +
         ";async_function_sites=" + std::to_string(contract.async_function_sites) +
         ";continuation_allocation_sites=" +
         std::to_string(contract.continuation_allocation_sites) +
         ";continuation_resume_sites=" +
         std::to_string(contract.continuation_resume_sites) +
         ";continuation_suspend_sites=" +
         std::to_string(contract.continuation_suspend_sites) +
         ";async_state_machine_sites=" +
         std::to_string(contract.async_state_machine_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";gate_blocked_sites=" + std::to_string(contract.gate_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3AsyncContinuationLoweringLaneContract;
}

bool IsValidObjc3AwaitLoweringSuspensionStateLoweringContract(
    const Objc3AwaitLoweringSuspensionStateLoweringContract &contract) {
  if (contract.await_keyword_sites > contract.await_suspension_sites ||
      contract.await_suspension_point_sites > contract.await_suspension_sites ||
      contract.await_resume_sites > contract.await_suspension_point_sites ||
      contract.await_state_machine_sites > contract.await_suspension_point_sites ||
      contract.await_continuation_sites >
          contract.await_suspension_point_sites ||
      contract.normalized_sites > contract.await_suspension_sites ||
      contract.gate_blocked_sites > contract.await_suspension_sites ||
      contract.contract_violation_sites > contract.await_suspension_sites) {
    return false;
  }
  if (contract.normalized_sites + contract.gate_blocked_sites !=
      contract.await_suspension_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3AwaitLoweringSuspensionStateLoweringReplayKey(
    const Objc3AwaitLoweringSuspensionStateLoweringContract &contract) {
  return std::string("await_suspension_sites=") +
             std::to_string(contract.await_suspension_sites) +
         ";await_keyword_sites=" + std::to_string(contract.await_keyword_sites) +
         ";await_suspension_point_sites=" +
         std::to_string(contract.await_suspension_point_sites) +
         ";await_resume_sites=" + std::to_string(contract.await_resume_sites) +
         ";await_state_machine_sites=" +
         std::to_string(contract.await_state_machine_sites) +
         ";await_continuation_sites=" +
         std::to_string(contract.await_continuation_sites) +
         ";normalized_sites=" + std::to_string(contract.normalized_sites) +
         ";gate_blocked_sites=" + std::to_string(contract.gate_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3AwaitLoweringSuspensionStateLoweringLaneContract;
}

bool IsValidObjc3ActorIsolationSendabilityLoweringContract(
    const Objc3ActorIsolationSendabilityLoweringContract &contract) {
  if (contract.sendability_check_sites > contract.actor_isolation_sites ||
      contract.cross_actor_hop_sites > contract.actor_isolation_sites ||
      contract.non_sendable_capture_sites > contract.sendability_check_sites ||
      contract.sendable_transfer_sites > contract.sendability_check_sites ||
      contract.isolation_boundary_sites > contract.actor_isolation_sites ||
      contract.guard_blocked_sites > contract.actor_isolation_sites ||
      contract.contract_violation_sites > contract.actor_isolation_sites) {
    return false;
  }
  if (contract.isolation_boundary_sites + contract.guard_blocked_sites !=
      contract.actor_isolation_sites) {
    return false;
  }
  if (contract.contract_violation_sites > 0 && contract.deterministic) {
    return false;
  }
  return true;
}

std::string Objc3ActorIsolationSendabilityLoweringReplayKey(
    const Objc3ActorIsolationSendabilityLoweringContract &contract) {
  return std::string("actor_isolation_sites=") +
             std::to_string(contract.actor_isolation_sites) +
         ";sendability_check_sites=" +
         std::to_string(contract.sendability_check_sites) +
         ";cross_actor_hop_sites=" +
         std::to_string(contract.cross_actor_hop_sites) +
         ";non_sendable_capture_sites=" +
         std::to_string(contract.non_sendable_capture_sites) +
         ";sendable_transfer_sites=" +
         std::to_string(contract.sendable_transfer_sites) +
         ";isolation_boundary_sites=" +
         std::to_string(contract.isolation_boundary_sites) +
         ";guard_blocked_sites=" + std::to_string(contract.guard_blocked_sites) +
         ";contract_violation_sites=" +
         std::to_string(contract.contract_violation_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" +
         kObjc3ActorIsolationSendabilityLoweringLaneContract;
}
