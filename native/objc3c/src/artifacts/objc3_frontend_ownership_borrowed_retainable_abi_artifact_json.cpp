#include "artifacts/objc3_frontend_ownership_semantic_artifacts.h"

#include <sstream>
#include <string>

#include "io/objc3_json.h"
#include "lower/contracts/ownership_system_extension_contracts.h"
#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

inline constexpr const char
    *kObjc3OwnershipBorrowedRetainableAbiCompletionArtifactModel =
        "borrowed-return-contracts-and-retainable-family-call-boundaries-now-publish-a-dedicated-ownership-abi-and-replay-packet-above-the-frozen-lowering-contract";
inline constexpr const char
    *kObjc3OwnershipBorrowedRetainableAbiCompletionProofModel =
        "the-supported-proof-slice-remains-direct-call-abi-emission-and-replay-stability-without-claiming-lane-d-runtime-helper-integration";

}  // namespace

std::string BuildOwnershipBorrowedRetainableAbiCompletionReplayKey(
    const Objc3OwnershipSystemExtensionLoweringContract &contract,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &source_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &retainable_summary) {
  std::ostringstream out;
  out << "lowering_replay_key="
      << Objc3OwnershipSystemExtensionLoweringReplayKey(contract)
      << ";returns_borrowed_attribute_sites="
      << source_summary.returns_borrowed_attribute_sites
      << ";family_retain_sites=" << retainable_summary.family_retain_sites
      << ";family_release_sites=" << retainable_summary.family_release_sites
      << ";family_autorelease_sites="
      << retainable_summary.family_autorelease_sites
      << ";compatibility_returns_retained_sites="
      << retainable_summary.compatibility_returns_retained_sites
      << ";compatibility_returns_not_retained_sites="
      << retainable_summary.compatibility_returns_not_retained_sites
      << ";compatibility_consumed_sites="
      << retainable_summary.compatibility_consumed_sites
      << ";deterministic=true;lane_contract="
      << kObjc3OwnershipBorrowedRetainableAbiCompletionLaneContract;
  return out.str();
}

std::string BuildOwnershipBorrowedRetainableAbiCompletionJson(
    const Objc3OwnershipSystemExtensionLoweringContract &contract,
    const Objc3FrontendOwnershipSystemExtensionSourceClosureSummary &source_summary,
    const Objc3FrontendOwnershipRetainableCFamilySourceCompletionSummary
        &retainable_summary,
    const std::string &lowering_replay_key,
    const std::string &abi_completion_replay_key) {
  const bool ready_for_ir_emission =
      contract.deterministic &&
      IsValidObjc3OwnershipSystemExtensionLoweringContract(contract) &&
      source_summary.returns_borrowed_attribute_sites <=
          contract.borrowed_return_callable_sites &&
      retainable_summary.family_retain_sites +
              retainable_summary.family_release_sites +
              retainable_summary.family_autorelease_sites ==
          contract.retainable_family_operation_callable_sites &&
      retainable_summary.compatibility_returns_retained_sites +
              retainable_summary.compatibility_returns_not_retained_sites +
              retainable_summary.compatibility_consumed_sites ==
          contract.retainable_family_alias_callable_sites;
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3OwnershipBorrowedRetainableAbiCompletionContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3OwnershipBorrowedRetainableAbiCompletionSurfacePath)
      << "\",\"lowering_contract_id\":\""
      << EscapeJsonString(kObjc3OwnershipSystemExtensionLoweringContractId)
      << "\",\"artifact_model\":\""
      << EscapeJsonString(kObjc3OwnershipBorrowedRetainableAbiCompletionArtifactModel)
      << "\",\"proof_model\":\""
      << EscapeJsonString(kObjc3OwnershipBorrowedRetainableAbiCompletionProofModel)
      << "\",\"lowering_replay_key\":\""
      << EscapeJsonString(lowering_replay_key)
      << "\",\"replay_key\":\"" << EscapeJsonString(abi_completion_replay_key)
      << "\",\"borrowed_parameter_sites\":"
      << contract.borrowed_parameter_sites
      << ",\"borrowed_return_callable_sites\":"
      << contract.borrowed_return_callable_sites
      << ",\"returns_borrowed_attribute_sites\":"
      << source_summary.returns_borrowed_attribute_sites
      << ",\"retainable_family_callable_sites\":"
      << contract.retainable_family_callable_sites
      << ",\"retainable_family_operation_callable_sites\":"
      << contract.retainable_family_operation_callable_sites
      << ",\"retainable_family_alias_callable_sites\":"
      << contract.retainable_family_alias_callable_sites
      << ",\"family_retain_sites\":"
      << retainable_summary.family_retain_sites
      << ",\"family_release_sites\":"
      << retainable_summary.family_release_sites
      << ",\"family_autorelease_sites\":"
      << retainable_summary.family_autorelease_sites
      << ",\"compatibility_returns_retained_sites\":"
      << retainable_summary.compatibility_returns_retained_sites
      << ",\"compatibility_returns_not_retained_sites\":"
      << retainable_summary.compatibility_returns_not_retained_sites
      << ",\"compatibility_consumed_sites\":"
      << retainable_summary.compatibility_consumed_sites
      << ",\"deterministic_handoff\":true"
      << ",\"ready_for_ir_emission\":"
      << (ready_for_ir_emission ? "true" : "false")
      << "}";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
