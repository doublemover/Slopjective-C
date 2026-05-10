void CollectBlockLiteralMutatedIdentifiersFromForClause(
    const ForClause &clause,
    std::vector<std::string> &mutated_identifiers) {
  if (BlockLiteralForClauseMutatesIdentifier(clause)) {
    mutated_identifiers.push_back(clause.name);
  }
}
