#pragma once

#include <cstddef>
#include <string>

inline constexpr const char
    *kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelFrontendDependencyContractId =
        "objc3c.concurrency.async.source.closure.v1";
inline constexpr const char
    *kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelContractId =
        "objc3c.concurrency.async.effect.suspension.semantic.model.v1";
inline constexpr const char
    *kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelSurfacePath =
        "frontend.pipeline.semantic_surface.objc_concurrency_async_effect_and_suspension_semantic_model";
inline constexpr const char
    *kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelRule =
        "async-effect-and-await-legality-semantics-plus-deterministic-continuation-suspension-and-concurrency-profile-carriage-are-live-while-runnable-frame-lowering-cleanup-and-executor-runtime-integration-remain-later-runtime-work";
inline constexpr const char
    *kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelDeferredRule =
        "async-frame-abi-resume-lowering-suspension-cleanup-task-runtime-execution-and-executor-dispatch-remain-deferred-to-later-runtime-lanes";

struct Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary {
  std::string contract_id =
      kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelContractId;
  std::string frontend_dependency_contract_id =
      kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelFrontendDependencyContractId;
  std::string surface_path =
      kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelSurfacePath;
  std::string semantic_model =
      kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelRule;
  std::string deferred_model =
      kObjc3ConcurrencyAsyncEffectSuspensionSemanticModelDeferredRule;
  std::size_t async_continuation_sites = 0;
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t async_method_sites = 0;
  std::size_t executor_attribute_sites = 0;
  std::size_t executor_main_sites = 0;
  std::size_t executor_global_sites = 0;
  std::size_t executor_named_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
  std::size_t await_suspension_sites = 0;
  std::size_t await_keyword_sites = 0;
  std::size_t await_expression_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t await_state_machine_sites = 0;
  std::size_t await_continuation_sites = 0;
  std::size_t actor_isolation_sendability_sites = 0;
  std::size_t actor_isolation_decl_sites = 0;
  std::size_t actor_hop_sites = 0;
  std::size_t sendable_annotation_sites = 0;
  std::size_t non_sendable_crossing_sites = 0;
  std::size_t isolation_boundary_sites = 0;
  std::size_t task_runtime_interop_sites = 0;
  std::size_t runtime_hook_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
  std::size_t suspension_point_sites = 0;
  std::size_t cancellation_propagation_sites = 0;
  std::size_t concurrency_replay_race_guard_sites = 0;
  std::size_t concurrency_replay_sites = 0;
  std::size_t replay_proof_sites = 0;
  std::size_t race_guard_sites = 0;
  std::size_t task_handoff_sites = 0;
  std::size_t actor_isolation_sites = 0;
  std::size_t deterministic_schedule_sites = 0;
  bool source_dependency_required = false;
  bool async_declaration_semantics_landed = false;
  bool executor_affinity_semantics_landed = false;
  bool await_legality_semantics_landed = false;
  bool continuation_profile_semantics_landed = false;
  bool await_suspension_profile_semantics_landed = false;
  bool actor_isolation_sendability_semantics_landed = false;
  bool task_runtime_cancellation_semantics_landed = false;
  bool concurrency_replay_race_guard_semantics_landed = false;
  bool runnable_lowering_deferred = false;
  bool executor_runtime_deferred = false;
  bool deterministic = true;
  bool ready_for_lowering_and_runtime = false;
  std::string replay_key;
  std::string failure_reason;
};

inline bool IsReadyObjc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary(
    const Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary &summary) {
  return !summary.contract_id.empty() &&
         !summary.frontend_dependency_contract_id.empty() &&
         !summary.surface_path.empty() && !summary.semantic_model.empty() &&
         !summary.deferred_model.empty() &&
         summary.source_dependency_required &&
         summary.async_declaration_semantics_landed &&
         summary.executor_affinity_semantics_landed &&
         summary.await_legality_semantics_landed &&
         summary.continuation_profile_semantics_landed &&
         summary.await_suspension_profile_semantics_landed &&
         summary.actor_isolation_sendability_semantics_landed &&
         summary.task_runtime_cancellation_semantics_landed &&
         summary.concurrency_replay_race_guard_semantics_landed &&
         summary.runnable_lowering_deferred &&
         summary.executor_runtime_deferred && summary.deterministic &&
         summary.ready_for_lowering_and_runtime &&
         !summary.replay_key.empty() && summary.failure_reason.empty();
}
