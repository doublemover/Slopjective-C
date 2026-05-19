void CollectPointerArithmeticForClauseSites(const ForClause &clause,
                                            std::size_t &sites) {
  if (clause.kind == ForClause::Kind::Assign &&
      IsPointerArithmeticMutationOperator(clause.op)) {
    sites += 1u;
  }
  CollectPointerArithmeticExprSites(clause.value.get(), sites);
}

void CollectPointerArithmeticDoWhileStmtSites(const Stmt *stmt,
                                             std::size_t &sites) {
  if (stmt->do_while_stmt == nullptr) {
    return;
  }

  CollectPointerArithmeticStmtBodySites(stmt->do_while_stmt->body, sites);
  CollectPointerArithmeticExprSites(stmt->do_while_stmt->condition.get(),
                                    sites);
}

void CollectPointerArithmeticForStmtSites(const Stmt *stmt,
                                          std::size_t &sites) {
  if (stmt->for_stmt == nullptr) {
    return;
  }

  CollectPointerArithmeticForClauseSites(stmt->for_stmt->init, sites);
  CollectPointerArithmeticExprSites(stmt->for_stmt->condition.get(), sites);
  CollectPointerArithmeticForClauseSites(stmt->for_stmt->step, sites);
  CollectPointerArithmeticStmtBodySites(stmt->for_stmt->body, sites);
}

void CollectPointerArithmeticWhileStmtSites(const Stmt *stmt,
                                           std::size_t &sites) {
  if (stmt->while_stmt == nullptr) {
    return;
  }

  CollectPointerArithmeticExprSites(stmt->while_stmt->condition.get(), sites);
  CollectPointerArithmeticStmtBodySites(stmt->while_stmt->body, sites);
}
