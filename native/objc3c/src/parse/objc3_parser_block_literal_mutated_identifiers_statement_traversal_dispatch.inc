void CollectBlockLiteralMutatedIdentifiersFromStatement(
    const Stmt *stmt,
    std::vector<std::string> &mutated_identifiers) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
  case Stmt::Kind::Return:
  case Stmt::Kind::Expr:
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  case Stmt::Kind::Assign:
    CollectBlockLiteralMutatedIdentifiersFromAssignStmt(
        stmt, mutated_identifiers);
    return;
  case Stmt::Kind::If:
    CollectBlockLiteralMutatedIdentifiersFromIfStmt(
        stmt, mutated_identifiers);
    return;
  case Stmt::Kind::DoWhile:
    CollectBlockLiteralMutatedIdentifiersFromDoWhileStmt(
        stmt, mutated_identifiers);
    return;
  case Stmt::Kind::For:
    CollectBlockLiteralMutatedIdentifiersFromForStmt(
        stmt, mutated_identifiers);
    return;
  case Stmt::Kind::Switch:
    CollectBlockLiteralMutatedIdentifiersFromSwitchStmt(
        stmt, mutated_identifiers);
    return;
  case Stmt::Kind::While:
    CollectBlockLiteralMutatedIdentifiersFromWhileStmt(
        stmt, mutated_identifiers);
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    CollectBlockLiteralMutatedIdentifiersFromBlockStmt(
        stmt, mutated_identifiers);
    return;
  }
}
