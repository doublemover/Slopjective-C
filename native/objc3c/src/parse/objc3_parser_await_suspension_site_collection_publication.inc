Objc3AwaitSuspensionSiteCounts CountAwaitSuspensionSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3AwaitSuspensionSiteCounts counts;
  for (const auto &stmt : body) {
    CollectAwaitSuspensionStmtSites(stmt.get(), counts);
  }
  return counts;
}
