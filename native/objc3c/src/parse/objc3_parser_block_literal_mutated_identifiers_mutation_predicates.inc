bool BlockLiteralForClauseMutatesIdentifier(const ForClause &clause) {
  return clause.kind == ForClause::Kind::Assign && !clause.name.empty();
}

bool BlockLiteralAssignStatementMutatesIdentifier(const AssignStmt *stmt) {
  return stmt != nullptr && !stmt->name.empty();
}
