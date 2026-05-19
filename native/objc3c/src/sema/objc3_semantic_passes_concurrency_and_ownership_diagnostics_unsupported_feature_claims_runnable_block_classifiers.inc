static bool ExprContainsRunnableBlockLiteral(const Expr *expr) {
  if (expr == nullptr) {
    return false;
  }
  if (expr->kind == Expr::Kind::BlockLiteral) {
    return BlockLiteralUsesRunnableC002Subset(*expr);
  }
  if (ExprContainsRunnableBlockLiteral(expr->left.get()) ||
      ExprContainsRunnableBlockLiteral(expr->right.get()) ||
      ExprContainsRunnableBlockLiteral(expr->third.get()) ||
      ExprContainsRunnableBlockLiteral(expr->receiver.get())) {
    return true;
  }
  for (const auto &arg : expr->args) {
    if (ExprContainsRunnableBlockLiteral(arg.get())) {
      return true;
    }
  }
  return false;
}

static bool StmtContainsRunnableBlockLiteral(const Stmt *stmt) {
  if (stmt == nullptr) {
    return false;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    return stmt->let_stmt != nullptr &&
           ExprContainsRunnableBlockLiteral(stmt->let_stmt->value.get());
  case Stmt::Kind::Assign:
    return stmt->assign_stmt != nullptr &&
           ExprContainsRunnableBlockLiteral(stmt->assign_stmt->value.get());
  case Stmt::Kind::Return:
    return stmt->return_stmt != nullptr &&
           ExprContainsRunnableBlockLiteral(stmt->return_stmt->value.get());
  case Stmt::Kind::Expr:
    return stmt->expr_stmt != nullptr &&
           ExprContainsRunnableBlockLiteral(stmt->expr_stmt->value.get());
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return false;
    }
    if (ExprContainsRunnableBlockLiteral(stmt->if_stmt->condition.get())) {
      return true;
    }
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      if (StmtContainsRunnableBlockLiteral(then_stmt.get())) {
        return true;
      }
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      if (StmtContainsRunnableBlockLiteral(else_stmt.get())) {
        return true;
      }
    }
    return false;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return false;
    }
    if (ExprContainsRunnableBlockLiteral(
            stmt->do_while_stmt->condition.get())) {
      return true;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      if (StmtContainsRunnableBlockLiteral(body_stmt.get())) {
        return true;
      }
    }
    return false;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return false;
    }
    if (ExprContainsRunnableBlockLiteral(stmt->for_stmt->condition.get()) ||
        ExprContainsRunnableBlockLiteral(stmt->for_stmt->init.value.get()) ||
        ExprContainsRunnableBlockLiteral(stmt->for_stmt->step.value.get())) {
      return true;
    }
    for (const auto &body_stmt : stmt->for_stmt->body) {
      if (StmtContainsRunnableBlockLiteral(body_stmt.get())) {
        return true;
      }
    }
    return false;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return false;
    }
    if (ExprContainsRunnableBlockLiteral(stmt->switch_stmt->condition.get())) {
      return true;
    }
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &body_stmt : switch_case.body) {
        if (StmtContainsRunnableBlockLiteral(body_stmt.get())) {
          return true;
        }
      }
    }
    return false;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return false;
    }
    if (ExprContainsRunnableBlockLiteral(stmt->while_stmt->condition.get())) {
      return true;
    }
    for (const auto &body_stmt : stmt->while_stmt->body) {
      if (StmtContainsRunnableBlockLiteral(body_stmt.get())) {
        return true;
      }
    }
    return false;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return false;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      if (StmtContainsRunnableBlockLiteral(body_stmt.get())) {
        return true;
      }
    }
    return false;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return false;
  }
  return false;
}

static bool FunctionBodyContainsRunnableBlockLiteral(const FunctionDecl &fn) {
  for (const auto &stmt : fn.body) {
    if (StmtContainsRunnableBlockLiteral(stmt.get())) {
      return true;
    }
  }
  return false;
}

static bool MethodBodyContainsRunnableBlockLiteral(
    const Objc3MethodDecl &method) {
  for (const auto &stmt : method.body) {
    if (StmtContainsRunnableBlockLiteral(stmt.get())) {
      return true;
    }
  }
  return false;
}
