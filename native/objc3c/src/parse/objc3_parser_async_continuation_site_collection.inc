bool IsAsyncKeywordSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered == "async" || lowered.find("async_") != std::string::npos;
}

bool IsAsyncFunctionSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("async_fn") != std::string::npos ||
         lowered.find("future") != std::string::npos ||
         lowered.find("task") != std::string::npos;
}

bool IsContinuationAllocationSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("continuation_alloc") != std::string::npos ||
         lowered.find("make_continuation") != std::string::npos ||
         lowered.find("continuation_new") != std::string::npos;
}

bool IsContinuationResumeSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("continuation_resume") != std::string::npos ||
         lowered.find("resume_continuation") != std::string::npos ||
         lowered.find("resume") != std::string::npos;
}

bool IsContinuationSuspendSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("continuation_suspend") != std::string::npos ||
         lowered.find("suspend_continuation") != std::string::npos ||
         lowered.find("suspend") != std::string::npos;
}

bool IsAsyncStateMachineSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("state_machine") != std::string::npos ||
         lowered.find("poll") != std::string::npos ||
         lowered.find("waker") != std::string::npos;
}

struct Objc3AsyncContinuationSiteCounts {
  std::size_t async_keyword_sites = 0;
  std::size_t async_function_sites = 0;
  std::size_t continuation_allocation_sites = 0;
  std::size_t continuation_resume_sites = 0;
  std::size_t continuation_suspend_sites = 0;
  std::size_t async_state_machine_sites = 0;
};

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
  auto *counts =
      static_cast<Objc3AsyncContinuationSiteCounts *>(context);
  CollectAsyncContinuationSitesFromSymbol(symbol, *counts);
}

Objc3AsyncContinuationSiteCounts CountAsyncContinuationSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3AsyncContinuationSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectAsyncContinuationProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInBody(body, walker);
  return counts;
}
