#pragma once

#include "ir/objc3_ir_statement_emission.h"

struct BlockStmt;

void EmitObjc3IRBlockStatement(
    const BlockStmt *block_stmt, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks);
