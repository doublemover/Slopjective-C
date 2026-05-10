void CollectBlockLiteralMutatedIdentifiersFromSwitchStmt(
    const Stmt *stmt,
    std::vector<std::string> &mutated_identifiers) {
  if (stmt->switch_stmt == nullptr) {
    return;
  }
  for (const auto &switch_case : stmt->switch_stmt->cases) {
    CollectBlockLiteralMutatedIdentifiersFromStatementBody(
        switch_case.body, mutated_identifiers);
  }
}
