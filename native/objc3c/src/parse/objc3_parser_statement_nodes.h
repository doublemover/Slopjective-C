#pragma once

#include <memory>
#include <string>

#include "ast/objc3_ast_core.h"
#include "token/objc3_token_contract.h"

namespace objc3c::parse {

std::unique_ptr<Stmt> BuildObjc3AssignmentStatement(
    const Objc3LexToken &name,
    const std::string &op,
    std::unique_ptr<Expr> value);

std::unique_ptr<Stmt> BuildObjc3ExpressionStatement(
    const Objc3LexToken &anchor,
    std::unique_ptr<Expr> value);

std::unique_ptr<Stmt> BuildObjc3LoopControlStatement(
    Stmt::Kind kind,
    const Objc3LexToken &anchor);

}  // namespace objc3c::parse
