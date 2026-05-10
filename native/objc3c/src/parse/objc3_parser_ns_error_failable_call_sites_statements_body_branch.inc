std::size_t CountFailableCallSitesInStmtBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  std::size_t count = 0;
  for (const auto &body_stmt : body) {
    count += CountFailableCallSitesInStmt(body_stmt.get());
  }
  return count;
}

std::size_t CountFailableCallSitesInIfStmt(const Stmt *stmt) {
  if (stmt->if_stmt == nullptr) {
    return 0;
  }

  std::size_t count =
      CountFailableCallSitesInExpr(stmt->if_stmt->condition.get());
  count += CountFailableCallSitesInStmtBody(stmt->if_stmt->then_body);
  count += CountFailableCallSitesInStmtBody(stmt->if_stmt->else_body);
  return count;
}

std::size_t CountFailableCallSitesInBlockStmt(const Stmt *stmt) {
  if (stmt->block_stmt == nullptr) {
    return 0;
  }

  return CountFailableCallSitesInStmtBody(stmt->block_stmt->body);
}
