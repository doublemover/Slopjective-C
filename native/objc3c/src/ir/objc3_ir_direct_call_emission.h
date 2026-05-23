#pragma once

#include <functional>
#include <string>

#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_function_signature_model.h"

struct Expr;

struct Objc3IRDirectCallEmissionCallbacks {
  std::function<std::string(const Expr *expr)> emit_expr;
  std::function<std::string(FunctionContext &ctx)> new_temp;
  std::function<std::string(const std::string &value, FunctionContext &ctx)>
      coerce_i32_to_bool_i1;
  std::function<std::string(const std::string &value, ValueType value_type,
                            FunctionContext &ctx)>
      coerce_value_to_i32;
  std::function<bool(const std::string &function_name)>
      function_may_have_global_side_effects;
  std::function<std::string(const std::string &reason)>
      emit_unsupported_i32_value;
  std::function<bool(const Expr *expr, FunctionContext &ctx,
                     std::string &result_out)>
      try_emit_concurrency_actor_lowering_call;
  std::function<bool(const Expr *expr, FunctionContext &ctx,
                     std::string &result_out)>
      try_emit_concurrency_task_runtime_lowering_call;
  std::function<void(FunctionContext &ctx)> invalidate_global_proof_state;
  std::function<void(FunctionContext &ctx)>
      emit_return_await_cleanup_before_handoff;
  std::function<const LoweredFunctionSignature *(const std::string &name)>
      lookup_function_signature;
};

std::string EmitObjc3IRDirectFunctionCall(
    const Expr *expr, const LoweredFunctionSignature *signature,
    FunctionContext &ctx,
    const Objc3IRDirectCallEmissionCallbacks &callbacks,
    const std::string &throws_error_slot_ptr,
    bool *bridge_failed_out = nullptr,
    std::string *bridge_error_value_out = nullptr,
    std::string *bridge_failure_condition_out = nullptr);
