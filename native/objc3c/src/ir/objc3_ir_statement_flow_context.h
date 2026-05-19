#pragma once

#include <functional>
#include <string>

#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_function_local_flow.h"
#include "ir/objc3_ir_scope_cleanup_emission.h"

struct Stmt;

struct Objc3IRStatementFlowContextServices {
  std::function<std::string(FunctionContext &ctx)> new_temp;
  std::function<std::string(FunctionContext &ctx, const std::string &prefix)>
      new_label;
  std::function<void(const Stmt *stmt, FunctionContext &ctx)> emit_statement;
};

Objc3IRScopeCleanupEmissionCallbacks
BuildObjc3IRStatementFlowScopeCleanupCallbacks(
    const Objc3IRStatementFlowContextServices &services);

Objc3IRFunctionLocalFlowContext BuildObjc3IRStatementFlowFunctionLocalContext(
    bool arc_mode_enabled,
    const Objc3IRStatementFlowContextServices &services);
