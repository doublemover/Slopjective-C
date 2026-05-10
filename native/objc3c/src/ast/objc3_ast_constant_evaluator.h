#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "ast/objc3_ast_declarations.h"

bool EvaluateObjc3ConstExpr(
    const Expr *expr, int &value,
    const std::unordered_map<std::string, int> *resolved_globals = nullptr);

bool ResolveObjc3GlobalInitializerValues(const std::vector<GlobalDecl> &globals,
                                         std::vector<int> &values);
