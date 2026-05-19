#pragma once

#include <string>
#include <vector>

#include "artifacts/objc3_frontend_artifact_diagnostics.h"
#include "artifacts/objc3_frontend_error_semantic_artifacts.h"
#include "lower/contracts/error_handling_result_bridging_contracts.h"
#include "lower/contracts/error_handling_throws_unwind_contracts.h"
#include "pipeline/objc3_frontend_types.h"

struct Objc3FrontendArtifactErrorLoweringPlan {
  Objc3ThrowsPropagationLoweringContract throws_propagation_lowering_contract;
  std::string throws_propagation_lowering_replay_key;
  Objc3ResultLikeLoweringContract result_like_lowering_contract;
  std::string result_like_lowering_replay_key;
  Objc3NSErrorBridgingLoweringContract ns_error_bridging_lowering_contract;
  std::string ns_error_bridging_lowering_replay_key;
  Objc3UnwindCleanupLoweringContract unwind_cleanup_lowering_contract;
  std::string unwind_cleanup_lowering_replay_key;
  bool deterministic_error_handling_throws_abi_propagation_lowering = false;
  std::string error_handling_throws_abi_propagation_lowering_replay_key;
  std::vector<
      objc3::artifacts::frontend::Objc3FrontendArtifactPostPipelineFailure>
      post_pipeline_failures;
};

Objc3FrontendArtifactErrorLoweringPlan
BuildObjc3FrontendArtifactErrorLoweringPlan(
    const Objc3FrontendPipelineResult &pipeline_result);
