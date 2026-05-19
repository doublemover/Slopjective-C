void CollectPointerArithmeticLetStmtSites(const Stmt *stmt, std::size_t &sites) {
  if (stmt->let_stmt != nullptr) {
    CollectPointerArithmeticExprSites(stmt->let_stmt->value.get(), sites);
  }
}

void CollectPointerArithmeticAssignStmtSites(const Stmt *stmt,
                                            std::size_t &sites) {
  if (stmt->assign_stmt != nullptr) {
    if (IsPointerArithmeticMutationOperator(stmt->assign_stmt->op)) {
      sites += 1u;
    }
    CollectPointerArithmeticExprSites(stmt->assign_stmt->value.get(), sites);
  }
}

void CollectPointerArithmeticReturnStmtSites(const Stmt *stmt,
                                            std::size_t &sites) {
  if (stmt->return_stmt != nullptr) {
    CollectPointerArithmeticExprSites(stmt->return_stmt->value.get(), sites);
  }
}

void CollectPointerArithmeticExprStmtSites(const Stmt *stmt,
                                          std::size_t &sites) {
  if (stmt->expr_stmt != nullptr) {
    CollectPointerArithmeticExprSites(stmt->expr_stmt->value.get(), sites);
  }
}
