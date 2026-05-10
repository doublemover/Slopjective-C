namespace {

void WalkObjc3ProfileSymbolsInSwitchStmt(
    const Stmt *stmt,
    const Objc3ProfileSymbolWalker &walker) {
  if (stmt->switch_stmt == nullptr) {
    return;
  }
  WalkObjc3ProfileSymbolsInExpr(stmt->switch_stmt->condition.get(), walker);
  for (const auto &switch_case : stmt->switch_stmt->cases) {
    WalkObjc3ProfileSymbolsInStmtBody(switch_case.body, walker);
  }
}

}  // namespace
