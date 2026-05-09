#pragma once

#include <cstddef>
#include <string>

// Error result/bridging contracts own result-like lowering, NSError out
// parameter bridging, and the replayable sidecar/import-surface artifacts that
// prove those bridging paths across object and separate-compilation boundaries.
inline constexpr const char *kObjc3ResultLikeLoweringLaneContract =
    "objc3c.result.like.lowering.v1";
inline constexpr const char *kObjc3NSErrorBridgingLoweringLaneContract =
    "objc3c.ns.error.bridging.lowering.v1";

inline constexpr const char
    *kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId =
        "objc3c.error_handling.result.and.bridging.artifact.replay.v1";
inline constexpr const char
    *kObjc3ErrorHandlingResultAndBridgingArtifactReplaySourceModel =
        "error_handling-lowering-replay-keys-survive-object-emission-manifest-emission-and-emitted-sidecar-artifacts";
inline constexpr const char
    *kObjc3ErrorHandlingResultAndBridgingArtifactReplayModel =
        "provider-and-consumer-sidecar-artifacts-preserve-result-and-bridge-replay-packets-for-separate-compilation-proof";
inline constexpr const char
    *kObjc3ErrorHandlingResultAndBridgingArtifactReplayFailClosedModel =
        "missing-or-drifted-result-bridge-replay-sidecars-disable-separate-compilation-proof";
inline constexpr const char
    *kObjc3ErrorHandlingResultAndBridgingArtifactReplaySurfacePath =
        "frontend.pipeline.semantic_surface.objc_error_handling_result_and_bridging_artifact_replay";
inline constexpr const char
    *kObjc3ErrorHandlingResultAndBridgingArtifactReplayImportArtifactMemberName =
        "objc_error_handling_result_and_bridging_artifact_replay";
inline constexpr const char
    *kObjc3ErrorHandlingResultAndBridgingArtifactReplayArtifactSuffix =
        ".error_handling-error-replay.json";

struct Objc3ResultLikeLoweringContract {
  std::size_t result_like_sites = 0;
  std::size_t result_success_sites = 0;
  std::size_t result_failure_sites = 0;
  std::size_t result_branch_sites = 0;
  std::size_t result_payload_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t branch_merge_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

struct Objc3NSErrorBridgingLoweringContract {
  std::size_t ns_error_bridging_sites = 0;
  std::size_t ns_error_parameter_sites = 0;
  std::size_t ns_error_out_parameter_sites = 0;
  std::size_t ns_error_bridge_path_sites = 0;
  std::size_t failable_call_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t bridge_boundary_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

std::string Objc3ErrorHandlingResultAndBridgingArtifactReplaySummary();

bool IsValidObjc3ResultLikeLoweringContract(
    const Objc3ResultLikeLoweringContract &contract);
std::string Objc3ResultLikeLoweringReplayKey(
    const Objc3ResultLikeLoweringContract &contract);
bool IsValidObjc3NSErrorBridgingLoweringContract(
    const Objc3NSErrorBridgingLoweringContract &contract);
std::string Objc3NSErrorBridgingLoweringReplayKey(
    const Objc3NSErrorBridgingLoweringContract &contract);
