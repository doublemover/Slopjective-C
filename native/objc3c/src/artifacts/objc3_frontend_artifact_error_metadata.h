#pragma once

#include <string>

#include "artifacts/objc3_frontend_artifact_metadata_dtos.h"

namespace objc3::artifacts::frontend {

void ApplyObjc3FrontendErrorMetadata(
    Objc3IRFrontendMetadata &ir_frontend_metadata,
    const std::string &throws_propagation_lowering_replay_key,
    const Objc3ThrowsPropagationLoweringContract
        &throws_propagation_lowering_contract,
    const std::string
        &error_handling_throws_abi_propagation_lowering_replay_key,
    const std::string &result_like_lowering_replay_key,
    const Objc3ResultLikeLoweringContract &result_like_lowering_contract,
    const std::string &ns_error_bridging_lowering_replay_key,
    const Objc3NSErrorBridgingLoweringContract
        &ns_error_bridging_lowering_contract,
    const std::string &unwind_cleanup_lowering_replay_key,
    const Objc3UnwindCleanupLoweringContract &unwind_cleanup_lowering_contract,
    const objc3::artifacts::evidence::
        ErrorHandlingResultAndBridgingArtifactReplayEvidence
            &error_handling_result_and_bridging_artifact_replay_summary);

}  // namespace objc3::artifacts::frontend
