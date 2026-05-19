#include "pipeline/frontend_source_closure_replay_keys.h"

#include <sstream>

namespace objc3c::pipeline::orchestration {

std::string BuildConcurrencyAsyncSourceClosureReplayKey(
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";async_sites=" << summary.async_keyword_sites << ":"
      << summary.async_function_sites << ":" << summary.async_method_sites
      << ";await_sites=" << summary.await_keyword_sites << ":"
      << summary.await_expression_sites
      << ";executor_sites=" << summary.executor_attribute_sites << ":"
      << summary.executor_main_sites << ":" << summary.executor_global_sites
      << ":" << summary.executor_named_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildConcurrencyActorMemberIsolationSourceClosureReplayKey(
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";actor_sites=" << summary.actor_interface_sites << ":"
      << summary.actor_method_sites << ":" << summary.actor_property_sites
      << ";nonisolated_sites=" << summary.objc_nonisolated_annotation_sites
      << ";executor_sites=" << summary.actor_member_executor_annotation_sites
      << ";async_sites=" << summary.actor_async_method_sites
      << ";metadata_sites=" << summary.actor_member_metadata_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

std::string BuildConcurrencyTaskGroupCancellationSourceClosureReplayKey(
    const Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary
        &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";async_callable_sites=" << summary.async_callable_sites
      << ";executor_sites=" << summary.executor_attribute_sites
      << ";task_creation_sites=" << summary.task_creation_sites
      << ";task_group_sites=" << summary.task_group_scope_sites << ":"
      << summary.task_group_add_task_sites << ":"
      << summary.task_group_wait_next_sites << ":"
      << summary.task_group_cancel_all_sites
      << ";cancellation_sites=" << summary.cancellation_check_sites << ":"
      << summary.cancellation_handler_sites
      << ";deterministic="
      << (summary.deterministic_handoff ? "true" : "false");
  return out.str();
}

}  // namespace objc3c::pipeline::orchestration
