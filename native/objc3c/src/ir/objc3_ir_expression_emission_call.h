#pragma once

#include <functional>
#include <string>

struct Expr;
struct FunctionContext;
struct Objc3IRExpressionEmissionCallbacks;

using Objc3IRExpressionChildEmitter = std::function<std::string(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionEmissionCallbacks &callbacks)>;

std::string EmitObjc3IRCallExpression(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionEmissionCallbacks &callbacks,
    const Objc3IRExpressionChildEmitter &emit_child_expr);
