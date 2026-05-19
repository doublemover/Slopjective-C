#pragma once

#include "ir/objc3_ir_statement_emission.h"

struct SwitchStmt;

void EmitObjc3IRSwitchStatement(
    const SwitchStmt *switch_stmt, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks);
