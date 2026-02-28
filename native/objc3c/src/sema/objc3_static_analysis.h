#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast/objc3_ast.h"

using StaticScalarBindings = std::unordered_map<std::string, int>;

bool IsBoolLikeI32Literal(const Expr *expr);
bool TryEvalStaticScalarValue(const Expr *expr, int &value, const StaticScalarBindings *bindings);
bool ExprIsStaticallyFalse(const Expr *expr, const StaticScalarBindings *bindings = nullptr);
bool ExprIsStaticallyTrue(const Expr *expr, const StaticScalarBindings *bindings = nullptr);

bool StatementAlwaysReturns(const Stmt *stmt, const StaticScalarBindings *bindings = nullptr);
bool BlockAlwaysReturns(const std::vector<std::unique_ptr<Stmt>> &statements,
                        const StaticScalarBindings *bindings = nullptr);
