static void AccumulateDoCatchClauseShape(
    const BlockStmt &block,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary) {
  ++summary.do_catch_sites;
  summary.catch_clause_sites += block.catch_clauses.size();
  for (const auto &clause : block.catch_clauses) {
    if (clause.catch_all) {
      ++summary.catch_all_sites;
    }
    if (clause.has_binding) {
      ++summary.catch_binding_sites;
    }
  }
}

static void CollectTypedThrowsPayloadsFromExpr(
    const Expr *expr,
    const Objc3Program &program,
    const Objc3ErrorHandlingSemanticWalkContext &context,
    std::vector<std::string> &typed_payloads,
    bool &saw_untyped_or_bridge) {
  if (expr == nullptr) {
    return;
  }
  if (expr->try_expression_enabled) {
    const Expr *operand =
        !expr->args.empty() ? expr->args.front().get() : expr->left.get();
    if (operand != nullptr) {
      const Objc3ErrorHandlingTryOperandSurface surface =
          ResolveTryOperandSurface(operand, program, context);
      if (!surface.typed_throws_payload.empty()) {
        typed_payloads.push_back(surface.typed_throws_payload);
      } else if (surface.throwing_callable || surface.bridged_callable) {
        saw_untyped_or_bridge = true;
      }
    }
  }
  CollectTypedThrowsPayloadsFromExpr(expr->receiver.get(), program, context,
                                     typed_payloads, saw_untyped_or_bridge);
  CollectTypedThrowsPayloadsFromExpr(expr->left.get(), program, context,
                                     typed_payloads, saw_untyped_or_bridge);
  CollectTypedThrowsPayloadsFromExpr(expr->right.get(), program, context,
                                     typed_payloads, saw_untyped_or_bridge);
  CollectTypedThrowsPayloadsFromExpr(expr->third.get(), program, context,
                                     typed_payloads, saw_untyped_or_bridge);
  for (const auto &key : expr->collection_keys) {
    CollectTypedThrowsPayloadsFromExpr(
        key.get(), program, context, typed_payloads, saw_untyped_or_bridge);
  }
  for (const auto &value : expr->collection_values) {
    CollectTypedThrowsPayloadsFromExpr(
        value.get(), program, context, typed_payloads, saw_untyped_or_bridge);
  }
  for (const auto &arg : expr->args) {
    CollectTypedThrowsPayloadsFromExpr(
        arg.get(), program, context, typed_payloads, saw_untyped_or_bridge);
  }
}

static void CollectTypedThrowsPayloadsFromStmt(
    const Stmt *stmt,
    const Objc3Program &program,
    const Objc3ErrorHandlingSemanticWalkContext &context,
    std::vector<std::string> &typed_payloads,
    bool &saw_untyped_or_bridge) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    CollectTypedThrowsPayloadsFromExpr(
        stmt->let_stmt != nullptr ? stmt->let_stmt->value.get() : nullptr,
        program, context, typed_payloads, saw_untyped_or_bridge);
    return;
  case Stmt::Kind::Assign:
    CollectTypedThrowsPayloadsFromExpr(
        stmt->assign_stmt != nullptr ? stmt->assign_stmt->value.get() : nullptr,
        program, context, typed_payloads, saw_untyped_or_bridge);
    return;
  case Stmt::Kind::Return:
    CollectTypedThrowsPayloadsFromExpr(
        stmt->return_stmt != nullptr ? stmt->return_stmt->value.get()
                                     : nullptr,
        program, context, typed_payloads, saw_untyped_or_bridge);
    return;
  case Stmt::Kind::Expr:
    CollectTypedThrowsPayloadsFromExpr(
        stmt->expr_stmt != nullptr ? stmt->expr_stmt->value.get() : nullptr,
        program, context, typed_payloads, saw_untyped_or_bridge);
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectTypedThrowsPayloadsFromExpr(
          stmt->if_stmt->condition.get(), program, context, typed_payloads,
          saw_untyped_or_bridge);
      for (const auto &nested_stmt : stmt->if_stmt->then_body) {
        CollectTypedThrowsPayloadsFromStmt(
            nested_stmt.get(), program, context, typed_payloads,
            saw_untyped_or_bridge);
      }
      for (const auto &nested_stmt : stmt->if_stmt->else_body) {
        CollectTypedThrowsPayloadsFromStmt(
            nested_stmt.get(), program, context, typed_payloads,
            saw_untyped_or_bridge);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &nested_stmt : stmt->do_while_stmt->body) {
        CollectTypedThrowsPayloadsFromStmt(
            nested_stmt.get(), program, context, typed_payloads,
            saw_untyped_or_bridge);
      }
      CollectTypedThrowsPayloadsFromExpr(
          stmt->do_while_stmt->condition.get(), program, context, typed_payloads,
          saw_untyped_or_bridge);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectTypedThrowsPayloadsFromExpr(
          stmt->for_stmt->init.value.get(), program, context, typed_payloads,
          saw_untyped_or_bridge);
      CollectTypedThrowsPayloadsFromExpr(
          stmt->for_stmt->condition.get(), program, context, typed_payloads,
          saw_untyped_or_bridge);
      CollectTypedThrowsPayloadsFromExpr(
          stmt->for_stmt->step.value.get(), program, context, typed_payloads,
          saw_untyped_or_bridge);
      for (const auto &nested_stmt : stmt->for_stmt->body) {
        CollectTypedThrowsPayloadsFromStmt(
            nested_stmt.get(), program, context, typed_payloads,
            saw_untyped_or_bridge);
      }
    }
    return;
  case Stmt::Kind::ForIn:
    if (stmt->for_in_stmt != nullptr) {
      CollectTypedThrowsPayloadsFromExpr(
          stmt->for_in_stmt->collection.get(), program, context, typed_payloads,
          saw_untyped_or_bridge);
      for (const auto &nested_stmt : stmt->for_in_stmt->body) {
        CollectTypedThrowsPayloadsFromStmt(
            nested_stmt.get(), program, context, typed_payloads,
            saw_untyped_or_bridge);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectTypedThrowsPayloadsFromExpr(
          stmt->switch_stmt->condition.get(), program, context, typed_payloads,
          saw_untyped_or_bridge);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &nested_stmt : switch_case.body) {
          CollectTypedThrowsPayloadsFromStmt(
              nested_stmt.get(), program, context, typed_payloads,
              saw_untyped_or_bridge);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectTypedThrowsPayloadsFromExpr(
          stmt->while_stmt->condition.get(), program, context, typed_payloads,
          saw_untyped_or_bridge);
      for (const auto &nested_stmt : stmt->while_stmt->body) {
        CollectTypedThrowsPayloadsFromStmt(
            nested_stmt.get(), program, context, typed_payloads,
            saw_untyped_or_bridge);
      }
    }
    return;
  case Stmt::Kind::CollectionMutation:
    if (stmt->collection_mutation_stmt != nullptr) {
      CollectTypedThrowsPayloadsFromExpr(
          stmt->collection_mutation_stmt->key_or_index.get(), program,
          context, typed_payloads, saw_untyped_or_bridge);
      CollectTypedThrowsPayloadsFromExpr(
          stmt->collection_mutation_stmt->value.get(), program, context,
          typed_payloads, saw_untyped_or_bridge);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &nested_stmt : stmt->block_stmt->body) {
        CollectTypedThrowsPayloadsFromStmt(
            nested_stmt.get(), program, context, typed_payloads,
            saw_untyped_or_bridge);
      }
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  default:
    return;
  }
}

static std::string SingleTypedThrowsPayloadForDoCatchBody(
    const BlockStmt &block,
    const Objc3Program &program,
    const Objc3ErrorHandlingSemanticWalkContext &context) {
  std::vector<std::string> typed_payloads;
  bool saw_untyped_or_bridge = false;
  for (const auto &stmt : block.body) {
    CollectTypedThrowsPayloadsFromStmt(
        stmt.get(), program, context, typed_payloads, saw_untyped_or_bridge);
  }
  if (typed_payloads.empty() || saw_untyped_or_bridge) {
    return "";
  }
  const std::string first_identity =
      Objc3TypedThrowsPayloadIdentity(typed_payloads.front());
  for (const std::string &payload : typed_payloads) {
    if (Objc3TypedThrowsPayloadIdentity(payload) != first_identity) {
      return "";
    }
  }
  return typed_payloads.front();
}

static void DiagnoseDoCatchClauseShape(
    const Stmt &stmt,
    const BlockStmt &block,
    const Objc3Program &program,
    const Objc3ErrorHandlingSemanticWalkContext &context,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    std::vector<std::string> &diagnostics,
    bool allow_source_only_error_runtime_surface) {
  if (!allow_source_only_error_runtime_surface) {
    RecordDoCatchUnsupportedRuntimeDiagnostic(summary, stmt.line, stmt.column,
                                              diagnostics);
  }
  if (block.catch_clauses.empty()) {
    RecordDoCatchEmptyCatchDiagnostic(summary, stmt.line, stmt.column,
                                      diagnostics);
  }

  bool saw_catch_all = false;
  const std::string typed_throw_payload =
      SingleTypedThrowsPayloadForDoCatchBody(block, program, context);
  for (std::size_t index = 0; index < block.catch_clauses.size(); ++index) {
    const auto &clause = block.catch_clauses[index];
    if (saw_catch_all) {
      RecordDoCatchUnreachableCatchDiagnostic(summary, clause.line,
                                              clause.column, diagnostics);
    }
    if (clause.catch_all) {
      saw_catch_all = true;
      continue;
    }
    if (!typed_throw_payload.empty()) {
      const std::string match_status = Objc3TypedThrowsCatchMatchStatus(
          typed_throw_payload, clause.binding_type_spelling, true);
      if (match_status == "incompatible-catch-rejected" ||
          match_status == "unsupported-foreign-carrier-fail-closed") {
        RecordTypedThrowsCatchCompatibilityDiagnostic(
            summary, clause.line, clause.column, typed_throw_payload,
            clause.binding_type_spelling, match_status, diagnostics);
      }
    }
  }
}

static void WalkDoCatchProtectedBody(
    const BlockStmt &block,
    const Objc3Program &program,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    std::vector<std::string> &diagnostics,
    bool allow_source_only_error_runtime_surface,
    const Objc3ErrorHandlingSemanticWalkContext &context) {
  Objc3ErrorHandlingSemanticWalkContext body_context = context;
  body_context.local_handler_active = true;
  for (const auto &body_stmt : block.body) {
    WalkErrorHandlingTryDoCatchStmt(body_stmt.get(), program, summary,
                                    diagnostics,
                                    allow_source_only_error_runtime_surface,
                                    body_context);
  }
}

static void WalkDoCatchHandlers(
    const BlockStmt &block,
    const Objc3Program &program,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    std::vector<std::string> &diagnostics,
    bool allow_source_only_error_runtime_surface,
    const Objc3ErrorHandlingSemanticWalkContext &context) {
  for (const auto &clause : block.catch_clauses) {
    Objc3ErrorHandlingSemanticWalkContext catch_context = context;
    catch_context.in_catch_body = true;
    for (const auto &body_stmt : clause.body) {
      WalkErrorHandlingTryDoCatchStmt(
          body_stmt.get(), program, summary, diagnostics,
          allow_source_only_error_runtime_surface, catch_context);
    }
  }
}

static void WalkDoCatchScope(
    const Stmt &stmt,
    const BlockStmt &block,
    const Objc3Program &program,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    std::vector<std::string> &diagnostics,
    bool allow_source_only_error_runtime_surface,
    const Objc3ErrorHandlingSemanticWalkContext &context) {
  AccumulateDoCatchClauseShape(block, summary);
  DiagnoseDoCatchClauseShape(stmt, block, program, context, summary,
                             diagnostics,
                             allow_source_only_error_runtime_surface);
  WalkDoCatchProtectedBody(block, program, summary, diagnostics,
                           allow_source_only_error_runtime_surface, context);
  WalkDoCatchHandlers(block, program, summary, diagnostics,
                      allow_source_only_error_runtime_surface, context);
}
