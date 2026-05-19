#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"

#include <sstream>

#include "io/objc3_json.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildOwnershipResourceMoveUseAfterMoveSemanticsSummaryJson(
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"cleanup_owned_local_sites\":"
      << summary.cleanup_owned_local_sites
      << ",\"resource_move_capture_sites\":"
      << summary.resource_move_capture_sites
      << ",\"illegal_non_resource_move_sites\":"
      << summary.illegal_non_resource_move_sites
      << ",\"illegal_use_after_move_sites\":"
      << summary.illegal_use_after_move_sites
      << ",\"illegal_duplicate_move_sites\":"
      << summary.illegal_duplicate_move_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"cleanup_ownership_transfer_enforced\":"
      << (summary.cleanup_ownership_transfer_enforced ? "true" : "false")
      << ",\"use_after_move_fail_closed\":"
      << (summary.use_after_move_fail_closed ? "true" : "false")
      << ",\"duplicate_move_fail_closed\":"
      << (summary.duplicate_move_fail_closed ? "true" : "false")
      << ",\"borrowed_escape_semantics_deferred\":"
      << (summary.borrowed_escape_semantics_deferred ? "true" : "false")
      << ",\"retainable_family_legality_deferred\":"
      << (summary.retainable_family_legality_deferred ? "true" : "false")
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
