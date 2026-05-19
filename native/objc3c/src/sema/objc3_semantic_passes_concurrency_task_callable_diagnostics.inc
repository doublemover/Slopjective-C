static void DiagnoseConcurrencyTaskCallableLegalityExpr(
    const Expr *expr, const Objc3ConcurrencyTaskCallableLegalityProfile &profile,
    bool inside_async_context, std::vector<std::string> &diagnostics) {
  if (expr == nullptr) {
    return;
  }
  if (expr->kind == Expr::Kind::Call) {
    const bool task_creation =
        objc3c::support::IsConcurrencyTaskCreationSymbol(expr->ident);
    const bool detached_task_creation =
        objc3c::support::IsConcurrencyDetachedTaskCreationSymbol(expr->ident);
    const bool task_group_scope =
        objc3c::support::IsConcurrencyTaskGroupScopeSymbol(expr->ident);
    const bool task_group_add =
        objc3c::support::IsConcurrencyTaskGroupAddTaskSymbol(expr->ident);
    const bool task_group_wait =
        objc3c::support::IsConcurrencyTaskGroupWaitNextSymbol(expr->ident);
    const bool task_group_cancel =
        objc3c::support::IsConcurrencyTaskGroupCancelAllSymbol(expr->ident);
    const bool cancellation_check =
        objc3c::support::IsConcurrencyCancellationCheckSymbol(expr->ident);
    const bool cancellation_handler =
        objc3c::support::IsConcurrencyCancellationHandlerSymbol(expr->ident);
    const bool task_related = task_creation || task_group_scope || task_group_add ||
                              task_group_wait || task_group_cancel ||
                              cancellation_check || cancellation_handler;

    if (task_related && !inside_async_context) {
      diagnostics.push_back(MakeDiag(
          expr->line, expr->column, "O3S227",
          "task semantics failed: task runtime, task-group, and cancellation calls require an async function or method"));
    } else if (task_group_add || task_group_wait || task_group_cancel) {
      if (profile.task_group_scope_sites == 0u) {
        diagnostics.push_back(MakeDiag(
            expr->line, expr->column, "O3S228",
            "task semantics failed: task-group add, wait-next, and cancel-all calls require a task-group scope in the same async function or method"));
      }
      if (task_group_cancel && profile.cancellation_check_sites == 0u) {
        diagnostics.push_back(MakeDiag(
            expr->line, expr->column, "O3S230",
            "task semantics failed: cancellation handlers and cancel-all calls require a cancellation check in the same async function or method"));
      }
    } else if (detached_task_creation) {
      if (profile.task_group_scope_sites > 0u ||
          profile.task_group_add_task_sites > 0u ||
          profile.task_group_wait_next_sites > 0u ||
          profile.task_group_cancel_all_sites > 0u) {
        diagnostics.push_back(MakeDiag(
            expr->line, expr->column, "O3S229",
            "task semantics failed: detached task creation cannot share a structured task-group callable"));
      }
    } else if (cancellation_handler && profile.cancellation_check_sites == 0u) {
      diagnostics.push_back(MakeDiag(
          expr->line, expr->column, "O3S230",
          "task semantics failed: cancellation handlers and cancel-all calls require a cancellation check in the same async function or method"));
    }
  }

  DiagnoseConcurrencyTaskCallableLegalityExpr(expr->receiver.get(), profile,
                                        inside_async_context, diagnostics);
  DiagnoseConcurrencyTaskCallableLegalityExpr(expr->left.get(), profile,
                                        inside_async_context, diagnostics);
  DiagnoseConcurrencyTaskCallableLegalityExpr(expr->right.get(), profile,
                                        inside_async_context, diagnostics);
  DiagnoseConcurrencyTaskCallableLegalityExpr(expr->third.get(), profile,
                                        inside_async_context, diagnostics);
  for (const auto &arg : expr->args) {
    DiagnoseConcurrencyTaskCallableLegalityExpr(arg.get(), profile,
                                          inside_async_context, diagnostics);
  }
}

static void DiagnoseConcurrencyTaskCallableLegalityStmt(
    const Stmt *stmt, const Objc3ConcurrencyTaskCallableLegalityProfile &profile,
    bool inside_async_context, std::vector<std::string> &diagnostics) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    if (stmt->let_stmt != nullptr) {
      DiagnoseConcurrencyTaskCallableLegalityExpr(stmt->let_stmt->value.get(), profile,
                                            inside_async_context, diagnostics);
    }
    return;
  case Stmt::Kind::Assign:
    if (stmt->assign_stmt != nullptr) {
      DiagnoseConcurrencyTaskCallableLegalityExpr(
          stmt->assign_stmt->value.get(), profile, inside_async_context,
          diagnostics);
    }
    return;
  case Stmt::Kind::Return:
    if (stmt->return_stmt != nullptr) {
      DiagnoseConcurrencyTaskCallableLegalityExpr(
          stmt->return_stmt->value.get(), profile, inside_async_context,
          diagnostics);
    }
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      DiagnoseConcurrencyTaskCallableLegalityExpr(
          stmt->if_stmt->condition.get(), profile, inside_async_context,
          diagnostics);
      for (const auto &then_stmt : stmt->if_stmt->then_body) {
        DiagnoseConcurrencyTaskCallableLegalityStmt(then_stmt.get(), profile,
                                              inside_async_context, diagnostics);
      }
      for (const auto &else_stmt : stmt->if_stmt->else_body) {
        DiagnoseConcurrencyTaskCallableLegalityStmt(else_stmt.get(), profile,
                                              inside_async_context, diagnostics);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &body_stmt : stmt->do_while_stmt->body) {
        DiagnoseConcurrencyTaskCallableLegalityStmt(body_stmt.get(), profile,
                                              inside_async_context, diagnostics);
      }
      DiagnoseConcurrencyTaskCallableLegalityExpr(
          stmt->do_while_stmt->condition.get(), profile, inside_async_context,
          diagnostics);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      DiagnoseConcurrencyTaskCallableLegalityExpr(stmt->for_stmt->init.value.get(),
                                            profile, inside_async_context,
                                            diagnostics);
      DiagnoseConcurrencyTaskCallableLegalityExpr(stmt->for_stmt->condition.get(),
                                            profile, inside_async_context,
                                            diagnostics);
      DiagnoseConcurrencyTaskCallableLegalityExpr(stmt->for_stmt->step.value.get(),
                                            profile, inside_async_context,
                                            diagnostics);
      for (const auto &body_stmt : stmt->for_stmt->body) {
        DiagnoseConcurrencyTaskCallableLegalityStmt(body_stmt.get(), profile,
                                              inside_async_context, diagnostics);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      DiagnoseConcurrencyTaskCallableLegalityExpr(
          stmt->switch_stmt->condition.get(), profile, inside_async_context,
          diagnostics);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &case_stmt : switch_case.body) {
          DiagnoseConcurrencyTaskCallableLegalityStmt(case_stmt.get(), profile,
                                                inside_async_context,
                                                diagnostics);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      DiagnoseConcurrencyTaskCallableLegalityExpr(
          stmt->while_stmt->condition.get(), profile, inside_async_context,
          diagnostics);
      for (const auto &body_stmt : stmt->while_stmt->body) {
        DiagnoseConcurrencyTaskCallableLegalityStmt(body_stmt.get(), profile,
                                              inside_async_context, diagnostics);
      }
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &body_stmt : stmt->block_stmt->body) {
        DiagnoseConcurrencyTaskCallableLegalityStmt(body_stmt.get(), profile,
                                              inside_async_context, diagnostics);
      }
    }
    return;
  case Stmt::Kind::Expr:
    if (stmt->expr_stmt != nullptr) {
      DiagnoseConcurrencyTaskCallableLegalityExpr(stmt->expr_stmt->value.get(), profile,
                                            inside_async_context, diagnostics);
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}
