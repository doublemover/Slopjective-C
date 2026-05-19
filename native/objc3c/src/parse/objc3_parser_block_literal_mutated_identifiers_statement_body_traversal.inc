void CollectBlockLiteralMutatedIdentifiersFromStatementBody(
    const std::vector<std::unique_ptr<Stmt>> &body,
    std::vector<std::string> &mutated_identifiers) {
  for (const auto &body_stmt : body) {
    CollectBlockLiteralMutatedIdentifiersFromStatement(
        body_stmt.get(), mutated_identifiers);
  }
}
