#pragma once

#include <string>
#include <unordered_map>

#include "ast/objc3_ast_core.h"

using StaticScalarBindings = std::unordered_map<std::string, int>;

bool IsBoolLikeI32Literal(const Expr *expr);
bool TryEvalStaticScalarValue(const Expr *expr, int &value,
                              const StaticScalarBindings *bindings);
bool ExprIsStaticallyFalse(const Expr *expr,
                           const StaticScalarBindings *bindings = nullptr);
bool ExprIsStaticallyTrue(const Expr *expr,
                          const StaticScalarBindings *bindings = nullptr);
