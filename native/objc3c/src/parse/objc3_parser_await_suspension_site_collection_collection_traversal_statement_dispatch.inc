void CollectAwaitSuspensionStmtSites(
    const Stmt *stmt,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    CollectAwaitSuspensionLetStmtSites(stmt->let_stmt.get(), counts);
    return;
  case Stmt::Kind::Assign:
    CollectAwaitSuspensionAssignStmtSites(stmt->assign_stmt.get(), counts);
    return;
  case Stmt::Kind::Return:
    CollectAwaitSuspensionReturnStmtSites(stmt->return_stmt.get(), counts);
    return;
  case Stmt::Kind::If:
    CollectAwaitSuspensionIfStmtSites(stmt->if_stmt.get(), counts);
    return;
  case Stmt::Kind::DoWhile:
    CollectAwaitSuspensionDoWhileStmtSites(stmt->do_while_stmt.get(), counts);
    return;
  case Stmt::Kind::For:
    CollectAwaitSuspensionForStmtSites(stmt->for_stmt.get(), counts);
    return;
  case Stmt::Kind::ForIn:
    if (stmt->for_in_stmt != nullptr) {
      CollectAwaitSuspensionExprSites(stmt->for_in_stmt->collection.get(),
                                      counts);
      CollectAwaitSuspensionStmtBodySites(stmt->for_in_stmt->body, counts);
    }
    return;
  case Stmt::Kind::Switch:
    CollectAwaitSuspensionSwitchStmtSites(stmt->switch_stmt.get(), counts);
    return;
  case Stmt::Kind::While:
    CollectAwaitSuspensionWhileStmtSites(stmt->while_stmt.get(), counts);
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    CollectAwaitSuspensionBlockStmtSites(stmt->block_stmt.get(), counts);
    return;
  case Stmt::Kind::Expr:
    CollectAwaitSuspensionExprStmtSites(stmt->expr_stmt.get(), counts);
    return;
  case Stmt::Kind::CollectionMutation:
    if (stmt->collection_mutation_stmt != nullptr) {
      CollectAwaitSuspensionExprSites(
          stmt->collection_mutation_stmt->key_or_index.get(), counts);
      CollectAwaitSuspensionExprSites(
          stmt->collection_mutation_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}
