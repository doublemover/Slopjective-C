void CollectPointerArithmeticStmtSites(const Stmt *stmt, std::size_t &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    CollectPointerArithmeticLetStmtSites(stmt, sites);
    return;
  case Stmt::Kind::Assign:
    CollectPointerArithmeticAssignStmtSites(stmt, sites);
    return;
  case Stmt::Kind::Return:
    CollectPointerArithmeticReturnStmtSites(stmt, sites);
    return;
  case Stmt::Kind::If:
    CollectPointerArithmeticIfStmtSites(stmt, sites);
    return;
  case Stmt::Kind::DoWhile:
    CollectPointerArithmeticDoWhileStmtSites(stmt, sites);
    return;
  case Stmt::Kind::For:
    CollectPointerArithmeticForStmtSites(stmt, sites);
    return;
  case Stmt::Kind::ForIn:
    if (stmt->for_in_stmt != nullptr) {
      CollectPointerArithmeticExprSites(stmt->for_in_stmt->collection.get(),
                                        sites);
      CollectPointerArithmeticStmtBodySites(stmt->for_in_stmt->body, sites);
    }
    return;
  case Stmt::Kind::Switch:
    CollectPointerArithmeticSwitchStmtSites(stmt, sites);
    return;
  case Stmt::Kind::While:
    CollectPointerArithmeticWhileStmtSites(stmt, sites);
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    CollectPointerArithmeticBlockStmtSites(stmt, sites);
    return;
  case Stmt::Kind::Expr:
    CollectPointerArithmeticExprStmtSites(stmt, sites);
    return;
  case Stmt::Kind::CollectionMutation:
    if (stmt->collection_mutation_stmt != nullptr) {
      CollectPointerArithmeticExprSites(
          stmt->collection_mutation_stmt->key_or_index.get(), sites);
      CollectPointerArithmeticExprSites(
          stmt->collection_mutation_stmt->value.get(), sites);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}
