#include "ir/objc3_ir_compile_time_proof_analysis.h"

#include "ast/objc3_ast.h"

namespace {

int LookupImmediateIdentifierValue(const FunctionContext &ctx,
                                   const std::string &name) {
  const auto value_it = ctx.immediate_identifiers.find(name);
  if (value_it == ctx.immediate_identifiers.end()) {
    return 0;
  }
  return value_it->second;
}

}  // namespace

void InvalidateObjc3IRGlobalProofState(FunctionContext &ctx) {
  ctx.global_proofs_invalidated = true;
  for (auto it = ctx.nil_bound_ptrs.begin(); it != ctx.nil_bound_ptrs.end();) {
    if (it->rfind("@", 0) == 0) {
      it = ctx.nil_bound_ptrs.erase(it);
    } else {
      ++it;
    }
  }
  for (auto it = ctx.nonzero_bound_ptrs.begin();
       it != ctx.nonzero_bound_ptrs.end();) {
    if (it->rfind("@", 0) == 0) {
      it = ctx.nonzero_bound_ptrs.erase(it);
    } else {
      ++it;
    }
  }
  for (auto it = ctx.const_value_ptrs.begin();
       it != ctx.const_value_ptrs.end();) {
    if (it->first.rfind("@", 0) == 0) {
      it = ctx.const_value_ptrs.erase(it);
    } else {
      ++it;
    }
  }
}

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
