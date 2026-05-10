#include "sema/objc3_semantic_passes.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
BuildConcurrencyStructuredTaskCancellationSemanticSummary(
    const Objc3ConcurrencyTaskExecutorCancellationSemanticModelSummary
        &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3ConcurrencyStructuredTaskCancellationSemanticSummary summary;
  const auto count_diagnostic_code = [&](const char *code) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(), [&](const std::string &diag) {
          return diag.find(code) != std::string::npos;
        }));
  };

  const std::size_t task_related_sites =
      dependency_summary.task_creation_sites +
      dependency_summary.task_group_scope_sites +
      dependency_summary.task_group_add_task_sites +
      dependency_summary.task_group_wait_next_sites +
      dependency_summary.task_group_cancel_all_sites +
      dependency_summary.cancellation_check_sites +
      dependency_summary.cancellation_handler_sites;

  summary.async_callable_sites = dependency_summary.async_callable_sites;
  summary.task_creation_sites = dependency_summary.task_creation_sites;
  summary.task_group_scope_sites = dependency_summary.task_group_scope_sites;
  summary.task_group_add_task_sites =
      dependency_summary.task_group_add_task_sites;
  summary.task_group_wait_next_sites =
      dependency_summary.task_group_wait_next_sites;
  summary.task_group_cancel_all_sites =
      dependency_summary.task_group_cancel_all_sites;
  summary.cancellation_check_sites =
      dependency_summary.cancellation_check_sites;
  summary.cancellation_handler_sites =
      dependency_summary.cancellation_handler_sites;
  summary.illegal_non_async_task_sites = count_diagnostic_code("O3S227");
  summary.illegal_task_group_scope_sites = count_diagnostic_code("O3S228");
  summary.illegal_task_hierarchy_sites = count_diagnostic_code("O3S229");
  summary.illegal_cancellation_usage_sites =
      count_diagnostic_code("O3S230");

  summary.source_dependency_required = true;
  summary.async_task_boundary_enforced =
      dependency_summary.ready_for_lowering_and_runtime &&
      summary.illegal_non_async_task_sites <= task_related_sites;
  summary.structured_task_scope_enforced =
      dependency_summary.structured_task_legality_semantics_landed &&
      summary.illegal_task_group_scope_sites <=
          summary.task_group_add_task_sites +
              summary.task_group_wait_next_sites +
              summary.task_group_cancel_all_sites;
  summary.task_hierarchy_enforced =
      dependency_summary.task_lifetime_semantics_landed &&
      summary.illegal_task_hierarchy_sites <= summary.task_creation_sites;
  summary.cancellation_usage_enforced =
      dependency_summary.cancellation_observation_semantics_landed &&
      summary.illegal_cancellation_usage_sites <=
          summary.cancellation_handler_sites +
              summary.task_group_cancel_all_sites;
  summary.runnable_lowering_deferred = true;
  summary.executor_runtime_deferred = true;
  summary.scheduler_runtime_deferred = true;
  summary.deterministic =
      dependency_summary.deterministic && summary.async_task_boundary_enforced &&
      summary.structured_task_scope_enforced &&
      summary.task_hierarchy_enforced &&
      summary.cancellation_usage_enforced;
  summary.ready_for_lowering_and_runtime = summary.deterministic;

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";task-sites=" << summary.task_creation_sites << ":"
      << summary.task_group_scope_sites << ":"
      << summary.task_group_add_task_sites << ":"
      << summary.task_group_wait_next_sites << ":"
      << summary.task_group_cancel_all_sites
      << ";cancellation-sites=" << summary.cancellation_check_sites << ":"
      << summary.cancellation_handler_sites
      << ";illegal-sites=" << summary.illegal_non_async_task_sites << ":"
      << summary.illegal_task_group_scope_sites << ":"
      << summary.illegal_task_hierarchy_sites << ":"
      << summary.illegal_cancellation_usage_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary
BuildConcurrencyExecutorHopAffinityCompatibilitySummary(
    const Objc3ConcurrencyStructuredTaskCancellationSemanticSummary
        &dependency_summary,
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &source_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3ConcurrencyExecutorHopAffinityCompatibilitySummary summary;
  const auto count_diagnostic_code = [&](const char *code) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(), [&](const std::string &diag) {
          return diag.find(code) != std::string::npos;
        }));
  };

  const std::size_t executor_affinity_sites =
      source_summary.executor_main_sites + source_summary.executor_global_sites +
      source_summary.executor_named_sites;

  summary.async_callable_sites = source_summary.async_function_sites +
                                 source_summary.async_method_sites;
  summary.executor_affinity_sites = executor_affinity_sites;
  summary.executor_main_sites = source_summary.executor_main_sites;
  summary.executor_global_sites = source_summary.executor_global_sites;
  summary.executor_named_sites = source_summary.executor_named_sites;
  summary.task_creation_sites = dependency_summary.task_creation_sites;
  summary.detached_task_creation_sites =
      dependency_summary.task_creation_sites > 0u &&
              source_summary.executor_named_sites > 0u
          ? 1u
          : 0u;
  summary.illegal_missing_executor_affinity_sites =
      count_diagnostic_code("O3S231");
  summary.illegal_main_executor_detached_sites =
      count_diagnostic_code("O3S232");

  summary.dependency_required = true;
  summary.executor_affinity_required_for_task_callables_enforced =
      summary.illegal_missing_executor_affinity_sites <=
          dependency_summary.async_callable_sites;
  summary.detached_task_hop_boundary_enforced =
      summary.illegal_main_executor_detached_sites <=
          summary.executor_main_sites + summary.executor_affinity_sites;
  summary.runnable_lowering_deferred = true;
  summary.executor_runtime_deferred = true;
  summary.scheduler_runtime_deferred = true;
  summary.deterministic =
      summary.executor_affinity_required_for_task_callables_enforced &&
      summary.detached_task_hop_boundary_enforced;
  summary.ready_for_lowering_and_runtime = summary.deterministic;

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";async-callables=" << summary.async_callable_sites
      << ";executor-sites=" << summary.executor_affinity_sites << ":"
      << summary.executor_main_sites << ":" << summary.executor_global_sites
      << ":" << summary.executor_named_sites
      << ";task-creation-sites=" << summary.task_creation_sites
      << ";illegal-affinity=" << summary.illegal_missing_executor_affinity_sites
      << ";illegal-main-detached="
      << summary.illegal_main_executor_detached_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary
BuildConcurrencyAwaitSuspensionResumeSemanticSummary(
    const Objc3ConcurrencyAsyncEffectSuspensionSemanticModelSummary
        &dependency_summary,
    const std::vector<std::string> &diagnostics) {
  Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary summary;
  const auto count_diagnostic_code = [&](const char *code) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(), [&](const std::string &diag) {
          return diag.find(code) != std::string::npos;
        }));
  };

  summary.async_callable_sites =
      dependency_summary.async_function_sites +
      dependency_summary.async_method_sites;
  summary.await_expression_sites = dependency_summary.await_expression_sites;
  summary.illegal_await_sites = count_diagnostic_code("O3S223");
  summary.await_in_async_callable_sites =
      summary.await_expression_sites >= summary.illegal_await_sites
          ? summary.await_expression_sites - summary.illegal_await_sites
          : 0u;
  summary.await_suspension_point_sites =
      dependency_summary.await_suspension_point_sites;
  summary.await_resume_sites = dependency_summary.await_resume_sites;
  summary.continuation_resume_sites =
      dependency_summary.continuation_resume_sites;
  summary.continuation_suspend_sites =
      dependency_summary.continuation_suspend_sites;

  summary.source_dependency_required = true;
  summary.await_placement_enforced =
      dependency_summary.async_declaration_semantics_landed &&
      dependency_summary.await_legality_semantics_landed &&
      summary.illegal_await_sites <= summary.await_expression_sites &&
      summary.await_in_async_callable_sites + summary.illegal_await_sites ==
          summary.await_expression_sites;
  summary.suspension_profile_enforced =
      dependency_summary.await_suspension_profile_semantics_landed &&
      summary.await_suspension_point_sites <= summary.await_expression_sites;
  summary.resume_profile_enforced =
      dependency_summary.continuation_profile_semantics_landed &&
      summary.continuation_resume_sites <=
          dependency_summary.async_continuation_sites &&
      summary.continuation_suspend_sites <=
          dependency_summary.async_continuation_sites &&
      summary.await_resume_sites <= dependency_summary.await_suspension_sites;
  summary.non_async_await_fail_closed = true;
  summary.deterministic =
      dependency_summary.deterministic && summary.await_placement_enforced &&
      summary.suspension_profile_enforced &&
      summary.resume_profile_enforced;
  summary.ready_for_lowering_and_runtime = summary.deterministic;

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";async-callables=" << summary.async_callable_sites
      << ";await-expressions=" << summary.await_expression_sites
      << ";await-in-async=" << summary.await_in_async_callable_sites
      << ";illegal-await=" << summary.illegal_await_sites
      << ";suspension-sites=" << summary.await_suspension_point_sites << ":"
      << summary.await_resume_sites
      << ";continuation-sites=" << summary.continuation_resume_sites << ":"
      << summary.continuation_suspend_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}

Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary
BuildConcurrencyAsyncDiagnosticsCompatibilitySummary(
    const Objc3ConcurrencyAwaitSuspensionResumeSemanticSummary
        &dependency_summary,
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &source_summary,
    const Objc3Program &ast,
    const std::vector<std::string> &diagnostics) {
  Objc3ConcurrencyAsyncDiagnosticsCompatibilitySummary summary;
  const auto count_diagnostic_code = [&](const char *code) {
    return static_cast<std::size_t>(std::count_if(
        diagnostics.begin(), diagnostics.end(), [&](const std::string &diag) {
          return diag.find(code) != std::string::npos;
        }));
  };

  const auto count_async_function_prototypes = [&]() {
    return static_cast<std::size_t>(std::count_if(
        ast.functions.begin(), ast.functions.end(),
        [](const FunctionDecl &fn) {
          return fn.async_declared && fn.is_prototype;
        }));
  };

  summary.async_callable_sites = dependency_summary.async_callable_sites;
  summary.executor_affinity_sites = source_summary.executor_attribute_sites;
  summary.illegal_non_async_executor_sites = count_diagnostic_code("O3S224");
  summary.illegal_async_function_prototype_sites =
      count_diagnostic_code("O3S225");
  summary.illegal_async_throws_sites = count_diagnostic_code("O3S226");
  summary.compatibility_diagnostic_sites =
      summary.illegal_non_async_executor_sites +
      summary.illegal_async_function_prototype_sites +
      summary.illegal_async_throws_sites;
  summary.supported_async_callable_sites =
      summary.async_callable_sites >=
              summary.illegal_async_function_prototype_sites +
                  summary.illegal_async_throws_sites
          ? summary.async_callable_sites -
                (summary.illegal_async_function_prototype_sites +
                 summary.illegal_async_throws_sites)
          : 0u;
  summary.dependency_required = true;
  summary.executor_affinity_requires_async_enforced =
      summary.illegal_non_async_executor_sites <=
      source_summary.executor_attribute_sites;
  summary.async_function_prototypes_fail_closed =
      summary.illegal_async_function_prototype_sites ==
      count_async_function_prototypes();
  summary.async_throws_fail_closed =
      summary.illegal_async_throws_sites <= summary.async_callable_sites;
  summary.unsupported_topology_fail_closed =
      dependency_summary.ready_for_lowering_and_runtime &&
      summary.executor_affinity_requires_async_enforced &&
      summary.async_function_prototypes_fail_closed &&
      summary.async_throws_fail_closed;
  summary.deterministic =
      dependency_summary.deterministic &&
      source_summary.deterministic_handoff &&
      summary.unsupported_topology_fail_closed;
  summary.ready_for_lowering_and_runtime = summary.deterministic;

  std::ostringstream out;
  out << summary.contract_id
      << ";dependency=" << summary.dependency_contract_id
      << ";async-callables=" << summary.async_callable_sites
      << ";executor-sites=" << summary.executor_affinity_sites
      << ";illegal-executor=" << summary.illegal_non_async_executor_sites
      << ";illegal-prototypes=" << summary.illegal_async_function_prototype_sites
      << ";illegal-throws=" << summary.illegal_async_throws_sites
      << ";supported-async=" << summary.supported_async_callable_sites
      << ";deterministic=" << (summary.deterministic ? "true" : "false");
  summary.replay_key = out.str();
  return summary;
}
