bool IsObjc3IRCompileTimeNilReceiverExprInContext(
    const Expr *expr, const FunctionContext &ctx,
    const Objc3IRCompileTimeProofAnalysisContext &analysis_context) {
  if (expr == nullptr) {
    return false;
  }
  if (expr->kind == Expr::Kind::NilLiteral) {
    return true;
  }
  if (expr->kind == Expr::Kind::Conditional) {
    if (expr->left == nullptr || expr->right == nullptr ||
        expr->third == nullptr) {
      return false;
    }
    int cond_value = 0;
    if (!TryGetObjc3IRCompileTimeI32ExprInContext(
            expr->left.get(), ctx, cond_value, analysis_context)) {
      return false;
    }
    if (cond_value != 0) {
      return IsObjc3IRCompileTimeNilReceiverExprInContext(
          expr->right.get(), ctx, analysis_context);
    }
    return IsObjc3IRCompileTimeNilReceiverExprInContext(
        expr->third.get(), ctx, analysis_context);
  }
  if (expr->kind != Expr::Kind::Identifier) {
    return false;
  }
  const std::string ptr = analysis_context.lookup_var_ptr(ctx, expr->ident);
  if (ptr.empty()) {
    return LookupImmediateIdentifierValue(ctx, expr->ident) == 0;
  }
  if (ctx.nil_bound_ptrs.find(ptr) != ctx.nil_bound_ptrs.end()) {
    return true;
  }
  if (ptr.rfind("@", 0) == 0 && !ctx.global_proofs_invalidated) {
    return analysis_context.global_nil_proven_symbols.find(expr->ident) !=
           analysis_context.global_nil_proven_symbols.end();
  }
  return false;
}

bool IsObjc3IRCompileTimeGlobalNilExpr(
    const Expr *expr,
    const Objc3IRCompileTimeProofAnalysisContext &analysis_context) {
  if (expr == nullptr) {
    return false;
  }
  if (expr->kind == Expr::Kind::NilLiteral) {
    return true;
  }
  if (expr->kind == Expr::Kind::Identifier) {
    return analysis_context.global_nil_proven_symbols.find(expr->ident) !=
           analysis_context.global_nil_proven_symbols.end();
  }
  if (expr->kind == Expr::Kind::Conditional) {
    if (expr->left == nullptr || expr->right == nullptr ||
        expr->third == nullptr) {
      return false;
    }
    int cond_value = 0;
    const FunctionContext global_eval_ctx;
    if (!TryGetObjc3IRCompileTimeI32ExprInContext(
            expr->left.get(), global_eval_ctx, cond_value,
            analysis_context)) {
      return false;
    }
    if (cond_value != 0) {
      return IsObjc3IRCompileTimeGlobalNilExpr(expr->right.get(),
                                               analysis_context);
    }
    return IsObjc3IRCompileTimeGlobalNilExpr(expr->third.get(),
                                             analysis_context);
  }
  return false;
}
