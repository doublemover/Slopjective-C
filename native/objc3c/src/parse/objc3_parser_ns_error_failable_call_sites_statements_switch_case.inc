std::size_t CountFailableCallSitesInSwitchStmt(const Stmt *stmt) {
  if (stmt->switch_stmt == nullptr) {
    return 0;
  }

  std::size_t count =
      CountFailableCallSitesInExpr(stmt->switch_stmt->condition.get());
  for (const auto &switch_case : stmt->switch_stmt->cases) {
    count += CountFailableCallSitesInStmtBody(switch_case.body);
  }
  return count;
}
