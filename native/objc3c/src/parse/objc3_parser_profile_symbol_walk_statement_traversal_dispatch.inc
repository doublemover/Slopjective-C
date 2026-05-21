void WalkObjc3ProfileSymbolsInStmt(
    const Stmt *stmt,
    const Objc3ProfileSymbolWalker &walker) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    WalkObjc3ProfileSymbolsInLetStmt(stmt, walker);
    return;
  case Stmt::Kind::Assign:
    WalkObjc3ProfileSymbolsInAssignStmt(stmt, walker);
    return;
  case Stmt::Kind::Return:
    WalkObjc3ProfileSymbolsInReturnStmt(stmt, walker);
    return;
  case Stmt::Kind::If:
    WalkObjc3ProfileSymbolsInIfStmt(stmt, walker);
    return;
  case Stmt::Kind::DoWhile:
    WalkObjc3ProfileSymbolsInDoWhileStmt(stmt, walker);
    return;
  case Stmt::Kind::For:
    WalkObjc3ProfileSymbolsInForStmt(stmt, walker);
    return;
  case Stmt::Kind::ForIn:
    if (stmt->for_in_stmt != nullptr) {
      WalkObjc3ProfileSymbolsInExpr(stmt->for_in_stmt->collection.get(),
                                    walker);
      WalkObjc3ProfileSymbolsInStmtBody(stmt->for_in_stmt->body, walker);
    }
    return;
  case Stmt::Kind::Switch:
    WalkObjc3ProfileSymbolsInSwitchStmt(stmt, walker);
    return;
  case Stmt::Kind::While:
    WalkObjc3ProfileSymbolsInWhileStmt(stmt, walker);
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    WalkObjc3ProfileSymbolsInBlockStmt(stmt, walker);
    return;
  case Stmt::Kind::Expr:
    WalkObjc3ProfileSymbolsInExprStmt(stmt, walker);
    return;
  case Stmt::Kind::CollectionMutation:
    if (stmt->collection_mutation_stmt != nullptr) {
      WalkObjc3ProfileSymbolsInExpr(
          stmt->collection_mutation_stmt->key_or_index.get(), walker);
      WalkObjc3ProfileSymbolsInExpr(
          stmt->collection_mutation_stmt->value.get(), walker);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}
