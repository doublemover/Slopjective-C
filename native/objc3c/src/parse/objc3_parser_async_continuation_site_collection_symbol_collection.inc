void CollectAsyncContinuationSitesFromSymbol(
    const std::string &symbol,
    Objc3AsyncContinuationSiteCounts &counts) {
  if (IsAsyncKeywordSymbol(symbol)) {
    counts.async_keyword_sites += 1u;
  }
  if (IsAsyncFunctionSymbol(symbol)) {
    counts.async_function_sites += 1u;
  }
  if (IsContinuationAllocationSymbol(symbol)) {
    counts.continuation_allocation_sites += 1u;
  }
  if (IsContinuationResumeSymbol(symbol)) {
    counts.continuation_resume_sites += 1u;
  }
  if (IsContinuationSuspendSymbol(symbol)) {
    counts.continuation_suspend_sites += 1u;
  }
  if (IsAsyncStateMachineSymbol(symbol)) {
    counts.async_state_machine_sites += 1u;
  }
}

void CollectAsyncContinuationProfileSymbol(
    const std::string &symbol,
    void *context) {
  auto *counts = static_cast<Objc3AsyncContinuationSiteCounts *>(context);
  CollectAsyncContinuationSitesFromSymbol(symbol, *counts);
}
