void CollectBlockLiteralMutatedIdentifiersFromForClause(
    const ForClause &clause,
    std::vector<std::string> &mutated_identifiers) {
  if (clause.kind == ForClause::Kind::Assign && !clause.name.empty()) {
    mutated_identifiers.push_back(clause.name);
  }
}

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
    if (stmt->assign_stmt != nullptr && !stmt->assign_stmt->name.empty()) {
      mutated_identifiers.push_back(stmt->assign_stmt->name);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        CollectBlockLiteralMutatedIdentifiersFromStatement(
            then_stmt.get(), mutated_identifiers);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectBlockLiteralMutatedIdentifiersFromStatement(
            else_stmt.get(), mutated_identifiers);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectBlockLiteralMutatedIdentifiersFromStatement(
            body_stmt.get(), mutated_identifiers);
      }
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectBlockLiteralMutatedIdentifiersFromForClause(
          stmt->for_stmt->init, mutated_identifiers);
      CollectBlockLiteralMutatedIdentifiersFromForClause(
          stmt->for_stmt->step, mutated_identifiers);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectBlockLiteralMutatedIdentifiersFromStatement(
            body_stmt.get(), mutated_identifiers);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          CollectBlockLiteralMutatedIdentifiersFromStatement(
              case_stmt.get(), mutated_identifiers);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectBlockLiteralMutatedIdentifiersFromStatement(
            body_stmt.get(), mutated_identifiers);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectBlockLiteralMutatedIdentifiersFromStatement(
            body_stmt.get(), mutated_identifiers);
      }
    }
    return;
  }
}
