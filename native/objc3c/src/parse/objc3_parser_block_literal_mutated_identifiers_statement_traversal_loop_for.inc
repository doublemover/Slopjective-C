void CollectBlockLiteralMutatedIdentifiersFromDoWhileStmt(
    const Stmt *stmt,
    std::vector<std::string> &mutated_identifiers) {
  if (stmt->do_while_stmt == nullptr) {
    return;
  }
  CollectBlockLiteralMutatedIdentifiersFromStatementBody(
      stmt->do_while_stmt->body, mutated_identifiers);
}

void CollectBlockLiteralMutatedIdentifiersFromForStmt(
    const Stmt *stmt,
    std::vector<std::string> &mutated_identifiers) {
  if (stmt->for_stmt == nullptr) {
    return;
  }
  CollectBlockLiteralMutatedIdentifiersFromForClause(
      stmt->for_stmt->init, mutated_identifiers);
  CollectBlockLiteralMutatedIdentifiersFromForClause(
      stmt->for_stmt->step, mutated_identifiers);
  CollectBlockLiteralMutatedIdentifiersFromStatementBody(
      stmt->for_stmt->body, mutated_identifiers);
}

void CollectBlockLiteralMutatedIdentifiersFromWhileStmt(
    const Stmt *stmt,
    std::vector<std::string> &mutated_identifiers) {
  if (stmt->while_stmt == nullptr) {
    return;
  }
  CollectBlockLiteralMutatedIdentifiersFromStatementBody(
      stmt->while_stmt->body, mutated_identifiers);
}
