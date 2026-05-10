std::size_t CountFailableCallSitesInLetStmt(const Stmt *stmt) {
  return stmt->let_stmt == nullptr
             ? 0u
             : CountFailableCallSitesInExpr(stmt->let_stmt->value.get());
}

std::size_t CountFailableCallSitesInAssignStmt(const Stmt *stmt) {
  return stmt->assign_stmt == nullptr
             ? 0u
             : CountFailableCallSitesInExpr(stmt->assign_stmt->value.get());
}

std::size_t CountFailableCallSitesInReturnStmt(const Stmt *stmt) {
  return stmt->return_stmt == nullptr
             ? 0u
             : CountFailableCallSitesInExpr(stmt->return_stmt->value.get());
}

std::size_t CountFailableCallSitesInExprStmt(const Stmt *stmt) {
  return stmt->expr_stmt == nullptr
             ? 0u
             : CountFailableCallSitesInExpr(stmt->expr_stmt->value.get());
}
