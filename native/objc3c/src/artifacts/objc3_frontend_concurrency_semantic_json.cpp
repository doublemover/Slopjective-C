#include "artifacts/objc3_frontend_concurrency_semantic_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildConcurrencyTaskExecutorCancellationSemanticModelSummaryJson(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_dependency_contract_id\":\""
      << EscapeJsonString(summary.frontend_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"async_callable_sites\":" << summary.async_callable_sites
      << ",\"executor_attribute_sites\":" << summary.executor_attribute_sites
      << ",\"task_creation_sites\":" << summary.task_creation_sites
      << ",\"task_group_scope_sites\":" << summary.task_group_scope_sites
      << ",\"task_group_add_task_sites\":" << summary.task_group_add_task_sites
      << ",\"task_group_wait_next_sites\":" << summary.task_group_wait_next_sites
      << ",\"task_group_cancel_all_sites\":" << summary.task_group_cancel_all_sites
      << ",\"task_runtime_interop_sites\":" << summary.task_runtime_interop_sites
      << ",\"runtime_hook_sites\":" << summary.runtime_hook_sites
      << ",\"cancellation_check_sites\":" << summary.cancellation_check_sites
      << ",\"cancellation_handler_sites\":" << summary.cancellation_handler_sites
      << ",\"suspension_point_sites\":" << summary.suspension_point_sites
      << ",\"cancellation_propagation_sites\":"
      << summary.cancellation_propagation_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"task_lifetime_semantics_landed\":"
      << (summary.task_lifetime_semantics_landed ? "true" : "false")
      << ",\"executor_affinity_semantics_landed\":"
      << (summary.executor_affinity_semantics_landed ? "true" : "false")
      << ",\"cancellation_observation_semantics_landed\":"
      << (summary.cancellation_observation_semantics_landed ? "true" : "false")
      << ",\"structured_task_legality_semantics_landed\":"
      << (summary.structured_task_legality_semantics_landed ? "true" : "false")
      << ",\"runnable_lowering_deferred\":"
      << (summary.runnable_lowering_deferred ? "true" : "false")
      << ",\"executor_runtime_deferred\":"
      << (summary.executor_runtime_deferred ? "true" : "false")
      << ",\"scheduler_runtime_deferred\":"
      << (summary.scheduler_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildConcurrencyAsyncEffectSuspensionSemanticModelSummaryJson(
    const Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"frontend_dependency_contract_id\":\""
      << EscapeJsonString(summary.frontend_dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"async_continuation_sites\":"
      << summary.async_continuation_sites
      << ",\"async_keyword_sites\":" << summary.async_keyword_sites
      << ",\"async_function_sites\":" << summary.async_function_sites
      << ",\"async_method_sites\":" << summary.async_method_sites
      << ",\"executor_attribute_sites\":"
      << summary.executor_attribute_sites
      << ",\"executor_main_sites\":" << summary.executor_main_sites
      << ",\"executor_global_sites\":" << summary.executor_global_sites
      << ",\"executor_named_sites\":" << summary.executor_named_sites
      << ",\"continuation_allocation_sites\":"
      << summary.continuation_allocation_sites
      << ",\"continuation_resume_sites\":"
      << summary.continuation_resume_sites
      << ",\"continuation_suspend_sites\":"
      << summary.continuation_suspend_sites
      << ",\"async_state_machine_sites\":" << summary.async_state_machine_sites
      << ",\"await_suspension_sites\":" << summary.await_suspension_sites
      << ",\"await_keyword_sites\":" << summary.await_keyword_sites
      << ",\"await_expression_sites\":" << summary.await_expression_sites
      << ",\"await_suspension_point_sites\":"
      << summary.await_suspension_point_sites
      << ",\"await_resume_sites\":" << summary.await_resume_sites
      << ",\"await_state_machine_sites\":"
      << summary.await_state_machine_sites
      << ",\"await_continuation_sites\":"
      << summary.await_continuation_sites
      << ",\"actor_isolation_sendability_sites\":"
      << summary.actor_isolation_sendability_sites
      << ",\"actor_isolation_decl_sites\":"
      << summary.actor_isolation_decl_sites
      << ",\"actor_hop_sites\":" << summary.actor_hop_sites
      << ",\"sendable_annotation_sites\":"
      << summary.sendable_annotation_sites
      << ",\"non_sendable_crossing_sites\":"
      << summary.non_sendable_crossing_sites
      << ",\"isolation_boundary_sites\":"
      << summary.isolation_boundary_sites
      << ",\"task_runtime_interop_sites\":"
      << summary.task_runtime_interop_sites
      << ",\"runtime_hook_sites\":" << summary.runtime_hook_sites
      << ",\"cancellation_check_sites\":"
      << summary.cancellation_check_sites
      << ",\"cancellation_handler_sites\":"
      << summary.cancellation_handler_sites
      << ",\"suspension_point_sites\":" << summary.suspension_point_sites
      << ",\"cancellation_propagation_sites\":"
      << summary.cancellation_propagation_sites
      << ",\"concurrency_replay_race_guard_sites\":"
      << summary.concurrency_replay_race_guard_sites
      << ",\"concurrency_replay_sites\":" << summary.concurrency_replay_sites
      << ",\"replay_proof_sites\":" << summary.replay_proof_sites
      << ",\"race_guard_sites\":" << summary.race_guard_sites
      << ",\"task_handoff_sites\":" << summary.task_handoff_sites
      << ",\"actor_isolation_sites\":" << summary.actor_isolation_sites
      << ",\"deterministic_schedule_sites\":"
      << summary.deterministic_schedule_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"async_declaration_semantics_landed\":"
      << (summary.async_declaration_semantics_landed ? "true" : "false")
      << ",\"executor_affinity_semantics_landed\":"
      << (summary.executor_affinity_semantics_landed ? "true" : "false")
      << ",\"await_legality_semantics_landed\":"
      << (summary.await_legality_semantics_landed ? "true" : "false")
      << ",\"continuation_profile_semantics_landed\":"
      << (summary.continuation_profile_semantics_landed ? "true" : "false")
      << ",\"await_suspension_profile_semantics_landed\":"
      << (summary.await_suspension_profile_semantics_landed ? "true" : "false")
      << ",\"actor_isolation_sendability_semantics_landed\":"
      << (summary.actor_isolation_sendability_semantics_landed ? "true" : "false")
      << ",\"task_runtime_cancellation_semantics_landed\":"
      << (summary.task_runtime_cancellation_semantics_landed ? "true" : "false")
      << ",\"concurrency_replay_race_guard_semantics_landed\":"
      << (summary.concurrency_replay_race_guard_semantics_landed ? "true" : "false")
      << ",\"runnable_lowering_deferred\":"
      << (summary.runnable_lowering_deferred ? "true" : "false")
      << ",\"executor_runtime_deferred\":"
      << (summary.executor_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildConcurrencyAwaitSuspensionResumeSemanticSummaryJson(
    const Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\""
      << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(summary.deferred_model)
      << "\",\"async_callable_sites\":" << summary.async_callable_sites
      << ",\"await_expression_sites\":" << summary.await_expression_sites
      << ",\"await_in_async_callable_sites\":"
      << summary.await_in_async_callable_sites
      << ",\"illegal_await_sites\":" << summary.illegal_await_sites
      << ",\"await_suspension_point_sites\":"
      << summary.await_suspension_point_sites
      << ",\"await_resume_sites\":" << summary.await_resume_sites
      << ",\"continuation_resume_sites\":"
      << summary.continuation_resume_sites
      << ",\"continuation_suspend_sites\":"
      << summary.continuation_suspend_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"await_placement_enforced\":"
      << (summary.await_placement_enforced ? "true" : "false")
      << ",\"suspension_profile_enforced\":"
      << (summary.suspension_profile_enforced ? "true" : "false")
      << ",\"resume_profile_enforced\":"
      << (summary.resume_profile_enforced ? "true" : "false")
      << ",\"non_async_await_fail_closed\":"
      << (summary.non_async_await_fail_closed ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildConcurrencyAsyncDiagnosticsCompatibilitySummaryJson(
    const Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary &summary) {
  std::ostringstream out;
  out << '{'
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id) << "\""
      << ",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id) << "\""
      << ",\"surface_path\":\"" << EscapeJsonString(summary.surface_path) << "\""
      << ",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model) << "\""
      << ",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model) << "\""
      << ",\"async_callable_sites\":" << summary.async_callable_sites
      << ",\"executor_affinity_sites\":" << summary.executor_affinity_sites
      << ",\"illegal_non_async_executor_sites\":"
      << summary.illegal_non_async_executor_sites
      << ",\"illegal_async_function_prototype_sites\":"
      << summary.illegal_async_function_prototype_sites
      << ",\"illegal_async_throws_sites\":"
      << summary.illegal_async_throws_sites
      << ",\"compatibility_diagnostic_sites\":"
      << summary.compatibility_diagnostic_sites
      << ",\"supported_async_callable_sites\":"
      << summary.supported_async_callable_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"executor_affinity_requires_async_enforced\":"
      << (summary.executor_affinity_requires_async_enforced ? "true" : "false")
      << ",\"async_function_prototypes_fail_closed\":"
      << (summary.async_function_prototypes_fail_closed ? "true" : "false")
      << ",\"async_throws_fail_closed\":"
      << (summary.async_throws_fail_closed ? "true" : "false")
      << ",\"unsupported_topology_fail_closed\":"
      << (summary.unsupported_topology_fail_closed ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason) << "\""
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key) << "\""
      << '}';
  return out.str();
}

std::string BuildConcurrencyStructuredTaskCancellationSemanticSummaryJson(
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"async_callable_sites\":" << summary.async_callable_sites
      << ",\"task_creation_sites\":" << summary.task_creation_sites
      << ",\"task_group_scope_sites\":" << summary.task_group_scope_sites
      << ",\"task_group_add_task_sites\":" << summary.task_group_add_task_sites
      << ",\"task_group_wait_next_sites\":"
      << summary.task_group_wait_next_sites
      << ",\"task_group_cancel_all_sites\":"
      << summary.task_group_cancel_all_sites
      << ",\"cancellation_check_sites\":"
      << summary.cancellation_check_sites
      << ",\"cancellation_handler_sites\":"
      << summary.cancellation_handler_sites
      << ",\"illegal_non_async_task_sites\":"
      << summary.illegal_non_async_task_sites
      << ",\"illegal_task_group_scope_sites\":"
      << summary.illegal_task_group_scope_sites
      << ",\"illegal_task_hierarchy_sites\":"
      << summary.illegal_task_hierarchy_sites
      << ",\"illegal_cancellation_usage_sites\":"
      << summary.illegal_cancellation_usage_sites
      << ",\"source_dependency_required\":"
      << (summary.source_dependency_required ? "true" : "false")
      << ",\"async_task_boundary_enforced\":"
      << (summary.async_task_boundary_enforced ? "true" : "false")
      << ",\"structured_task_scope_enforced\":"
      << (summary.structured_task_scope_enforced ? "true" : "false")
      << ",\"task_hierarchy_enforced\":"
      << (summary.task_hierarchy_enforced ? "true" : "false")
      << ",\"cancellation_usage_enforced\":"
      << (summary.cancellation_usage_enforced ? "true" : "false")
      << ",\"runnable_lowering_deferred\":"
      << (summary.runnable_lowering_deferred ? "true" : "false")
      << ",\"executor_runtime_deferred\":"
      << (summary.executor_runtime_deferred ? "true" : "false")
      << ",\"scheduler_runtime_deferred\":"
      << (summary.scheduler_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildConcurrencyExecutorHopAffinityCompatibilitySummaryJson(
    const Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"async_callable_sites\":" << summary.async_callable_sites
      << ",\"executor_affinity_sites\":" << summary.executor_affinity_sites
      << ",\"executor_main_sites\":" << summary.executor_main_sites
      << ",\"executor_global_sites\":" << summary.executor_global_sites
      << ",\"executor_named_sites\":" << summary.executor_named_sites
      << ",\"task_creation_sites\":" << summary.task_creation_sites
      << ",\"detached_task_creation_sites\":"
      << summary.detached_task_creation_sites
      << ",\"illegal_missing_executor_affinity_sites\":"
      << summary.illegal_missing_executor_affinity_sites
      << ",\"illegal_main_executor_detached_sites\":"
      << summary.illegal_main_executor_detached_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"executor_affinity_required_for_task_callables_enforced\":"
      << (summary.executor_affinity_required_for_task_callables_enforced
              ? "true"
              : "false")
      << ",\"detached_task_hop_boundary_enforced\":"
      << (summary.detached_task_hop_boundary_enforced ? "true" : "false")
      << ",\"runnable_lowering_deferred\":"
      << (summary.runnable_lowering_deferred ? "true" : "false")
      << ",\"executor_runtime_deferred\":"
      << (summary.executor_runtime_deferred ? "true" : "false")
      << ",\"scheduler_runtime_deferred\":"
      << (summary.scheduler_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
