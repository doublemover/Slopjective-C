void CollectBlockLiteralStmtBodyIdentifiers(
    const std::vector<std::unique_ptr<Stmt>> &body,
    std::vector<std::string> &used_identifiers,
    std::vector<std::string> &declared_identifiers) {
  for (const auto &body_stmt : body) {
    CollectBlockLiteralStmtIdentifiers(
        body_stmt.get(), used_identifiers, declared_identifiers);
  }
}
