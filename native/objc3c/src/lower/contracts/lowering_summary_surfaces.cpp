#include "lower/objc3_lowering_contract.h"

#include "lower/metadata/lowering_metadata_helpers.h"

#include <sstream>
#include <string>

#include "sema/objc3_sema_contract_async_error_surfaces.h"
#include "sema/objc3_sema_contract_effects_flow_error_handling.h"

std::string Objc3ArcInteractionSemanticsSummary() {
  std::ostringstream out;
  // ARC interaction-semantics expansion anchor: explicit ARC mode
  // now carries one truthful semantic packet over weak/non-owning property
  // and block interactions, explicit autorelease returns, and synthesized
  // property accessor ownership packets for the supported runnable slice,
  // while generalized ARC cleanup and broader automation remain deferred.
  out << "contract=" << Expr::kObjc3ArcInteractionSemanticsContractId
      << ";source_model=" << Expr::kObjc3ArcInteractionSemanticsSourceModel
      << ";semantic_model=" << Expr::kObjc3ArcInteractionSemanticsSemanticModel
      << ";arc_inference_contract=" << Expr::kObjc3ArcInferenceLifetimeContractId
      << ";weak_unowned_lane=" << kObjc3WeakUnownedSemanticsLoweringLaneContract
      << ";retain_release_lane="
      << kObjc3RetainReleaseOperationLoweringLaneContract
      << ";autoreleasepool_lane="
      << kObjc3AutoreleasePoolScopeLoweringLaneContract
      << ";block_escape_lane=" << kObjc3BlockStorageEscapeLoweringLaneContract
      << ";synthesized_accessor_contract="
      << kObjc3ExecutableSynthesizedAccessorPropertyLoweringContractId
      << ";fail_closed_model="
      << Expr::kObjc3ArcInteractionSemanticsFailClosedModel
      << ";non_goal_model="
      << Expr::kObjc3ArcInteractionSemanticsNonGoalModel
      << ";follow_on_surface=objc3c.arc.interactionsemantics.surface.v1";
  return out.str();
}

std::string Objc3ArcLoweringAbiCleanupModelSummary() {
  std::ostringstream out;
  // ARC lowering ABI/cleanup freeze anchor: lane-C now freezes the
  // current lowering boundary as the combination of semantic ARC packets,
  // unwind-cleanup accounting, and private runtime helper entrypoints, while
  // generalized cleanup scheduling and helper-placement automation remain
  // deferred to the later lane-C implementation issues.
  out << "contract=" << kObjc3ArcLoweringAbiCleanupModelContractId
      << ";source_model=" << kObjc3ArcLoweringAbiCleanupModelSourceModel
      << ";abi_model=" << kObjc3ArcLoweringAbiCleanupModelAbiModel
      << ";cleanup_model=" << kObjc3ArcLoweringAbiCleanupModelCleanupModel
      << ";arc_mode_contract=" << Expr::kObjc3ArcModeHandlingContractId
      << ";arc_semantic_rules_contract=" << Expr::kObjc3ArcSemanticRulesContractId
      << ";arc_inference_contract=" << Expr::kObjc3ArcInferenceLifetimeContractId
      << ";arc_interaction_contract=" << Expr::kObjc3ArcInteractionSemanticsContractId
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";weak_load_symbol=" << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
      << ";weak_store_symbol=" << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
      << ";autoreleasepool_push_symbol="
      << kObjc3RuntimePushAutoreleasepoolScopeSymbol
      << ";autoreleasepool_pop_symbol="
      << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";retain_release_lane="
      << kObjc3RetainReleaseOperationLoweringLaneContract
      << ";weak_unowned_lane=" << kObjc3WeakUnownedSemanticsLoweringLaneContract
      << ";autoreleasepool_lane="
      << kObjc3AutoreleasePoolScopeLoweringLaneContract
      << ";unwind_cleanup_lane=" << kObjc3UnwindCleanupLoweringLaneContract
      << ";fail_closed_model="
      << kObjc3ArcLoweringAbiCleanupModelFailClosedModel
      << ";non_goal_model="
      << kObjc3ArcLoweringAbiCleanupModelNonGoalModel
      << ";follow_on_surface=objc3c.arc.loweringabicleanup.surface.v1";
  return out.str();
}

std::string Objc3ErrorHandlingThrowsAbiPropagationLoweringSummary() {
  std::ostringstream out;
  // Part 6 lowering freeze anchor: lane-C first froze the combined
  // throws ABI and propagation packet before the runnable implementation
  // tranche landed. ;follow_on_surface=objc3c.errors.throws.loweringboundary.v1
  // Part 6 lowering implementation anchor: lane-C now materializes
  // the runnable hidden error-out ABI, propagation operators, status/NSError
  // bridge propagation, and do/catch control-flow in real native IR/object
  // artifacts while deferring the D001 runtime-helper contract tranche.
  out << "contract=" << kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId
      << ";source_model="
      << kObjc3ErrorHandlingThrowsAbiPropagationLoweringSourceModel
      << ";abi_model=" << kObjc3ErrorHandlingThrowsAbiPropagationLoweringAbiModel
      << ";error_handling_semantic_contract="
      << kObjc3ErrorHandlingErrorSemanticModelContractId
      << ";error_handling_try_semantic_contract="
      << kObjc3ErrorHandlingTryDoCatchSemanticSummaryContractId
      << ";error_handling_bridge_legality_contract="
      << kObjc3ErrorHandlingErrorBridgeLegalitySummaryContractId
      << ";throws_lane=" << kObjc3ThrowsPropagationLoweringLaneContract
      << ";result_like_lane=" << kObjc3ResultLikeLoweringLaneContract
      << ";ns_error_lane=" << kObjc3NSErrorBridgingLoweringLaneContract
      << ";unwind_lane=" << kObjc3UnwindCleanupLoweringLaneContract
      << ";fail_closed_model="
      << kObjc3ErrorHandlingThrowsAbiPropagationLoweringFailClosedModel
      << ";non_goal_model="
      << kObjc3ErrorHandlingThrowsAbiPropagationLoweringNonGoalModel
      << ";follow_on_surface=objc3c.errors.throws.propagationlowering.v1";
  return out.str();
}

std::string Objc3ErrorHandlingResultAndBridgingArtifactReplaySummary() {
  std::ostringstream out;
  // Part 6 replay-completion anchor: lane-C now extends the live
  // C002 lowering surface with deterministic replay artifacts that survive
  // object emission, manifest emission, and emitted sidecar publication so
  // separate provider/consumer compilation can prove preserved result/bridge
  // metadata without claiming import-surface ingestion yet.
  out << "contract=" << kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId
      << ";source_contract="
      << kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId
      << ";source_model="
      << kObjc3ErrorHandlingResultAndBridgingArtifactReplaySourceModel
      << ";replay_model="
      << kObjc3ErrorHandlingResultAndBridgingArtifactReplayModel
      << ";surface_path="
      << kObjc3ErrorHandlingResultAndBridgingArtifactReplaySurfacePath
      << ";artifact_member="
      << kObjc3ErrorHandlingResultAndBridgingArtifactReplayImportArtifactMemberName
      << ";artifact_suffix="
      << kObjc3ErrorHandlingResultAndBridgingArtifactReplayArtifactSuffix
      << ";fail_closed_model="
      << kObjc3ErrorHandlingResultAndBridgingArtifactReplayFailClosedModel
      << ";follow_on_surface=objc3c.errors.resultbridging.artifactsurface.v1";
  return out.str();
}

std::string Objc3ErrorHandlingErrorRuntimeBridgeHelperSummary() {
  std::ostringstream out;
  // error-runtime/bridge-helper anchor: lane-D freezes the first
  // real private runtime helper ABI consumed by the runnable Part 6 lowering
  // so thrown-error storage, bridge normalization, and do/catch dispatch stop
  // pretending raw local slots are the runtime boundary.
  out << "contract=" << kObjc3ErrorHandlingErrorRuntimeBridgeHelperContractId
      << ";source_contract=" << kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId
      << ";replay_contract="
      << kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId
      << ";source_model=" << kObjc3ErrorHandlingErrorRuntimeBridgeHelperSourceModel
      << ";abi_model=" << kObjc3ErrorHandlingErrorRuntimeBridgeHelperAbiModel
      << ";store_symbol=" << kObjc3RuntimeStoreThrownErrorI32Symbol
      << ";load_symbol=" << kObjc3RuntimeLoadThrownErrorI32Symbol
      << ";status_bridge_symbol=" << kObjc3RuntimeBridgeStatusErrorI32Symbol
      << ";nserror_bridge_symbol=" << kObjc3RuntimeBridgeNSErrorErrorI32Symbol
      << ";catch_match_symbol=" << kObjc3RuntimeCatchMatchesErrorI32Symbol
      << ";fail_closed_model="
      << kObjc3ErrorHandlingErrorRuntimeBridgeHelperFailClosedModel
      << ";follow_on_surface=objc3c.errors.runtimebridge.helpersurface.v1";
  return out.str();
}

std::string Objc3ErrorHandlingLiveErrorRuntimeIntegrationSummary() {
  std::ostringstream out;
  // live catch/bridge/runtime integration anchor: lane-D now proves
  // the runnable Part 6 lowering does not just mention the private helper ABI;
  // linked native object code executes through that helper cluster and the
  // existing runtime-library packaging path remains sufficient for the current
  // supported error/bridge slice.
  out << "contract=" << kObjc3ErrorHandlingLiveErrorRuntimeIntegrationContractId
      << ";helper_contract=" << kObjc3ErrorHandlingErrorRuntimeBridgeHelperContractId
      << ";replay_contract="
      << kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId
      << ";source_model="
      << kObjc3ErrorHandlingLiveErrorRuntimeIntegrationSourceModel
      << ";execution_model="
      << kObjc3ErrorHandlingLiveErrorRuntimeIntegrationExecutionModel
      << ";packaging_model="
      << kObjc3ErrorHandlingLiveErrorRuntimeIntegrationPackagingModel
      << ";store_symbol=" << kObjc3RuntimeStoreThrownErrorI32Symbol
      << ";load_symbol=" << kObjc3RuntimeLoadThrownErrorI32Symbol
      << ";status_bridge_symbol=" << kObjc3RuntimeBridgeStatusErrorI32Symbol
      << ";catch_match_symbol=" << kObjc3RuntimeCatchMatchesErrorI32Symbol
      << ";snapshot_symbol=objc3_runtime_copy_error_bridge_state_for_testing"
      << ";fail_closed_model="
      << kObjc3ErrorHandlingLiveErrorRuntimeIntegrationFailClosedModel
      << ";follow_on_surface=objc3c.errors.liveruntime.integration.v1";
  return out.str();
}
