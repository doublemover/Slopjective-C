namespace {

inline constexpr const char
    *kObjc3ConcurrencyAsyncDirectCallLoweringContractId =
        "objc3c.concurrency.async.direct.call.lowering.v1";
inline constexpr const char
    *kObjc3ConcurrencyAsyncDirectCallLoweringSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_concurrency_async_function_await_and_continuation_lowering";
inline constexpr const char
    *kObjc3ConcurrencyAsyncDirectCallLoweringImplementationModel =
        "supported-async-functions-and-await-lower-through-direct-calls-in-native-ir-and-object-emission-for-the-current-non-suspending-slice";
inline constexpr const char *kObjc3ConcurrencyAsyncDirectCallLoweringAwaitModel =
    "await-marked-expressions-currently-reuse-the-operand-direct-call-lowering-path-without-materializing-a-suspension-state-machine";
inline constexpr const char
    *kObjc3ConcurrencyAsyncDirectCallLoweringDeferredModel =
        "continuation-allocation-resume-suspend-state-machine-cleanup-and-executor-runtime-scheduling-remain-later-runtime-work";

}  // namespace

std::string BuildConcurrencyAsyncDirectCallLoweringJson(
    const Objc3FrontendConcurrencyAsyncSourceClosureSummary &source_summary,
    const Objc3AsyncContinuationLoweringContract &continuation_contract,
    const Objc3AwaitLoweringSuspensionStateLoweringContract &await_contract,
    const std::string &continuation_replay_key,
    const std::string &await_replay_key) {
  const bool deterministic = source_summary.deterministic_handoff &&
                             continuation_contract.deterministic &&
                             await_contract.deterministic;
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencyAsyncDirectCallLoweringContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3ConcurrencyAsyncDirectCallLoweringSurfacePath)
      << "\",\"implementation_model\":\""
      << EscapeJsonString(
             kObjc3ConcurrencyAsyncDirectCallLoweringImplementationModel)
      << "\",\"await_lowering_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyAsyncDirectCallLoweringAwaitModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3ConcurrencyAsyncDirectCallLoweringDeferredModel)
      << "\",\"source_closure_contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencyAsyncSourceClosureContractId)
      << "\",\"continuation_lane_contract_id\":\""
      << EscapeJsonString(kObjc3AsyncContinuationLoweringLaneContract)
      << "\",\"await_lane_contract_id\":\""
      << EscapeJsonString(kObjc3AwaitLoweringSuspensionStateLoweringLaneContract)
      << "\",\"async_function_sites\":" << source_summary.async_function_sites
      << ",\"async_method_sites\":" << source_summary.async_method_sites
      << ",\"await_expression_sites\":"
      << source_summary.await_expression_sites
      << ",\"continuation_allocation_sites\":"
      << continuation_contract.continuation_allocation_sites
      << ",\"continuation_resume_sites\":"
      << continuation_contract.continuation_resume_sites
      << ",\"continuation_suspend_sites\":"
      << continuation_contract.continuation_suspend_sites
      << ",\"async_state_machine_sites\":"
      << continuation_contract.async_state_machine_sites
      << ",\"await_resume_sites\":" << await_contract.await_resume_sites
      << ",\"await_state_machine_sites\":"
      << await_contract.await_state_machine_sites
      << ",\"await_continuation_sites\":"
      << await_contract.await_continuation_sites
      << ",\"direct_call_lowering_supported\":"
      << (deterministic ? "true" : "false")
      << ",\"non_suspending_happy_path_only\":true"
      << ",\"object_emission_supported\":"
      << (deterministic ? "true" : "false")
      << ",\"runtime_scheduler_required\":false"
      << ",\"continuation_replay_key\":\""
      << EscapeJsonString(continuation_replay_key)
      << "\",\"await_replay_key\":\"" << EscapeJsonString(await_replay_key)
      << "\",\"deterministic\":" << (deterministic ? "true" : "false")
      << ",\"ready_for_ir_object_emission\":"
      << (deterministic ? "true" : "false")
      << "}";
  return out.str();
}
