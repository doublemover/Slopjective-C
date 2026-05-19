bool IsExceptionalExitSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("throw") != std::string::npos ||
         lowered.find("exception") != std::string::npos ||
         lowered.find("unwind") != std::string::npos ||
         lowered.find("raise") != std::string::npos;
}

bool IsCleanupActionSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("cleanup") != std::string::npos ||
         lowered.find("defer") != std::string::npos ||
         lowered.find("finally") != std::string::npos ||
         lowered.find("release") != std::string::npos;
}

bool IsCleanupScopeSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("cleanup_scope") != std::string::npos ||
         lowered.find("scope_guard") != std::string::npos ||
         lowered.find("defer_scope") != std::string::npos ||
         lowered.find("guard_scope") != std::string::npos;
}

bool IsCleanupResumeSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("resume") != std::string::npos ||
         lowered.find("rethrow") != std::string::npos ||
         lowered.find("continue_unwind") != std::string::npos;
}
