#pragma once

#include <string>

#include "ast/objc3_ast_core.h"

const char *LLVMScalarType(ValueType type);
ValueType RuntimeMetadataValueType(const std::string &type_name);
