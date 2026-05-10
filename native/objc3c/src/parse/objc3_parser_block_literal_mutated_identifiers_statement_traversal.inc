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
    if (BlockLiteralAssignStatementMutatesIdentifier(stmt->assign_stmt.get())) {
      mutated_identifiers.push_back(stmt->assign_stmt->name);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectBlockLiteralMutatedIdentifiersFromStatementBody(
          stmt->if_stmt->then_body, mutated_identifiers);
      CollectBlockLiteralMutatedIdentifiersFromStatementBody(
          stmt->if_stmt->else_body, mutated_identifiers);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      CollectBlockLiteralMutatedIdentifiersFromStatementBody(
          stmt->do_while_stmt->body, mutated_identifiers);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectBlockLiteralMutatedIdentifiersFromForClause(
          stmt->for_stmt->init, mutated_identifiers);
      CollectBlockLiteralMutatedIdentifiersFromForClause(
          stmt->for_stmt->step, mutated_identifiers);
      CollectBlockLiteralMutatedIdentifiersFromStatementBody(
          stmt->for_stmt->body, mutated_identifiers);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        CollectBlockLiteralMutatedIdentifiersFromStatementBody(
            switch_case.body, mutated_identifiers);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectBlockLiteralMutatedIdentifiersFromStatementBody(
          stmt->while_stmt->body, mutated_identifiers);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      CollectBlockLiteralMutatedIdentifiersFromStatementBody(
          stmt->block_stmt->body, mutated_identifiers);
    }
    return;
  }
}
