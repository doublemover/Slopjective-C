std::size_t CountFailableCallSitesInDoWhileStmt(const Stmt *stmt) {
  if (stmt->do_while_stmt == nullptr) {
    return 0;
  }

  std::size_t count =
      CountFailableCallSitesInExpr(stmt->do_while_stmt->condition.get());
  count += CountFailableCallSitesInStmtBody(stmt->do_while_stmt->body);
  return count;
}

std::size_t CountFailableCallSitesInForStmt(const Stmt *stmt) {
  if (stmt->for_stmt == nullptr) {
    return 0;
  }

  std::size_t count = CountFailableCallSitesInForClause(stmt->for_stmt->init);
  count += CountFailableCallSitesInExpr(stmt->for_stmt->condition.get());
  count += CountFailableCallSitesInForClause(stmt->for_stmt->step);
  count += CountFailableCallSitesInStmtBody(stmt->for_stmt->body);
  return count;
}

std::size_t CountFailableCallSitesInWhileStmt(const Stmt *stmt) {
  if (stmt->while_stmt == nullptr) {
    return 0;
  }

  std::size_t count =
      CountFailableCallSitesInExpr(stmt->while_stmt->condition.get());
  count += CountFailableCallSitesInStmtBody(stmt->while_stmt->body);
  return count;
}
