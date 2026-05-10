void CollectBlockLiteralForClauseIdentifiers(
    const ForClause &clause,
    std::vector<std::string> &used_identifiers,
    std::vector<std::string> &declared_identifiers) {
  if (clause.kind == ForClause::Kind::Let && !clause.name.empty()) {
    declared_identifiers.push_back(clause.name);
  } else if ((clause.kind == ForClause::Kind::Assign ||
              clause.kind == ForClause::Kind::Expr) &&
             !clause.name.empty()) {
    used_identifiers.push_back(clause.name);
  }
  CollectBlockLiteralExprIdentifiers(clause.value.get(), used_identifiers);
}
