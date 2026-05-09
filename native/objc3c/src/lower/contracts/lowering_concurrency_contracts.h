#pragma once

#include <cstddef>
#include <string>

// Concurrency lowering owns the private continuation/task/actor helper ABI and
// the replayable lowering contracts that feed task-runtime integration.
inline constexpr const char *kObjc3RuntimeAllocateAsyncContinuationI32Symbol =
    "objc3_runtime_allocate_async_continuation_i32";
inline constexpr const char
    *kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol =
        "objc3_runtime_handoff_async_continuation_to_executor_i32";
inline constexpr const char *kObjc3RuntimeResumeAsyncContinuationI32Symbol =
    "objc3_runtime_resume_async_continuation_i32";
inline constexpr const char *kObjc3RuntimeSpawnTaskI32Symbol =
    "objc3_runtime_spawn_task_i32";
inline constexpr const char *kObjc3RuntimeEnterTaskGroupScopeI32Symbol =
    "objc3_runtime_enter_task_group_scope_i32";
inline constexpr const char *kObjc3RuntimeAddTaskGroupTaskI32Symbol =
    "objc3_runtime_add_task_group_task_i32";
inline constexpr const char *kObjc3RuntimeWaitTaskGroupNextI32Symbol =
    "objc3_runtime_wait_task_group_next_i32";
inline constexpr const char *kObjc3RuntimeCancelTaskGroupI32Symbol =
    "objc3_runtime_cancel_task_group_i32";
inline constexpr const char *kObjc3RuntimeTaskIsCancelledI32Symbol =
    "objc3_runtime_task_is_cancelled_i32";
inline constexpr const char *kObjc3RuntimeTaskOnCancelI32Symbol =
    "objc3_runtime_task_on_cancel_i32";
inline constexpr const char *kObjc3RuntimeExecutorHopI32Symbol =
    "objc3_runtime_executor_hop_i32";
inline constexpr const char *kObjc3RuntimeActorEnterIsolationThunkI32Symbol =
    "objc3_runtime_actor_enter_isolation_thunk_i32";
inline constexpr const char *kObjc3RuntimeActorEnterNonisolatedI32Symbol =
    "objc3_runtime_actor_enter_nonisolated_i32";
inline constexpr const char *kObjc3RuntimeActorHopToExecutorI32Symbol =
    "objc3_runtime_actor_hop_to_executor_i32";
inline constexpr const char *kObjc3RuntimeActorRecordReplayProofI32Symbol =
    "objc3_runtime_actor_record_replay_proof_i32";
inline constexpr const char *kObjc3RuntimeActorRecordRaceGuardI32Symbol =
    "objc3_runtime_actor_record_race_guard_i32";
inline constexpr const char *kObjc3RuntimeActorBindExecutorI32Symbol =
    "objc3_runtime_actor_bind_executor_i32";
inline constexpr const char *kObjc3RuntimeActorMailboxEnqueueI32Symbol =
    "objc3_runtime_actor_mailbox_enqueue_i32";
inline constexpr const char *kObjc3RuntimeActorMailboxDrainNextI32Symbol =
    "objc3_runtime_actor_mailbox_drain_next_i32";

inline constexpr const char *kObjc3ConcurrencyContinuationRuntimeHelperContractId =
    "objc3c.concurrency.continuation.runtime.helper.api.v1";
inline constexpr const char *kObjc3ConcurrencyContinuationRuntimeHelperSourceModel =
    "concurrency-lowering-publishes-a-private-runtime-helper-abi-for-logical-continuation-allocation-resume-and-executor-handoff";
inline constexpr const char *kObjc3ConcurrencyContinuationRuntimeHelperAbiModel =
    "i32-backed-logical-continuation-handles-resume-entry-tags-and-executor-tags-remain-bootstrap-internal-runtime-abi";
inline constexpr const char
    *kObjc3ConcurrencyContinuationRuntimeHelperExecutionModel =
        "runtime-helpers-materialize-deterministic-logical-continuation-handles-resume-traffic-and-executor-handoff-without-public-header-widening";
inline constexpr const char
    *kObjc3ConcurrencyContinuationRuntimeHelperFailClosedModel =
        "no-public-async-runtime-header-no-suspension-state-machine-no-executor-runtime-scheduling-claim-yet";

inline constexpr const char
    *kObjc3ConcurrencyLiveContinuationRuntimeIntegrationContractId =
        "objc3c.concurrency.live.continuation.runtime.integration.v1";
inline constexpr const char
    *kObjc3ConcurrencyLiveContinuationRuntimeIntegrationSourceModel =
        "supported-direct-call-await-sites-now-execute-through-the-private-continuation-helper-cluster";
inline constexpr const char
    *kObjc3ConcurrencyLiveContinuationRuntimeIntegrationExecutionModel =
        "non-suspending-async-functions-and-methods-allocate-handoff-and-resume-logical-continuations-through-runtime-owned-helpers";
inline constexpr const char
    *kObjc3ConcurrencyLiveContinuationRuntimeIntegrationPackagingModel =
        "driver-emitted-object-artifacts-link-against-the-existing-runtime-support-archive-for-live-concurrency-helper-execution";
inline constexpr const char
    *kObjc3ConcurrencyLiveContinuationRuntimeIntegrationFailClosedModel =
        "no-suspension-state-machine-no-general-executor-runtime-no-cross-module-live-claim-yet";

inline constexpr const char *kObjc3ConcurrencySchedulerExecutorRuntimeContractId =
    "objc3c.concurrency.scheduler.executor.runtime.contract.v1";
inline constexpr const char *kObjc3ConcurrencySchedulerExecutorRuntimeSourceModel =
    "helper-backed-task-runtime-abi-completion-freezes-one-private-scheduler-executor-task-and-cancellation-runtime-boundary";
inline constexpr const char *kObjc3ConcurrencySchedulerExecutorRuntimeAbiModel =
    "private-bootstrap-internal-task-runtime-helpers-and-snapshot-publish-executor-tags-task-state-and-cancellation-observation";
inline constexpr const char
    *kObjc3ConcurrencySchedulerExecutorRuntimeExecutionModel =
        "runtime-library-materializes-deterministic-task-spawn-task-group-cancellation-and-executor-hop-helper-traffic-without-public-abi-widening";
inline constexpr const char
    *kObjc3ConcurrencySchedulerExecutorRuntimePackagingModel =
        "native-driver-and-runtime-probes-link-against-the-existing-runtime-support-archive-for-private-task-runtime-helper-execution";
inline constexpr const char
    *kObjc3ConcurrencySchedulerExecutorRuntimeFailClosedModel =
        "no-public-task-runtime-header-no-general-scheduler-implementation-claim-no-cross-module-task-runtime-claim-yet";

inline constexpr const char *kObjc3ConcurrencyLiveTaskRuntimeIntegrationContractId =
    "objc3c.concurrency.live.task.runtime.integration.v1";
inline constexpr const char *kObjc3ConcurrencyLiveTaskRuntimeIntegrationSourceModel =
    "supported-task-spawn-task-group-cancellation-and-executor-hop-sites-now-execute-through-the-private-task-runtime-helper-cluster";
inline constexpr const char
    *kObjc3ConcurrencyLiveTaskRuntimeIntegrationExecutionModel =
        "native-runtime-helpers-materialize-deterministic-task-spawn-task-group-cancellation-and-executor-hop-results-through-linked-runtime-probes";
inline constexpr const char
    *kObjc3ConcurrencyLiveTaskRuntimeIntegrationPackagingModel =
        "driver-emitted-object-artifacts-and-runtime-probes-link-against-the-existing-runtime-support-archive-for-live-concurrency-task-execution";
inline constexpr const char
    *kObjc3ConcurrencyLiveTaskRuntimeIntegrationFailClosedModel =
        "retained-runtime-metadata-export-gates-and-no-public-task-runtime-header-mean-broader-native-task-scheduler-claims-remain-deferred";

inline constexpr const char *kObjc3ConcurrencyTaskRuntimeHardeningContractId =
    "objc3c.concurrency.task.runtime.hardening.v1";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeHardeningSourceModel =
    "supported-task-runtime-helper-traffic-now-preserves-cancellation-cleanup-autorelease-scope-and-reset-replay-determinism";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeHardeningExecutionModel =
    "task-runtime-helper-state-memory-management-scope-state-and-arc-debug-counters-remain-stable-across-reset-and-autorelease-boundaries";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeHardeningPackagingModel =
    "linked-runtime-probes-consume-the-existing-runtime-support-archive-and-validate-two-pass-reset-stable-task-runtime-state";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeHardeningFailClosedModel =
    "no-public-task-scheduler-abi-no-cross-module-task-runtime-claim-and-no-broader-front-door-metadata-export-unblock-claim-yet";

inline constexpr const char *kObjc3AsyncContinuationLoweringLaneContract =
    "objc3c.async.continuation.lowering.v1";
inline constexpr const char
    *kObjc3AwaitLoweringSuspensionStateLoweringLaneContract =
        "objc3c.await.lowering.suspension.state.lowering.v1";
inline constexpr const char *kObjc3ActorIsolationSendabilityLoweringLaneContract =
    "objc3c.actor.isolation.sendability.lowering.v1";

inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataContractId =
    "objc3c.concurrency.actor.lowering.and.metadata.contract.v1";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_concurrency_actor_lowering_and_metadata_contract";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataModel =
    "actor-member-semantic-and-hazard-packets-now-lower-through-one-deterministic-actor-metadata-isolation-thunk-and-hop-artifact-contract";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataDeferredModel =
    "live-actor-thunk-bodies-mailbox-runtime-entrypoints-and-runnable-cross-actor-scheduling-remain-later-actor-lowering-and-replay-runtime-work";
inline constexpr const char *kObjc3ConcurrencyActorLoweringMetadataLaneContract =
    "objc3c.actor.lowering.metadata.contract.v1";

inline constexpr const char *kObjc3TaskRuntimeInteropCancellationLoweringLaneContract =
    "objc3c.task.runtime.interop.cancellation.lowering.v1";
inline constexpr const char *kObjc3ConcurrencyReplayRaceGuardLoweringLaneContract =
    "objc3c.concurrency.replay.race.guard.lowering.v1";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeLoweringContractId =
    "objc3c.concurrency.task.runtime.lowering.contract.v1";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeLoweringSurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_task_runtime_lowering_contract";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeAbiCompletionContractId =
    "objc3c.concurrency.task.runtime.abi.completion.v1";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeAbiCompletionSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_concurrency_task_group_and_runtime_abi_completion";

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

std::string Objc3ConcurrencyContinuationRuntimeHelperSummary();
std::string Objc3ConcurrencyLiveContinuationRuntimeIntegrationSummary();
std::string Objc3ConcurrencySchedulerExecutorRuntimeSummary();
std::string Objc3ConcurrencyLiveTaskRuntimeIntegrationSummary();
std::string Objc3ConcurrencyTaskRuntimeHardeningSummary();

bool IsValidObjc3AsyncContinuationLoweringContract(
    const Objc3AsyncContinuationLoweringContract &contract);
std::string Objc3AsyncContinuationLoweringReplayKey(
    const Objc3AsyncContinuationLoweringContract &contract);
bool IsValidObjc3AwaitLoweringSuspensionStateLoweringContract(
    const Objc3AwaitLoweringSuspensionStateLoweringContract &contract);
std::string Objc3AwaitLoweringSuspensionStateLoweringReplayKey(
    const Objc3AwaitLoweringSuspensionStateLoweringContract &contract);
bool IsValidObjc3ActorIsolationSendabilityLoweringContract(
    const Objc3ActorIsolationSendabilityLoweringContract &contract);
std::string Objc3ActorIsolationSendabilityLoweringReplayKey(
    const Objc3ActorIsolationSendabilityLoweringContract &contract);
bool IsValidObjc3ActorLoweringMetadataContract(
    const Objc3ActorLoweringMetadataContract &contract);
std::string Objc3ActorLoweringMetadataReplayKey(
    const Objc3ActorLoweringMetadataContract &contract);
bool IsValidObjc3TaskRuntimeInteropCancellationLoweringContract(
    const Objc3TaskRuntimeInteropCancellationLoweringContract &contract);
std::string Objc3TaskRuntimeInteropCancellationLoweringReplayKey(
    const Objc3TaskRuntimeInteropCancellationLoweringContract &contract);
bool IsValidObjc3ConcurrencyReplayRaceGuardLoweringContract(
    const Objc3ConcurrencyReplayRaceGuardLoweringContract &contract);
std::string Objc3ConcurrencyReplayRaceGuardLoweringReplayKey(
    const Objc3ConcurrencyReplayRaceGuardLoweringContract &contract);
