#pragma once

#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_scope_cleanup_emission.h"

struct Expr;
struct Stmt;

struct Objc3IRBlockLoweringState {
  std::vector<std::string> *block_function_definitions = nullptr;
  std::unordered_set<std::string> *emitted_block_invoke_symbols = nullptr;
  std::unordered_set<std::string> *emitted_block_descriptor_symbols = nullptr;
  std::unordered_set<std::string> *emitted_block_copy_helper_symbols = nullptr;
  std::unordered_set<std::string> *emitted_block_dispose_helper_symbols =
      nullptr;
};

struct Objc3IRBlockLoweringCallbacks {
  std::function<std::string(const std::string &reason)>
      emit_unsupported_i32_value;
  std::function<std::string(const Expr *expr, FunctionContext &ctx)> emit_expr;
  std::function<void(const Stmt *stmt, FunctionContext &ctx)> emit_statement;
  std::function<std::string(const FunctionContext &ctx,
                            const std::string &name)>
      lookup_var_ptr;
  std::function<std::string(const std::string &name, FunctionContext &ctx)>
      emit_identifier_value;
};

struct Objc3IRBlockLoweringContext {
  Objc3IRBlockLoweringState state;
  Objc3IRScopeCleanupEmissionCallbacks scope_cleanup_callbacks;
  Objc3IRBlockLoweringCallbacks callbacks;
  std::string current_implementation_name;
  std::string current_superclass_name;
  bool current_method_is_class_method = false;
};

std::string EmitObjc3IRPromotedBlockHandle(
    const Expr &expr, const std::string &storage_ptr, FunctionContext &ctx,
    const Objc3IRBlockLoweringContext &lowering_context,
    bool allow_nonescaping_scalar_promotion = false);

std::string EmitObjc3IRPromotedBlockHandleLoad(
    BlockBinding &binding, FunctionContext &ctx,
    const Objc3IRBlockLoweringContext &lowering_context);

std::string EmitObjc3IRBlockLiteralStorage(
    const Expr &expr, FunctionContext &ctx,
    const Objc3IRBlockLoweringContext &lowering_context);

std::string EmitObjc3IRBlockInvokeCall(
    const BlockBinding &binding, const Expr *call_expr, FunctionContext &ctx,
    const Objc3IRBlockLoweringContext &lowering_context);
