#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"

#include <sstream>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildEffectsOwnershipSemanticModelSummaryJson(
    const Objc3EffectsOwnershipSemanticModelSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"arc_ownership_qualified_sites\":"
      << summary.arc_ownership_qualified_sites
      << ",\"retain_insertion_sites\":" << summary.retain_insertion_sites
      << ",\"release_insertion_sites\":" << summary.release_insertion_sites
      << ",\"autorelease_insertion_sites\":"
      << summary.autorelease_insertion_sites
      << ",\"weak_zeroing_sites\":" << summary.weak_zeroing_sites
      << ",\"unowned_reference_sites\":" << summary.unowned_reference_sites
      << ",\"weak_unowned_conflict_sites\":"
      << summary.weak_unowned_conflict_sites
      << ",\"autoreleasepool_scope_sites\":"
      << summary.autoreleasepool_scope_sites
      << ",\"cleanup_order_exit_sites\":" << summary.cleanup_order_exit_sites
      << ",\"block_literal_sites\":" << summary.block_literal_sites
      << ",\"stack_to_heap_promotion_sites\":"
      << summary.stack_to_heap_promotion_sites
      << ",\"byref_forwarding_cell_sites\":"
      << summary.byref_forwarding_cell_sites
      << ",\"copy_helper_required_sites\":"
      << summary.copy_helper_required_sites
      << ",\"dispose_helper_required_sites\":"
      << summary.dispose_helper_required_sites
      << ",\"copy_helper_symbolized_sites\":"
      << summary.copy_helper_symbolized_sites
      << ",\"dispose_helper_symbolized_sites\":"
      << summary.dispose_helper_symbolized_sites
      << ",\"captured_object_lifetime_sites\":"
      << summary.captured_object_lifetime_sites
      << ",\"throws_propagation_sites\":"
      << summary.throws_propagation_sites
      << ",\"unwind_cleanup_sites\":" << summary.unwind_cleanup_sites
      << ",\"bridged_error_sites\":" << summary.bridged_error_sites
      << ",\"nested_cleanup_sites\":" << summary.nested_cleanup_sites
      << ",\"foreign_boundary_sites\":" << summary.foreign_boundary_sites
      << ",\"async_continuation_sites\":"
      << summary.async_continuation_sites
      << ",\"continuation_resume_sites\":"
      << summary.continuation_resume_sites
      << ",\"continuation_suspend_sites\":"
      << summary.continuation_suspend_sites
      << ",\"async_state_machine_sites\":"
      << summary.async_state_machine_sites
      << ",\"cancellation_propagation_sites\":"
      << summary.cancellation_propagation_sites
      << ",\"actor_isolation_sites\":" << summary.actor_isolation_sites
      << ",\"actor_hop_sites\":" << summary.actor_hop_sites
      << ",\"sendability_check_sites\":" << summary.sendability_check_sites
      << ",\"reentrancy_policy_sites\":"
      << summary.reentrancy_policy_sites
      << ",\"imported_actor_api_sites\":"
      << summary.imported_actor_api_sites
      << ",\"contract_violation_sites\":"
      << summary.contract_violation_sites
      << ",\"arc_semantics_landed\":"
      << (summary.arc_semantics_landed ? "true" : "false")
      << ",\"block_escape_semantics_landed\":"
      << (summary.block_escape_semantics_landed ? "true" : "false")
      << ",\"throws_cleanup_semantics_landed\":"
      << (summary.throws_cleanup_semantics_landed ? "true" : "false")
      << ",\"async_task_semantics_landed\":"
      << (summary.async_task_semantics_landed ? "true" : "false")
      << ",\"actor_semantics_landed\":"
      << (summary.actor_semantics_landed ? "true" : "false")
      << ",\"foreign_boundary_semantics_landed\":"
      << (summary.foreign_boundary_semantics_landed ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\",\"failure_reason\":\""
      << EscapeJsonString(summary.failure_reason) << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
