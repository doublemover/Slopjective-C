namespace {

void WalkObjc3ProfileSymbolsInLetStmt(
    const Stmt *stmt,
    const Objc3ProfileSymbolWalker &walker) {
  if (stmt->let_stmt != nullptr) {
    if (stmt->let_stmt->cleanup_attribute_declared) {
      walker.Visit(stmt->let_stmt->cleanup_function_symbol);
    }
    if (stmt->let_stmt->resource_attribute_declared) {
      walker.Visit(stmt->let_stmt->resource_close_symbol);
    }
    WalkObjc3ProfileSymbolsInExpr(stmt->let_stmt->value.get(), walker);
  }
}

void WalkObjc3ProfileSymbolsInAssignStmt(
    const Stmt *stmt,
    const Objc3ProfileSymbolWalker &walker) {
  if (stmt->assign_stmt != nullptr) {
    WalkObjc3ProfileSymbolsInExpr(stmt->assign_stmt->value.get(), walker);
  }
}

void WalkObjc3ProfileSymbolsInReturnStmt(
    const Stmt *stmt,
    const Objc3ProfileSymbolWalker &walker) {
  if (stmt->return_stmt != nullptr) {
    WalkObjc3ProfileSymbolsInExpr(stmt->return_stmt->value.get(), walker);
  }
}

void WalkObjc3ProfileSymbolsInExprStmt(
    const Stmt *stmt,
    const Objc3ProfileSymbolWalker &walker) {
  if (stmt->expr_stmt != nullptr) {
    WalkObjc3ProfileSymbolsInExpr(stmt->expr_stmt->value.get(), walker);
  }
}

}  // namespace
