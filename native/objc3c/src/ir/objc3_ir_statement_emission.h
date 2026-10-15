#pragma once

#include <cstddef>
#include <functional>
#include <string>

#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_function_signature_model.h"

struct Expr;
struct Stmt;

struct Objc3IRStatementEmissionCallbacks {
  std::function<std::string(const Expr *expr, FunctionContext &ctx)> emit_expr;
  std::function<std::string(const Expr &expr, FunctionContext &ctx)>
      emit_block_literal_storage;
  std::function<std::string(FunctionContext &ctx)> new_temp;
  std::function<std::string(FunctionContext &ctx, const std::string &prefix)>
      new_label;
  std::function<std::string(const FunctionContext &ctx,
                            const std::string &name)>
      lookup_var_ptr;
  std::function<bool(const Expr *expr, const FunctionContext &ctx, int &value)>
      try_get_compile_time_i32_expr;
  std::function<bool(const Expr *expr, const FunctionContext &ctx)>
      is_compile_time_nil_receiver_expr;
  std::function<const LoweredFunctionSignature *(const std::string &name)>
      lookup_function_signature;
  std::function<std::string(const std::string &reason)>
      emit_unsupported_i32_value;
  std::function<void(FunctionContext &ctx)> push_scope;
  std::function<void(FunctionContext &ctx, bool emit_cleanup)> pop_scope;
  std::function<void(FunctionContext &ctx, std::size_t target_depth)>
      emit_autoreleasepool_unwind_to_depth;
  std::function<void(const std::string &i32_value, FunctionContext &ctx)>
      emit_typed_return;
  std::function<void(FunctionContext &ctx, std::size_t scope_depth,
                     std::size_t autoreleasepool_depth,
                     std::size_t pending_block_dispose_depth,
                     std::size_t ownership_cleanup_depth,
                     std::size_t arc_cleanup_depth)>
      emit_terminal_cleanup_to_depth;
  std::function<std::string(FunctionContext &ctx, const std::string &prefix)>
      build_throws_error_slot_alloca;
  std::function<std::string(const std::string &slot, FunctionContext &ctx)>
      emit_load_thrown_error;
  std::function<void(const std::string &error_value, FunctionContext &ctx)>
      emit_propagate_thrown_error;
};

void EmitObjc3IRStatement(
    const Stmt *stmt, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks);
