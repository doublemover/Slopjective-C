struct Objc3MessageSendResolutionContext {
  const Objc3SemanticIntegrationSurface *surface = nullptr;
  const std::unordered_map<std::string, Objc3ProtocolSemanticDefinition>
      *protocol_definitions = nullptr;
  std::string current_implementation_name;
  std::string current_super_name;
  bool inside_method = false;
  bool is_class_method = false;
  bool inside_async_context = false;
};

struct Objc3ConcurrencyTaskCallableLegalityProfile {
  std::size_t task_creation_sites = 0;
  std::size_t detached_task_creation_sites = 0;
  std::size_t task_group_scope_sites = 0;
  std::size_t task_group_add_task_sites = 0;
  std::size_t task_group_wait_next_sites = 0;
  std::size_t task_group_cancel_all_sites = 0;
  std::size_t cancellation_check_sites = 0;
  std::size_t cancellation_handler_sites = 0;
};

struct Objc3ResolvedMessageSendMethod {
  enum class Status {
    RuntimeDynamic,
    Resolved,
    MissingConcreteSelector,
    AmbiguousConcreteSelector
  };

  Status status = Status::RuntimeDynamic;
  const Objc3MethodInfo *method = nullptr;
  std::string owner_name;
  bool is_class_method = false;
  bool missing_base = false;
  bool cycle_detected = false;
};

struct Objc3ResolvedMessageSendPropertyAccessor {
  const Objc3PropertyInfo *property = nullptr;
  bool is_setter = false;
};

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
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}

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

static void DiagnoseConcurrencyTaskCallableLegality(
    const std::vector<std::unique_ptr<Stmt>> &body, bool inside_async_context,
    std::vector<std::string> &diagnostics) {
  Objc3ConcurrencyTaskCallableLegalityProfile profile;
  for (const auto &stmt : body) {
    CollectConcurrencyTaskCallableLegalityStmtSites(stmt.get(), profile);
  }
  for (const auto &stmt : body) {
    DiagnoseConcurrencyTaskCallableLegalityStmt(stmt.get(), profile,
                                          inside_async_context, diagnostics);
  }
}

static Objc3ConcurrencyTaskCallableLegalityProfile
BuildConcurrencyTaskCallableLegalityProfile(
    const std::vector<std::unique_ptr<Stmt>> &body) {
  Objc3ConcurrencyTaskCallableLegalityProfile profile;
  for (const auto &stmt : body) {
    CollectConcurrencyTaskCallableLegalityStmtSites(stmt.get(), profile);
  }
  return profile;
}
