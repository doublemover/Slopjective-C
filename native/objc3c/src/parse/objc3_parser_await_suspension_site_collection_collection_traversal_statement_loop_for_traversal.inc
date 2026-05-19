void CollectAwaitSuspensionDoWhileStmtSites(
    const DoWhileStmt *stmt,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  CollectAwaitSuspensionStmtBodySites(stmt->body, counts);
  CollectAwaitSuspensionExprSites(stmt->condition.get(), counts);
}

void CollectAwaitSuspensionForStmtSites(
    const ForStmt *stmt,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  CollectAwaitSuspensionForClauseSites(stmt->init, counts);
  CollectAwaitSuspensionExprSites(stmt->condition.get(), counts);
  CollectAwaitSuspensionForClauseSites(stmt->step, counts);
  CollectAwaitSuspensionStmtBodySites(stmt->body, counts);
}

void CollectAwaitSuspensionWhileStmtSites(
    const WhileStmt *stmt,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  CollectAwaitSuspensionExprSites(stmt->condition.get(), counts);
  CollectAwaitSuspensionStmtBodySites(stmt->body, counts);
}
