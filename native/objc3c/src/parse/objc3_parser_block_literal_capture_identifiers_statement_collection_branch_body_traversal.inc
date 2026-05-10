void CollectBlockLiteralIfStmtIdentifiers(
    const IfStmt *stmt,
    std::vector<std::string> &used_identifiers,
    std::vector<std::string> &declared_identifiers) {
  if (stmt == nullptr) {
    return;
  }
  CollectBlockLiteralExprIdentifiers(stmt->condition.get(), used_identifiers);
  CollectBlockLiteralStmtBodyIdentifiers(
      stmt->then_body, used_identifiers, declared_identifiers);
  CollectBlockLiteralStmtBodyIdentifiers(
      stmt->else_body, used_identifiers, declared_identifiers);
}

void CollectBlockLiteralBlockStmtIdentifiers(
    const BlockStmt *stmt,
    std::vector<std::string> &used_identifiers,
    std::vector<std::string> &declared_identifiers) {
  if (stmt == nullptr) {
    return;
  }
  CollectBlockLiteralStmtBodyIdentifiers(
      stmt->body, used_identifiers, declared_identifiers);
}
