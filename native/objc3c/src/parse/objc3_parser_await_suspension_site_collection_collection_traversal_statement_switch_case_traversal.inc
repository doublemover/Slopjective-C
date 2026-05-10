void CollectAwaitSuspensionSwitchStmtSites(
    const SwitchStmt *stmt,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  CollectAwaitSuspensionExprSites(stmt->condition.get(), counts);
  for (const auto &switch_case : stmt->cases) {
    CollectAwaitSuspensionStmtBodySites(switch_case.body, counts);
  }
}
