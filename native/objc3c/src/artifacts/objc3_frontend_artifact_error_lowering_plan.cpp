#include "artifacts/objc3_frontend_artifact_error_lowering_plan.h"

#include <utility>

#include "lower/contracts/error_handling_result_bridging_contracts.h"
#include "lower/contracts/error_handling_throws_unwind_contracts.h"

namespace {

using objc3::artifacts::frontend::BuildNSErrorBridgingLoweringContract;
using objc3::artifacts::frontend::BuildResultLikeLoweringContract;
using objc3::artifacts::frontend::BuildThrowsPropagationLoweringContract;
using objc3::artifacts::frontend::BuildUnwindCleanupLoweringContract;
using objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure;

void AddPostPipelineFailure(
    std::vector<Objc3FrontendArtifactPostPipelineFailure> &failures,
    const char *code,
    std::string message) {
  Objc3FrontendArtifactPostPipelineFailure failure;
  failure.present = true;
  failure.code = code == nullptr ? "" : code;
  failure.message = std::move(message);
  failures.push_back(std::move(failure));
}

}  // namespace

Objc3FrontendArtifactErrorLoweringPlan
BuildObjc3FrontendArtifactErrorLoweringPlan(
    const Objc3FrontendPipelineResult &pipeline_result) {
  Objc3FrontendArtifactErrorLoweringPlan plan;
  plan.throws_propagation_lowering_contract =
      BuildThrowsPropagationLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3ThrowsPropagationLoweringContract(
          plan.throws_propagation_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid throws propagation lowering "
        "contract");
  }
  plan.throws_propagation_lowering_replay_key =
      Objc3ThrowsPropagationLoweringReplayKey(
          plan.throws_propagation_lowering_contract);
  plan.result_like_lowering_contract =
      BuildResultLikeLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3ResultLikeLoweringContract(
          plan.result_like_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid result-like lowering contract");
  }
  plan.result_like_lowering_replay_key =
      Objc3ResultLikeLoweringReplayKey(plan.result_like_lowering_contract);
  plan.ns_error_bridging_lowering_contract =
      BuildNSErrorBridgingLoweringContract(
          pipeline_result.sema_parity_surface);
  if (!IsValidObjc3NSErrorBridgingLoweringContract(
          plan.ns_error_bridging_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid NSError bridging lowering contract");
  }
  plan.ns_error_bridging_lowering_replay_key =
      Objc3NSErrorBridgingLoweringReplayKey(
          plan.ns_error_bridging_lowering_contract);
  plan.unwind_cleanup_lowering_contract =
      BuildUnwindCleanupLoweringContract(pipeline_result.sema_parity_surface);
  if (!IsValidObjc3UnwindCleanupLoweringContract(
          plan.unwind_cleanup_lowering_contract)) {
    AddPostPipelineFailure(
        plan.post_pipeline_failures,
        "O3L300",
        "LLVM IR emission failed: invalid unwind cleanup lowering contract");
  }
  plan.unwind_cleanup_lowering_replay_key =
      Objc3UnwindCleanupLoweringReplayKey(plan.unwind_cleanup_lowering_contract);
  plan.deterministic_error_handling_throws_abi_propagation_lowering =
      plan.result_like_lowering_contract.deterministic &&
      plan.throws_propagation_lowering_contract.deterministic &&
      plan.ns_error_bridging_lowering_contract.deterministic &&
      plan.unwind_cleanup_lowering_contract.deterministic;
  plan.error_handling_throws_abi_propagation_lowering_replay_key =
      Objc3ErrorHandlingThrowsAbiPropagationLoweringSummary() +
      ";throws_replay_key=" + plan.throws_propagation_lowering_replay_key +
      ";result_like_replay_key=" + plan.result_like_lowering_replay_key +
      ";ns_error_replay_key=" + plan.ns_error_bridging_lowering_replay_key +
      ";unwind_replay_key=" + plan.unwind_cleanup_lowering_replay_key +
      ";deterministic=" +
      (plan.deterministic_error_handling_throws_abi_propagation_lowering
           ? "true"
           : "false") +
      ";ready_for_runtime_execution=true" +
      ";follow_on_surface=objc3c.errors.throws.propagationlowering.v1";
  return plan;
}
