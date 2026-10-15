#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <string>
#include <unordered_map>

#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_method_definition_plan.h"
#include "ir/objc3_ir_runtime_dispatch_state.h"

struct Expr;

struct Objc3IRMessageSendEmissionOptions {
  const std::map<std::string, std::string> &selector_pool_globals;
  const std::map<std::string, std::string> &runtime_string_pool_globals;
  const std::unordered_map<std::string, int> &class_receiver_constants;
  const std::unordered_map<std::string, std::string>
      &direct_dispatch_symbols_by_key;
  const std::unordered_map<std::string, Objc3IRDirectDispatchSignature>
      &direct_dispatch_signatures_by_key;
  const std::unordered_map<std::string, ValueType>
      &runtime_dispatch_return_types_by_key;
  const std::unordered_map<std::string, Objc3IRValueOptionalCarrierMetadata>
      &runtime_dispatch_return_value_optional_carriers_by_key;
  const std::unordered_map<std::string, std::string>
      &runtime_dispatch_superclass_by_name;
  std::size_t runtime_dispatch_arg_slots = 0;
  std::string runtime_dispatch_symbol;
  Objc3IRRuntimeDispatchCallState &runtime_dispatch_call_state;
  bool arc_mode_enabled = false;
};

struct Objc3IRMessageSendEmissionCallbacks {
  std::function<std::string(const Expr *expr, FunctionContext &ctx)> emit_expr;
  std::function<std::string(const std::string &name, FunctionContext &ctx)>
      emit_identifier_value;
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

bool TryResolveObjc3IRDirectDispatchSignature(
    const Expr *expr, const FunctionContext &ctx,
    const Objc3IRMessageSendEmissionOptions &options,
    Objc3IRDirectDispatchSignature *signature_out,
    std::string *symbol_out = nullptr);

std::string EmitObjc3IRMessageSendExpr(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRMessageSendEmissionOptions &options,
    const Objc3IRMessageSendEmissionCallbacks &callbacks);
