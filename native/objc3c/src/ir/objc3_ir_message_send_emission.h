#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <string>
#include <unordered_map>

#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_runtime_dispatch_state.h"

struct Expr;

struct Objc3IRMessageSendEmissionOptions {
  const std::map<std::string, std::string> &selector_pool_globals;
  const std::unordered_map<std::string, int> &class_receiver_constants;
  const std::unordered_map<std::string, std::string>
      &direct_dispatch_symbols_by_key;
  std::size_t runtime_dispatch_arg_slots = 0;
  std::string runtime_dispatch_symbol;
  Objc3IRRuntimeDispatchCallState &runtime_dispatch_call_state;
};

struct Objc3IRMessageSendEmissionCallbacks {
  std::function<std::string(const Expr *expr, FunctionContext &ctx)> emit_expr;
  std::function<std::string(FunctionContext &ctx)> new_temp;
  std::function<std::string(FunctionContext &ctx, const std::string &prefix)>
      new_label;
  std::function<bool(const Expr *expr, const FunctionContext &ctx)>
      is_compile_time_nil_receiver_expr;
  std::function<bool(const Expr *expr, const FunctionContext &ctx)>
      is_compile_time_known_non_nil_expr;
  std::function<std::string(const std::string &reason)>
      emit_unsupported_i32_value;
  std::function<void(FunctionContext &ctx)> invalidate_global_proof_state;
};

std::string EmitObjc3IRMessageSendExpr(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRMessageSendEmissionOptions &options,
    const Objc3IRMessageSendEmissionCallbacks &callbacks);
