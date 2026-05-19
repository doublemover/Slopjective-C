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

static void DiagnoseDoCatchClauseShape(
    const Stmt &stmt,
    const BlockStmt &block,
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
  for (std::size_t index = 0; index < block.catch_clauses.size(); ++index) {
    const auto &clause = block.catch_clauses[index];
    if (saw_catch_all) {
      RecordDoCatchUnreachableCatchDiagnostic(summary, clause.line,
                                              clause.column, diagnostics);
    }
    if (clause.catch_all) {
      saw_catch_all = true;
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
    catch_context.local_handler_active = false;
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
  DiagnoseDoCatchClauseShape(stmt, block, summary, diagnostics,
                             allow_source_only_error_runtime_surface);
  WalkDoCatchProtectedBody(block, program, summary, diagnostics,
                           allow_source_only_error_runtime_surface, context);
  WalkDoCatchHandlers(block, program, summary, diagnostics,
                      allow_source_only_error_runtime_surface, context);
}
