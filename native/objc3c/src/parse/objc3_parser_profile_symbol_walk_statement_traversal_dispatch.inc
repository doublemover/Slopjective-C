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
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}
