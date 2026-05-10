bool IsObjc3IRCompileTimeKnownNonNilExprInContext(
    const Expr *expr, const FunctionContext &ctx,
    const Objc3IRCompileTimeProofAnalysisContext &analysis_context) {
  int const_value = 0;
  if (!TryGetObjc3IRCompileTimeI32ExprInContext(
          expr, ctx, const_value, analysis_context)) {
    return false;
  }
  return const_value != 0;
}
