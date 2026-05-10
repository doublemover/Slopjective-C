bool IsAwaitKeywordSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered == "await" || lowered.find("await_") != std::string::npos;
}

bool IsAwaitSuspensionPointSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("suspend") != std::string::npos ||
         lowered.find("yield") != std::string::npos;
}

bool IsAwaitResumeSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("resume") != std::string::npos ||
         lowered.find("wakeup") != std::string::npos;
}

bool IsAwaitStateMachineSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("state") != std::string::npos ||
         lowered.find("continuation") != std::string::npos ||
         lowered.find("poll") != std::string::npos;
}

bool IsAwaitContinuationSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("continuation") != std::string::npos ||
         lowered.find("future") != std::string::npos ||
         lowered.find("promise") != std::string::npos;
}
