#pragma once

#include "ir/objc3_ir_statement_emission.h"

struct DoWhileStmt;
struct ForStmt;
struct WhileStmt;

void EmitObjc3IRWhileStatement(
    const WhileStmt *while_stmt, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks);

void EmitObjc3IRDoWhileStatement(
    const DoWhileStmt *do_while_stmt, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks);

void EmitObjc3IRForStatement(
    const ForStmt *for_stmt, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks);
