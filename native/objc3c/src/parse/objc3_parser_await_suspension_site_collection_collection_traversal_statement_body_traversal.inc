void CollectAwaitSuspensionStmtBodySites(
    const std::vector<std::unique_ptr<Stmt>> &body,
    Objc3AwaitSuspensionSiteCounts &counts) {
  for (const auto &body_stmt : body) {
    CollectAwaitSuspensionStmtSites(body_stmt.get(), counts);
  }
}
