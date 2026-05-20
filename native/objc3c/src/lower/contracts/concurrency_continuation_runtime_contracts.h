#pragma once

#include <cstddef>
#include <string>

// Continuation runtime contracts own async continuation/await lowering records
// and the private helper ABI used by live continuation integration.
inline constexpr const char *kObjc3RuntimeAllocateAsyncContinuationI32Symbol =
    "objc3_runtime_allocate_async_continuation_i32";
inline constexpr const char
    *kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol =
        "objc3_runtime_handoff_async_continuation_to_executor_i32";
inline constexpr const char *kObjc3RuntimeResumeAsyncContinuationI32Symbol =
    "objc3_runtime_resume_async_continuation_i32";
inline constexpr const char *kObjc3RuntimeCancelAsyncContinuationI32Symbol =
    "objc3_runtime_cancel_async_continuation_i32";

inline constexpr const char *kObjc3ConcurrencyContinuationRuntimeHelperContractId =
    "objc3c.concurrency.continuation.runtime.helper.api.v1";
inline constexpr const char *kObjc3ConcurrencyContinuationRuntimeHelperSourceModel =
    "concurrency-lowering-publishes-a-private-runtime-helper-abi-for-logical-continuation-allocation-resume-cancel-and-executor-handoff";
inline constexpr const char *kObjc3ConcurrencyContinuationRuntimeHelperAbiModel =
    "i32-backed-logical-continuation-handles-resume-entry-tags-and-executor-tags-remain-bootstrap-internal-runtime-abi";
inline constexpr const char
    *kObjc3ConcurrencyContinuationRuntimeHelperExecutionModel =
        "runtime-helpers-materialize-deterministic-logical-continuation-handles-resume-cancel-traffic-and-executor-handoff-without-public-header-widening";
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

std::string Objc3ConcurrencyContinuationRuntimeHelperSummary();
std::string Objc3ConcurrencyLiveContinuationRuntimeIntegrationSummary();

bool IsValidObjc3AsyncContinuationLoweringContract(
    const Objc3AsyncContinuationLoweringContract &contract);
std::string Objc3AsyncContinuationLoweringReplayKey(
    const Objc3AsyncContinuationLoweringContract &contract);
bool IsValidObjc3AwaitLoweringSuspensionStateLoweringContract(
    const Objc3AwaitLoweringSuspensionStateLoweringContract &contract);
std::string Objc3AwaitLoweringSuspensionStateLoweringReplayKey(
    const Objc3AwaitLoweringSuspensionStateLoweringContract &contract);
