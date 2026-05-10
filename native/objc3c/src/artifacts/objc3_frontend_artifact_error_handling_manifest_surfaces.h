#pragma once

#include <iosfwd>
#include <string>

struct Objc3NSErrorBridgingLoweringContract;
struct Objc3ResultLikeLoweringContract;
struct Objc3ThrowsPropagationLoweringContract;
struct Objc3UnwindCleanupLoweringContract;

namespace objc3::artifacts::evidence {
struct ErrorHandlingResultAndBridgingArtifactReplayEvidence;
}

namespace objc3::artifacts::frontend {

void WriteErrorHandlingManifestSurfaces(
    std::ostream &manifest,
    const Objc3ThrowsPropagationLoweringContract
        &throws_propagation_lowering_contract,
    const std::string &throws_propagation_lowering_replay_key,
    const Objc3ResultLikeLoweringContract &result_like_lowering_contract,
    const std::string &result_like_lowering_replay_key,
    const Objc3NSErrorBridgingLoweringContract
        &ns_error_bridging_lowering_contract,
    const std::string &ns_error_bridging_lowering_replay_key,
    const Objc3UnwindCleanupLoweringContract &unwind_cleanup_lowering_contract,
    const std::string &unwind_cleanup_lowering_replay_key,
    bool deterministic_error_handling_throws_abi_propagation_lowering,
    const objc3::artifacts::evidence::
        ErrorHandlingResultAndBridgingArtifactReplayEvidence
            &error_handling_result_and_bridging_artifact_replay_summary);

}  // namespace objc3::artifacts::frontend
