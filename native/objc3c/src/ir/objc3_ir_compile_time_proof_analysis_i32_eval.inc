bool TryGetObjc3IRCompileTimeI32ExprInContext(
    const Expr *expr, const FunctionContext &ctx, int &value,
    const Objc3IRCompileTimeProofAnalysisContext &analysis_context) {
  if (expr == nullptr) {
    return false;
  }
  if (expr->kind == Expr::Kind::Number) {
    value = expr->number;
    return true;
  }
  if (expr->kind == Expr::Kind::BoolLiteral) {
    value = expr->bool_value ? 1 : 0;
    return true;
  }
  if (expr->kind == Expr::Kind::NilLiteral) {
    value = 0;
    return true;
  }
  if (expr->kind == Expr::Kind::Identifier) {
    const std::string ptr = analysis_context.lookup_var_ptr(ctx, expr->ident);
    if (ptr.empty()) {
      const int immediate_value =
          LookupImmediateIdentifierValue(ctx, expr->ident);
      if (immediate_value != 0) {
        value = immediate_value;
        return true;
      }
      return false;
    }
    auto value_it = ctx.const_value_ptrs.find(ptr);
    if (value_it != ctx.const_value_ptrs.end()) {
      value = value_it->second;
      return true;
    }
    if (ptr.rfind("@", 0) == 0 && !ctx.global_proofs_invalidated) {
      auto global_it = analysis_context.global_const_values.find(expr->ident);
      if (global_it != analysis_context.global_const_values.end()) {
        value = global_it->second;
        return true;
      }
    }
    return false;
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
      return TryGetObjc3IRCompileTimeI32ExprInContext(
          expr->right.get(), ctx, value, analysis_context);
    }
    return TryGetObjc3IRCompileTimeI32ExprInContext(
        expr->third.get(), ctx, value, analysis_context);
  }
  if (expr->kind != Expr::Kind::Binary || expr->left == nullptr ||
      expr->right == nullptr) {
    return false;
  }
  if (expr->op == "&&" || expr->op == "||") {
    int lhs = 0;
    if (!TryGetObjc3IRCompileTimeI32ExprInContext(
            expr->left.get(), ctx, lhs, analysis_context)) {
      return false;
    }
    if (expr->op == "&&") {
      if (lhs == 0) {
        value = 0;
        return true;
      }
      int rhs = 0;
      if (!TryGetObjc3IRCompileTimeI32ExprInContext(
              expr->right.get(), ctx, rhs, analysis_context)) {
        return false;
      }
      value = rhs != 0 ? 1 : 0;
      return true;
    }
    if (lhs != 0) {
      value = 1;
      return true;
    }
    int rhs = 0;
    if (!TryGetObjc3IRCompileTimeI32ExprInContext(
            expr->right.get(), ctx, rhs, analysis_context)) {
      return false;
    }
    value = rhs != 0 ? 1 : 0;
    return true;
  }
  if (expr->op == "??") {
    int lhs = 0;
    if (!TryGetObjc3IRCompileTimeI32ExprInContext(
            expr->left.get(), ctx, lhs, analysis_context)) {
      return false;
    }
    if (lhs != 0) {
      value = lhs;
      return true;
    }
    return TryGetObjc3IRCompileTimeI32ExprInContext(
        expr->right.get(), ctx, value, analysis_context);
  }
  int lhs = 0;
  int rhs = 0;
  if (!TryGetObjc3IRCompileTimeI32ExprInContext(
          expr->left.get(), ctx, lhs, analysis_context) ||
      !TryGetObjc3IRCompileTimeI32ExprInContext(
          expr->right.get(), ctx, rhs, analysis_context)) {
    return false;
  }
  if (expr->op == "+") {
    value = lhs + rhs;
    return true;
  }
  if (expr->op == "-") {
    value = lhs - rhs;
    return true;
  }
  if (expr->op == "*") {
    value = lhs * rhs;
    return true;
  }
  if (expr->op == "/") {
    if (rhs == 0) {
      return false;
    }
    value = lhs / rhs;
    return true;
  }
  if (expr->op == "%") {
    if (rhs == 0) {
      return false;
    }
    value = lhs % rhs;
    return true;
  }
  if (expr->op == "&") {
    value = lhs & rhs;
    return true;
  }
  if (expr->op == "|") {
    value = lhs | rhs;
    return true;
  }
  if (expr->op == "^") {
    value = lhs ^ rhs;
    return true;
  }
  if (expr->op == "<<" || expr->op == ">>") {
    if (rhs < 0 || rhs > 31) {
      return false;
    }
    value = expr->op == "<<" ? (lhs << rhs) : (lhs >> rhs);
    return true;
  }
  if (expr->op == "==") {
    value = lhs == rhs ? 1 : 0;
    return true;
  }
  if (expr->op == "!=") {
    value = lhs != rhs ? 1 : 0;
    return true;
  }
  if (expr->op == "<") {
    value = lhs < rhs ? 1 : 0;
    return true;
  }
  if (expr->op == "<=") {
    value = lhs <= rhs ? 1 : 0;
    return true;
  }
  if (expr->op == ">") {
    value = lhs > rhs ? 1 : 0;
    return true;
  }
  if (expr->op == ">=") {
    value = lhs >= rhs ? 1 : 0;
    return true;
  }
  return false;
}
