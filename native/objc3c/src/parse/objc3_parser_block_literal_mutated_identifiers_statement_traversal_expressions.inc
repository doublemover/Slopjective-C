void CollectBlockLiteralMutatedIdentifiersFromAssignStmt(
    const Stmt *stmt,
    std::vector<std::string> &mutated_identifiers) {
  if (BlockLiteralAssignStatementMutatesIdentifier(stmt->assign_stmt.get())) {
    mutated_identifiers.push_back(stmt->assign_stmt->name);
  }
}
