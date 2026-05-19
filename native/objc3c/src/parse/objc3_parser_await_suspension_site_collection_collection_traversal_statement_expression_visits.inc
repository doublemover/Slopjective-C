void CollectAwaitSuspensionLetStmtSites(
    const LetStmt *stmt,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  CollectAwaitSuspensionExprSites(stmt->value.get(), counts);
}

void CollectAwaitSuspensionAssignStmtSites(
    const AssignStmt *stmt,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  CollectAwaitSuspensionExprSites(stmt->value.get(), counts);
}

void CollectAwaitSuspensionReturnStmtSites(
    const ReturnStmt *stmt,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  CollectAwaitSuspensionExprSites(stmt->value.get(), counts);
}

void CollectAwaitSuspensionExprStmtSites(
    const ExprStmt *stmt,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  CollectAwaitSuspensionExprSites(stmt->value.get(), counts);
}
