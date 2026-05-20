#pragma once

#include <functional>
#include <string>

#include "ir/objc3_ir_emitter_context.h"

struct Expr;

struct Objc3IRConcurrencyRuntimeCallEmissionCallbacks {
  std::function<std::string(FunctionContext &ctx)> new_temp;
  std::function<std::string(const Expr *expr)> emit_expr;
  std::function<void(FunctionContext &ctx)> invalidate_global_proof_state;
};

bool TryEmitObjc3IRConcurrencyTaskRuntimeLoweringCall(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRConcurrencyRuntimeCallEmissionCallbacks &callbacks,
    std::string &result_out);

bool IsObjc3IRConcurrencyTaskRuntimeHelperName(const std::string &name);

bool TryEmitObjc3IRConcurrencyActorLoweringCall(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRConcurrencyRuntimeCallEmissionCallbacks &callbacks,
    std::string &result_out);
