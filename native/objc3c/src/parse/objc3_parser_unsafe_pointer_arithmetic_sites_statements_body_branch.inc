void CollectPointerArithmeticStmtBodySites(
    const std::vector<std::unique_ptr<Stmt>> &body, std::size_t &sites) {
  for (const auto &body_stmt : body) {
    CollectPointerArithmeticStmtSites(body_stmt.get(), sites);
  }
}

void CollectPointerArithmeticIfStmtSites(const Stmt *stmt, std::size_t &sites) {
  if (stmt->if_stmt == nullptr) {
    return;
  }

  CollectPointerArithmeticExprSites(stmt->if_stmt->condition.get(), sites);
  CollectPointerArithmeticStmtBodySites(stmt->if_stmt->then_body, sites);
  CollectPointerArithmeticStmtBodySites(stmt->if_stmt->else_body, sites);
}

void CollectPointerArithmeticBlockStmtSites(const Stmt *stmt,
                                           std::size_t &sites) {
  if (stmt->block_stmt == nullptr) {
    return;
  }

  CollectPointerArithmeticStmtBodySites(stmt->block_stmt->body, sites);
}
