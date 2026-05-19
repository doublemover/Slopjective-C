#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"

#include <sstream>
#include <string>

#include "artifacts/objc3_frontend_artifact_semantic_contract_records.h"
#include "io/objc3_json.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

}  // namespace

std::string BuildOwnershipSystemExtensionLoweringContractJson(
    const Objc3OwnershipSystemExtensionSemanticModelSummary &semantic_summary,
    const Objc3OwnershipResourceMoveUseAfterMoveSemanticsSummary &resource_summary,
    const Objc3OwnershipBorrowedPointerEscapeAnalysisSummary &borrowed_summary,
    const Objc3OwnershipCaptureListRetainableFamilyLegalityCompletionSummary
        &family_summary,
    const Objc3OwnershipSystemExtensionLoweringContract &contract,
    const std::string &replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3OwnershipSystemExtensionLoweringContract(contract);
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringSurfacePath)
      << "\",\"semantic_contract_id\":\""
      << EscapeJsonString(semantic_summary.contract_id)
      << "\",\"resource_semantic_contract_id\":\""
      << EscapeJsonString(resource_summary.contract_id)
      << "\",\"borrowed_semantic_contract_id\":\""
      << EscapeJsonString(borrowed_summary.contract_id)
      << "\",\"family_semantic_contract_id\":\""
      << EscapeJsonString(family_summary.contract_id)
      << "\",\"lane_contract_id\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringLaneContract)
      << "\",\"lowering_model\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringDeferredModel)
      << "\",\"replay_key\":\"" << EscapeJsonString(replay_key)
      << "\",\"cleanup_hook_sites\":" << contract.cleanup_hook_sites
      << ",\"resource_local_sites\":" << contract.resource_local_sites
      << ",\"cleanup_owned_local_sites\":"
      << contract.cleanup_owned_local_sites
      << ",\"resource_move_capture_sites\":"
      << contract.resource_move_capture_sites
      << ",\"borrowed_parameter_sites\":" << contract.borrowed_parameter_sites
      << ",\"borrowed_return_callable_sites\":"
      << contract.borrowed_return_callable_sites
      << ",\"borrowed_escape_candidate_sites\":"
      << contract.borrowed_escape_candidate_sites
      << ",\"explicit_capture_item_sites\":"
      << contract.explicit_capture_item_sites
      << ",\"retainable_family_callable_sites\":"
      << contract.retainable_family_callable_sites
      << ",\"retainable_family_operation_callable_sites\":"
      << contract.retainable_family_operation_callable_sites
      << ",\"retainable_family_alias_callable_sites\":"
      << contract.retainable_family_alias_callable_sites
      << ",\"guard_blocked_sites\":" << contract.guard_blocked_sites
      << ",\"contract_violation_sites\":"
      << contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (contract.deterministic ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
