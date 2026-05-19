#pragma once

#include <cstddef>
#include <string>

// Task runtime lowering contracts own task interop/cancellation replay facts and
// the lower-owned ABI completion surface consumed before private runtime helper
// execution.
inline constexpr const char *kObjc3TaskRuntimeInteropCancellationLoweringLaneContract =
    "objc3c.task.runtime.interop.cancellation.lowering.v1";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeLoweringContractId =
    "objc3c.concurrency.task.runtime.lowering.contract.v1";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeLoweringSurfacePath =
    "frontend.pipeline.semantic_surface.objc_concurrency_task_runtime_lowering_contract";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeAbiCompletionContractId =
    "objc3c.concurrency.task.runtime.abi.completion.v1";
inline constexpr const char *kObjc3ConcurrencyTaskRuntimeAbiCompletionSurfacePath =
    "frontend.pipeline.semantic_surface."
    "objc_concurrency_task_group_and_runtime_abi_completion";

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

bool IsValidObjc3TaskRuntimeInteropCancellationLoweringContract(
    const Objc3TaskRuntimeInteropCancellationLoweringContract &contract);
std::string Objc3TaskRuntimeInteropCancellationLoweringReplayKey(
    const Objc3TaskRuntimeInteropCancellationLoweringContract &contract);
