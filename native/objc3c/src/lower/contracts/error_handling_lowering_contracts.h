#pragma once

#include <cstddef>
#include <string>

// Error-handling lowering owns the private runtime helper ABI for thrown
// errors, status/NSError bridging, catch matching, and the replayable lowering
// contracts that publish those boundaries.
inline constexpr const char *kObjc3RuntimeStoreThrownErrorI32Symbol =
    "objc3_runtime_store_thrown_error_i32";
inline constexpr const char *kObjc3RuntimeLoadThrownErrorI32Symbol =
    "objc3_runtime_load_thrown_error_i32";
inline constexpr const char *kObjc3RuntimeBridgeStatusErrorI32Symbol =
    "objc3_runtime_bridge_status_error_i32";
inline constexpr const char *kObjc3RuntimeBridgeNSErrorErrorI32Symbol =
    "objc3_runtime_bridge_nserror_error_i32";
inline constexpr const char *kObjc3RuntimeCatchMatchesErrorI32Symbol =
    "objc3_runtime_catch_matches_error_i32";

inline constexpr const char *kObjc3ThrowsPropagationLoweringLaneContract =
    "objc3c.throws.propagation.lowering.v1";
inline constexpr const char *kObjc3ResultLikeLoweringLaneContract =
    "objc3c.result.like.lowering.v1";
inline constexpr const char *kObjc3NSErrorBridgingLoweringLaneContract =
    "objc3c.ns.error.bridging.lowering.v1";
inline constexpr const char *kObjc3UnwindCleanupLoweringLaneContract =
    "objc3c.unwind.cleanup.lowering.v1";

inline constexpr const char
    *kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId =
        "objc3c.error_handling.throws.abi.propagation.lowering.v1";
inline constexpr const char
    *kObjc3ErrorHandlingThrowsAbiPropagationLoweringSourceModel =
        "error_handling-semantic-packets-feed-runnable-error-out-abi-propagation-and-catch-dispatch-lowering";
inline constexpr const char
    *kObjc3ErrorHandlingThrowsAbiPropagationLoweringAbiModel =
        "native-lowering-emits-hidden-error-out-abi-propagation-operators-and-do-catch-control-flow-through-real-ir-and-object-artifacts";
inline constexpr const char
    *kObjc3ErrorHandlingThrowsAbiPropagationLoweringFailClosedModel =
        "generalized-foreign-exception-abi-and-runtime-bridge-helper-contract-remain-deferred-to-the-error_handling-error-runtime-bridge-helper-boundary";
inline constexpr const char
    *kObjc3ErrorHandlingThrowsAbiPropagationLoweringNonGoalModel =
        "no-generalized-foreign-exception-abi-no-stable-cross-module-replay-claim-yet";

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

inline constexpr const char
    *kObjc3ErrorHandlingErrorRuntimeBridgeHelperContractId =
        "objc3c.error_handling.error.runtime.and.bridge.helper.api.v1";
inline constexpr const char
    *kObjc3ErrorHandlingErrorRuntimeBridgeHelperSourceModel =
        "error_handling-lowering-routes-error-storage-bridge-normalization-and-catch-dispatch-through-private-runtime-helpers";
inline constexpr const char *kObjc3ErrorHandlingErrorRuntimeBridgeHelperAbiModel =
    "i32-backed-native-error-object-handles-and-catch-kind-matching-remain-private-bootstrap-internal-runtime-abi";
inline constexpr const char
    *kObjc3ErrorHandlingErrorRuntimeBridgeHelperFailClosedModel =
        "no-public-error_handling-error-runtime-header-widening-no-generalized-foreign-exception-abi-yet";

inline constexpr const char
    *kObjc3ErrorHandlingLiveErrorRuntimeIntegrationContractId =
        "objc3c.error_handling.live.error.runtime.integration.v1";
inline constexpr const char
    *kObjc3ErrorHandlingLiveErrorRuntimeIntegrationSourceModel =
        "runnable-error_handling-object-code-links-and-executes-through-the-private-error-runtime-helper-cluster";
inline constexpr const char
    *kObjc3ErrorHandlingLiveErrorRuntimeIntegrationExecutionModel =
        "linked-native-error_handling-fixtures-drive-status-bridge-thrown-error-store-load-and-catch-dispatch-through-runtime-owned-helpers";
inline constexpr const char
    *kObjc3ErrorHandlingLiveErrorRuntimeIntegrationPackagingModel =
        "driver-emitted-object-and-registration-manifest-artifacts-preserve-runtime-library-link-inputs-for-runnable-error_handling-probes";
inline constexpr const char
    *kObjc3ErrorHandlingLiveErrorRuntimeIntegrationFailClosedModel =
        "no-public-error-runtime-abi-no-generalized-foreign-exception-support-no-cross-module-live-claim-yet";

struct Objc3ThrowsPropagationLoweringContract {
  std::size_t throws_propagation_sites = 0;
  std::size_t namespace_segment_sites = 0;
  std::size_t import_edge_candidate_sites = 0;
  std::size_t object_pointer_type_sites = 0;
  std::size_t pointer_declarator_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t cache_invalidation_candidate_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

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

struct Objc3UnwindCleanupLoweringContract {
  std::size_t unwind_cleanup_sites = 0;
  std::size_t unwind_edge_sites = 0;
  std::size_t cleanup_scope_sites = 0;
  std::size_t cleanup_emit_sites = 0;
  std::size_t landing_pad_sites = 0;
  std::size_t cleanup_resume_sites = 0;
  std::size_t normalized_sites = 0;
  std::size_t guard_blocked_sites = 0;
  std::size_t contract_violation_sites = 0;
  bool deterministic = true;
};

std::string Objc3ErrorHandlingThrowsAbiPropagationLoweringSummary();
std::string Objc3ErrorHandlingResultAndBridgingArtifactReplaySummary();
std::string Objc3ErrorHandlingErrorRuntimeBridgeHelperSummary();
std::string Objc3ErrorHandlingLiveErrorRuntimeIntegrationSummary();

bool IsValidObjc3ThrowsPropagationLoweringContract(
    const Objc3ThrowsPropagationLoweringContract &contract);
std::string Objc3ThrowsPropagationLoweringReplayKey(
    const Objc3ThrowsPropagationLoweringContract &contract);
bool IsValidObjc3ResultLikeLoweringContract(
    const Objc3ResultLikeLoweringContract &contract);
std::string Objc3ResultLikeLoweringReplayKey(
    const Objc3ResultLikeLoweringContract &contract);
bool IsValidObjc3NSErrorBridgingLoweringContract(
    const Objc3NSErrorBridgingLoweringContract &contract);
std::string Objc3NSErrorBridgingLoweringReplayKey(
    const Objc3NSErrorBridgingLoweringContract &contract);
bool IsValidObjc3UnwindCleanupLoweringContract(
    const Objc3UnwindCleanupLoweringContract &contract);
std::string Objc3UnwindCleanupLoweringReplayKey(
    const Objc3UnwindCleanupLoweringContract &contract);
