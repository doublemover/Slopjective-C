static void CollectOwnershipBorrowedEscapeExprSites(
    const Expr *expr, const std::unordered_map<std::string, OwnershipBorrowedCallableContract> &callable_contracts,
    OwnershipBorrowedEscapeSiteProfile &profile) {
  if (expr == nullptr) {
    return;
  }
  if (expr->kind == Expr::Kind::Call) {
    auto contract_it = callable_contracts.find(expr->ident);
    if (contract_it != callable_contracts.end() &&
        contract_it->second.return_borrowed) {
      ++profile.borrowed_return_callable_sites;
    }
  }
  CollectOwnershipBorrowedEscapeExprSites(expr->receiver.get(), callable_contracts,
                                      profile);
  CollectOwnershipBorrowedEscapeExprSites(expr->left.get(), callable_contracts,
                                      profile);
  CollectOwnershipBorrowedEscapeExprSites(expr->right.get(), callable_contracts,
                                      profile);
  CollectOwnershipBorrowedEscapeExprSites(expr->third.get(), callable_contracts,
                                      profile);
  for (const auto &arg : expr->args) {
    CollectOwnershipBorrowedEscapeExprSites(arg.get(), callable_contracts, profile);
  }
}

static void CollectOwnershipBorrowedEscapeStmtSites(
    const Stmt *stmt,
    const std::unordered_map<std::string, OwnershipBorrowedCallableContract>
        &callable_contracts,
    OwnershipBorrowedEscapeSiteProfile &profile) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
    case Stmt::Kind::Let:
      if (stmt->let_stmt != nullptr) {
        CollectOwnershipBorrowedEscapeExprSites(stmt->let_stmt->value.get(),
                                            callable_contracts, profile);
      }
      return;
    case Stmt::Kind::Assign:
      if (stmt->assign_stmt != nullptr) {
        CollectOwnershipBorrowedEscapeExprSites(stmt->assign_stmt->value.get(),
                                            callable_contracts, profile);
      }
      return;
    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        CollectOwnershipBorrowedEscapeExprSites(stmt->return_stmt->value.get(),
                                            callable_contracts, profile);
      }
      return;
    case Stmt::Kind::If:
      if (stmt->if_stmt != nullptr) {
        CollectOwnershipBorrowedEscapeExprSites(stmt->if_stmt->condition.get(),
                                            callable_contracts, profile);
        for (const auto &body_stmt : stmt->if_stmt->then_body) {
          CollectOwnershipBorrowedEscapeStmtSites(body_stmt.get(), callable_contracts,
                                              profile);
        }
        for (const auto &body_stmt : stmt->if_stmt->else_body) {
          CollectOwnershipBorrowedEscapeStmtSites(body_stmt.get(), callable_contracts,
                                              profile);
        }
      }
      return;
    case Stmt::Kind::DoWhile:
      if (stmt->do_while_stmt != nullptr) {
        for (const auto &body_stmt : stmt->do_while_stmt->body) {
          CollectOwnershipBorrowedEscapeStmtSites(body_stmt.get(),
                                              callable_contracts, profile);
        }
        CollectOwnershipBorrowedEscapeExprSites(stmt->do_while_stmt->condition.get(),
                                            callable_contracts, profile);
      }
      return;
    case Stmt::Kind::For:
      if (stmt->for_stmt != nullptr) {
        CollectOwnershipBorrowedEscapeExprSites(stmt->for_stmt->init.value.get(),
                                            callable_contracts, profile);
        CollectOwnershipBorrowedEscapeExprSites(stmt->for_stmt->condition.get(),
                                            callable_contracts, profile);
        CollectOwnershipBorrowedEscapeExprSites(stmt->for_stmt->step.value.get(),
                                            callable_contracts, profile);
        for (const auto &body_stmt : stmt->for_stmt->body) {
          CollectOwnershipBorrowedEscapeStmtSites(body_stmt.get(),
                                              callable_contracts, profile);
        }
      }
      return;
    case Stmt::Kind::Switch:
      if (stmt->switch_stmt != nullptr) {
        CollectOwnershipBorrowedEscapeExprSites(stmt->switch_stmt->condition.get(),
                                            callable_contracts, profile);
        for (const auto &case_stmt : stmt->switch_stmt->cases) {
          for (const auto &body_stmt : case_stmt.body) {
            CollectOwnershipBorrowedEscapeStmtSites(body_stmt.get(),
                                                callable_contracts, profile);
          }
        }
      }
      return;
    case Stmt::Kind::While:
      if (stmt->while_stmt != nullptr) {
        CollectOwnershipBorrowedEscapeExprSites(stmt->while_stmt->condition.get(),
                                            callable_contracts, profile);
        for (const auto &body_stmt : stmt->while_stmt->body) {
          CollectOwnershipBorrowedEscapeStmtSites(body_stmt.get(),
                                              callable_contracts, profile);
        }
      }
      return;
    case Stmt::Kind::Block:
    case Stmt::Kind::Defer:
      if (stmt->block_stmt != nullptr) {
        for (const auto &body_stmt : stmt->block_stmt->body) {
          CollectOwnershipBorrowedEscapeStmtSites(body_stmt.get(),
                                              callable_contracts, profile);
        }
      }
      return;
    case Stmt::Kind::Expr:
      if (stmt->expr_stmt != nullptr) {
        CollectOwnershipBorrowedEscapeExprSites(stmt->expr_stmt->value.get(),
                                            callable_contracts, profile);
      }
      return;
    case Stmt::Kind::Break:
    case Stmt::Kind::Continue:
    case Stmt::Kind::Empty:
      return;
  }
}
