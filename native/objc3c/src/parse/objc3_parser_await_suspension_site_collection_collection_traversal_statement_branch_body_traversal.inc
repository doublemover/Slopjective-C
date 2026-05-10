void CollectAwaitSuspensionIfStmtSites(
    const IfStmt *stmt,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  CollectAwaitSuspensionExprSites(stmt->condition.get(), counts);
  CollectAwaitSuspensionStmtBodySites(stmt->then_body, counts);
  CollectAwaitSuspensionStmtBodySites(stmt->else_body, counts);
}

void CollectAwaitSuspensionBlockStmtSites(
    const BlockStmt *stmt,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  CollectAwaitSuspensionStmtBodySites(stmt->body, counts);
}
