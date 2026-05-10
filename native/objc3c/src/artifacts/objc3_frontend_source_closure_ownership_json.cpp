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

std::string BuildOwnershipSystemExtensionSourceClosureSummaryJson(
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"resource_attribute_sites\":" << summary.resource_attribute_sites
      << ",\"resource_close_clause_sites\":"
      << summary.resource_close_clause_sites
      << ",\"resource_invalid_clause_sites\":"
      << summary.resource_invalid_clause_sites
      << ",\"borrowed_pointer_sites\":" << summary.borrowed_pointer_sites
      << ",\"returns_borrowed_attribute_sites\":"
      << summary.returns_borrowed_attribute_sites
      << ",\"explicit_capture_list_sites\":"
      << summary.explicit_capture_list_sites
      << ",\"explicit_capture_item_sites\":"
      << summary.explicit_capture_item_sites
      << ",\"explicit_capture_weak_sites\":"
      << summary.explicit_capture_weak_sites
      << ",\"explicit_capture_unowned_sites\":"
      << summary.explicit_capture_unowned_sites
      << ",\"explicit_capture_move_sites\":"
      << summary.explicit_capture_move_sites
      << ",\"explicit_capture_plain_sites\":"
      << summary.explicit_capture_plain_sites
      << ",\"resource_attribute_source_supported\":"
      << (summary.resource_attribute_source_supported ? "true" : "false")
      << ",\"borrowed_pointer_source_supported\":"
      << (summary.borrowed_pointer_source_supported ? "true" : "false")
      << ",\"returns_borrowed_source_supported\":"
      << (summary.returns_borrowed_source_supported ? "true" : "false")
      << ",\"explicit_capture_list_source_supported\":"
      << (summary.explicit_capture_list_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildOwnershipCleanupResourceCaptureSourceCompletionSummaryJson(
    const Objc3FrontendOwnershipCleanupResourceCaptureSourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"cleanup_attribute_sites\":" << summary.cleanup_attribute_sites
      << ",\"cleanup_sugar_sites\":" << summary.cleanup_sugar_sites
      << ",\"resource_attribute_sites\":" << summary.resource_attribute_sites
      << ",\"resource_sugar_sites\":" << summary.resource_sugar_sites
      << ",\"resource_close_clause_sites\":"
      << summary.resource_close_clause_sites
      << ",\"resource_invalid_clause_sites\":"
      << summary.resource_invalid_clause_sites
      << ",\"explicit_capture_list_sites\":"
      << summary.explicit_capture_list_sites
      << ",\"explicit_capture_item_sites\":"
      << summary.explicit_capture_item_sites
      << ",\"explicit_capture_weak_sites\":"
      << summary.explicit_capture_weak_sites
      << ",\"explicit_capture_unowned_sites\":"
      << summary.explicit_capture_unowned_sites
      << ",\"explicit_capture_move_sites\":"
      << summary.explicit_capture_move_sites
      << ",\"explicit_capture_plain_sites\":"
      << summary.explicit_capture_plain_sites
      << ",\"cleanup_attribute_source_supported\":"
      << (summary.cleanup_attribute_source_supported ? "true" : "false")
      << ",\"resource_sugar_source_supported\":"
      << (summary.resource_sugar_source_supported ? "true" : "false")
      << ",\"explicit_capture_list_source_supported\":"
      << (summary.explicit_capture_list_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

std::string BuildOwnershipRetainableCFamilySourceCompletionSummaryJson(
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << summary.contract_id
      << "\",\"frontend_surface_path\":\"" << summary.frontend_surface_path
      << "\",\"source_model\":\"" << summary.source_model
      << "\",\"failure_model\":\"" << summary.failure_model
      << "\",\"source_only_claim_ids\":"
      << BuildStringArrayJson(summary.source_only_claim_ids)
      << ",\"family_retain_sites\":" << summary.family_retain_sites
      << ",\"family_release_sites\":" << summary.family_release_sites
      << ",\"family_autorelease_sites\":"
      << summary.family_autorelease_sites
      << ",\"compatibility_returns_retained_sites\":"
      << summary.compatibility_returns_retained_sites
      << ",\"compatibility_returns_not_retained_sites\":"
      << summary.compatibility_returns_not_retained_sites
      << ",\"compatibility_consumed_sites\":"
      << summary.compatibility_consumed_sites
      << ",\"callable_annotation_source_supported\":"
      << (summary.callable_annotation_source_supported ? "true" : "false")
      << ",\"compatibility_alias_source_supported\":"
      << (summary.compatibility_alias_source_supported ? "true" : "false")
      << ",\"deterministic_handoff\":"
      << (summary.deterministic_handoff ? "true" : "false")
      << ",\"ready_for_semantic_expansion\":"
      << (summary.ready_for_semantic_expansion ? "true" : "false")
      << ",\"replay_key\":\"" << EscapeJsonString(summary.replay_key)
      << "\"}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
