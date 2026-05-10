#include "artifacts/objc3_frontend_source_closure_artifacts.h"

#include <sstream>
#include <string>
#include <vector>

#include "io/json/json_writer.h"
#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  return objc3::io::json::RenderJsonStringArray(values);
}

}  // namespace

std::string BuildConcurrencyAsyncSourceClosureSummaryJson(
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"async_keyword_sites\":" << summary.async_keyword_sites
      << ",\"async_function_sites\":" << summary.async_function_sites
      << ",\"async_method_sites\":" << summary.async_method_sites
      << ",\"await_keyword_sites\":" << summary.await_keyword_sites
      << ",\"await_expression_sites\":" << summary.await_expression_sites
      << ",\"executor_attribute_sites\":"
      << summary.executor_attribute_sites
      << ",\"executor_main_sites\":" << summary.executor_main_sites
      << ",\"executor_global_sites\":" << summary.executor_global_sites
      << ",\"executor_named_sites\":" << summary.executor_named_sites
      << ",\"async_function_source_supported\":"
      << (summary.async_function_source_supported ? "true" : "false")
      << ",\"async_method_source_supported\":"
      << (summary.async_method_source_supported ? "true" : "false")
      << ",\"await_expression_source_supported\":"
      << (summary.await_expression_source_supported ? "true" : "false")
      << ",\"executor_attribute_source_supported\":"
      << (summary.executor_attribute_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildConcurrencyActorMemberIsolationSourceClosureSummaryJson(
    const Objc3FrontendConcurrencyActorMemberIsolationSourceClosureSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"actor_interface_sites\":" << summary.actor_interface_sites
      << ",\"actor_method_sites\":" << summary.actor_method_sites
      << ",\"actor_property_sites\":" << summary.actor_property_sites
      << ",\"objc_nonisolated_annotation_sites\":"
      << summary.objc_nonisolated_annotation_sites
      << ",\"actor_member_executor_annotation_sites\":"
      << summary.actor_member_executor_annotation_sites
      << ",\"actor_async_method_sites\":" << summary.actor_async_method_sites
      << ",\"actor_member_metadata_sites\":"
      << summary.actor_member_metadata_sites
      << ",\"actor_declaration_source_supported\":"
      << (summary.actor_declaration_source_supported ? "true" : "false")
      << ",\"actor_member_source_supported\":"
      << (summary.actor_member_source_supported ? "true" : "false")
      << ",\"isolation_annotation_source_supported\":"
      << (summary.isolation_annotation_source_supported ? "true" : "false")
      << ",\"actor_metadata_surface_supported\":"
      << (summary.actor_metadata_surface_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildConcurrencyTaskGroupCancellationSourceClosureSummaryJson(
    const Objc3FrontendConcurrencyTaskGroupCancellationSourceClosureSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"async_callable_sites\":" << summary.async_callable_sites
      << ",\"executor_attribute_sites\":" << summary.executor_attribute_sites
      << ",\"task_creation_sites\":" << summary.task_creation_sites
      << ",\"task_group_scope_sites\":" << summary.task_group_scope_sites
      << ",\"task_group_add_task_sites\":"
      << summary.task_group_add_task_sites
      << ",\"task_group_wait_next_sites\":"
      << summary.task_group_wait_next_sites
      << ",\"task_group_cancel_all_sites\":"
      << summary.task_group_cancel_all_sites
      << ",\"cancellation_check_sites\":" << summary.cancellation_check_sites
      << ",\"cancellation_handler_sites\":"
      << summary.cancellation_handler_sites
      << ",\"task_creation_source_supported\":"
      << (summary.task_creation_source_supported ? "true" : "false")
      << ",\"task_group_source_supported\":"
      << (summary.task_group_source_supported ? "true" : "false")
      << ",\"cancellation_source_supported\":"
      << (summary.cancellation_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
