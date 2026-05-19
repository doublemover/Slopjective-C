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
