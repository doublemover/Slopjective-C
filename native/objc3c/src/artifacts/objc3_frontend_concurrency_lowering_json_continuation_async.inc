namespace {

inline constexpr const char
    *kObjc3ConcurrencyContinuationAbiAsyncLoweringContractId =
        "objc3c.concurrency.continuation.abi.async.lowering.contract.v1";
inline constexpr const char
    *kObjc3ConcurrencyContinuationAbiAsyncLoweringSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_concurrency_continuation_abi_and_async_lowering_contract";
inline constexpr const char
    *kObjc3ConcurrencyContinuationAbiAsyncLoweringContinuationModel =
        "async-entry-points-carry-deterministic-continuation-lowering-replay-keys-and-counts-into-emitted-ir";
inline constexpr const char
    *kObjc3ConcurrencyContinuationAbiAsyncLoweringAwaitModel =
        "await-suspension-state-lowering-replay-keys-and-counts-are-published-alongside-continuation-lowering";
inline constexpr const char
    *kObjc3ConcurrencyContinuationAbiAsyncLoweringDeferredModel =
        "runnable-async-frame-layout-resume-cleanup-and-executor-runtime-execution-remain-later-runtime-work";

}  // namespace

std::string BuildConcurrencyContinuationAbiAsyncLoweringContractJson(
    const Objc3AsyncContinuationLoweringContract &continuation_contract,
    const Objc3AwaitLoweringSuspensionStateLoweringContract &await_contract,
    const std::string &continuation_replay_key,
    const std::string &await_replay_key) {
  const bool deterministic_handoff =
      continuation_contract.deterministic && await_contract.deterministic;
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencyContinuationAbiAsyncLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3ConcurrencyContinuationAbiAsyncLoweringSurfacePath)
      << "\",\"continuation_lane_contract_id\":\""
      << EscapeJsonString(kObjc3AsyncContinuationLoweringLaneContract)
      << "\",\"await_lane_contract_id\":\""
      << EscapeJsonString(kObjc3AwaitLoweringSuspensionStateLoweringLaneContract)
      << "\",\"continuation_model\":\""
      << EscapeJsonString(
             kObjc3ConcurrencyContinuationAbiAsyncLoweringContinuationModel)
      << "\",\"await_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyContinuationAbiAsyncLoweringAwaitModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(
             kObjc3ConcurrencyContinuationAbiAsyncLoweringDeferredModel)
      << "\",\"async_continuation_replay_key\":\""
      << EscapeJsonString(continuation_replay_key)
      << "\",\"await_suspension_replay_key\":\""
      << EscapeJsonString(await_replay_key)
      << "\",\"async_continuation_sites\":"
      << continuation_contract.async_continuation_sites
      << ",\"async_keyword_sites\":"
      << continuation_contract.async_keyword_sites
      << ",\"async_function_sites\":"
      << continuation_contract.async_function_sites
      << ",\"continuation_allocation_sites\":"
      << continuation_contract.continuation_allocation_sites
      << ",\"continuation_resume_sites\":"
      << continuation_contract.continuation_resume_sites
      << ",\"continuation_suspend_sites\":"
      << continuation_contract.continuation_suspend_sites
      << ",\"async_state_machine_sites\":"
      << continuation_contract.async_state_machine_sites
      << ",\"async_normalized_sites\":"
      << continuation_contract.normalized_sites
      << ",\"async_gate_blocked_sites\":"
      << continuation_contract.gate_blocked_sites
      << ",\"async_contract_violation_sites\":"
      << continuation_contract.contract_violation_sites
      << ",\"await_suspension_sites\":"
      << await_contract.await_suspension_sites
      << ",\"await_keyword_sites\":" << await_contract.await_keyword_sites
      << ",\"await_suspension_point_sites\":"
      << await_contract.await_suspension_point_sites
      << ",\"await_resume_sites\":" << await_contract.await_resume_sites
      << ",\"await_state_machine_sites\":"
      << await_contract.await_state_machine_sites
      << ",\"await_continuation_sites\":"
      << await_contract.await_continuation_sites
      << ",\"await_normalized_sites\":" << await_contract.normalized_sites
      << ",\"await_gate_blocked_sites\":"
      << await_contract.gate_blocked_sites
      << ",\"await_contract_violation_sites\":"
      << await_contract.contract_violation_sites
      << ",\"deterministic_handoff\":"
      << (deterministic_handoff ? "true" : "false")
      << ",\"ready_for_ir_emission\":"
      << (deterministic_handoff ? "true" : "false")
      << "}";
  return out.str();
}
