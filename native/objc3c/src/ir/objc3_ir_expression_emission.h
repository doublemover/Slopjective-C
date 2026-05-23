#pragma once

#include <functional>
#include <string>

#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_function_signature_model.h"

struct Expr;

struct Objc3IRExpressionEmissionCallbacks {
  std::function<std::string(FunctionContext &ctx)> new_temp;
  std::function<std::string(FunctionContext &ctx, const std::string &prefix)>
      new_label;
  std::function<std::string(const std::string &reason)>
      emit_unsupported_i32_value;
  std::function<std::string(const std::string &name, FunctionContext &ctx)>
      emit_identifier_value;
  std::function<std::string(const Expr &expr)> emit_typed_keypath_literal_value;
  std::function<std::string(const Expr &expr, FunctionContext &ctx)>
      emit_block_literal_storage;
  std::function<std::string(const Expr &expr,
                            const std::string &storage_ptr,
                            FunctionContext &ctx)>
      emit_promoted_block_handle;
  std::function<std::string(const BlockBinding &binding, const Expr *call_expr,
                            FunctionContext &ctx)>
      emit_block_invoke_call;
  std::function<const LoweredFunctionSignature *(const std::string &name)>
      lookup_function_signature;
  std::function<std::string(const Expr *expr,
                            const LoweredFunctionSignature *signature,
                            FunctionContext &ctx,
                            const std::string &throws_error_slot_ptr,
                            bool *bridge_failed_out,
                            std::string *bridge_error_value_out,
                            std::string *bridge_failure_condition_out)>
      emit_direct_function_call;
  std::function<std::string(FunctionContext &ctx, const std::string &prefix)>
      build_throws_error_slot_alloca;
  std::function<std::string(const std::string &slot, FunctionContext &ctx)>
      emit_load_thrown_error;
  std::function<void(const std::string &error_value, FunctionContext &ctx)>
      emit_propagate_thrown_error;
  std::function<std::string(const Expr *expr, FunctionContext &ctx)>
      emit_message_send_expr;
};

std::string EmitObjc3IRExpr(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionEmissionCallbacks &callbacks);
