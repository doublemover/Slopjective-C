namespace {

void WalkObjc3ProfileSymbolsInStmtBody(
    const std::vector<std::unique_ptr<Stmt>> &body,
    const Objc3ProfileSymbolWalker &walker) {
  for (const auto &body_stmt : body) {
    WalkObjc3ProfileSymbolsInStmt(body_stmt.get(), walker);
  }
}

void WalkObjc3ProfileSymbolsInIfStmt(
    const Stmt *stmt,
    const Objc3ProfileSymbolWalker &walker) {
  if (stmt->if_stmt == nullptr) {
    return;
  }
  WalkObjc3ProfileSymbolsInExpr(stmt->if_stmt->condition.get(), walker);
  WalkObjc3ProfileSymbolsInStmtBody(stmt->if_stmt->then_body, walker);
  WalkObjc3ProfileSymbolsInStmtBody(stmt->if_stmt->else_body, walker);
}

void WalkObjc3ProfileSymbolsInBlockStmt(
    const Stmt *stmt,
    const Objc3ProfileSymbolWalker &walker) {
  if (stmt->block_stmt == nullptr) {
    return;
  }
  WalkObjc3ProfileSymbolsInStmtBody(stmt->block_stmt->body, walker);
}

}  // namespace
