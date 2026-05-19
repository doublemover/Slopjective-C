void CollectBlockLiteralSwitchStmtIdentifiers(
    const SwitchStmt *stmt,
    std::vector<std::string> &used_identifiers,
    std::vector<std::string> &declared_identifiers) {
  if (stmt == nullptr) {
    return;
  }
  CollectBlockLiteralExprIdentifiers(stmt->condition.get(), used_identifiers);
  for (const auto &switch_case : stmt->cases) {
    CollectBlockLiteralStmtBodyIdentifiers(
        switch_case.body, used_identifiers, declared_identifiers);
  }
}
