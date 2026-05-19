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
      << ";runtime_record_model=private-runtime-record-copies-emitted-block-storage-bytes-and-internal-descriptor-with-invoke-thunk"
      << ";descriptor_model=storage-slot-zero-points-to-internal-descriptor-with-size-capture-count-flags-arity-and-invoke-thunk"
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
      << ";descriptor_model=promotion-validates-descriptor-storage-size-capture-count-flags-and-invoke-thunk-before-publishing-a-handle"
      << ";copy_dispose_model=pointer-capture-promotion-runs-copy-helper-and-final-release-runs-dispose-helper"
      << ";invoke_model=runtime-invoke-loads-the-descriptor-owned-thunk-from-a-runnable-invocation-plan"
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
