bool IsFailableCallSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("error") != std::string::npos ||
         lowered.find("fail") != std::string::npos ||
         lowered.find("try") != std::string::npos;
}

std::size_t CountFailableCallSitesInExpr(const Expr *expr) {
  if (expr == nullptr) {
    return 0;
  }
  switch (expr->kind) {
  case Expr::Kind::Call: {
    std::size_t count = IsFailableCallSymbol(expr->ident) ? 1u : 0u;
    for (const auto &arg : expr->args) {
      count += CountFailableCallSitesInExpr(arg.get());
    }
    return count;
  }
  case Expr::Kind::MessageSend: {
    std::size_t count = IsFailableCallSymbol(expr->selector) ? 1u : 0u;
    count += CountFailableCallSitesInExpr(expr->receiver.get());
    for (const auto &arg : expr->args) {
      count += CountFailableCallSitesInExpr(arg.get());
    }
    return count;
  }
  case Expr::Kind::Binary:
    return CountFailableCallSitesInExpr(expr->left.get()) +
           CountFailableCallSitesInExpr(expr->right.get());
  case Expr::Kind::Conditional:
    return CountFailableCallSitesInExpr(expr->left.get()) +
           CountFailableCallSitesInExpr(expr->right.get()) +
           CountFailableCallSitesInExpr(expr->third.get());
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::Number:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::NilLiteral:
  default:
    return 0;
  }
}

std::size_t CountFailableCallSitesInForClause(const ForClause &clause) {
  return CountFailableCallSitesInExpr(clause.value.get());
}

std::size_t CountFailableCallSitesInStmt(const Stmt *stmt) {
  if (stmt == nullptr) {
    return 0;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    return stmt->let_stmt == nullptr
               ? 0u
               : CountFailableCallSitesInExpr(stmt->let_stmt->value.get());
  case Stmt::Kind::Assign:
    return stmt->assign_stmt == nullptr
               ? 0u
               : CountFailableCallSitesInExpr(stmt->assign_stmt->value.get());
  case Stmt::Kind::Return:
    return stmt->return_stmt == nullptr
               ? 0u
               : CountFailableCallSitesInExpr(stmt->return_stmt->value.get());
  case Stmt::Kind::If: {
    if (stmt->if_stmt == nullptr) {
      return 0;
    }
    std::size_t count =
        CountFailableCallSitesInExpr(stmt->if_stmt->condition.get());
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      count += CountFailableCallSitesInStmt(then_stmt.get());
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      count += CountFailableCallSitesInStmt(else_stmt.get());
    }
    return count;
  }
  case Stmt::Kind::DoWhile: {
    if (stmt->do_while_stmt == nullptr) {
      return 0;
    }
    std::size_t count =
        CountFailableCallSitesInExpr(stmt->do_while_stmt->condition.get());
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      count += CountFailableCallSitesInStmt(body_stmt.get());
    }
    return count;
  }
  case Stmt::Kind::For: {
    if (stmt->for_stmt == nullptr) {
      return 0;
    }
    std::size_t count =
        CountFailableCallSitesInForClause(stmt->for_stmt->init);
    count += CountFailableCallSitesInExpr(stmt->for_stmt->condition.get());
    count += CountFailableCallSitesInForClause(stmt->for_stmt->step);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      count += CountFailableCallSitesInStmt(body_stmt.get());
    }
    return count;
  }
  case Stmt::Kind::Switch: {
    if (stmt->switch_stmt == nullptr) {
      return 0;
    }
    std::size_t count =
        CountFailableCallSitesInExpr(stmt->switch_stmt->condition.get());
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        count += CountFailableCallSitesInStmt(case_stmt.get());
      }
    }
    return count;
  }
  case Stmt::Kind::While: {
    if (stmt->while_stmt == nullptr) {
      return 0;
    }
    std::size_t count =
        CountFailableCallSitesInExpr(stmt->while_stmt->condition.get());
    for (const auto &body_stmt : stmt->while_stmt->body) {
      count += CountFailableCallSitesInStmt(body_stmt.get());
    }
    return count;
  }
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer: {
    if (stmt->block_stmt == nullptr) {
      return 0;
    }
    std::size_t count = 0;
    for (const auto &body_stmt : stmt->block_stmt->body) {
      count += CountFailableCallSitesInStmt(body_stmt.get());
    }
    return count;
  }
  case Stmt::Kind::Expr:
    return stmt->expr_stmt == nullptr
               ? 0u
               : CountFailableCallSitesInExpr(stmt->expr_stmt->value.get());
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return 0;
  }
}

std::size_t CountFailableCallSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  std::size_t count = 0;
  for (const auto &stmt : body) {
    count += CountFailableCallSitesInStmt(stmt.get());
  }
  return count;
}
