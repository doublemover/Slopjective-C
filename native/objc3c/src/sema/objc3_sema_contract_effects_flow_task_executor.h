#pragma once

#include <cstddef>
#include <string>

inline constexpr const char
    *kObjc3ConcurrencyTaskExecutorCancellationSemanticModelDependencyContractId =
        "objc3c.concurrency.task.group.cancellation.source.closure.v1";
inline constexpr const char
    *kObjc3ConcurrencyTaskExecutorCancellationSemanticModelContractId =
        "objc3c.concurrency.task.executor.cancellation.semantic.model.v1";
inline constexpr const char
    *kObjc3ConcurrencyTaskExecutorCancellationSemanticModelSurfacePath =
        "frontend.pipeline.semantic_surface.objc_concurrency_task_executor_and_cancellation_semantic_model";
inline constexpr const char
    *kObjc3ConcurrencyTaskExecutorCancellationSemanticModelRule =
        "task-lifetime-executor-affinity-cancellation-observation-and-structured-task-legality-are-live-in-sema-while-runnable-task-allocation-and-scheduler-execution-remain-later-runtime-work";
inline constexpr const char
    *kObjc3ConcurrencyTaskExecutorCancellationSemanticModelDeferredRule =
        "task-allocation-executor-hop-runtime-task-group-execution-and-scheduler-backed-cancellation-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary {
  std::string contract_id =
      kObjc3ConcurrencyTaskExecutorCancellationSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3ConcurrencyTaskExecutorCancellationSemanticModelDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyTaskExecutorCancellationSemanticModelSurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyTaskExecutorCancellationSemanticModelRule;
  std::string deferred_model =
      kObjc3ConcurrencyTaskExecutorCancellationSemanticModelDeferredRule;
  std::size_t async_callable_sites = 0;
  std::size_t executor_attribute_sites = 0;
  std::size_t task_creation_sites = 0;
  std::size_t task_group_scope_sites = 0;
  std::size_t task_group_add_task_sites = 0;
  std::size_t task_group_wait_next_sites = 0;
  std::size_t task_group_cancel_all_sites = 0;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  bool source_dependency_required = false;
  bool task_lifetime_semantics_landed = false;
  bool executor_affinity_semantics_landed = false;
  bool cancellation_observation_semantics_landed = false;
  bool structured_task_legality_semantics_landed = false;
  bool runnable_lowering_deferred = false;
  bool executor_runtime_deferred = false;
  bool scheduler_runtime_deferred = false;
  bool deterministic = false;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ConcurrencyTaskExecutorCancellationSemanticModelSummary(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.frontend_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.source_dependency_required &&
         summary.task_lifetime_semantics_landed &&
         summary.executor_affinity_semantics_landed &&
         summary.cancellation_observation_semantics_landed &&
         summary.structured_task_legality_semantics_landed &&
         summary.runnable_lowering_deferred &&
         summary.executor_runtime_deferred &&
         summary.scheduler_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
