void CollectPointerArithmeticForClauseSites(const ForClause &clause,
                                            std::size_t &sites) {
  if (clause.kind == ForClause::Kind::Assign &&
      IsPointerArithmeticMutationOperator(clause.op)) {
    sites += 1u;
  }
  CollectPointerArithmeticExprSites(clause.value.get(), sites);
}

void CollectPointerArithmeticStmtSites(const Stmt *stmt, std::size_t &sites) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectPointerArithmeticExprSites(stmt->let_stmt->value.get(), sites);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      if (IsPointerArithmeticMutationOperator(stmt->assign_stmt->op)) {
        sites += 1u;
      }
      CollectPointerArithmeticExprSites(stmt->assign_stmt->value.get(), sites);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectPointerArithmeticExprSites(stmt->return_stmt->value.get(), sites);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectPointerArithmeticExprSites(stmt->if_stmt->condition.get(), sites);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectPointerArithmeticStmtSites(then_stmt.get(), sites);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectPointerArithmeticStmtSites(else_stmt.get(), sites);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectPointerArithmeticStmtSites(body_stmt.get(), sites);
    }
    CollectPointerArithmeticExprSites(stmt->do_while_stmt->condition.get(),
                                      sites);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectPointerArithmeticForClauseSites(stmt->for_stmt->init, sites);
    CollectPointerArithmeticExprSites(stmt->for_stmt->condition.get(), sites);
    CollectPointerArithmeticForClauseSites(stmt->for_stmt->step, sites);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectPointerArithmeticStmtSites(body_stmt.get(), sites);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectPointerArithmeticExprSites(stmt->switch_stmt->condition.get(),
                                      sites);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectPointerArithmeticStmtSites(case_stmt.get(), sites);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectPointerArithmeticExprSites(stmt->while_stmt->condition.get(),
                                      sites);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectPointerArithmeticStmtSites(body_stmt.get(), sites);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectPointerArithmeticStmtSites(body_stmt.get(), sites);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectPointerArithmeticExprSites(stmt->expr_stmt->value.get(), sites);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}
