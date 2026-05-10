namespace {

inline constexpr const char
    *kObjc3ConcurrencySuspensionCleanupIntegrationContractId =
        "objc3c.concurrency.suspension.autorelease.cleanup.integration.v1";
inline constexpr const char
    *kObjc3ConcurrencySuspensionCleanupIntegrationSurfacePath =
        "frontend.pipeline.semantic_surface."
        "objc_concurrency_suspension_autorelease_and_cleanup_integration";
inline constexpr const char *kObjc3ConcurrencySuspensionCleanupIntegrationModel =
    "supported-non-suspending-async-lowering-reuses-existing-autoreleasepool-scope-and-defer-cleanup-lowering-through-real-ir-and-object-emission";
inline constexpr const char
    *kObjc3ConcurrencySuspensionCleanupIntegrationOrderingModel =
        "current-proof-fixtures-show-terminal-return-paths-compose-await-direct-call-lowering-with-autoreleasepool-pop-and-defer-cleanup-without-a-separate-suspension-runtime";
inline constexpr const char
    *kObjc3ConcurrencySuspensionCleanupIntegrationDeferredModel =
        "continuation-resume-cleanup-suspension-state-frames-and-executor-runtime-execution-remain-later-runtime-work";

}  // namespace

std::string BuildConcurrencySuspensionCleanupIntegrationJson(
    const Objc3ControlFlowControlFlowSafetyLoweringContract &control_flow_contract,
    const std::string &control_flow_replay_key,
    const Objc3AutoreleasePoolScopeLoweringContract &autoreleasepool_contract,
    const std::string &autoreleasepool_replay_key,
    const Objc3AsyncContinuationLoweringContract &continuation_contract,
    const Objc3AwaitLoweringSuspensionStateLoweringContract &await_contract,
    const std::string &continuation_replay_key,
    const std::string &await_replay_key) {
  const bool deterministic = control_flow_contract.deterministic &&
                             autoreleasepool_contract.deterministic &&
                             continuation_contract.deterministic &&
                             await_contract.deterministic;
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencySuspensionCleanupIntegrationContractId)
      << "\",\"surface_path\":\""
      << EscapeJsonString(kObjc3ConcurrencySuspensionCleanupIntegrationSurfacePath)
      << "\",\"integration_model\":\""
      << EscapeJsonString(kObjc3ConcurrencySuspensionCleanupIntegrationModel)
      << "\",\"cleanup_ordering_model\":\""
      << EscapeJsonString(
             kObjc3ConcurrencySuspensionCleanupIntegrationOrderingModel)
      << "\",\"deferred_model\":\""
      << EscapeJsonString(kObjc3ConcurrencySuspensionCleanupIntegrationDeferredModel)
      << "\",\"async_lowering_contract_id\":\""
      << EscapeJsonString(kObjc3ConcurrencyAsyncDirectCallLoweringContractId)
      << "\",\"control_flow_contract_id\":\""
      << EscapeJsonString(kObjc3ControlFlowControlFlowSafetyLoweringContractId)
      << "\",\"autoreleasepool_lane_contract_id\":\""
      << EscapeJsonString(kObjc3AutoreleasePoolScopeLoweringLaneContract)
      << "\",\"continuation_lane_contract_id\":\""
      << EscapeJsonString(kObjc3AsyncContinuationLoweringLaneContract)
      << "\",\"await_lane_contract_id\":\""
      << EscapeJsonString(kObjc3AwaitLoweringSuspensionStateLoweringLaneContract)
      << "\",\"defer_statement_sites\":"
      << control_flow_contract.defer_statement_sites
      << ",\"live_defer_cleanup_sites\":"
      << control_flow_contract.live_defer_cleanup_sites
      << ",\"continuation_allocation_sites\":"
      << continuation_contract.continuation_allocation_sites
      << ",\"continuation_resume_sites\":"
      << continuation_contract.continuation_resume_sites
      << ",\"continuation_suspend_sites\":"
      << continuation_contract.continuation_suspend_sites
      << ",\"await_resume_sites\":" << await_contract.await_resume_sites
      << ",\"await_state_machine_sites\":"
      << await_contract.await_state_machine_sites
      << ",\"await_continuation_sites\":"
      << await_contract.await_continuation_sites
      << ",\"control_flow_replay_key\":\""
      << EscapeJsonString(control_flow_replay_key)
      << "\",\"autoreleasepool_replay_key\":\""
      << EscapeJsonString(autoreleasepool_replay_key)
      << "\",\"continuation_replay_key\":\""
      << EscapeJsonString(continuation_replay_key)
      << "\",\"await_replay_key\":\"" << EscapeJsonString(await_replay_key)
      << "\",\"autoreleasepool_scope_supported\":"
      << (!autoreleasepool_replay_key.empty() ? "true" : "false")
      << ",\"defer_cleanup_supported\":"
      << (control_flow_contract.live_defer_cleanup_sites > 0 ? "true" : "false")
      << ",\"direct_call_lowering_supported\":"
      << (deterministic ? "true" : "false")
      << ",\"suspension_runtime_required\":false"
      << ",\"state_machine_emission_present\":false"
      << ",\"deterministic\":" << (deterministic ? "true" : "false")
      << ",\"ready_for_ir_object_emission\":"
      << (deterministic ? "true" : "false")
      << "}";
  return out.str();
}
