#pragma once

#include <string>
#include <vector>

#include "ast/objc3_ast_core.h"

bool Objc3FuncParamHasConcreteTypeSurface(const FuncParam &param);
std::string Objc3FuncParamTypeReplayKey(const FuncParam &param);
std::string Objc3CallableSignatureReplayKey(
    const std::string &name, const std::vector<FuncParam> &params,
    ValueType return_type, bool async_declared, bool throws_declared);
