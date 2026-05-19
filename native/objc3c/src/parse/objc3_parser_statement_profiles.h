#pragma once

#include <string>

#include "ast/objc3_ast_core.h"

namespace objc3c::parse {

const char *Objc3TryOperatorKindSpelling(Expr::TryOperatorKind kind);

std::string BuildObjc3TryExpressionProfile(const Expr &expr);

std::string BuildObjc3ThrowStatementProfile(const Expr &expr);

std::string BuildObjc3DoCatchProfile(const BlockStmt &block);

}  // namespace objc3c::parse
