bool IsAwaitKeywordSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered == "await" || lowered.find("await_") != std::string::npos;
}

bool IsAwaitSuspensionPointSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("suspend") != std::string::npos ||
         lowered.find("yield") != std::string::npos;
}

bool IsAwaitResumeSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("resume") != std::string::npos ||
         lowered.find("wakeup") != std::string::npos;
}

bool IsAwaitStateMachineSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("state") != std::string::npos ||
         lowered.find("continuation") != std::string::npos ||
         lowered.find("poll") != std::string::npos;
}

bool IsAwaitContinuationSymbol(const std::string &symbol) {
  if (symbol.empty()) {
    return false;
  }
  const std::string lowered = BuildLowercaseProfileToken(symbol);
  return lowered.find("continuation") != std::string::npos ||
         lowered.find("future") != std::string::npos ||
         lowered.find("promise") != std::string::npos;
}

struct Objc3AwaitSuspensionSiteCounts {
  std::size_t await_keyword_sites = 0;
  std::size_t await_suspension_point_sites = 0;
  std::size_t await_resume_sites = 0;
  std::size_t await_state_machine_sites = 0;
  std::size_t await_continuation_sites = 0;
};

void CollectAwaitSuspensionSitesFromSymbol(
    const std::string &symbol,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (IsAwaitKeywordSymbol(symbol)) {
    counts.await_keyword_sites += 1u;
  }
  if (IsAwaitSuspensionPointSymbol(symbol)) {
    counts.await_suspension_point_sites += 1u;
  }
  if (IsAwaitResumeSymbol(symbol)) {
    counts.await_resume_sites += 1u;
  }
  if (IsAwaitStateMachineSymbol(symbol)) {
    counts.await_state_machine_sites += 1u;
  }
  if (IsAwaitContinuationSymbol(symbol)) {
    counts.await_continuation_sites += 1u;
  }
}

void CollectAwaitSuspensionExprSites(
    const Expr *expr,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (expr == nullptr) {
    return;
  }
  if (expr->await_expression_enabled) {
    counts.await_keyword_sites += 1u;
    counts.await_suspension_point_sites += 1u;
    counts.await_continuation_sites += 1u;
  }
  switch (expr->kind) {
  case Expr::Kind::Call:
    CollectAwaitSuspensionSitesFromSymbol(expr->ident, counts);
    for (const auto &arg : expr->args) {
      CollectAwaitSuspensionExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::MessageSend:
    CollectAwaitSuspensionSitesFromSymbol(expr->selector, counts);
    CollectAwaitSuspensionExprSites(expr->receiver.get(), counts);
    for (const auto &arg : expr->args) {
      CollectAwaitSuspensionExprSites(arg.get(), counts);
    }
    return;
  case Expr::Kind::Binary:
    CollectAwaitSuspensionExprSites(expr->left.get(), counts);
    CollectAwaitSuspensionExprSites(expr->right.get(), counts);
    return;
  case Expr::Kind::Conditional:
    CollectAwaitSuspensionExprSites(expr->left.get(), counts);
    CollectAwaitSuspensionExprSites(expr->right.get(), counts);
    CollectAwaitSuspensionExprSites(expr->third.get(), counts);
    return;
  case Expr::Kind::BlockLiteral:
  case Expr::Kind::BoolLiteral:
  case Expr::Kind::Identifier:
  case Expr::Kind::NilLiteral:
  case Expr::Kind::Number:
  default:
    return;
  }
}

void CollectAwaitSuspensionForClauseSites(
    const ForClause &clause,
    Objc3AwaitSuspensionSiteCounts &counts) {
  CollectAwaitSuspensionExprSites(clause.value.get(), counts);
}

void CollectAwaitSuspensionStmtSites(
    const Stmt *stmt,
    Objc3AwaitSuspensionSiteCounts &counts) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectAwaitSuspensionExprSites(stmt->let_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectAwaitSuspensionExprSites(stmt->assign_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectAwaitSuspensionExprSites(stmt->return_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    CollectAwaitSuspensionExprSites(stmt->if_stmt->condition.get(), counts);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      CollectAwaitSuspensionStmtSites(then_stmt.get(), counts);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      CollectAwaitSuspensionStmtSites(else_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      CollectAwaitSuspensionStmtSites(body_stmt.get(), counts);
    }
    CollectAwaitSuspensionExprSites(stmt->do_while_stmt->condition.get(),
                                    counts);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    CollectAwaitSuspensionForClauseSites(stmt->for_stmt->init, counts);
    CollectAwaitSuspensionExprSites(stmt->for_stmt->condition.get(), counts);
    CollectAwaitSuspensionForClauseSites(stmt->for_stmt->step, counts);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      CollectAwaitSuspensionStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    CollectAwaitSuspensionExprSites(stmt->switch_stmt->condition.get(),
                                    counts);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        CollectAwaitSuspensionStmtSites(case_stmt.get(), counts);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    CollectAwaitSuspensionExprSites(stmt->while_stmt->condition.get(), counts);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      CollectAwaitSuspensionStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      CollectAwaitSuspensionStmtSites(body_stmt.get(), counts);
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectAwaitSuspensionExprSites(stmt->expr_stmt->value.get(), counts);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

Objc3AwaitSuspensionSiteCounts CountAwaitSuspensionSitesInBody(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3AwaitSuspensionSiteCounts counts;
  for (const auto &stmt : body) {
    CollectAwaitSuspensionStmtSites(stmt.get(), counts);
  }
  return counts;
}
