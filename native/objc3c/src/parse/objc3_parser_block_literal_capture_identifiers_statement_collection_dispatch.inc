void CollectBlockLiteralStmtIdentifiers(
    const Stmt *stmt,
    std::vector<std::string> &used_identifiers,
    std::vector<std::string> &declared_identifiers) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    CollectBlockLiteralLetStmtIdentifiers(
        stmt->let_stmt.get(), used_identifiers, declared_identifiers);
    return;
  case Stmt::Kind::Assign:
    CollectBlockLiteralAssignStmtIdentifiers(
        stmt->assign_stmt.get(), used_identifiers);
    return;
  case Stmt::Kind::Return:
    CollectBlockLiteralReturnStmtIdentifiers(
        stmt->return_stmt.get(), used_identifiers);
    return;
  case Stmt::Kind::Expr:
    CollectBlockLiteralExprStmtIdentifiers(
        stmt->expr_stmt.get(), used_identifiers);
    return;
  case Stmt::Kind::If:
    CollectBlockLiteralIfStmtIdentifiers(
        stmt->if_stmt.get(), used_identifiers, declared_identifiers);
    return;
  case Stmt::Kind::DoWhile:
    CollectBlockLiteralDoWhileStmtIdentifiers(
        stmt->do_while_stmt.get(), used_identifiers, declared_identifiers);
    return;
  case Stmt::Kind::For:
    CollectBlockLiteralForStmtIdentifiers(
        stmt->for_stmt.get(), used_identifiers, declared_identifiers);
    return;
  case Stmt::Kind::ForIn:
    if (stmt->for_in_stmt != nullptr) {
      if (!stmt->for_in_stmt->value_name.empty()) {
        declared_identifiers.push_back(stmt->for_in_stmt->value_name);
      }
      if (stmt->for_in_stmt->has_key_binding &&
          !stmt->for_in_stmt->key_name.empty()) {
        declared_identifiers.push_back(stmt->for_in_stmt->key_name);
      }
      CollectBlockLiteralExprIdentifiers(
          stmt->for_in_stmt->collection.get(), used_identifiers);
      CollectBlockLiteralStmtBodyIdentifiers(
          stmt->for_in_stmt->body, used_identifiers, declared_identifiers);
    }
    return;
  case Stmt::Kind::Switch:
    CollectBlockLiteralSwitchStmtIdentifiers(
        stmt->switch_stmt.get(), used_identifiers, declared_identifiers);
    return;
  case Stmt::Kind::While:
    CollectBlockLiteralWhileStmtIdentifiers(
        stmt->while_stmt.get(), used_identifiers, declared_identifiers);
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    CollectBlockLiteralBlockStmtIdentifiers(
        stmt->block_stmt.get(), used_identifiers, declared_identifiers);
    return;
  case Stmt::Kind::CollectionMutation:
    if (stmt->collection_mutation_stmt != nullptr) {
      if (!stmt->collection_mutation_stmt->collection_name.empty()) {
        used_identifiers.push_back(
            stmt->collection_mutation_stmt->collection_name);
      }
      CollectBlockLiteralExprIdentifiers(
          stmt->collection_mutation_stmt->key_or_index.get(),
          used_identifiers);
      CollectBlockLiteralExprIdentifiers(
          stmt->collection_mutation_stmt->value.get(), used_identifiers);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}
