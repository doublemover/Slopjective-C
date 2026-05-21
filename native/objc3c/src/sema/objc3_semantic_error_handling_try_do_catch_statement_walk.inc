static void WalkErrorHandlingTryDoCatchStmt(
    const Stmt *stmt,
    const Objc3Program &program,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    std::vector<std::string> &diagnostics,
    bool allow_source_only_error_runtime_surface,
    const Objc3ErrorHandlingSemanticWalkContext &context) {
  if (stmt == nullptr) {
    return;
  }

  switch (stmt->kind) {
  case Stmt::Kind::Let:
    WalkErrorHandlingTryDoCatchExpr(
        stmt->let_stmt != nullptr ? stmt->let_stmt->value.get() : nullptr,
        program, summary, diagnostics, allow_source_only_error_runtime_surface,
        context);
    return;
  case Stmt::Kind::Assign:
    WalkErrorHandlingTryDoCatchExpr(
        stmt->assign_stmt != nullptr ? stmt->assign_stmt->value.get() : nullptr,
        program, summary, diagnostics, allow_source_only_error_runtime_surface,
        context);
    return;
  case Stmt::Kind::Return:
    WalkErrorHandlingTryDoCatchExpr(
        stmt->return_stmt != nullptr ? stmt->return_stmt->value.get() : nullptr,
        program, summary, diagnostics, allow_source_only_error_runtime_surface,
        context);
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt == nullptr) {
      return;
    }
    WalkErrorHandlingTryDoCatchExpr(
        stmt->if_stmt->condition.get(), program, summary, diagnostics,
        allow_source_only_error_runtime_surface, context);
    for (const auto &then_stmt : stmt->if_stmt->then_body) {
      WalkErrorHandlingTryDoCatchStmt(then_stmt.get(), program, summary,
                                      diagnostics,
                                      allow_source_only_error_runtime_surface,
                                      context);
    }
    for (const auto &else_stmt : stmt->if_stmt->else_body) {
      WalkErrorHandlingTryDoCatchStmt(else_stmt.get(), program, summary,
                                      diagnostics,
                                      allow_source_only_error_runtime_surface,
                                      context);
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt == nullptr) {
      return;
    }
    for (const auto &body_stmt : stmt->do_while_stmt->body) {
      WalkErrorHandlingTryDoCatchStmt(body_stmt.get(), program, summary,
                                      diagnostics,
                                      allow_source_only_error_runtime_surface,
                                      context);
    }
    WalkErrorHandlingTryDoCatchExpr(
        stmt->do_while_stmt->condition.get(), program, summary, diagnostics,
        allow_source_only_error_runtime_surface, context);
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt == nullptr) {
      return;
    }
    WalkErrorHandlingTryDoCatchExpr(
        stmt->for_stmt->init.value.get(), program, summary, diagnostics,
        allow_source_only_error_runtime_surface, context);
    WalkErrorHandlingTryDoCatchExpr(
        stmt->for_stmt->condition.get(), program, summary, diagnostics,
        allow_source_only_error_runtime_surface, context);
    WalkErrorHandlingTryDoCatchExpr(
        stmt->for_stmt->step.value.get(), program, summary, diagnostics,
        allow_source_only_error_runtime_surface, context);
    for (const auto &body_stmt : stmt->for_stmt->body) {
      WalkErrorHandlingTryDoCatchStmt(body_stmt.get(), program, summary,
                                      diagnostics,
                                      allow_source_only_error_runtime_surface,
                                      context);
    }
    return;
  case Stmt::Kind::ForIn:
    if (stmt->for_in_stmt == nullptr) {
      return;
    }
    WalkErrorHandlingTryDoCatchExpr(
        stmt->for_in_stmt->collection.get(), program, summary, diagnostics,
        allow_source_only_error_runtime_surface, context);
    for (const auto &body_stmt : stmt->for_in_stmt->body) {
      WalkErrorHandlingTryDoCatchStmt(body_stmt.get(), program, summary,
                                      diagnostics,
                                      allow_source_only_error_runtime_surface,
                                      context);
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt == nullptr) {
      return;
    }
    WalkErrorHandlingTryDoCatchExpr(
        stmt->switch_stmt->condition.get(), program, summary, diagnostics,
        allow_source_only_error_runtime_surface, context);
    for (const auto &switch_case : stmt->switch_stmt->cases) {
      for (const auto &case_stmt : switch_case.body) {
        WalkErrorHandlingTryDoCatchStmt(case_stmt.get(), program, summary,
                                        diagnostics,
                                        allow_source_only_error_runtime_surface,
                                        context);
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt == nullptr) {
      return;
    }
    WalkErrorHandlingTryDoCatchExpr(
        stmt->while_stmt->condition.get(), program, summary, diagnostics,
        allow_source_only_error_runtime_surface, context);
    for (const auto &body_stmt : stmt->while_stmt->body) {
      WalkErrorHandlingTryDoCatchStmt(body_stmt.get(), program, summary,
                                      diagnostics,
                                      allow_source_only_error_runtime_surface,
                                      context);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt == nullptr) {
      return;
    }
    if (stmt->block_stmt->is_do_catch_scope) {
      WalkDoCatchScope(*stmt, *stmt->block_stmt, program, summary, diagnostics,
                       allow_source_only_error_runtime_surface, context);
      return;
    }
    for (const auto &body_stmt : stmt->block_stmt->body) {
      WalkErrorHandlingTryDoCatchStmt(body_stmt.get(), program, summary,
                                      diagnostics,
                                      allow_source_only_error_runtime_surface,
                                      context);
    }
    return;
  case Stmt::Kind::Expr:
    WalkErrorHandlingTryDoCatchExpr(
        stmt->expr_stmt != nullptr ? stmt->expr_stmt->value.get() : nullptr,
        program, summary, diagnostics, allow_source_only_error_runtime_surface,
        context);
    return;
  case Stmt::Kind::CollectionMutation:
    if (stmt->collection_mutation_stmt == nullptr) {
      return;
    }
    WalkErrorHandlingTryDoCatchExpr(
        stmt->collection_mutation_stmt->key_or_index.get(), program, summary,
        diagnostics, allow_source_only_error_runtime_surface, context);
    WalkErrorHandlingTryDoCatchExpr(
        stmt->collection_mutation_stmt->value.get(), program, summary,
        diagnostics, allow_source_only_error_runtime_surface, context);
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  }
}
