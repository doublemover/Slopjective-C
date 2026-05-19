void CollectBlockLiteralMutatedIdentifiersFromIfStmt(
    const Stmt *stmt,
    std::vector<std::string> &mutated_identifiers) {
  if (stmt->if_stmt == nullptr) {
    return;
  }
  CollectBlockLiteralMutatedIdentifiersFromStatementBody(
      stmt->if_stmt->then_body, mutated_identifiers);
  CollectBlockLiteralMutatedIdentifiersFromStatementBody(
      stmt->if_stmt->else_body, mutated_identifiers);
}

void CollectBlockLiteralMutatedIdentifiersFromBlockStmt(
    const Stmt *stmt,
    std::vector<std::string> &mutated_identifiers) {
  if (stmt->block_stmt == nullptr) {
    return;
  }
  CollectBlockLiteralMutatedIdentifiersFromStatementBody(
      stmt->block_stmt->body, mutated_identifiers);
}
