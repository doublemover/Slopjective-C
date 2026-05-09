#pragma once

#include <string>

#include "sema/objc3_semantic_type_helpers.h"

bool IsSameSemanticType(const SemanticTypeInfo &lhs, const SemanticTypeInfo &rhs);
bool IsEscapingBlockRuntimeHandleCompatible(
    const SemanticTypeInfo &expected,
    const SemanticTypeInfo &value);
std::string SemanticTypeName(const SemanticTypeInfo &info);
