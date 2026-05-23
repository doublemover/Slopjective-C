#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ast/objc3_ast_core.h"
#include "token/objc3_token_contract.h"

namespace objc3c::parse {

std::unique_ptr<Expr> BuildObjc3BinaryExpr(
    const Objc3LexToken &op,
    std::unique_ptr<Expr> lhs,
    std::unique_ptr<Expr> rhs);

std::unique_ptr<Expr> BuildObjc3ConditionalExpr(
    const Objc3LexToken &question,
    std::unique_ptr<Expr> condition,
    std::unique_ptr<Expr> when_true,
    std::unique_ptr<Expr> when_false);

std::unique_ptr<Expr> BuildObjc3NumericLiteralExpr(
    int value,
    const Objc3LexToken &token);

std::unique_ptr<Expr> BuildObjc3BoolLiteralExpr(
    bool value,
    const Objc3LexToken &token);

std::unique_ptr<Expr> BuildObjc3BoolLiteralExprAt(
    bool value,
    unsigned line,
    unsigned column);

std::unique_ptr<Expr> BuildObjc3NilLiteralExpr(const Objc3LexToken &token);

std::unique_ptr<Expr> BuildObjc3StringLiteralExpr(
    const std::string &value,
    int byte_count,
    int unit_count,
    const Objc3LexToken &token);

std::unique_ptr<Expr> BuildObjc3StringInterpolationExpr(
    std::vector<std::string> segments,
    std::vector<std::unique_ptr<Expr>> payloads,
    const Objc3LexToken &token);

std::unique_ptr<Expr> BuildObjc3CollectionLiteralExpr(
    Expr::CollectionLiteralKind kind,
    const Objc3LexToken &token,
    std::vector<std::unique_ptr<Expr>> keys,
    std::vector<std::unique_ptr<Expr>> values);

std::unique_ptr<Expr> BuildObjc3IndexAccessExpr(
    const Objc3LexToken &open,
    std::unique_ptr<Expr> collection,
    std::unique_ptr<Expr> index);

std::unique_ptr<Expr> BuildObjc3IdentifierExpr(
    const std::string &identifier,
    const Objc3LexToken &token);

std::unique_ptr<Expr> BuildObjc3TypedKeyPathLiteralExpr(
    const Objc3LexToken &keypath_token,
    const Objc3LexToken &root_token,
    std::vector<std::string> components);

std::unique_ptr<Expr> BuildObjc3UnaryLoweringBinaryExpr(
    const Objc3LexToken &op,
    const std::string &lowered_binary_op,
    int synthetic_lhs_value,
    std::unique_ptr<Expr> rhs);

std::unique_ptr<Expr> BuildObjc3LogicalNotExpr(
    const Objc3LexToken &op,
    std::unique_ptr<Expr> rhs);

std::unique_ptr<Expr> BuildObjc3BitwiseNotExpr(
    const Objc3LexToken &op,
    std::unique_ptr<Expr> rhs);

std::unique_ptr<Expr> BuildObjc3TryExpression(
    const Objc3LexToken &try_token,
    Expr::TryOperatorKind try_kind,
    bool requires_throwing_context,
    std::unique_ptr<Expr> rhs);

}  // namespace objc3c::parse
