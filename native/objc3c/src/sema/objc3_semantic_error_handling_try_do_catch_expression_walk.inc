static void AccumulateTryOperatorKind(
    const Expr &expr,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary) {
  ++summary.try_expression_sites;
  switch (expr.try_operator_kind) {
  case Expr::TryOperatorKind::Propagate:
    ++summary.try_propagating_sites;
    break;
  case Expr::TryOperatorKind::Optional:
    ++summary.try_optional_sites;
    break;
  case Expr::TryOperatorKind::Forced:
    ++summary.try_forced_sites;
    break;
  case Expr::TryOperatorKind::None:
    break;
  }
}

static void AccumulateTryOperandSurface(
    const Expr &expr,
    const Objc3Program &program,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    std::vector<std::string> &diagnostics) {
  const Expr *operand =
      !expr.args.empty() ? expr.args.front().get() : expr.left.get();
  const Objc3ErrorHandlingTryOperandSurface surface =
      ResolveTryOperandSurface(operand, program);

  if (surface.throwing_callable) {
    ++summary.throwing_callable_try_sites;
  }
  if (surface.bridged_callable) {
    ++summary.bridged_callable_try_sites;
  }
  if (!surface.throwing_callable && !surface.bridged_callable) {
    RecordTryOperandSurfaceDiagnostic(summary, expr.line, expr.column,
                                      diagnostics);
  }
}

static void AccumulateTryPropagationLegality(
    const Expr &expr,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    std::vector<std::string> &diagnostics,
    const Objc3ErrorHandlingSemanticWalkContext &context) {
  if (!expr.try_expression_requires_throwing_context) {
    return;
  }

  if (context.local_handler_active) {
    ++summary.local_handler_sites;
    return;
  }
  if (context.in_throws_callable) {
    ++summary.caller_propagation_sites;
    return;
  }
  RecordTryPropagationContextDiagnostic(summary, expr.line, expr.column,
                                        diagnostics);
}

static void AccumulateTryExpressionSemantics(
    const Expr &expr,
    const Objc3Program &program,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    std::vector<std::string> &diagnostics,
    bool allow_source_only_error_runtime_surface,
    const Objc3ErrorHandlingSemanticWalkContext &context) {
  AccumulateTryOperatorKind(expr, summary);
  if (!allow_source_only_error_runtime_surface) {
    RecordTryUnsupportedRuntimeDiagnostic(summary, expr.line, expr.column,
                                          diagnostics);
  }
  AccumulateTryOperandSurface(expr, program, summary, diagnostics);
  AccumulateTryPropagationLegality(expr, summary, diagnostics, context);
}

static void AccumulateThrowStatementSemantics(
    const Expr &expr,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    std::vector<std::string> &diagnostics,
    bool allow_source_only_error_runtime_surface,
    const Objc3ErrorHandlingSemanticWalkContext &context) {
  ++summary.throw_statement_sites;
  if (context.in_catch_body) {
    ++summary.rethrow_sites;
  }
  if (!allow_source_only_error_runtime_surface) {
    RecordThrowUnsupportedRuntimeDiagnostic(summary, expr.line, expr.column,
                                            diagnostics);
  }
  if (!(context.in_throws_callable || context.in_catch_body)) {
    RecordThrowContextDiagnostic(summary, expr.line, expr.column, diagnostics);
  }
}

static void WalkErrorHandlingTryDoCatchChildExprs(
    const Expr &expr,
    const Objc3Program &program,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    std::vector<std::string> &diagnostics,
    bool allow_source_only_error_runtime_surface,
    const Objc3ErrorHandlingSemanticWalkContext &context) {
  WalkErrorHandlingTryDoCatchExpr(expr.receiver.get(), program, summary,
                                  diagnostics,
                                  allow_source_only_error_runtime_surface,
                                  context);
  WalkErrorHandlingTryDoCatchExpr(expr.left.get(), program, summary,
                                  diagnostics,
                                  allow_source_only_error_runtime_surface,
                                  context);
  WalkErrorHandlingTryDoCatchExpr(expr.right.get(), program, summary,
                                  diagnostics,
                                  allow_source_only_error_runtime_surface,
                                  context);
  WalkErrorHandlingTryDoCatchExpr(expr.third.get(), program, summary,
                                  diagnostics,
                                  allow_source_only_error_runtime_surface,
                                  context);
  for (const auto &arg : expr.args) {
    WalkErrorHandlingTryDoCatchExpr(arg.get(), program, summary, diagnostics,
                                    allow_source_only_error_runtime_surface,
                                    context);
  }
}

static void WalkErrorHandlingTryDoCatchExpr(
    const Expr *expr,
    const Objc3Program &program,
    Objc3ErrorHandlingTryDoCatchSemanticSummary &summary,
    std::vector<std::string> &diagnostics,
    bool allow_source_only_error_runtime_surface,
    const Objc3ErrorHandlingSemanticWalkContext &context) {
  if (expr == nullptr) {
    return;
  }

  if (expr->try_expression_enabled) {
    AccumulateTryExpressionSemantics(
        *expr, program, summary, diagnostics,
        allow_source_only_error_runtime_surface, context);
  }
  if (expr->throw_statement_enabled) {
    AccumulateThrowStatementSemantics(
        *expr, summary, diagnostics, allow_source_only_error_runtime_surface,
        context);
  }
  WalkErrorHandlingTryDoCatchChildExprs(
      *expr, program, summary, diagnostics,
      allow_source_only_error_runtime_surface, context);
}
