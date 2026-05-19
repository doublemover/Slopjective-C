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
