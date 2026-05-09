#pragma once

#include "ast/objc3_ast_core.h"

#include <string>
#include <vector>

const char *Objc3ValueTypeSpelling(ValueType type);
bool Objc3ValueTypeIsScalar(ValueType type);
bool Objc3ValueTypeIsObjectReference(ValueType type);
bool Objc3ValueTypeIsRuntimeMetadataReference(ValueType type);
bool Objc3FuncParamHasConcreteTypeSurface(const FuncParam &param);
std::string Objc3FuncParamTypeReplayKey(const FuncParam &param);
std::string Objc3CallableSignatureReplayKey(
    const std::string &name, const std::vector<FuncParam> &params,
    ValueType return_type, bool async_declared, bool throws_declared);
