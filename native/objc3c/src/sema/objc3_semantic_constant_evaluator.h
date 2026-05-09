#pragma once

#include "parse/objc3_parser_contract_types.h"

#include <string>
#include <unordered_map>
#include <vector>

bool EvaluateSemanticConstExpr(
    const Expr *expr, int &value,
    const std::unordered_map<std::string, int> *resolved_globals = nullptr);
bool ResolveGlobalInitializerValues(
    const std::vector<Objc3ParsedGlobalDecl> &globals, std::vector<int> &values);
