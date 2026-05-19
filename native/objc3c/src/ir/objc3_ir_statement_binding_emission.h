#pragma once

#include <string>

#include "ir/objc3_ir_statement_emission.h"

struct Expr;
struct ForClause;
struct IfStmt;

void EmitObjc3IRAssignmentStore(
    const std::string &ptr, const std::string &op, const Expr *value_expr,
    FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks);

void EmitObjc3IRForClause(
    const ForClause &clause, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks);

std::string EmitObjc3IRLocalI32Binding(
    const std::string &name, const Expr *value_expr, FunctionContext &ctx,
    bool mark_runtime_nonnull,
    const Objc3IRStatementEmissionCallbacks &callbacks);

void EmitObjc3IROptionalBindingIfStatement(
    const IfStmt *if_stmt, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks);
