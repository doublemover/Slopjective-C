namespace {

void WalkObjc3ProfileSymbolsInDoWhileStmt(
    const Stmt *stmt,
    const Objc3ProfileSymbolWalker &walker) {
  if (stmt->do_while_stmt == nullptr) {
    return;
  }
  WalkObjc3ProfileSymbolsInStmtBody(stmt->do_while_stmt->body, walker);
  WalkObjc3ProfileSymbolsInExpr(stmt->do_while_stmt->condition.get(), walker);
}

void WalkObjc3ProfileSymbolsInForStmt(
    const Stmt *stmt,
    const Objc3ProfileSymbolWalker &walker) {
  if (stmt->for_stmt == nullptr) {
    return;
  }
  WalkObjc3ProfileSymbolsInForClause(stmt->for_stmt->init, walker);
  WalkObjc3ProfileSymbolsInExpr(stmt->for_stmt->condition.get(), walker);
  WalkObjc3ProfileSymbolsInForClause(stmt->for_stmt->step, walker);
  WalkObjc3ProfileSymbolsInStmtBody(stmt->for_stmt->body, walker);
}

void WalkObjc3ProfileSymbolsInWhileStmt(
    const Stmt *stmt,
    const Objc3ProfileSymbolWalker &walker) {
  if (stmt->while_stmt == nullptr) {
    return;
  }
  WalkObjc3ProfileSymbolsInExpr(stmt->while_stmt->condition.get(), walker);
  WalkObjc3ProfileSymbolsInStmtBody(stmt->while_stmt->body, walker);
}

}  // namespace
