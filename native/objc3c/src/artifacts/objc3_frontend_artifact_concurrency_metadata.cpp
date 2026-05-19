#include "artifacts/objc3_frontend_artifact_concurrency_metadata.h"

#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/contracts/concurrency_actor_contracts.h"
#include "lower/contracts/concurrency_continuation_runtime_contracts.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendConcurrencyMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &async_continuation_lowering_replay_key,
    const Objc3AsyncContinuationLoweringContract
        &async_continuation_lowering_contract,
    const std::string &await_lowering_suspension_state_lowering_replay_key,
    const Objc3AwaitLoweringSuspensionStateLoweringContract
        &await_lowering_suspension_state_lowering_contract,
    const std::string &actor_isolation_sendability_lowering_replay_key,
    const Objc3ActorIsolationSendabilityLoweringContract
        &actor_isolation_sendability_lowering_contract,
    const std::string &actor_lowering_metadata_replay_key,
    const Objc3ActorLoweringMetadataContract &actor_lowering_metadata_contract) {
  ir_frontend_metadata.lowering_async_continuation_replay_key =
      async_continuation_lowering_replay_key;
  ir_frontend_metadata.async_continuation_lowering_sites =
      async_continuation_lowering_contract.async_continuation_sites;
  ir_frontend_metadata.async_continuation_lowering_async_keyword_sites =
      async_continuation_lowering_contract.async_keyword_sites;
  ir_frontend_metadata.async_continuation_lowering_async_function_sites =
      async_continuation_lowering_contract.async_function_sites;
  ir_frontend_metadata
      .async_continuation_lowering_continuation_allocation_sites =
      async_continuation_lowering_contract.continuation_allocation_sites;
  ir_frontend_metadata.async_continuation_lowering_continuation_resume_sites =
      async_continuation_lowering_contract.continuation_resume_sites;
  ir_frontend_metadata.async_continuation_lowering_continuation_suspend_sites =
      async_continuation_lowering_contract.continuation_suspend_sites;
  ir_frontend_metadata.async_continuation_lowering_async_state_machine_sites =
      async_continuation_lowering_contract.async_state_machine_sites;
  ir_frontend_metadata.async_continuation_lowering_normalized_sites =
      async_continuation_lowering_contract.normalized_sites;
  ir_frontend_metadata.async_continuation_lowering_gate_blocked_sites =
      async_continuation_lowering_contract.gate_blocked_sites;
  ir_frontend_metadata.async_continuation_lowering_contract_violation_sites =
      async_continuation_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_async_continuation_lowering_handoff =
      async_continuation_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_await_lowering_suspension_state_replay_key =
      await_lowering_suspension_state_lowering_replay_key;
  ir_frontend_metadata.await_lowering_suspension_state_lowering_sites =
      await_lowering_suspension_state_lowering_contract.await_suspension_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_await_keyword_sites =
      await_lowering_suspension_state_lowering_contract.await_keyword_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_await_suspension_point_sites =
      await_lowering_suspension_state_lowering_contract
          .await_suspension_point_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_await_resume_sites =
      await_lowering_suspension_state_lowering_contract.await_resume_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_await_state_machine_sites =
      await_lowering_suspension_state_lowering_contract
          .await_state_machine_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_await_continuation_sites =
      await_lowering_suspension_state_lowering_contract
          .await_continuation_sites;
  ir_frontend_metadata.await_lowering_suspension_state_lowering_normalized_sites =
      await_lowering_suspension_state_lowering_contract.normalized_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_gate_blocked_sites =
      await_lowering_suspension_state_lowering_contract.gate_blocked_sites;
  ir_frontend_metadata
      .await_lowering_suspension_state_lowering_contract_violation_sites =
      await_lowering_suspension_state_lowering_contract.contract_violation_sites;
  ir_frontend_metadata
      .deterministic_await_lowering_suspension_state_lowering_handoff =
      await_lowering_suspension_state_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_actor_isolation_sendability_replay_key =
      actor_isolation_sendability_lowering_replay_key;
  ir_frontend_metadata.actor_isolation_sendability_lowering_sites =
      actor_isolation_sendability_lowering_contract.actor_isolation_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_sendability_check_sites =
      actor_isolation_sendability_lowering_contract.sendability_check_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_cross_actor_hop_sites =
      actor_isolation_sendability_lowering_contract.cross_actor_hop_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_non_sendable_capture_sites =
      actor_isolation_sendability_lowering_contract.non_sendable_capture_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_sendable_transfer_sites =
      actor_isolation_sendability_lowering_contract.sendable_transfer_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_isolation_boundary_sites =
      actor_isolation_sendability_lowering_contract.isolation_boundary_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_guard_blocked_sites =
      actor_isolation_sendability_lowering_contract.guard_blocked_sites;
  ir_frontend_metadata
      .actor_isolation_sendability_lowering_contract_violation_sites =
      actor_isolation_sendability_lowering_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_actor_isolation_sendability_lowering_handoff =
      actor_isolation_sendability_lowering_contract.deterministic;

  ir_frontend_metadata.lowering_actor_lowering_metadata_replay_key =
      actor_lowering_metadata_replay_key;
  // implementation anchor: the deterministic actor-lowering contract feeds
  // helper-backed actor thunk/hop/nonisolated rewrites in IR.
  ir_frontend_metadata.actor_lowering_metadata_actor_interface_sites =
      actor_lowering_metadata_contract.actor_interface_sites;
  ir_frontend_metadata.actor_lowering_metadata_actor_method_sites =
      actor_lowering_metadata_contract.actor_method_sites;
  ir_frontend_metadata.actor_lowering_metadata_actor_metadata_record_sites =
      actor_lowering_metadata_contract.actor_metadata_record_sites;
  ir_frontend_metadata.actor_lowering_metadata_nonisolated_entry_sites =
      actor_lowering_metadata_contract.nonisolated_entry_sites;
  ir_frontend_metadata.actor_lowering_metadata_executor_affinity_sites =
      actor_lowering_metadata_contract.executor_affinity_sites;
  ir_frontend_metadata.actor_lowering_metadata_actor_hop_artifact_sites =
      actor_lowering_metadata_contract.actor_hop_artifact_sites;
  ir_frontend_metadata.actor_lowering_metadata_actor_isolation_thunk_sites =
      actor_lowering_metadata_contract.actor_isolation_thunk_sites;
  ir_frontend_metadata.actor_lowering_metadata_replay_proof_dependency_sites =
      actor_lowering_metadata_contract.replay_proof_dependency_sites;
  ir_frontend_metadata.actor_lowering_metadata_race_guard_dependency_sites =
      actor_lowering_metadata_contract.race_guard_dependency_sites;
  ir_frontend_metadata.actor_lowering_metadata_task_handoff_sites =
      actor_lowering_metadata_contract.task_handoff_sites;
  ir_frontend_metadata.actor_lowering_metadata_guard_blocked_sites =
      actor_lowering_metadata_contract.guard_blocked_sites;
  ir_frontend_metadata.actor_lowering_metadata_contract_violation_sites =
      actor_lowering_metadata_contract.contract_violation_sites;
  ir_frontend_metadata.deterministic_actor_lowering_metadata_handoff =
      actor_lowering_metadata_contract.deterministic;
}

}  // namespace objc3::artifacts::frontend
