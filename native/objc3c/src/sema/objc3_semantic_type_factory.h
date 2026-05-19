#pragma once

#include <string>
#include <vector>

#include "ast/objc3_ast_core.h"
#include "sema/objc3_semantic_type_helpers.h"

SemanticTypeInfo MakeScalarSemanticType(ValueType type);
SemanticTypeInfo MakeVectorSemanticType(
    ValueType base_type,
    const std::string &base_spelling,
    unsigned lane_count);
SemanticTypeInfo MakeCallableSemanticType(
    std::vector<ValueType> param_types,
    ValueType return_type);
SemanticTypeInfo MakeCallableSemanticTypeFromBlockLiteral(const Expr &expr);
SemanticTypeInfo MakeSemanticTypeFromGlobal(ValueType type);
