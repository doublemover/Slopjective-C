#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"

#include <sstream>

#include "io/objc3_json.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildOwnershipCaptureListRetainableFamilyLegalityCompletionSummaryJson(
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"explicit_capture_list_sites\":"
      << summary.explicit_capture_list_sites
      << ",\"explicit_capture_item_sites\":"
      << summary.explicit_capture_item_sites
      << ",\"explicit_capture_ownership_mode_sites\":"
      << summary.explicit_capture_ownership_mode_sites
      << ",\"retainable_family_callable_sites\":"
      << summary.retainable_family_callable_sites
      << ",\"retainable_family_operation_callable_sites\":"
      << summary.retainable_family_operation_callable_sites
      << ",\"retainable_family_alias_callable_sites\":"
      << summary.retainable_family_alias_callable_sites
      << ",\"illegal_duplicate_explicit_capture_sites\":"
      << summary.illegal_duplicate_explicit_capture_sites
      << ",\"illegal_non_object_capture_mode_sites\":"
      << summary.illegal_non_object_capture_mode_sites
      << ",\"illegal_unused_explicit_capture_sites\":"
      << summary.illegal_unused_explicit_capture_sites
      << ",\"illegal_conflicting_retainable_family_sites\":"
      << summary.illegal_conflicting_retainable_family_sites
      << ",\"illegal_invalid_family_operation_shape_sites\":"
      << summary.illegal_invalid_family_operation_shape_sites
      << ",\"illegal_invalid_family_alias_shape_sites\":"
      << summary.illegal_invalid_family_alias_shape_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"explicit_capture_duplicate_fail_closed\":"
      << (summary.explicit_capture_duplicate_fail_closed ? "true" : "false")
      << ",\"explicit_capture_ownership_mode_enforced\":"
      << (summary.explicit_capture_ownership_mode_enforced ? "true" : "false")
      << ",\"explicit_capture_inventory_enforced\":"
      << (summary.explicit_capture_inventory_enforced ? "true" : "false")
      << ",\"retainable_family_conflict_enforced\":"
      << (summary.retainable_family_conflict_enforced ? "true" : "false")
      << ",\"retainable_family_operation_shape_enforced\":"
      << (summary.retainable_family_operation_shape_enforced ? "true" : "false")
      << ",\"retainable_family_alias_shape_enforced\":"
      << (summary.retainable_family_alias_shape_enforced ? "true" : "false")
      << ",\"lowering_runtime_deferred\":"
      << (summary.lowering_runtime_deferred ? "true" : "false")
      << ",\"deterministic\":" << (summary.deterministic ? "true" : "false")
      << ",\"ready_for_lowering_and_runtime\":"
      << (summary.ready_for_lowering_and_runtime ? "true" : "false")
      << ",\"failure_reason\":\"" << EscapeJsonString(summary.failure_reason)
      << "\",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
