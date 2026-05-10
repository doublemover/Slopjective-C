void CollectBlockLiteralLetStmtIdentifiers(
    const LetStmt *stmt,
    std::vector<std::string> &used_identifiers,
    std::vector<std::string> &declared_identifiers) {
  if (stmt == nullptr) {
    return;
  }
  if (!stmt->name.empty()) {
    declared_identifiers.push_back(stmt->name);
  }
  CollectBlockLiteralExprIdentifiers(stmt->value.get(), used_identifiers);
}

void CollectBlockLiteralAssignStmtIdentifiers(
    const AssignStmt *stmt,
    std::vector<std::string> &used_identifiers) {
  if (stmt == nullptr) {
    return;
  }
  if (!stmt->name.empty()) {
    used_identifiers.push_back(stmt->name);
  }
  CollectBlockLiteralExprIdentifiers(stmt->value.get(), used_identifiers);
}

void CollectBlockLiteralReturnStmtIdentifiers(
    const ReturnStmt *stmt,
    std::vector<std::string> &used_identifiers) {
  if (stmt == nullptr) {
    return;
  }
  CollectBlockLiteralExprIdentifiers(stmt->value.get(), used_identifiers);
}

void CollectBlockLiteralExprStmtIdentifiers(
    const ExprStmt *stmt,
    std::vector<std::string> &used_identifiers) {
  if (stmt == nullptr) {
    return;
  }
  CollectBlockLiteralExprIdentifiers(stmt->value.get(), used_identifiers);
}
