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
