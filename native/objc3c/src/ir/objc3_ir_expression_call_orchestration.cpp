#include "ir/objc3_ir_expression_call_orchestration.h"

#include <string>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_concurrency_runtime_call_emission.h"
#include "ir/objc3_ir_direct_call_emission.h"
#include "ir/objc3_ir_expression_emission.h"
#include "ir/objc3_ir_function_effect_analysis.h"
#include "ir/objc3_ir_message_send_emission.h"

namespace {

std::string EmitObjc3IRExpressionCallImpl(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionCallEmissionOptions &options);

bool TryEmitObjc3IRExpressionCallConcurrencyActorLoweringCall(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionCallEmissionOptions &options,
    std::string &result_out) {
  Objc3IRConcurrencyRuntimeCallEmissionCallbacks callbacks{
      options.services.new_temp,
      [&ctx, &options](const Expr *arg) {
        return EmitObjc3IRExpressionCallImpl(arg, ctx, options);
      },
      options.services.invalidate_global_proof_state};
  return TryEmitObjc3IRConcurrencyActorLoweringCall(
      expr, ctx, callbacks, result_out);
}

bool TryEmitObjc3IRExpressionCallConcurrencyTaskRuntimeLoweringCall(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionCallEmissionOptions &options,
    std::string &result_out) {
  Objc3IRConcurrencyRuntimeCallEmissionCallbacks callbacks{
      options.services.new_temp,
      [&ctx, &options](const Expr *arg) {
        return EmitObjc3IRExpressionCallImpl(arg, ctx, options);
      },
      options.services.invalidate_global_proof_state};
  return TryEmitObjc3IRConcurrencyTaskRuntimeLoweringCall(
      expr, ctx, callbacks, result_out);
}

std::string EmitObjc3IRExpressionCallDirectFunctionCall(
    const Expr *expr, const LoweredFunctionSignature *signature,
    FunctionContext &ctx, const std::string &throws_error_slot_ptr,
    bool *bridge_failed_out,
    std::string *bridge_error_value_out,
    const Objc3IRExpressionCallEmissionOptions &options) {
  return EmitObjc3IRDirectFunctionCall(
      expr, signature, ctx,
      Objc3IRDirectCallEmissionCallbacks{
          [&ctx, &options](const Expr *arg_expr) {
            return EmitObjc3IRExpressionCallImpl(arg_expr, ctx, options);
          },
          options.services.new_temp,
          CoerceObjc3IRI32ToBoolI1,
          CoerceObjc3IRValueToI32,
          [&options](const std::string &function_name) {
            return Objc3IRFunctionMayHaveGlobalSideEffects(
                function_name, options.defined_functions,
                options.declared_pure_functions, options.impure_functions);
          },
          [&options](const Expr *call_expr, FunctionContext &callback_ctx,
                     std::string &result_out) {
            return TryEmitObjc3IRExpressionCallConcurrencyActorLoweringCall(
                call_expr, callback_ctx, options, result_out);
          },
          [&options](const Expr *call_expr, FunctionContext &callback_ctx,
                     std::string &result_out) {
            return TryEmitObjc3IRExpressionCallConcurrencyTaskRuntimeLoweringCall(
                call_expr, callback_ctx, options, result_out);
          },
          options.services.invalidate_global_proof_state,
          options.services.lookup_function_signature},
      throws_error_slot_ptr, bridge_failed_out, bridge_error_value_out);
}

std::string EmitObjc3IRExpressionCallMessageSendExpr(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionCallEmissionOptions &options) {
  return EmitObjc3IRMessageSendExpr(
      expr, ctx,
      Objc3IRMessageSendEmissionOptions{
          options.selector_pool_globals,
          options.class_receiver_constants,
          options.direct_dispatch_symbols_by_key,
          options.runtime_dispatch_arg_slots,
          options.runtime_dispatch_symbol,
          options.runtime_dispatch_call_state},
      Objc3IRMessageSendEmissionCallbacks{
          [&options](const Expr *arg_expr, FunctionContext &callback_ctx) {
            return EmitObjc3IRExpressionCallImpl(arg_expr, callback_ctx,
                                                 options);
          },
          options.services.new_temp,
          options.services.new_label,
          [&options](const Expr *receiver_expr,
                     const FunctionContext &callback_ctx) {
            return IsObjc3IRCompileTimeNilReceiverExprInContext(
                receiver_expr, callback_ctx,
                options.services.build_compile_time_proof_analysis_context());
          },
          [&options](const Expr *receiver_expr,
                     const FunctionContext &callback_ctx) {
            return IsObjc3IRCompileTimeKnownNonNilExprInContext(
                receiver_expr, callback_ctx,
                options.services.build_compile_time_proof_analysis_context());
          },
          options.services.emit_unsupported_i32_value,
          options.services.invalidate_global_proof_state});
}

std::string EmitObjc3IRExpressionCallImpl(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionCallEmissionOptions &options) {
  return EmitObjc3IRExpr(
      expr, ctx,
      Objc3IRExpressionEmissionCallbacks{
          options.services.new_temp,
          options.services.new_label,
          options.services.emit_unsupported_i32_value,
          options.services.emit_identifier_value,
          options.services.emit_typed_keypath_literal_value,
          [&options](const Expr &callback_expr, FunctionContext &callback_ctx) {
            return EmitObjc3IRBlockLiteralStorage(
                callback_expr, callback_ctx,
                options.services.build_block_lowering_context());
          },
          [&options](const Expr &callback_expr,
                     const std::string &storage_ptr,
                     FunctionContext &callback_ctx) {
            return EmitObjc3IRPromotedBlockHandle(
                callback_expr, storage_ptr, callback_ctx,
                options.services.build_block_lowering_context());
          },
          [&options](const BlockBinding &binding, const Expr *call_expr,
                     FunctionContext &callback_ctx) {
            return EmitObjc3IRBlockInvokeCall(
                binding, call_expr, callback_ctx,
                options.services.build_block_lowering_context());
          },
          options.services.lookup_function_signature,
          [&options](const Expr *call_expr,
                     const LoweredFunctionSignature *signature,
                     FunctionContext &callback_ctx,
                     const std::string &throws_error_slot_ptr,
                     bool *bridge_failed_out,
                     std::string *bridge_error_value_out) {
            return EmitObjc3IRExpressionCallDirectFunctionCall(
                call_expr, signature, callback_ctx, throws_error_slot_ptr,
                bridge_failed_out, bridge_error_value_out, options);
          },
          BuildObjc3IRThrowsErrorSlotAlloca,
          [&options](const std::string &error_value,
                     FunctionContext &callback_ctx) {
            EmitObjc3IRPropagateThrownError(
                error_value, callback_ctx,
                options.services.build_function_local_flow_context());
          },
          [&options](const Expr *message_expr, FunctionContext &callback_ctx) {
            return EmitObjc3IRExpressionCallMessageSendExpr(
                message_expr, callback_ctx, options);
          }});
}

}  // namespace

std::string EmitObjc3IRExpressionCall(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionCallEmissionOptions &options) {
  return EmitObjc3IRExpressionCallImpl(expr, ctx, options);
}
