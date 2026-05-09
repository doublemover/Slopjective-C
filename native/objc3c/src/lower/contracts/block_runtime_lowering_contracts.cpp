#include "lower/objc3_lowering_contract.h"

#include "ast/objc3_ast.h"

#include <sstream>
#include <string>

std::string Objc3ExecutableBlockSourceClosureSummary() {
  std::ostringstream out;
  // executable-block-source-closure freeze anchor: this summary is
  // intentionally truthful about the current boundary. Parser/AST/source
  // replay for block literals is live, while runnable lowering remains a
  // fail-closed non-goal until later work.
  out << "contract=" << Expr::kObjc3ExecutableBlockSourceClosureContractId
      << ";source_model=" << Expr::kObjc3ExecutableBlockSourceSurfaceModel
      << ";evidence_model=" << Expr::kObjc3ExecutableBlockSourceEvidenceModel
      << ";non_goal_model=" << Expr::kObjc3ExecutableBlockSourceNonGoalModel
      << ";fail_closed_model=" << Expr::kObjc3ExecutableBlockSourceFailureModel
      << ";capture_lane_contract=" << kObjc3BlockLiteralCaptureLoweringLaneContract;
  return out.str();
}

std::string Objc3ExecutableBlockSourceModelCompletionSummary() {
  std::ostringstream out;
  // block-source-model-completion anchor: lane-A now upgrades the
  // frozen block source closure into a deterministic parameter/capture/invoke
  // source model that source-only frontend runs may publish before runnable
  // lowering still fails closed on native emit paths.
  out << "contract="
      << Expr::kObjc3ExecutableBlockSourceModelCompletionContractId
      << ";signature_model=" << Expr::kObjc3ExecutableBlockSignatureModel
      << ";capture_inventory_model="
      << Expr::kObjc3ExecutableBlockCaptureInventoryModel
      << ";invoke_surface_model="
      << Expr::kObjc3ExecutableBlockInvokeSurfaceModel
      << ";evidence_model="
      << Expr::kObjc3ExecutableBlockSourceModelEvidenceModel
      << ";fail_closed_model="
      << Expr::kObjc3ExecutableBlockSourceModelFailureModel
      << ";lane_contract=" << kObjc3BlockSourceModelCompletionLaneContract;
  return out.str();
}

std::string Objc3ExecutableBlockSourceStorageAnnotationSummary() {
  std::ostringstream out;
  // block-source-storage-annotation anchor: lane-A now publishes a
  // truthful byref/helper/escape-shape source inventory without claiming that
  // runnable block lowering, helper emission, or heap promotion already exist.
  out << "contract="
      << Expr::kObjc3ExecutableBlockSourceStorageAnnotationContractId
      << ";byref_storage_model=" << Expr::kObjc3ExecutableBlockByrefStorageModel
      << ";helper_intent_model="
      << Expr::kObjc3ExecutableBlockHelperIntentModel
      << ";escape_shape_model="
      << Expr::kObjc3ExecutableBlockEscapeShapeModel
      << ";lane_contract="
      << kObjc3BlockSourceStorageAnnotationLaneContract;
  return out.str();
}

std::string Objc3ExecutableBlockRuntimeSemanticRulesSummary() {
  std::ostringstream out;
  // block-runtime-semantic-rules freeze anchor: lane-B now freezes
  // the current semantic split where source-only block admission is truthful,
  // deterministic capture/byref/helper/escape annotations exist, and native
  // emit paths still fail closed before runnable block semantics land.
  out << "contract="
      << Expr::kObjc3ExecutableBlockRuntimeSemanticRulesContractId
      << ";capture_legality_model="
      << Expr::kObjc3ExecutableBlockRuntimeCaptureLegalityModel
      << ";storage_class_model="
      << Expr::kObjc3ExecutableBlockRuntimeStorageClassModel
      << ";escape_behavior_model="
      << Expr::kObjc3ExecutableBlockRuntimeEscapeBehaviorModel
      << ";helper_generation_model="
      << Expr::kObjc3ExecutableBlockRuntimeHelperGenerationModel
      << ";invocation_model="
      << Expr::kObjc3ExecutableBlockRuntimeInvocationModel
      << ";fail_closed_model="
      << Expr::kObjc3ExecutableBlockRuntimeFailClosedModel
      << ";lane_contract=" << kObjc3BlockRuntimeSemanticRulesLaneContract;
  return out.str();
}

std::string Objc3ExecutableBlockLoweringAbiArtifactBoundarySummary() {
  std::ostringstream out;
  // block-lowering-ABI/artifact-boundary freeze anchor: lane-C now
  // freezes the truthful lowering boundary that later runnable block-object
  // emission must preserve. The current compiler publishes deterministic
  // capture/invoke/storage/copy-dispose lowering surfaces, but native emit
  // still fails closed before emitted block records, invoke thunks, byref
  // cells, or helper bodies exist.
  out << "contract="
      << Expr::kObjc3ExecutableBlockLoweringAbiArtifactBoundaryContractId
      << ";abi_model=" << Expr::kObjc3ExecutableBlockLoweringAbiModel
      << ";helper_symbol_policy="
      << Expr::kObjc3ExecutableBlockHelperSymbolPolicyModel
      << ";artifact_inventory_model="
      << Expr::kObjc3ExecutableBlockArtifactInventoryModel
      << ";fail_closed_model="
      << Expr::kObjc3ExecutableBlockLoweringFailClosedModel
      << ";non_goal_model="
      << Expr::kObjc3ExecutableBlockLoweringNonGoalModel
      << ";capture_lane_contract="
      << kObjc3BlockLiteralCaptureLoweringLaneContract
      << ";invoke_lane_contract="
      << kObjc3BlockAbiInvokeTrampolineLoweringLaneContract
      << ";storage_lane_contract="
      << kObjc3BlockStorageEscapeLoweringLaneContract
      << ";copy_dispose_lane_contract="
      << kObjc3BlockCopyDisposeLoweringLaneContract
      << ";lane_contract="
      << kObjc3ExecutableBlockLoweringAbiArtifactBoundaryLaneContract;
  return out.str();
}

std::string Objc3ExecutableBlockObjectInvokeThunkLoweringSummary() {
  std::ostringstream out;
  // executable-block-object/invoke-thunk implementation anchor:
  // lane-C now widens the frozen C001 boundary into one real runnable slice.
  // Native lowering emits stack block storage plus one internal invoke thunk
  // for direct local invocation when captures are readonly scalar values. Byref
  // cells, helper bodies, owned-object captures, and heap-promotion semantics
  // remain deferred to C003.
  out << "contract="
      << Expr::kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId
      << ";boundary_contract="
      << Expr::kObjc3ExecutableBlockLoweringAbiArtifactBoundaryContractId
      << ";active_model="
      << Expr::kObjc3ExecutableBlockObjectInvokeThunkActiveModel
      << ";deferred_model="
      << Expr::kObjc3ExecutableBlockObjectInvokeThunkDeferredModel
      << ";execution_evidence_model="
      << Expr::kObjc3ExecutableBlockObjectInvokeThunkExecutionEvidenceModel
      << ";invoke_lane_contract="
      << kObjc3BlockAbiInvokeTrampolineLoweringLaneContract
      << ";storage_lane_contract="
      << kObjc3BlockStorageEscapeLoweringLaneContract
      << ";copy_dispose_lane_contract="
      << kObjc3BlockCopyDisposeLoweringLaneContract
      << ";lane_contract="
      << kObjc3ExecutableBlockObjectInvokeThunkLoweringLaneContract;
  return out.str();
}

std::string Objc3ExecutableBlockByrefHelperLoweringSummary() {
  std::ostringstream out;
  // byref-cell/copy-helper/dispose-helper implementation anchor:
  // lane-C now makes the non-escaping byref and owned-capture block slice
  // runnable by emitting stack byref-cell references plus helper bodies and
  // helper call sites. Heap-promotion and runtime-managed copy/dispose remain
  // intentionally deferred.
  out << "contract="
      << Expr::kObjc3ExecutableBlockByrefHelperLoweringContractId
      << ";previous_contract="
      << Expr::kObjc3ExecutableBlockObjectInvokeThunkLoweringContractId
      << ";active_model="
      << Expr::kObjc3ExecutableBlockByrefHelperLoweringActiveModel
      << ";deferred_model="
      << Expr::kObjc3ExecutableBlockByrefHelperLoweringDeferredModel
      << ";execution_evidence_model="
      << Expr::kObjc3ExecutableBlockByrefHelperLoweringExecutionEvidenceModel
      << ";storage_lane_contract="
      << kObjc3BlockStorageEscapeLoweringLaneContract
      << ";copy_dispose_lane_contract="
      << kObjc3BlockCopyDisposeLoweringLaneContract
      << ";runtime_semantic_lane_contract="
      << kObjc3BlockRuntimeSemanticRulesLaneContract
      << ";lane_contract="
      << kObjc3ExecutableBlockEscapeRuntimeHookLoweringLaneContract;
  return out.str();
}

std::string Objc3ExecutableBlockEscapeRuntimeHookLoweringSummary() {
  std::ostringstream out;
  // escaping-block runtime-hook implementation anchor: lane-C now
  // widens runnable native block lowering to escaping readonly-scalar block
  // values by emitting runtime heap-promotion and invoke hooks, while
  // ownership-sensitive escaping captures remain deferred to later lane-D
  // runtime work.
  out << "contract="
      << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId
      << ";previous_contract="
      << Expr::kObjc3ExecutableBlockByrefHelperLoweringContractId
      << ";active_model="
      << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringActiveModel
      << ";deferred_model="
      << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringDeferredModel
      << ";execution_evidence_model="
      << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringExecutionEvidenceModel
      << ";storage_lane_contract="
      << kObjc3BlockStorageEscapeLoweringLaneContract
      << ";copy_dispose_lane_contract="
      << kObjc3BlockCopyDisposeLoweringLaneContract
      << ";runtime_semantic_lane_contract="
      << kObjc3BlockRuntimeSemanticRulesLaneContract
      << ";lane_contract="
      << kObjc3ExecutableBlockEscapeRuntimeHookLoweringLaneContract;
  return out.str();
}

std::string Objc3RuntimeBlockApiObjectLayoutSummary() {
  std::ostringstream out;
  // block-runtime API/object-layout freeze anchor: the current
  // runtime helper surface is frozen as a private lowering/runtime contract
  // with opaque storage copies and i32 block handles; no public block-object
  // ABI or generalized heap-managed copy/dispose surface is implied yet.
  out << "contract=" << kObjc3RuntimeBlockApiObjectLayoutContractId
      << ";public_surface=stable-public-runtime-header-excludes-block-helper-entrypoints"
      << ";private_helper_surface=objc3_runtime_promote_block_i32-and-objc3_runtime_invoke_block_i32-remain-private-to-objc3_runtime_bootstrap_internal_h"
      << ";handle_type=i32"
      << ";promotion_abi=ptr-storage-plus-i64-size-plus-i32-pointer-capture-flag"
      << ";invoke_abi=i32-handle-plus-four-i32-arguments-returning-i32"
      << ";runtime_record_model=private-runtime-record-copies-emitted-block-storage-bytes-and-invoke-pointer"
      << ";object_layout_model=runtime-block-records-are-private-runtime-state-not-public-object-abi"
      << ";fail_closed_model=byref-forwarding-and-owned-capture-escaping-block-lifetimes-remain-deferred-until-the-next-runtime-hardening-phase"
      << ";non_goals=no-public-block-object-abi-no-generalized-runtime-copy-dispose-allocation-surface";
  return out.str();
}

std::string Objc3RuntimeBlockAllocationCopyDisposeInvokeSupportSummary() {
  std::ostringstream out;
  // block-runtime allocation/copy-dispose/invoke implementation
  // anchor: promoted runtime block records now preserve helper pointers and
  // aligned copied storage so pointer-capture block records can run copy,
  // invoke, and final-dispose behavior without claiming byref/ownership
  // interop is solved yet.
  out << "contract="
      << kObjc3RuntimeBlockAllocationCopyDisposeInvokeSupportContractId
      << ";previous_contract=" << kObjc3RuntimeBlockApiObjectLayoutContractId
      << ";allocation_model=runtime-block-records-copy-promoted-storage-into-aligned-word-buffers"
      << ";copy_dispose_model=pointer-capture-promotion-runs-copy-helper-and-final-release-runs-dispose-helper"
      << ";invoke_model=runtime-invoke-supports-readonly-scalar-and-pointer-capture-block-records"
      << ";handle_lifetime_model=i32-block-handles-participate-in-runtime-retain-release"
      << ";fail_closed_model=byref-forwarding-runtime-reentrant-helper-bodies-and-owned-capture-escape-interop-remain-deferred-until-next-runtime-phase"
      << ";non_goals=no-public-block-object-abi-no-generalized-public-runtime-helper-surface";
  return out.str();
}

std::string Objc3RuntimeBlockByrefForwardingHeapPromotionInteropSummary() {
  std::ostringstream out;
  // byref-forwarding/heap-promotion/ownership-interop
  // implementation anchor: escaping pointer-capture block promotion now
  // rewrites capture slots onto runtime-owned heap cells before helper
  // execution so byref mutation and owned-capture lifetime hooks survive after
  // the source frame returns.
  out << "contract="
      << kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId
      << ";previous_contract="
      << kObjc3RuntimeBlockAllocationCopyDisposeInvokeSupportContractId
      << ";forwarding_model=escaping-pointer-capture-slots-rewrite-to-runtime-owned-forwarding-cells"
      << ";heap_promotion_model=promotion-deep-copies-captured-i32-cells-before-helper-execution"
      << ";ownership_interop_model=copy-dispose-helpers-run-against-runtime-owned-cells-for-owned-captures"
      << ";invoke_model=escaped-byref-and-owned-capture-block-handles-invoke-after-source-frame-return"
      << ";fail_closed_model=no-public-block-abi-widening-and-no-outer-stack-cell-forwarding-bridge-yet"
      << ";non_goals=no-public-byref-layout-surface-no-generalized-foreign-abi-block-interoperability";
  return out.str();
}

std::string Objc3RunnableBlockRuntimeGateSummary() {
  std::ostringstream out;
  // runnable-block-runtime gate anchor: lane-E now freezes one
  // integrated proof boundary above the retained source, sema, lowering, and
  // runtime summaries so runnable block behavior is validated against the live
  // native path rather than metadata-only claims.
  out << "contract=" << Expr::kObjc3RunnableBlockRuntimeGateContractId
      << ";evidence_model="
      << Expr::kObjc3RunnableBlockRuntimeGateEvidenceModel
      << ";active_model="
      << Expr::kObjc3RunnableBlockRuntimeGateActiveModel
      << ";source_contract="
      << Expr::kObjc3ExecutableBlockSourceStorageAnnotationContractId
      << ";semantic_contract="
      << Expr::kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId
      << ";lowering_contract="
      << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId
      << ";runtime_contract="
      << kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId
      << ";non_goals="
      << Expr::kObjc3RunnableBlockRuntimeGateNonGoalModel
      << ";fail_closed_model="
      << Expr::kObjc3RunnableBlockRuntimeGateFailClosedModel
      << ";follow_on_surface=objc3c.runtime.block.runnablegate.closeout.v1";
  return out.str();
}

std::string Objc3RunnableBlockExecutionMatrixSummary() {
  std::ostringstream out;
  // runnable-block execution-matrix anchor: lane-E now closes the
  // block-runtime tranche with one truthful executable matrix over the
  // retained source, sema, lowering, runtime, and E001 gate surfaces. This
  // repackages the already supported block slice into an operator-facing
  // closeout proof without widening the public block ABI or helper boundary.
  out << "contract=" << Expr::kObjc3RunnableBlockExecutionMatrixContractId
      << ";evidence_model="
      << Expr::kObjc3RunnableBlockExecutionMatrixEvidenceModel
      << ";active_model="
      << Expr::kObjc3RunnableBlockExecutionMatrixActiveModel
      << ";source_contract="
      << Expr::kObjc3ExecutableBlockSourceStorageAnnotationContractId
      << ";semantic_contract="
      << Expr::kObjc3ExecutableBlockOwnershipSemanticsImplementationContractId
      << ";lowering_contract="
      << Expr::kObjc3ExecutableBlockEscapeRuntimeHookLoweringContractId
      << ";runtime_contract="
      << kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId
      << ";gate_contract="
      << Expr::kObjc3RunnableBlockRuntimeGateContractId
      << ";non_goals=" << Expr::kObjc3RunnableBlockExecutionMatrixNonGoalModel
      << ";fail_closed_model="
      << Expr::kObjc3RunnableBlockExecutionMatrixFailClosedModel
      << ";follow_on_surface=objc3c.arc.executionmatrix.surface.v1";
  return out.str();
}

std::string Objc3RuntimeBackedSemanticsClosureSummary() {
  std::ostringstream out;
  // runtime-backed semantics closure anchor: this composite contract is the
  // reviewable boundary tying block, ARC, error, async/task, and actor lowering
  // to private runtime helpers plus durable fixture/report evidence.
  out << "contract=" << kObjc3RuntimeBackedSemanticsClosureContractId
      << ";surface_path=" << kObjc3RuntimeBackedSemanticsClosureSurfacePath
      << ";evidence_model=" << kObjc3RuntimeBackedSemanticsClosureEvidenceModel
      << ";block_model=" << kObjc3RuntimeBackedSemanticsClosureBlockModel
      << ";arc_model=" << kObjc3RuntimeBackedSemanticsClosureArcModel
      << ";error_model=" << kObjc3RuntimeBackedSemanticsClosureErrorModel
      << ";concurrency_model="
      << kObjc3RuntimeBackedSemanticsClosureConcurrencyModel
      << ";runtime_helper_model="
      << kObjc3RuntimeBackedSemanticsClosureRuntimeHelperModel
      << ";block_contract="
      << kObjc3RuntimeBlockByrefForwardingHeapPromotionInteropContractId
      << ";block_gate_contract=" << kObjc3RunnableBlockExecutionMatrixContractId
      << ";arc_contract=" << kObjc3RunnableArcCloseoutContractId
      << ";error_contract="
      << kObjc3ErrorHandlingLiveErrorRuntimeIntegrationContractId
      << ";continuation_contract="
      << kObjc3ConcurrencyLiveContinuationRuntimeIntegrationContractId
      << ";task_contract=" << kObjc3ConcurrencyTaskRuntimeHardeningContractId
      << ";runtime_helper_count=36"
      << ";runtime_helpers="
      << kObjc3RuntimeReadCurrentPropertyI32Symbol << ","
      << kObjc3RuntimeWriteCurrentPropertyI32Symbol << ","
      << kObjc3RuntimeExchangeCurrentPropertyI32Symbol << ","
      << kObjc3RuntimeStoreThrownErrorI32Symbol << ","
      << kObjc3RuntimeLoadThrownErrorI32Symbol << ","
      << kObjc3RuntimeBridgeStatusErrorI32Symbol << ","
      << kObjc3RuntimeBridgeNSErrorErrorI32Symbol << ","
      << kObjc3RuntimeCatchMatchesErrorI32Symbol << ","
      << kObjc3RuntimeAllocateAsyncContinuationI32Symbol << ","
      << kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol << ","
      << kObjc3RuntimeResumeAsyncContinuationI32Symbol << ","
      << kObjc3RuntimeSpawnTaskI32Symbol << ","
      << kObjc3RuntimeEnterTaskGroupScopeI32Symbol << ","
      << kObjc3RuntimeAddTaskGroupTaskI32Symbol << ","
      << kObjc3RuntimeWaitTaskGroupNextI32Symbol << ","
      << kObjc3RuntimeCancelTaskGroupI32Symbol << ","
      << kObjc3RuntimeTaskIsCancelledI32Symbol << ","
      << kObjc3RuntimeTaskOnCancelI32Symbol << ","
      << kObjc3RuntimeExecutorHopI32Symbol << ","
      << kObjc3RuntimeActorEnterIsolationThunkI32Symbol << ","
      << kObjc3RuntimeActorEnterNonisolatedI32Symbol << ","
      << kObjc3RuntimeActorHopToExecutorI32Symbol << ","
      << kObjc3RuntimeActorRecordReplayProofI32Symbol << ","
      << kObjc3RuntimeActorRecordRaceGuardI32Symbol << ","
      << kObjc3RuntimeActorBindExecutorI32Symbol << ","
      << kObjc3RuntimeActorMailboxEnqueueI32Symbol << ","
      << kObjc3RuntimeActorMailboxDrainNextI32Symbol << ","
      << kObjc3RuntimeLoadWeakCurrentPropertyI32Symbol << ","
      << kObjc3RuntimeStoreWeakCurrentPropertyI32Symbol << ","
      << kObjc3RuntimeRetainI32Symbol << ","
      << kObjc3RuntimeReleaseI32Symbol << ","
      << kObjc3RuntimeAutoreleaseI32Symbol << ","
      << kObjc3RuntimePromoteBlockI32Symbol << ","
      << kObjc3RuntimeInvokeBlockI32Symbol << ","
      << kObjc3RuntimePushAutoreleasepoolScopeSymbol << ","
      << kObjc3RuntimePopAutoreleasepoolScopeSymbol
      << ";fail_closed_model="
      << kObjc3RuntimeBackedSemanticsClosureFailClosedModel
      << ";follow_on_surface=objc3c.runtime.backed.semantics.closeout.v1";
  return out.str();
}
