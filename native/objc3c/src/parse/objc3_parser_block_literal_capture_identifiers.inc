void CollectBlockLiteralExprIdentifiers(
    const Expr *expr,
    std::vector<std::string> &used_identifiers) {
  if (expr == nullptr) {
    return;
  }
  switch (expr->kind) {
  case Expr::Kind::Identifier:
    if (!expr->ident.empty()) {
      used_identifiers.push_back(expr->ident);
    }
    return;
  case Expr::Kind::Binary:
    CollectBlockLiteralExprIdentifiers(expr->left.get(), used_identifiers);
    CollectBlockLiteralExprIdentifiers(expr->right.get(), used_identifiers);
    return;
  case Expr::Kind::Conditional:
    CollectBlockLiteralExprIdentifiers(expr->left.get(), used_identifiers);
    CollectBlockLiteralExprIdentifiers(expr->right.get(), used_identifiers);
    CollectBlockLiteralExprIdentifiers(expr->third.get(), used_identifiers);
    return;
  case Expr::Kind::Call:
    for (const auto &arg : expr->args) {
      CollectBlockLiteralExprIdentifiers(arg.get(), used_identifiers);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectBlockLiteralExprIdentifiers(expr->receiver.get(), used_identifiers);
    for (const auto &arg : expr->args) {
      CollectBlockLiteralExprIdentifiers(arg.get(), used_identifiers);
    }
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::Number:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::NilLiteral:
  default:
    return;
  }
}

void CollectBlockLiteralForClauseIdentifiers(
    const ForClause &clause,
    std::vector<std::string> &used_identifiers,
    std::vector<std::string> &declared_identifiers) {
  if (clause.kind == ForClause::Kind::Let && !clause.name.empty()) {
    declared_identifiers.push_back(clause.name);
  } else if ((clause.kind == ForClause::Kind::Assign ||
              clause.kind == ForClause::Kind::Expr) &&
             !clause.name.empty()) {
    used_identifiers.push_back(clause.name);
  }
  CollectBlockLiteralExprIdentifiers(clause.value.get(), used_identifiers);
}

void CollectBlockLiteralStmtIdentifiers(
    const Stmt *stmt,
    std::vector<std::string> &used_identifiers,
    std::vector<std::string> &declared_identifiers) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      if (!stmt->let_stmt->name.empty()) {
        declared_identifiers.push_back(stmt->let_stmt->name);
      }
      CollectBlockLiteralExprIdentifiers(stmt->let_stmt->value.get(), used_identifiers);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      if (!stmt->assign_stmt->name.empty()) {
        used_identifiers.push_back(stmt->assign_stmt->name);
      }
      CollectBlockLiteralExprIdentifiers(stmt->assign_stmt->value.get(), used_identifiers);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectBlockLiteralExprIdentifiers(stmt->return_stmt->value.get(), used_identifiers);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectBlockLiteralExprIdentifiers(stmt->expr_stmt->value.get(), used_identifiers);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectBlockLiteralExprIdentifiers(stmt->if_stmt->condition.get(), used_identifiers);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        CollectBlockLiteralStmtIdentifiers(
            then_stmt.get(), used_identifiers, declared_identifiers);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectBlockLiteralStmtIdentifiers(
            else_stmt.get(), used_identifiers, declared_identifiers);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectBlockLiteralStmtIdentifiers(
            body_stmt.get(), used_identifiers, declared_identifiers);
      }
      CollectBlockLiteralExprIdentifiers(
          stmt->do_while_stmt->condition.get(), used_identifiers);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectBlockLiteralForClauseIdentifiers(
          stmt->for_stmt->init, used_identifiers, declared_identifiers);
      CollectBlockLiteralExprIdentifiers(
          stmt->for_stmt->condition.get(), used_identifiers);
      CollectBlockLiteralForClauseIdentifiers(
          stmt->for_stmt->step, used_identifiers, declared_identifiers);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectBlockLiteralStmtIdentifiers(
            body_stmt.get(), used_identifiers, declared_identifiers);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectBlockLiteralExprIdentifiers(
          stmt->switch_stmt->condition.get(), used_identifiers);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          CollectBlockLiteralStmtIdentifiers(
              case_stmt.get(), used_identifiers, declared_identifiers);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectBlockLiteralExprIdentifiers(
          stmt->while_stmt->condition.get(), used_identifiers);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectBlockLiteralStmtIdentifiers(
            body_stmt.get(), used_identifiers, declared_identifiers);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectBlockLiteralStmtIdentifiers(
            body_stmt.get(), used_identifiers, declared_identifiers);
      }
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}
