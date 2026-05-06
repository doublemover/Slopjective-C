
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

std::string Objc3OwnershipSystemHelperRuntimeContractSummary() {
  std::ostringstream out;
  // runtime/helper-freeze anchor: lane-D freezes the currently
  // supported Part 8 runtime/helper proof as a reuse boundary over the private
  // ARC/autorelease helper cluster and testing snapshots. Cleanup execution and
  // resource invalidation still ride existing cleanup lowering plus
  // autoreleasepool state; retainable-family helper integration rides the same
  // retain/release/autorelease entrypoints; no public runtime ABI widening or
  // new Part 8 import surface is claimed here.
  out << "contract=" << kObjc3OwnershipSystemHelperRuntimeContractId
      << ";source_contract=" << kObjc3OwnershipSystemExtensionLoweringContractId
      << ";abi_completion_contract="
      << kObjc3OwnershipBorrowedRetainableAbiCompletionContractId
      << ";source_model=" << kObjc3OwnershipSystemHelperRuntimeSourceModel
      << ";abi_model=" << kObjc3OwnershipSystemHelperRuntimeAbiModel
      << ";packaging_model=" << kObjc3OwnershipSystemHelperRuntimePackagingModel
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";autoreleasepool_push_symbol="
      << kObjc3RuntimePushAutoreleasepoolScopeSymbol
      << ";autoreleasepool_pop_symbol="
      << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";memory_snapshot_symbol="
      << "objc3_runtime_copy_memory_management_state_for_testing"
      << ";arc_debug_snapshot_symbol="
      << "objc3_runtime_copy_arc_debug_state_for_testing"
      << ";fail_closed_model=" << kObjc3OwnershipSystemHelperRuntimeFailClosedModel
      << ";follow_on_surface=objc3c.ownership.systemhelper.runtime.v1";
  return out.str();
}

std::string Objc3OwnershipLiveCleanupRetainableIntegrationSummary() {
  std::ostringstream out;
  // live runtime-integration anchor: the supported Part 8 cleanup /
  // retainable-family slice now proves linked execution through emitted
  // scope-exit cleanup calls and the private ARC/autorelease helper cluster
  // frozen in D001. This remains a narrow executable slice, not a broader
  // borrowed-lifetime or escaping-ownership runtime claim.
  out << "contract=" << kObjc3OwnershipLiveCleanupRetainableIntegrationContractId
      << ";helper_contract=" << kObjc3OwnershipSystemHelperRuntimeContractId
      << ";abi_completion_contract="
      << kObjc3OwnershipBorrowedRetainableAbiCompletionContractId
      << ";source_model="
      << kObjc3OwnershipLiveCleanupRetainableIntegrationSourceModel
      << ";execution_model="
      << kObjc3OwnershipLiveCleanupRetainableIntegrationExecutionModel
      << ";packaging_model="
      << kObjc3OwnershipLiveCleanupRetainableIntegrationPackagingModel
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";autoreleasepool_push_symbol="
      << kObjc3RuntimePushAutoreleasepoolScopeSymbol
      << ";autoreleasepool_pop_symbol="
      << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";memory_snapshot_symbol="
      << "objc3_runtime_copy_memory_management_state_for_testing"
      << ";arc_debug_snapshot_symbol="
      << "objc3_runtime_copy_arc_debug_state_for_testing"
      << ";fail_closed_model="
      << kObjc3OwnershipLiveCleanupRetainableIntegrationFailClosedModel
      << ";follow_on_surface=objc3c.ownership.livecleanup.integration.v1";
  return out.str();
}

std::string Objc3ConcurrencyContinuationRuntimeHelperSummary() {
  std::ostringstream out;
  // continuation/runtime-helper anchor: lane-D freezes the first
  // real private Part 7 helper ABI. The helper cluster now allocates logical
  // continuation handles and supports deterministic handoff/resume probes even
  // though the current direct-call async lowering slice still does not consume
  // those helpers for live suspension.
  out << "contract=" << kObjc3ConcurrencyContinuationRuntimeHelperContractId
      << ";source_contract="
      << "objc3c.concurrency.continuation.abi.async.lowering.contract.v1"
      << ";direct_call_contract="
      << "objc3c.concurrency.async.direct.call.lowering.v1"
      << ";cleanup_contract="
      << "objc3c.concurrency.suspension.autorelease.cleanup.integration.v1"
      << ";source_model=" << kObjc3ConcurrencyContinuationRuntimeHelperSourceModel
      << ";abi_model=" << kObjc3ConcurrencyContinuationRuntimeHelperAbiModel
      << ";execution_model="
      << kObjc3ConcurrencyContinuationRuntimeHelperExecutionModel
      << ";allocate_symbol="
      << kObjc3RuntimeAllocateAsyncContinuationI32Symbol
      << ";handoff_symbol="
      << kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol
      << ";resume_symbol=" << kObjc3RuntimeResumeAsyncContinuationI32Symbol
      << ";snapshot_symbol=objc3_runtime_copy_async_continuation_state_for_testing"
      << ";fail_closed_model="
      << kObjc3ConcurrencyContinuationRuntimeHelperFailClosedModel
      << ";follow_on_surface=objc3c.concurrency.continuation.runtimehelpersurface.v1";
  return out.str();
}

std::string Objc3ConcurrencyLiveContinuationRuntimeIntegrationSummary() {
  std::ostringstream out;
  // live continuation/runtime integration anchor: the supported
  // non-suspending async slice now executes through the private continuation
  // helper cluster rather than merely advertising its ABI boundary in IR.
  out << "contract=" << kObjc3ConcurrencyLiveContinuationRuntimeIntegrationContractId
      << ";helper_contract=" << kObjc3ConcurrencyContinuationRuntimeHelperContractId
      << ";direct_call_contract="
      << "objc3c.concurrency.async.direct.call.lowering.v1"
      << ";cleanup_contract="
      << "objc3c.concurrency.suspension.autorelease.cleanup.integration.v1"
      << ";source_model="
      << kObjc3ConcurrencyLiveContinuationRuntimeIntegrationSourceModel
      << ";execution_model="
      << kObjc3ConcurrencyLiveContinuationRuntimeIntegrationExecutionModel
      << ";packaging_model="
      << kObjc3ConcurrencyLiveContinuationRuntimeIntegrationPackagingModel
      << ";allocate_symbol="
      << kObjc3RuntimeAllocateAsyncContinuationI32Symbol
      << ";handoff_symbol="
      << kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol
      << ";resume_symbol=" << kObjc3RuntimeResumeAsyncContinuationI32Symbol
      << ";snapshot_symbol=objc3_runtime_copy_async_continuation_state_for_testing"
      << ";fail_closed_model="
      << kObjc3ConcurrencyLiveContinuationRuntimeIntegrationFailClosedModel
      << ";follow_on_surface=objc3c.concurrency.continuation.liveruntimeintegration.v1";
  return out.str();
}

std::string Objc3ConcurrencySchedulerExecutorRuntimeSummary() {
  std::ostringstream out;
  out << "contract=" << kObjc3ConcurrencySchedulerExecutorRuntimeContractId
      << ";source_model=" << kObjc3ConcurrencySchedulerExecutorRuntimeSourceModel
      << ";abi_model=" << kObjc3ConcurrencySchedulerExecutorRuntimeAbiModel
      << ";execution_model="
      << kObjc3ConcurrencySchedulerExecutorRuntimeExecutionModel
      << ";packaging_model="
      << kObjc3ConcurrencySchedulerExecutorRuntimePackagingModel
      << ";helper_cluster="
      << "spawn_task,enter_task_group_scope,add_task_group_task,"
         "wait_task_group_next,cancel_task_group,task_is_cancelled,"
         "task_on_cancel,executor_hop"
      << ";snapshot_symbol=objc3_runtime_copy_task_runtime_state_for_testing"
      << ";fail_closed_model="
      << kObjc3ConcurrencySchedulerExecutorRuntimeFailClosedModel
      << ";follow_on_surface=objc3c.concurrency.schedulerexecutor.runtime.v1";
  return out.str();
}

std::string Objc3ConcurrencyLiveTaskRuntimeIntegrationSummary() {
  std::ostringstream out;
  // live task runtime anchor: the private helper cluster frozen in
  // D001 now serves as a live execution surface for the supported task spawn,
  // task-group, cancellation, and executor-hop slice rather than only an ABI
  // contract. Front-door metadata export gates remain deferred outside D002.
  out << "contract=" << kObjc3ConcurrencyLiveTaskRuntimeIntegrationContractId
      << ";helper_contract=" << kObjc3ConcurrencySchedulerExecutorRuntimeContractId
      << ";lowering_contract="
      << "objc3c.concurrency.task.runtime.lowering.implementation.v1"
      << ";abi_contract="
      << "objc3c.concurrency.task.runtime.abi.completion.v1"
      << ";source_model=" << kObjc3ConcurrencyLiveTaskRuntimeIntegrationSourceModel
      << ";execution_model="
      << kObjc3ConcurrencyLiveTaskRuntimeIntegrationExecutionModel
      << ";packaging_model="
      << kObjc3ConcurrencyLiveTaskRuntimeIntegrationPackagingModel
      << ";spawn_symbol=" << kObjc3RuntimeSpawnTaskI32Symbol
      << ";scope_symbol=" << kObjc3RuntimeEnterTaskGroupScopeI32Symbol
      << ";add_symbol=" << kObjc3RuntimeAddTaskGroupTaskI32Symbol
      << ";wait_symbol=" << kObjc3RuntimeWaitTaskGroupNextI32Symbol
      << ";cancel_symbol=" << kObjc3RuntimeCancelTaskGroupI32Symbol
      << ";cancelled_symbol=" << kObjc3RuntimeTaskIsCancelledI32Symbol
      << ";on_cancel_symbol=" << kObjc3RuntimeTaskOnCancelI32Symbol
      << ";executor_hop_symbol=" << kObjc3RuntimeExecutorHopI32Symbol
      << ";snapshot_symbol=objc3_runtime_copy_task_runtime_state_for_testing"
      << ";fail_closed_model="
      << kObjc3ConcurrencyLiveTaskRuntimeIntegrationFailClosedModel
      << ";follow_on_surface=objc3c.concurrency.taskruntime.liveintegration.v1";
  return out.str();
}

std::string Objc3ConcurrencyTaskRuntimeHardeningSummary() {
  std::ostringstream out;
  // hardening anchor: live task helper execution now carries one
  // explicit edge-case/runtime-stability packet for cancellation cleanup,
  // autorelease scopes, and reset-stable replay proof.
  out << "contract=" << kObjc3ConcurrencyTaskRuntimeHardeningContractId
      << ";live_runtime_contract="
      << kObjc3ConcurrencyLiveTaskRuntimeIntegrationContractId
      << ";source_model=" << kObjc3ConcurrencyTaskRuntimeHardeningSourceModel
      << ";execution_model=" << kObjc3ConcurrencyTaskRuntimeHardeningExecutionModel
      << ";packaging_model=" << kObjc3ConcurrencyTaskRuntimeHardeningPackagingModel
      << ";reset_symbol=objc3_runtime_reset_for_testing"
      << ";task_snapshot_symbol=objc3_runtime_copy_task_runtime_state_for_testing"
      << ";memory_snapshot_symbol=objc3_runtime_copy_memory_management_state_for_testing"
      << ";arc_snapshot_symbol=objc3_runtime_copy_arc_debug_state_for_testing"
      << ";push_scope_symbol=" << kObjc3RuntimePushAutoreleasepoolScopeSymbol
      << ";pop_scope_symbol=" << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";fail_closed_model=" << kObjc3ConcurrencyTaskRuntimeHardeningFailClosedModel
      << ";follow_on_surface=objc3c.concurrency.taskruntime.hardening.v1";
  return out.str();
}

std::string Objc3ArcAutomaticInsertionSummary() {
  std::ostringstream out;
  // ARC automatic-insertion anchor: lane-C now consumes the
  // existing ARC semantic insertion flags for the supported runnable slice so
  // ordinary function and method lowering materializes retain/release/
  // autorelease helper calls instead of publishing summary-only intent.
  out << "contract=" << kObjc3ArcAutomaticInsertionContractId
      << ";source_model=" << kObjc3ArcAutomaticInsertionSourceModel
      << ";lowering_model=" << kObjc3ArcAutomaticInsertionLoweringModel
      << ";arc_mode_contract=" << Expr::kObjc3ArcModeHandlingContractId
      << ";arc_inference_contract=" << Expr::kObjc3ArcInferenceLifetimeContractId
      << ";arc_interaction_contract=" << Expr::kObjc3ArcInteractionSemanticsContractId
      << ";arc_cleanup_contract=" << kObjc3ArcLoweringAbiCleanupModelContractId
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";fail_closed_model=" << kObjc3ArcAutomaticInsertionFailureModel
      << ";non_goal_model=" << kObjc3ArcAutomaticInsertionNonGoalModel
      << ";follow_on_surface=objc3c.arc.automaticinsertion.surface.v1";
  return out.str();
}

std::string Objc3ArcCleanupWeakLifetimeHooksSummary() {
  std::ostringstream out;
  // ARC cleanup/weak/lifetime lowering anchor: lane-C now widens the
  // supported ARC slice with scope-aware cleanup emission, the retained weak
  // current-property helper path, and deterministic block-capture lifetime
  // cleanup without claiming generalized weak-local or exception-driven ARC.
  out << "contract=" << kObjc3ArcCleanupWeakLifetimeHooksContractId
      << ";source_model=" << kObjc3ArcCleanupWeakLifetimeHooksSourceModel
      << ";lowering_model=" << kObjc3ArcCleanupWeakLifetimeHooksLoweringModel
      << ";arc_mode_contract=" << Expr::kObjc3ArcModeHandlingContractId
      << ";arc_interaction_contract="
      << Expr::kObjc3ArcInteractionSemanticsContractId
      << ";arc_cleanup_contract=" << kObjc3ArcLoweringAbiCleanupModelContractId
      << ";arc_insertion_contract=" << kObjc3ArcAutomaticInsertionContractId
      << ";weak_load_symbol=" << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
      << ";weak_store_symbol="
      << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";fail_closed_model="
      << kObjc3ArcCleanupWeakLifetimeHooksFailureModel
      << ";non_goal_model=" << kObjc3ArcCleanupWeakLifetimeHooksNonGoalModel
      << ";follow_on_surface=objc3c.arc.cleanupweaklifetime.surface.v1";
  return out.str();
}

std::string Objc3ArcBlockAutoreleaseReturnLoweringSummary() {
  std::ostringstream out;
  // ARC/block autorelease-return lowering anchor: lane-C now closes
  // the supported escaping-block plus autoreleasing-return edge inventory by
  // preserving terminal cleanup state across sibling branches while block
  // promotion/dispose helpers and autorelease-return conventions remain
  // runtime-lowered together.
  out << "contract=" << kObjc3ArcBlockAutoreleaseReturnLoweringContractId
      << ";source_model="
      << kObjc3ArcBlockAutoreleaseReturnLoweringSourceModel
      << ";lowering_model="
      << kObjc3ArcBlockAutoreleaseReturnLoweringModel
      << ";arc_cleanup_contract="
      << kObjc3ArcCleanupWeakLifetimeHooksContractId
      << ";arc_insertion_contract=" << kObjc3ArcAutomaticInsertionContractId
      << ";block_escape_contract="
      << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";promote_block_symbol=" << kObjc3RuntimePromoteBlockI32Symbol
      << ";invoke_block_symbol=" << kObjc3RuntimeInvokeBlockI32Symbol
      << ";fail_closed_model="
      << kObjc3ArcBlockAutoreleaseReturnLoweringFailureModel
      << ";non_goal_model="
      << kObjc3ArcBlockAutoreleaseReturnLoweringNonGoalModel
      << ";follow_on_surface=objc3c.arc.blockautoreleasereturn.surface.v1";
  return out.str();
}

std::string Objc3RuntimeArcHelperApiSurfaceSummary() {
  std::ostringstream out;
  // runtime ARC helper API surface anchor: lane-D now freezes the
  // private runtime helper ABI that current ARC lowering already consumes so
  // later runtime implementation work can widen behavior without silently
  // widening the public runtime header.
  out << "contract=" << kObjc3RuntimeArcHelperApiSurfaceContractId
      << ";reference_model=" << kObjc3RuntimeArcHelperApiSurfaceReferenceModel
      << ";weak_model=" << kObjc3RuntimeArcHelperApiSurfaceWeakModel
      << ";autoreleasepool_model="
      << kObjc3RuntimeArcHelperApiSurfaceAutoreleasepoolModel
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";read_current_property_symbol="
      << kObjc3RuntimeReadCurrentPropertyI32Symbol
      << ";write_current_property_symbol="
      << kObjc3RuntimeWriteCurrentPropertyI32Symbol
      << ";exchange_current_property_symbol="
      << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
      << ";load_weak_current_property_symbol="
      << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
      << ";store_weak_current_property_symbol="
      << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
      << ";push_autoreleasepool_symbol="
      << kObjc3RuntimePushAutoreleasepoolScopeSymbol
      << ";pop_autoreleasepool_symbol="
      << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";fail_closed_model="
      << kObjc3RuntimeArcHelperApiSurfaceFailClosedModel
      << ";follow_on_surface=objc3c.arc.runtimehelperapi.surface.v1";
  return out.str();
}

std::string Objc3RuntimeArcHelperRuntimeSupportSummary() {
  std::ostringstream out;
  // runtime ARC helper implementation anchor: lane-D now proves the
  // private ARC helper ABI from D001 is linked and executable for the
  // currently supported ARC property/weak/autorelease-return slice.
  out << "contract=" << kObjc3RuntimeArcHelperRuntimeSupportContractId
      << ";dependency_model="
      << kObjc3RuntimeArcHelperRuntimeSupportDependencyModel
      << ";weak_model=" << kObjc3RuntimeArcHelperRuntimeSupportWeakModel
      << ";autorelease_return_model="
      << kObjc3RuntimeArcHelperRuntimeSupportAutoreleaseReturnModel
      << ";execution_model="
      << kObjc3RuntimeArcHelperRuntimeSupportExecutionModel
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";load_weak_current_property_symbol="
      << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
      << ";store_weak_current_property_symbol="
      << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
      << ";push_autoreleasepool_symbol="
      << kObjc3RuntimePushAutoreleasepoolScopeSymbol
      << ";pop_autoreleasepool_symbol="
      << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";fail_closed_model="
      << kObjc3RuntimeArcHelperRuntimeSupportFailClosedModel
      << ";follow_on_surface=objc3c.arc.runtimehelpersupport.surface.v1";
  return out.str();
}

std::string Objc3RuntimeArcDebugInstrumentationSummary() {
  std::ostringstream out;
  // ownership-debug/runtime-validation anchor: lane-D now widens
  // the live ARC helper runtime surface with private testing snapshots and
  // deterministic counters so supported ARC helper traffic can be proven
  // without widening the public runtime ABI.
  out << "contract=" << kObjc3RuntimeArcDebugInstrumentationContractId
      << ";dependency_model="
      << kObjc3RuntimeArcDebugInstrumentationDependencyModel
      << ";coverage_model="
      << kObjc3RuntimeArcDebugInstrumentationCoverageModel
      << ";validation_model="
      << kObjc3RuntimeArcDebugInstrumentationValidationModel
      << ";retain_symbol=" << kObjc3RuntimeRetainI32Symbol
      << ";release_symbol=" << kObjc3RuntimeReleaseI32Symbol
      << ";autorelease_symbol=" << kObjc3RuntimeAutoreleaseI32Symbol
      << ";read_current_property_symbol="
      << kObjc3RuntimeReadCurrentPropertyI32Symbol
      << ";write_current_property_symbol="
      << kObjc3RuntimeWriteCurrentPropertyI32Symbol
      << ";exchange_current_property_symbol="
      << kObjc3RuntimeExchangeCurrentPropertyI32Symbol
      << ";load_weak_current_property_symbol="
      << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol
      << ";store_weak_current_property_symbol="
      << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol
      << ";push_autoreleasepool_symbol="
      << kObjc3RuntimePushAutoreleasepoolScopeSymbol
      << ";pop_autoreleasepool_symbol="
      << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";fail_closed_model="
      << kObjc3RuntimeArcDebugInstrumentationFailClosedModel
      << ";follow_on_surface=objc3c.arc.debuginstrumentation.surface.v1";
  return out.str();
}

std::string Objc3RunnableArcRuntimeGateSummary() {
  std::ostringstream out;
  // runnable-arc-runtime gate anchor: lane-E freezes the supported
  // runnable ARC slice by consuming the current mode-handling, interaction,
  // lowering, and runtime proofs rather than widening source, lowering, or
  // public runtime behavior.
  out << "contract=" << kObjc3RunnableArcRuntimeGateContractId
      << ";evidence_model=" << kObjc3RunnableArcRuntimeGateEvidenceModel
      << ";active_gate_model=" << kObjc3RunnableArcRuntimeGateActiveModel
      << ";non_goal_model=" << kObjc3RunnableArcRuntimeGateNonGoalModel
      << ";mode_contract=" << kObjc3ArcModeHandlingContractId
      << ";interaction_contract=" << kObjc3ArcInteractionSemanticsContractId
      << ";lowering_contract="
      << kObjc3ArcBlockAutoreleaseReturnLoweringContractId
      << ";runtime_contract=" << kObjc3RuntimeArcDebugInstrumentationContractId
      << ";fail_closed_model=" << kObjc3RunnableArcRuntimeGateFailClosedModel
      << ";follow_on_surface=objc3c.arc.runnablegate.surface.v1";
  return out.str();
}

std::string Objc3RunnableArcCloseoutSummary() {
  std::ostringstream out;
  // runnable-arc-closeout anchor: lane-E closes the current ARC
  // tranche by consuming the existing mode-handling, interaction, lowering,
  // runtime, and gate proofs plus integrated execution smoke instead of
  // widening the supported source/runtime slice.
  out << "contract=" << kObjc3RunnableArcCloseoutContractId
      << ";matrix_model=" << kObjc3RunnableArcCloseoutMatrixModel
      << ";smoke_model=" << kObjc3RunnableArcCloseoutSmokeModel
      << ";gate_contract=" << kObjc3RunnableArcRuntimeGateContractId
      << ";runtime_contract=" << kObjc3RuntimeArcDebugInstrumentationContractId
      << ";fail_closed_model=" << kObjc3RunnableArcCloseoutFailClosedModel
      << ";follow_on_surface=objc3c.arc.closeout.surface.v1";
  return out.str();
}

std::string Objc3ExecutableMethodBodyBindingSummary() {
  std::ostringstream out;
  // executable method-body binding implementation anchor: lane-C
  // upgrades the frozen C001 surface into a fail-closed runtime capability by
  // requiring every implementation-owned executable method entry to bind to
  // exactly one concrete LLVM definition symbol before the object artifact is
  // accepted.
  out << "contract=" << kObjc3ExecutableMethodBodyBindingContractId
      << ";source_model=" << kObjc3ExecutableMethodBodyBindingSourceModel
      << ";runtime_model=" << kObjc3ExecutableMethodBodyBindingRuntimeModel
      << ";fail_closed_model="
      << kObjc3ExecutableMethodBodyBindingFailClosedModel
      << ";scope_model=" << kObjc3ExecutableObjectArtifactLoweringScopeModel;
  return out.str();
}

std::string Objc3ExecutableRealizationRecordsSummary() {
  std::ostringstream out;
  // executable realization-record expansion anchor: emitted
  // class/protocol/category records now preserve the owner and graph edges that
  // D-lane runtime realization will consume directly. Parser/sema still own the
  // legality and canonical identities; lowering only serializes that closure
  // into stable artifact layouts.
  out << "contract=" << kObjc3ExecutableRealizationRecordsContractId
      << ";class_record_model="
      << kObjc3ExecutableRealizationClassRecordModel
      << ";protocol_record_model="
      << kObjc3ExecutableRealizationProtocolRecordModel
      << ";category_record_model="
      << kObjc3ExecutableRealizationCategoryRecordModel
      << ";fail_closed_model="
      << kObjc3ExecutableRealizationFailClosedModel
      << ";scope_model=" << kObjc3ExecutableObjectArtifactLoweringScopeModel;
  return out.str();
}

std::string Objc3RuntimeClassRealizationSummary() {
  std::ostringstream out;
  // class-realization-runtime freeze anchor: the current runtime
  // consumes emitted realization records directly, walks the class/metaclass
  // graph deterministically, attaches preferred category implementation
  // records after bundle selection, and uses protocol records only as
  // declaration-aware negative lookup evidence. Property/ivar storage and
  // executable protocol bodies remain outside this runtime boundary.
  out << "contract=" << kObjc3RuntimeClassRealizationContractId
      << ";class_realization_model=" << kObjc3RuntimeClassRealizationModel
      << ";metaclass_graph_model=" << kObjc3RuntimeMetaclassGraphModel
      << ";category_attachment_model="
      << kObjc3RuntimeClassRealizationCategoryAttachmentModel
      << ";protocol_check_model=" << kObjc3RuntimeProtocolCheckModel
      << ";fail_closed_model="
      << kObjc3RuntimeClassRealizationFailClosedModel
      << ";non_goals=no-property-storage-no-ivar-layout-no-protocol-body-dispatch";
  return out.str();
}

std::string Objc3RuntimeMetaclassGraphRootClassSummary() {
  std::ostringstream out;
  // metaclass-graph-root-class anchor: runtime now publishes a
  // realized class graph keyed by stable receiver base identities, preserves
  // root classes as explicit nodes with null superclass links, and keeps
  // known-class/class-self dispatch on the same metaclass graph without
  // widening the public runtime ABI. Allocation, instance storage, and
  // protocol-body execution remain outside this boundary.
  out << "contract=" << kObjc3RuntimeMetaclassGraphRootClassContractId
      << ";realized_class_graph_model="
      << kObjc3RuntimeRealizedClassGraphModel
      << ";root_class_baseline_model="
      << kObjc3RuntimeRootClassBaselineModel
      << ";fail_closed_model="
      << kObjc3RuntimeRealizedClassGraphFailClosedModel
      << ";non_goals=no-allocation-no-instance-storage-no-protocol-body-dispatch";
  return out.str();
}

std::string Objc3RuntimeCategoryAttachmentProtocolConformanceSummary() {
  std::ostringstream out;
  // category-attachment-protocol-conformance anchor: runtime-owned
  // realized class nodes now retain preferred category attachments and answer
  // protocol conformance queries from emitted class/category protocol refs
  // without rediscovering source legality or widening the public ABI.
  out << "contract="
      << kObjc3RuntimeCategoryAttachmentProtocolConformanceContractId
      << ";category_attachment_model="
      << kObjc3RuntimeCategoryAttachmentRealizedGraphModel
      << ";protocol_conformance_query_model="
      << kObjc3RuntimeProtocolConformanceQueryModel
      << ";fail_closed_model="
      << kObjc3RuntimeAttachmentConformanceFailClosedModel
      << ";non_goals=no-allocation-no-property-storage-no-cross-image-attachment";
  return out.str();
}

std::string Objc3RuntimeCanonicalRunnableObjectSampleSupportSummary() {
  std::ostringstream out;
  // canonical-runnable-object-sample anchor: runtime-owned builtin
  // alloc/new/init resolution now closes the smallest truthful executable
  // object sample while metadata-rich object-model behavior stays proven
  // through paired library/probe evidence instead of pretending the runtime
  // export gate is already open for every executable sample shape.
  out << "contract="
      << kObjc3RuntimeCanonicalRunnableObjectSampleSupportContractId
      << ";execution_model="
      << kObjc3RuntimeCanonicalRunnableObjectExecutionModel
      << ";probe_split_model="
      << kObjc3RuntimeCanonicalRunnableObjectProbeSplitModel
      << ";fail_closed_model="
      << kObjc3RuntimeCanonicalRunnableObjectFailClosedModel
      << ";non_goals=no-property-storage-no-ivar-layout-no-metadata-heavy-executable-export-bypass";
  return out.str();
}

std::string Objc3RuntimeMetadataBinaryInspectionHarnessSummary() {
  std::ostringstream out;
  // binary inspection harness expansion anchor: lane-C now proves
  // emitted metadata sections structurally through one shared llvm-readobj and
  // llvm-objdump corpus. The positive corpus covers scaffold-only, class-heavy,
  // category-heavy, and selector-pool-heavy objects, while the negative corpus
  // remains fail-closed when semantic validation prevents object emission.
  out << "contract=" << kObjc3RuntimeBinaryInspectionHarnessContractId
      << ";positive_corpus_model="
      << kObjc3RuntimeBinaryInspectionPositiveCorpusModel
      << ";negative_corpus_model="
      << kObjc3RuntimeBinaryInspectionNegativeCorpusModel
      << ";section_inventory_command="
      << kObjc3RuntimeBinaryInspectionSectionCommand
      << ";symbol_inventory_command="
      << kObjc3RuntimeBinaryInspectionSymbolCommand
      << ";non_goals=no-new-metadata-families-or-runtime-registration";
  return out.str();
}

std::string Objc3RuntimeMetadataObjectPackagingRetentionSummary() {
  std::ostringstream out;
  // object-packaging/retention freeze anchor: lane-D now freezes the
  // current produced-object handoff as module.obj plus retained aggregate
  // symbols. Later archive/link/startup-registration work must preserve this
  // boundary and may not silently replace llvm.used or aggregate symbol roots.
  out << "contract=" << kObjc3RuntimeObjectPackagingRetentionContractId
      << ";boundary_model="
      << kObjc3RuntimeObjectPackagingRetentionBoundaryModel
      << ";retention_anchor_model="
      << kObjc3RuntimeObjectPackagingRetentionAnchorModel
      << ";object_artifact=" << kObjc3RuntimeObjectPackagingRetentionArtifact
      << ";aggregate_symbol_prefix="
      << kObjc3RuntimeObjectPackagingRetentionSymbolPrefix
      << ";non_goals=no-archive-packaging-link-registration-or-startup-bootstrap";
  return out.str();
}

std::string Objc3RuntimeMetadataLinkerRetentionSummary() {
  std::ostringstream out;
  // linker-retention/dead-strip resistance anchor: lane-D adds one
  // real public linker anchor and one public discovery root, together with a
  // driver response-file payload that can force-retain the archived object that
  // owns the metadata sections. Multi-archive and multi-TU edge cases remain
  // deferred until the next runtime step.
  out << "contract=" << kObjc3RuntimeLinkerRetentionContractId
      << ";anchor_model=" << kObjc3RuntimeLinkerRetentionAnchorModel
      << ";discovery_model=" << kObjc3RuntimeLinkerDiscoveryModel
      << ";linker_anchor_logical_section="
      << kObjc3RuntimeLinkerAnchorLogicalSection
      << ";discovery_root_logical_section="
      << kObjc3RuntimeLinkerDiscoveryRootLogicalSection
      << ";linker_response_artifact_suffix="
      << kObjc3RuntimeLinkerResponseArtifactSuffix
      << ";discovery_artifact_suffix="
      << kObjc3RuntimeLinkerDiscoveryArtifactSuffix
      << ";coff_flag_model=" << kObjc3RuntimeLinkerRetentionCoffFlagModel
      << ";elf_flag_model=" << kObjc3RuntimeLinkerRetentionElfFlagModel
      << ";mach_o_flag_model=" << kObjc3RuntimeLinkerRetentionMachOFlagModel
      << ";non_goals=no-multi-archive-fan-in-or-cross-translation-unit-anchor-merging";
  return out.str();
}

std::string Objc3RuntimeMetadataArchiveStaticLinkDiscoverySummary() {
  std::ostringstream out;
  // archive/static-link discovery anchor: lane-D extends the D002
  // object-level discovery path with translation-unit-stable public anchors and
  // one merged discovery/response artifact pair that downstream archive/static
  // link orchestration can consume deterministically across multiple TUs.
  out << "contract=" << kObjc3RuntimeArchiveStaticLinkDiscoveryContractId
      << ";anchor_seed_model="
      << kObjc3RuntimeArchiveStaticLinkAnchorSeedModel
      << ";translation_unit_identity_model="
      << kObjc3RuntimeArchiveStaticLinkTranslationUnitIdentityModel
      << ";merge_model=" << kObjc3RuntimeArchiveStaticLinkMergeModel
      << ";merged_linker_response_artifact_suffix="
      << kObjc3RuntimeMergedLinkerResponseArtifactSuffix
      << ";merged_discovery_artifact_suffix="
      << kObjc3RuntimeMergedDiscoveryArtifactSuffix
      << ";non_goals=no-runtime-registration-or-startup-bootstrap";
  return out.str();
}

std::string Objc3RuntimeBootstrapLoweringBoundarySummary() {
  std::ostringstream out;
  // constructor-root/init-array lowering freeze anchor: the live
  // lowering path already materializes ctor-root/global_ctors/init-stub/
  // registration-table/image-local-init globals. This summary is the
  // canonical replay-stable description of that current boundary and the
  // registration-descriptor handoff it consumes.
  out << "contract=" << kObjc3RuntimeBootstrapLoweringContractId
      << ";boundary_model=" << kObjc3RuntimeBootstrapLoweringBoundaryModel
      << ";registration_descriptor_handoff_contract_id="
      << kObjc3RuntimeBootstrapRegistrationDescriptorHandoffContractId
      << ";registration_descriptor_artifact="
      << kObjc3RuntimeBootstrapRegistrationDescriptorArtifact
      << ";registration_descriptor_handoff_model="
      << kObjc3RuntimeBootstrapRegistrationDescriptorHandoffModel
      << ";constructor_root_symbol="
      << kObjc3RuntimeTranslationUnitRegistrationConstructorRootSymbol
      << ";init_stub_symbol_prefix="
      << kObjc3RuntimeTranslationUnitRegistrationInitStubSymbolPrefix
      << ";registration_table_symbol_prefix="
      << kObjc3RuntimeBootstrapRegistrationTableSymbolPrefix
      << ";registration_entrypoint_symbol="
      << kObjc3RuntimeTranslationUnitRegistrationEntryPointSymbol
      << ";global_ctor_list_model="
      << kObjc3RuntimeBootstrapGlobalCtorListModel
      << ";constructor_root_emission_state="
      << kObjc3RuntimeBootstrapConstructorRootEmissionState
      << ";init_stub_emission_state="
      << kObjc3RuntimeBootstrapInitStubEmissionState
      << ";registration_table_emission_state="
      << kObjc3RuntimeBootstrapRegistrationTableEmissionState
      << ";non_goals=no-multi-image-root-fanout-no-runtime-replay-partitioning-no-late-linker-synthesis";
  return out.str();
}

std::string Objc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringSummary() {
  std::ostringstream out;
  // registration-descriptor/image-root lowering anchor: this
  // summary freezes the first live binary lowering surface for the source
  // identifiers from the earlier runtime step. The IR/object path now materializes dedicated
  // registration-descriptor and image-root globals in their own sections
  // rather than treating those identifiers as sidecar-only metadata.
  out << "contract="
      << kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringContractId
      << ";lowering_model="
      << kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringModel
      << ";registration_descriptor_logical_section="
      << kObjc3RuntimeBootstrapRegistrationDescriptorLogicalSection
      << ";image_root_logical_section="
      << kObjc3RuntimeBootstrapImageRootLogicalSection
      << ";registration_descriptor_symbol_prefix="
      << kObjc3RuntimeBootstrapRegistrationDescriptorSymbolPrefix
      << ";image_root_symbol_prefix="
      << kObjc3RuntimeBootstrapImageRootSymbolPrefix
      << ";registration_descriptor_payload_model="
      << kObjc3RuntimeBootstrapRegistrationDescriptorPayloadModel
      << ";image_root_payload_model="
      << kObjc3RuntimeBootstrapImageRootPayloadModel
      << ";non_goals=no-cross-translation-unit-root-deduplication-or-runtime-fanout-merge";
  return out.str();
}

std::string Objc3RuntimeBootstrapArchiveStaticLinkReplayCorpusSummary() {
  std::ostringstream out;
  // archive/static-link bootstrap replay corpus anchor: this
  // summary binds the retained archive/static-link discovery path to
  // the live bootstrap reset/replay runtime so validation can prove
  // real startup registration and replay behavior over linked archives.
  out << "contract="
      << kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusContractId
      << ";corpus_model="
      << kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusModel
      << ";binary_proof_model="
      << kObjc3RuntimeBootstrapArchiveStaticLinkReplayCorpusBinaryProofModel
      << ";archive_static_link_discovery_contract_id="
      << kObjc3RuntimeArchiveStaticLinkDiscoveryContractId
      << ";bootstrap_failure_restart_contract_id="
      << kObjc3BootstrapFailureRestartSemanticsContractId
      << ";registration_descriptor_lowering_contract_id="
      << kObjc3RuntimeBootstrapRegistrationDescriptorImageRootLoweringContractId
      << ";replay_registered_images_symbol="
      << kObjc3RuntimeBootstrapReplayRegisteredImagesSymbol
      << ";reset_replay_state_snapshot_symbol="
      << kObjc3RuntimeBootstrapResetReplayStateSnapshotSymbol
      << ";non_goals=no-new-bootstrap-runtime-entrypoints-or-linker-merge-models";
  return out.str();
}

std::string Objc3RuntimeMetadataEmissionGateSummary() {
  std::ostringstream out;
  // metadata-emission gate anchor: lane-E freezes one fail-closed
  // evidence chain over the implemented A002/B003/C006/D003 summaries before
  // cross-lane closeout begins, so later work must preserve the same source
  // matrix, object-format policy, binary inspection corpus, and archive/static
  // link discovery proofs instead of redefining the gate ad hoc.
  out << "contract=" << kObjc3RuntimeMetadataEmissionGateContractId
      << ";evidence_model=" << kObjc3RuntimeMetadataEmissionGateEvidenceModel
      << ";failure_model=" << kObjc3RuntimeMetadataEmissionGateFailureModel
      << ";non_goals=no-new-emission-families-or-runtime-registration";
  return out.str();
}

std::string Objc3RuntimeMetadataObjectEmissionCloseoutSummary() {
  std::ostringstream out;
  // cross-lane object-emission closeout anchor: lane-E extends the
  // E001 summary chain with fresh integrated native object probes so class,
  // category, and message-send outputs all prove the same source graph,
  // object-format policy, binary inspection, and linker/discovery continuity
  // before later startup registration work begins.
  out << "contract=" << kObjc3RuntimeMetadataObjectEmissionCloseoutContractId
      << ";evidence_model="
      << kObjc3RuntimeMetadataObjectEmissionCloseoutEvidenceModel
      << ";failure_model="
      << kObjc3RuntimeMetadataObjectEmissionCloseoutFailureModel
      << ";non_goals=no-startup-registration-or-runtime-bootstrap";
  return out.str();
}

std::string Objc3ManifestObjectIrTruthGateSummary() {
  std::ostringstream out;
  // manifest/object/IR truth gate anchor: issue #8018 freezes one regenerated
  // artifact set above the existing metadata-emission and object-emission gates
  // so release claims cannot be widened from only one sidecar or object probe.
  out << "contract=" << kObjc3ManifestObjectIrTruthGateContractId
      << ";evidence_model=" << kObjc3ManifestObjectIrTruthGateEvidenceModel
      << ";manifest_model=" << kObjc3ManifestObjectIrTruthGateManifestModel
      << ";ir_model=" << kObjc3ManifestObjectIrTruthGateIrModel
      << ";object_model=" << kObjc3ManifestObjectIrTruthGateObjectModel
      << ";claim_model=" << kObjc3ManifestObjectIrTruthGateClaimModel
      << ";metadata_gate_contract="
      << kObjc3RuntimeMetadataEmissionGateContractId
      << ";object_closeout_contract="
      << kObjc3RuntimeMetadataObjectEmissionCloseoutContractId
      << ";versioned_conformance_contract="
      << kObjc3VersionedConformanceReportLoweringContractId
      << ";runtime_capability_contract="
      << kObjc3RuntimeCapabilityReportingContractId
      << ";required_artifacts=module.manifest.json,module.ll,module.obj,module.runtime-registration-descriptor.json,module.runtime-registration-manifest.json,module.runtime-metadata.bin,module.runtime-metadata-discovery.json,module.runtime-metadata-linker-options.rsp,module.objc3-conformance-report.json,module.objc3-conformance-publication.json,module.objc3-advanced-feature-gate.json,module.objc3-release-candidate-matrix.json"
      << ";failure_model=" << kObjc3ManifestObjectIrTruthGateFailureModel
      << ";follow_on_surface=objc3c.manifest.object.ir.truthgate.closeout.v1";
  return out.str();
}

std::string Objc3VersionedConformanceReportLoweringContractSummary() {
  std::ostringstream out;
  // lowering freeze anchor: lane-C lowers the existing A/B truth
  // packets into one emitted machine-readable sidecar so later runtime
  // capability reporting and driver publication consume the same versioned
  // report instead of reconstructing feature truth from docs or release notes.
  out << "contract=" << kObjc3VersionedConformanceReportLoweringContractId
      << ";semantic_contract_id="
      << kObjc3VersionedConformanceReportLoweringSemanticContractId
      << ";artifact_suffix="
      << kObjc3VersionedConformanceReportLoweringArtifactSuffix
      << ";artifact_schema_id="
      << kObjc3VersionedConformanceReportLoweringArtifactSchemaId
      << ";surface_path="
      << kObjc3VersionedConformanceReportLoweringSurfacePath
      << ";payload_model="
      << kObjc3VersionedConformanceReportLoweringPayloadModel
      << ";authority_model="
      << kObjc3VersionedConformanceReportLoweringAuthorityModel
      << ";known_unsupported_model="
      << kObjc3VersionedConformanceReportKnownUnsupportedModel
      << ";selection_model="
      << kObjc3VersionedConformanceReportSelectionModel
      << ";canonical_interface_mode="
      << kObjc3VersionedConformanceReportCanonicalInterfaceMode
      << ";publication_model="
      << kObjc3VersionedConformanceReportPublicationModel
      << ";non_goals=no-new-runtime-capability-families-or-strictness-enablement";
  return out.str();
}

std::string Objc3ToolingMachineReadableConformanceReportContractLoweringSummary() {
  std::ostringstream out;
  out << "contract=" << kObjc3ToolingMachineReadableConformanceReportContractId
      << ";dependency_contract_id="
      << kObjc3ToolingMachineReadableConformanceReportDependencyContractId
      << ";surface_path="
      << kObjc3ToolingMachineReadableConformanceReportSurfacePath
      << ";payload_model="
      << kObjc3ToolingMachineReadableConformanceReportPayloadModel
      << ";authority_model="
      << kObjc3ToolingMachineReadableConformanceReportAuthorityModel
      << ";artifact_suffix="
      << kObjc3VersionedConformanceReportLoweringArtifactSuffix
      << ";artifact_schema_id="
      << kObjc3VersionedConformanceReportLoweringArtifactSchemaId
      << ";runtime_capability_schema_id="
      << kObjc3RuntimeCapabilityReportingSchemaId
      << ";non_goals=no-second-report-format-or-release-truth-surface";
  return out.str();
}

std::string Objc3ToolingFeatureAwareConformanceReportEmissionLoweringSummary() {
  std::ostringstream out;
  out << "contract=" << kObjc3ToolingFeatureAwareConformanceReportEmissionContractId
      << ";dependency_contract_id="
      << kObjc3ToolingFeatureAwareConformanceReportEmissionDependencyContractId
      << ";surface_path="
      << kObjc3ToolingFeatureAwareConformanceReportEmissionSurfacePath
      << ";payload_model="
      << kObjc3ToolingFeatureAwareConformanceReportEmissionPayloadModel
      << ";authority_model="
      << kObjc3ToolingFeatureAwareConformanceReportEmissionAuthorityModel
      << ";artifact_suffix="
      << kObjc3VersionedConformanceReportLoweringArtifactSuffix
      << ";non_goals=no-second-report-sidecar-or-report-authority";
  return out.str();
}

std::string Objc3ToolingCorpusShardingReleaseEvidencePackagingLoweringSummary() {
  std::ostringstream out;
  out << "contract="
      << kObjc3ToolingCorpusShardingReleaseEvidencePackagingContractId
      << ";dependency_contract_id="
      << kObjc3ToolingCorpusShardingReleaseEvidencePackagingDependencyContractId
      << ";surface_path="
      << kObjc3ToolingCorpusShardingReleaseEvidencePackagingSurfacePath
      << ";payload_model="
      << kObjc3ToolingCorpusShardingReleaseEvidencePackagingPayloadModel
      << ";authority_model="
      << kObjc3ToolingCorpusShardingReleaseEvidencePackagingAuthorityModel
      << ";artifact_suffix="
      << kObjc3VersionedConformanceReportLoweringArtifactSuffix
      << ";non_goals=no-second-release-evidence-sidecar-or-driver-only-shard-truth";
  return out.str();
}

std::string Objc3RuntimeCapabilityReportingContractSummary() {
  std::ostringstream out;
  // capability-reporting anchor: lane-C must publish one truthful
  // machine-readable runtime/public capability payload that mirrors the
  // lowered conformance truth surface instead of deriving product claims from
  // release notes, docs, or ad hoc driver state.
  out << "contract=" << kObjc3RuntimeCapabilityReportingContractId
      << ";schema_id=" << kObjc3RuntimeCapabilityReportingSchemaId
      << ";surface_path=" << kObjc3RuntimeCapabilityReportingSurfacePath
      << ";profile_model=" << kObjc3RuntimeCapabilityReportingProfileModel
      << ";optional_feature_model="
      << kObjc3RuntimeCapabilityReportingOptionalFeatureModel
      << ";version_model=" << kObjc3RuntimeCapabilityReportingVersionModel
      << ";public_schema_id=" << kObjc3RuntimeCapabilityPublicSchemaId
      << ";strictness_mode=" << kObjc3RuntimeCapabilityStrictnessMode
      << ";concurrency_mode=" << kObjc3RuntimeCapabilityConcurrencyMode
      << ";target_triple=" << kObjc3RuntimeCapabilityTargetTriple
      << ";language_version=" << kObjc3RuntimeCapabilityLanguageVersion
      << ";non_goals=no-strictness-promotion-or-runtime-capability-overclaim";
  return out.str();
}

std::string Objc3RuntimeMetadataSectionForObjectFormat(
    const std::string &object_format, const std::string &logical_section) {
  return MapRuntimeMetadataSectionForObjectFormat(object_format, logical_section);
}

std::string Objc3RuntimeMetadataDriverLinkerRetentionFlagForObjectFormat(
    const std::string &object_format, const std::string &symbol_name) {
  return BuildRuntimeMetadataDriverLinkerRetentionFlagForObjectFormat(
      object_format, symbol_name);
}

std::string Objc3RuntimeMetadataHostSectionForLogicalName(
    const std::string &logical_section) {
  return MapRuntimeMetadataSectionForObjectFormat(HostRuntimeMetadataObjectFormat(),
                                                  logical_section);
}

std::string Objc3RuntimeDispatchDeclarationReplayKey(const Objc3LoweringIRBoundary &boundary) {
  std::ostringstream out;
  out << "declare i32 @" << boundary.runtime_dispatch_symbol << "(i32, ptr";
  for (std::size_t i = 0; i < boundary.runtime_dispatch_arg_slots; ++i) {
    out << ", i32";
  }
  out << ")";
  return out.str();
}

bool IsValidObjc3MethodLookupOverrideConflictContract(const Objc3MethodLookupOverrideConflictContract &contract) {
  if (contract.method_lookup_hits > contract.method_lookup_sites ||
      contract.method_lookup_misses > contract.method_lookup_sites ||
      contract.method_lookup_hits + contract.method_lookup_misses != contract.method_lookup_sites) {
    return false;
  }
  if (contract.override_lookup_hits > contract.override_lookup_sites ||
      contract.override_lookup_misses > contract.override_lookup_sites ||
      contract.override_lookup_hits + contract.override_lookup_misses != contract.override_lookup_sites) {
    return false;
  }
  if (contract.override_conflicts > contract.override_lookup_hits) {
    return false;
  }
  if (contract.unresolved_base_interfaces > contract.override_lookup_misses) {
    return false;
  }
  return true;
}

std::string Objc3MethodLookupOverrideConflictReplayKey(const Objc3MethodLookupOverrideConflictContract &contract) {
  return std::string("method_lookup_sites=") + std::to_string(contract.method_lookup_sites) +
         ";method_lookup_hits=" + std::to_string(contract.method_lookup_hits) +
         ";method_lookup_misses=" + std::to_string(contract.method_lookup_misses) +
         ";override_lookup_sites=" + std::to_string(contract.override_lookup_sites) +
         ";override_lookup_hits=" + std::to_string(contract.override_lookup_hits) +
         ";override_lookup_misses=" + std::to_string(contract.override_lookup_misses) +
         ";override_conflicts=" + std::to_string(contract.override_conflicts) +
         ";unresolved_base_interfaces=" + std::to_string(contract.unresolved_base_interfaces) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3MethodLookupOverrideConflictLaneContract;
}

Objc3PropertySynthesisIvarBindingContract Objc3DefaultPropertySynthesisIvarBindingContract(
    std::size_t property_synthesis_sites,
    bool deterministic) {
  Objc3PropertySynthesisIvarBindingContract contract;
  contract.property_synthesis_sites = property_synthesis_sites;
  contract.property_synthesis_explicit_ivar_bindings = 0;
  contract.property_synthesis_default_ivar_bindings = property_synthesis_sites;
  contract.interface_owned_property_synthesis_sites = property_synthesis_sites;
  contract.implementation_property_redeclaration_sites = 0;
  contract.ivar_binding_sites = property_synthesis_sites;
  contract.ivar_binding_resolved = property_synthesis_sites;
  contract.ivar_binding_missing = 0;
  contract.ivar_binding_conflicts = 0;
  contract.deterministic = deterministic;
  return contract;
}

bool IsValidObjc3PropertySynthesisIvarBindingContract(
    const Objc3PropertySynthesisIvarBindingContract &contract) {
  if (contract.property_synthesis_explicit_ivar_bindings +
              contract.property_synthesis_default_ivar_bindings !=
          contract.property_synthesis_sites ||
      contract.property_synthesis_explicit_ivar_bindings > contract.property_synthesis_sites ||
      contract.property_synthesis_default_ivar_bindings > contract.property_synthesis_sites) {
    return false;
  }
  if (contract.interface_owned_property_synthesis_sites !=
          contract.property_synthesis_sites ||
      contract.implementation_property_redeclaration_sites >
          contract.property_synthesis_sites) {
    return false;
  }
  if (contract.ivar_binding_sites != contract.property_synthesis_sites) {
    return false;
  }
  if (contract.ivar_binding_resolved > contract.ivar_binding_sites ||
      contract.ivar_binding_missing > contract.ivar_binding_sites ||
      contract.ivar_binding_conflicts > contract.ivar_binding_sites ||
      contract.ivar_binding_resolved + contract.ivar_binding_missing + contract.ivar_binding_conflicts !=
          contract.ivar_binding_sites) {
    return false;
  }
  return true;
}

std::string Objc3PropertySynthesisIvarBindingReplayKey(
    const Objc3PropertySynthesisIvarBindingContract &contract) {
  return std::string("property_synthesis_sites=") + std::to_string(contract.property_synthesis_sites) +
         ";property_synthesis_explicit_ivar_bindings=" +
         std::to_string(contract.property_synthesis_explicit_ivar_bindings) +
         ";property_synthesis_default_ivar_bindings=" +
         std::to_string(contract.property_synthesis_default_ivar_bindings) +
         ";interface_owned_property_synthesis_sites=" +
         std::to_string(contract.interface_owned_property_synthesis_sites) +
         ";implementation_property_redeclaration_sites=" +
         std::to_string(contract.implementation_property_redeclaration_sites) +
         ";ivar_binding_sites=" + std::to_string(contract.ivar_binding_sites) +
         ";ivar_binding_resolved=" + std::to_string(contract.ivar_binding_resolved) +
         ";ivar_binding_missing=" + std::to_string(contract.ivar_binding_missing) +
         ";ivar_binding_conflicts=" + std::to_string(contract.ivar_binding_conflicts) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3PropertySynthesisIvarBindingLaneContract;
}

bool IsValidObjc3IdClassSelObjectPointerTypecheckContract(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  const std::size_t computed_total = contract.id_typecheck_sites +
                                     contract.class_typecheck_sites +
                                     contract.sel_typecheck_sites +
                                     contract.object_pointer_typecheck_sites;
  return contract.total_typecheck_sites == computed_total;
}

std::string Objc3IdClassSelObjectPointerTypecheckReplayKey(
    const Objc3IdClassSelObjectPointerTypecheckContract &contract) {
  return std::string("id_typecheck_sites=") + std::to_string(contract.id_typecheck_sites) +
         ";class_typecheck_sites=" + std::to_string(contract.class_typecheck_sites) +
         ";sel_typecheck_sites=" + std::to_string(contract.sel_typecheck_sites) +
         ";object_pointer_typecheck_sites=" + std::to_string(contract.object_pointer_typecheck_sites) +
         ";total_typecheck_sites=" + std::to_string(contract.total_typecheck_sites) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3IdClassSelObjectPointerTypecheckLaneContract;
}

bool IsValidObjc3DispatchSurfaceClassificationContract(
    const Objc3DispatchSurfaceClassificationContract &contract) {
  const bool live_runtime_bindings_ok =
      contract.instance_entrypoint_family == kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily &&
      contract.class_entrypoint_family == kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily &&
      contract.super_entrypoint_family == kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily &&
      contract.dynamic_entrypoint_family == kObjc3DispatchSurfaceLiveRuntimeEntrypointFamily;
  return live_runtime_bindings_ok &&
         contract.direct_entrypoint_family == kObjc3DispatchSurfaceDirectDispatchBinding;
}

std::string Objc3DispatchSurfaceClassificationReplayKey(
    const Objc3DispatchSurfaceClassificationContract &contract) {
  return std::string("instance_dispatch_sites=") +
             std::to_string(contract.instance_dispatch_sites) +
         ";class_dispatch_sites=" + std::to_string(contract.class_dispatch_sites) +
         ";super_dispatch_sites=" + std::to_string(contract.super_dispatch_sites) +
         ";direct_dispatch_sites=" + std::to_string(contract.direct_dispatch_sites) +
         ";dynamic_dispatch_sites=" + std::to_string(contract.dynamic_dispatch_sites) +
         ";instance_entrypoint_family=" + contract.instance_entrypoint_family +
         ";class_entrypoint_family=" + contract.class_entrypoint_family +
         ";super_entrypoint_family=" + contract.super_entrypoint_family +
         ";direct_entrypoint_family=" + contract.direct_entrypoint_family +
         ";dynamic_entrypoint_family=" + contract.dynamic_entrypoint_family +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";contract_id=" + kObjc3DispatchSurfaceClassificationContractId;
}

bool IsValidObjc3MessageSendSelectorLoweringContract(
    const Objc3MessageSendSelectorLoweringContract &contract) {
  if (contract.unary_selector_sites + contract.keyword_selector_sites != contract.message_send_sites) {
    return false;
  }
  if (contract.receiver_expression_sites != contract.message_send_sites) {
    return false;
  }
  if (contract.selector_piece_sites < contract.message_send_sites) {
    return false;
  }
  if (contract.argument_expression_sites < contract.keyword_selector_sites) {
    return false;
  }
  if (contract.selector_literal_entries > contract.message_send_sites) {
    return false;
  }
  if (contract.selector_literal_entries == 0 && contract.selector_literal_characters != 0) {
    return false;
  }
  return true;
}

std::string Objc3MessageSendSelectorLoweringReplayKey(
    const Objc3MessageSendSelectorLoweringContract &contract) {
  return std::string("message_send_sites=") + std::to_string(contract.message_send_sites) +
         ";unary_selector_sites=" + std::to_string(contract.unary_selector_sites) +
         ";keyword_selector_sites=" + std::to_string(contract.keyword_selector_sites) +
         ";selector_piece_sites=" + std::to_string(contract.selector_piece_sites) +
         ";argument_expression_sites=" + std::to_string(contract.argument_expression_sites) +
         ";receiver_expression_sites=" + std::to_string(contract.receiver_expression_sites) +
         ";selector_literal_entries=" + std::to_string(contract.selector_literal_entries) +
         ";selector_literal_characters=" + std::to_string(contract.selector_literal_characters) +
         ";deterministic=" + BoolToken(contract.deterministic) +
         ";lane_contract=" + kObjc3MessageSendSelectorLoweringLaneContract;
}

bool IsValidObjc3DispatchAbiMarshallingContract(
    const Objc3DispatchAbiMarshallingContract &contract) {
  const std::size_t expected_argument_total =
      contract.message_send_sites * contract.runtime_dispatch_arg_slots;
  if (contract.receiver_slots_marshaled != contract.message_send_sites ||
      contract.selector_slots_marshaled != contract.message_send_sites) {
    return false;
  }
  if (contract.argument_total_slots_marshaled != expected_argument_total) {
    return false;
  }
  if (contract.argument_value_slots_marshaled > contract.argument_total_slots_marshaled) {
    return false;
  }
  if (contract.argument_padding_slots_marshaled + contract.argument_value_slots_marshaled !=
      contract.argument_total_slots_marshaled) {
    return false;
  }
  const std::size_t expected_total = contract.receiver_slots_marshaled +
                                     contract.selector_slots_marshaled +
                                     contract.argument_total_slots_marshaled;
