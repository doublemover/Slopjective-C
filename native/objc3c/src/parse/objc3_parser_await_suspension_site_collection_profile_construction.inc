struct Objc3AwaitSuspensionSiteCounts {
  std::size_t await_keyword_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t await_state_machine_sites = 0;
  std::size_t await_continuation_sites = 0;
};

void CollectAwaitSuspensionSitesFromSymbol(
    const std::string &symbol,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (IsAwaitKeywordSymbol(symbol)) {
    counts.await_keyword_sites += 1u;
  }
  if (IsAwaitSuspensionPointSymbol(symbol)) {
    counts.await_suspension_point_sites += 1u;
  }
  if (IsAwaitResumeSymbol(symbol)) {
    counts.await_resume_sites += 1u;
  }
  if (IsAwaitStateMachineSymbol(symbol)) {
    counts.await_state_machine_sites += 1u;
  }
  if (IsAwaitContinuationSymbol(symbol)) {
    counts.await_continuation_sites += 1u;
  }
}
