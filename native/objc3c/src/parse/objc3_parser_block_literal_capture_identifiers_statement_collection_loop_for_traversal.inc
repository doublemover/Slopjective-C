void CollectBlockLiteralDoWhileStmtIdentifiers(
    const DoWhileStmt *stmt,
    std::vector<std::string> &used_identifiers,
    std::vector<std::string> &declared_identifiers) {
  if (stmt == nullptr) {
    return;
  }
  CollectBlockLiteralStmtBodyIdentifiers(
      stmt->body, used_identifiers, declared_identifiers);
  CollectBlockLiteralExprIdentifiers(stmt->condition.get(), used_identifiers);
}

void CollectBlockLiteralForStmtIdentifiers(
    const ForStmt *stmt,
    std::vector<std::string> &used_identifiers,
    std::vector<std::string> &declared_identifiers) {
  if (stmt == nullptr) {
    return;
  }
  CollectBlockLiteralForClauseIdentifiers(
      stmt->init, used_identifiers, declared_identifiers);
  CollectBlockLiteralExprIdentifiers(stmt->condition.get(), used_identifiers);
  CollectBlockLiteralForClauseIdentifiers(
      stmt->step, used_identifiers, declared_identifiers);
  CollectBlockLiteralStmtBodyIdentifiers(
      stmt->body, used_identifiers, declared_identifiers);
}

void CollectBlockLiteralWhileStmtIdentifiers(
    const WhileStmt *stmt,
    std::vector<std::string> &used_identifiers,
    std::vector<std::string> &declared_identifiers) {
  if (stmt == nullptr) {
    return;
  }
  CollectBlockLiteralExprIdentifiers(stmt->condition.get(), used_identifiers);
  CollectBlockLiteralStmtBodyIdentifiers(
      stmt->body, used_identifiers, declared_identifiers);
}
