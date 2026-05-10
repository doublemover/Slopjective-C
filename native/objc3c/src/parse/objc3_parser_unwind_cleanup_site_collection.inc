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

struct Objc3UnwindCleanupSiteCounts {
  std::size_t exceptional_exit_sites = 0;
  std::size_t cleanup_action_sites = 0;
  std::size_t cleanup_scope_sites = 0;
  std::size_t cleanup_resume_sites = 0;
};

void CollectUnwindCleanupSitesFromSymbol(
    const std::string &symbol,
    Objc3UnwindCleanupSiteCounts &counts) {
  if (IsExceptionalExitSymbol(symbol)) {
    counts.exceptional_exit_sites += 1u;
  }
  if (IsCleanupActionSymbol(symbol)) {
    counts.cleanup_action_sites += 1u;
  }
  if (IsCleanupScopeSymbol(symbol)) {
    counts.cleanup_scope_sites += 1u;
  }
  if (IsCleanupResumeSymbol(symbol)) {
    counts.cleanup_resume_sites += 1u;
  }
}

void CollectUnwindCleanupProfileSymbol(
    const std::string &symbol,
    void *context) {
  auto *counts = static_cast<Objc3UnwindCleanupSiteCounts *>(context);
  CollectUnwindCleanupSitesFromSymbol(symbol, *counts);
}

Objc3UnwindCleanupSiteCounts CountUnwindCleanupSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3UnwindCleanupSiteCounts counts;
  Objc3ProfileSymbolWalker walker{
      &CollectUnwindCleanupProfileSymbol, &counts};
  WalkObjc3ProfileSymbolsInBody(body, walker);
  return counts;
}
