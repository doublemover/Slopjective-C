static void CollectConcurrencyTaskCallableLegalityExprSites(
    const Expr *expr, Objc3ConcurrencyTaskCallableLegalityProfile &profile) {
  if (expr == nullptr) {
    return;
  }
  if (expr->kind == Expr::Kind::Call) {
    if (objc3c::support::IsConcurrencyTaskCreationSymbol(expr->ident)) {
      ++profile.task_creation_sites;
    }
    if (objc3c::support::IsConcurrencyDetachedTaskCreationSymbol(expr->ident)) {
      ++profile.detached_task_creation_sites;
    }
    if (objc3c::support::IsConcurrencyTaskGroupScopeSymbol(expr->ident)) {
      ++profile.task_group_scope_sites;
    }
    if (objc3c::support::IsConcurrencyTaskGroupAddTaskSymbol(expr->ident)) {
      ++profile.task_group_add_task_sites;
    }
    if (objc3c::support::IsConcurrencyTaskGroupWaitNextSymbol(expr->ident)) {
      ++profile.task_group_wait_next_sites;
    }
    if (objc3c::support::IsConcurrencyTaskGroupCancelAllSymbol(expr->ident)) {
      ++profile.task_group_cancel_all_sites;
    }
    if (objc3c::support::IsConcurrencyCancellationCheckSymbol(expr->ident)) {
      ++profile.cancellation_check_sites;
    }
    if (objc3c::support::IsConcurrencyCancellationHandlerSymbol(expr->ident)) {
      ++profile.cancellation_handler_sites;
    }
  }
  CollectConcurrencyTaskCallableLegalityExprSites(expr->receiver.get(), profile);
  CollectConcurrencyTaskCallableLegalityExprSites(expr->left.get(), profile);
  CollectConcurrencyTaskCallableLegalityExprSites(expr->right.get(), profile);
  CollectConcurrencyTaskCallableLegalityExprSites(expr->third.get(), profile);
  for (const auto &key : expr->collection_keys) {
    CollectConcurrencyTaskCallableLegalityExprSites(key.get(), profile);
  }
  for (const auto &value : expr->collection_values) {
    CollectConcurrencyTaskCallableLegalityExprSites(value.get(), profile);
  }
  for (const auto &arg : expr->args) {
    CollectConcurrencyTaskCallableLegalityExprSites(arg.get(), profile);
  }
}

static void CollectConcurrencyTaskCallableLegalityStmtSites(
    const Stmt *stmt, Objc3ConcurrencyTaskCallableLegalityProfile &profile) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      CollectConcurrencyTaskCallableLegalityExprSites(stmt->let_stmt->value.get(),
                                                profile);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      CollectConcurrencyTaskCallableLegalityExprSites(
          stmt->assign_stmt->value.get(), profile);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      CollectConcurrencyTaskCallableLegalityExprSites(stmt->return_stmt->value.get(),
                                                profile);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectConcurrencyTaskCallableLegalityExprSites(
          stmt->if_stmt->condition.get(), profile);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        CollectConcurrencyTaskCallableLegalityStmtSites(then_stmt.get(), profile);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        CollectConcurrencyTaskCallableLegalityStmtSites(else_stmt.get(), profile);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        CollectConcurrencyTaskCallableLegalityStmtSites(body_stmt.get(), profile);
      }
      CollectConcurrencyTaskCallableLegalityExprSites(
          stmt->do_while_stmt->condition.get(), profile);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectConcurrencyTaskCallableLegalityExprSites(stmt->for_stmt->init.value.get(),
                                                profile);
      CollectConcurrencyTaskCallableLegalityExprSites(
          stmt->for_stmt->condition.get(), profile);
      CollectConcurrencyTaskCallableLegalityExprSites(stmt->for_stmt->step.value.get(),
                                                profile);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        CollectConcurrencyTaskCallableLegalityStmtSites(body_stmt.get(), profile);
      }
    }
    return;
  case Stmt::Kind::ForIn:
    if (stmt->for_in_stmt != nullptr) {
      CollectConcurrencyTaskCallableLegalityExprSites(stmt->for_in_stmt->collection.get(), profile);
      for (const auto &body_stmt : stmt->for_in_stmt->body) {
        CollectConcurrencyTaskCallableLegalityStmtSites(body_stmt.get(), profile);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectConcurrencyTaskCallableLegalityExprSites(
          stmt->switch_stmt->condition.get(), profile);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          CollectConcurrencyTaskCallableLegalityStmtSites(case_stmt.get(), profile);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectConcurrencyTaskCallableLegalityExprSites(
          stmt->while_stmt->condition.get(), profile);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        CollectConcurrencyTaskCallableLegalityStmtSites(body_stmt.get(), profile);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        CollectConcurrencyTaskCallableLegalityStmtSites(body_stmt.get(), profile);
      }
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      CollectConcurrencyTaskCallableLegalityExprSites(stmt->expr_stmt->value.get(),
                                                profile);
    }
    return;
  case Stmt::Kind::CollectionMutation:
    if (stmt->collection_mutation_stmt != nullptr) {
      CollectConcurrencyTaskCallableLegalityExprSites(stmt->collection_mutation_stmt->key_or_index.get(), profile);
      CollectConcurrencyTaskCallableLegalityExprSites(stmt->collection_mutation_stmt->value.get(), profile);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}
