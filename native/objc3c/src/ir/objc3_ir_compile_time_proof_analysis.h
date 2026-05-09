#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "ir/objc3_ir_emitter_context.h"

struct Expr;

struct Objc3IRCompileTimeProofAnalysisContext {
  const std::unordered_set<std::string> &global_nil_proven_symbols;
  const std::unordered_map<std::string, int> &global_const_values;
  std::function<std::string(const FunctionContext &ctx,
                            const std::string &name)>
      lookup_var_ptr;
};

void InvalidateObjc3IRGlobalProofState(FunctionContext &ctx);

bool IsObjc3IRCompileTimeNilReceiverExprInContext(
    const Expr *expr, const FunctionContext &ctx,
    const Objc3IRCompileTimeProofAnalysisContext &analysis_context);

bool IsObjc3IRCompileTimeGlobalNilExpr(
    const Expr *expr,
    const Objc3IRCompileTimeProofAnalysisContext &analysis_context);

bool TryGetObjc3IRCompileTimeI32ExprInContext(
    const Expr *expr, const FunctionContext &ctx, int &value,
    const Objc3IRCompileTimeProofAnalysisContext &analysis_context);

bool IsObjc3IRCompileTimeKnownNonNilExprInContext(
    const Expr *expr, const FunctionContext &ctx,
    const Objc3IRCompileTimeProofAnalysisContext &analysis_context);
