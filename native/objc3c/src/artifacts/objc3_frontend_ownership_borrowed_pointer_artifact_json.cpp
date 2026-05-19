#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"

#include <sstream>

#include "io/objc3_json.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildOwnershipBorrowedPointerEscapeAnalysisSummaryJson(
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary &summary) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\"" << EscapeJsonString(summary.contract_id)
      << "\",\"dependency_contract_id\":\""
      << EscapeJsonString(summary.dependency_contract_id)
      << "\",\"surface_path\":\"" << EscapeJsonString(summary.surface_path)
      << "\",\"semantic_model\":\"" << EscapeJsonString(summary.semantic_model)
      << "\",\"deferred_model\":\"" << EscapeJsonString(summary.deferred_model)
      << "\",\"borrowed_parameter_sites\":" << summary.borrowed_parameter_sites
      << ",\"borrowed_return_callable_sites\":"
      << summary.borrowed_return_callable_sites
      << ",\"borrowed_escape_candidate_sites\":"
      << summary.borrowed_escape_candidate_sites
      << ",\"illegal_unproven_call_escape_sites\":"
      << summary.illegal_unproven_call_escape_sites
      << ",\"illegal_escaping_block_capture_sites\":"
      << summary.illegal_escaping_block_capture_sites
      << ",\"illegal_borrowed_return_sites\":"
      << summary.illegal_borrowed_return_sites
      << ",\"dependency_required\":"
      << (summary.dependency_required ? "true" : "false")
      << ",\"borrowed_call_boundary_enforced\":"
      << (summary.borrowed_call_boundary_enforced ? "true" : "false")
      << ",\"escaping_block_capture_fail_closed\":"
      << (summary.escaping_block_capture_fail_closed ? "true" : "false")
      << ",\"borrowed_return_contract_enforced\":"
      << (summary.borrowed_return_contract_enforced ? "true" : "false")
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
