#pragma once

#include <string>

// Task runtime helpers own the private scheduler/executor ABI, task group and
// cancellation helper symbols, live task runtime integration, and hardening
// surfaces that keep public task runtime claims fail-closed.
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

std::string Objc3ConcurrencySchedulerExecutorRuntimeSummary();
std::string Objc3ConcurrencyLiveTaskRuntimeIntegrationSummary();
std::string Objc3ConcurrencyTaskRuntimeHardeningSummary();
