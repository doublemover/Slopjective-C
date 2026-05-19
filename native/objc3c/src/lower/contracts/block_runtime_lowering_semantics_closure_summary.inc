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
