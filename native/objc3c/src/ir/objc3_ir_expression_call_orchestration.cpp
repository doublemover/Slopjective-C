#include "ir/objc3_ir_expression_call_orchestration.h"

#include <string>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_concurrency_runtime_call_emission.h"
#include "ir/objc3_ir_direct_call_emission.h"
#include "ir/objc3_ir_expression_emission.h"
#include "ir/objc3_ir_function_effect_analysis.h"
#include "ir/objc3_ir_function_local_flow.h"
#include "ir/objc3_ir_message_send_emission.h"

LoweredFunctionSignature BuildObjc3IRLoweredSignatureFromDirectDispatch(
    const Objc3IRDirectDispatchSignature &direct_signature) {
  LoweredFunctionSignature lowered;
  lowered.return_type = direct_signature.return_type;
  lowered.param_types = direct_signature.param_types;
  lowered.return_value_optional_carrier =
      direct_signature.return_value_optional_carrier;
  lowered.param_value_optional_carriers =
      direct_signature.param_value_optional_carriers;
  lowered.throws_declared = direct_signature.throws_declared;
  lowered.typed_throws_declared = direct_signature.typed_throws_declared;
  lowered.throws_error_out_abi_ready =
      direct_signature.throws_error_out_abi_ready;
  lowered.typed_throws_error_type_spelling =
      direct_signature.typed_throws_error_type_spelling;
  lowered.has_value_optional_type_signature =
      direct_signature.has_value_optional_type_signature;
  lowered.value_optional_lowering_supported =
      direct_signature.value_optional_lowering_supported;
  lowered.value_optional_payload_type_spelling =
      direct_signature.value_optional_payload_type_spelling;
  return lowered;
}

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
    std::string *bridge_failure_condition_out,
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
          options.services.emit_unsupported_i32_value,
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
          [&options](FunctionContext &callback_ctx) {
            EmitObjc3IRFunctionLocalTerminalCleanupToDepth(
                callback_ctx, 0u, 0u, 0u, 0u, 0u,
                options.services.build_function_local_flow_context());
          },
          options.services.lookup_function_signature},
      throws_error_slot_ptr, bridge_failed_out, bridge_error_value_out,
      bridge_failure_condition_out);
}

std::string EmitObjc3IRExpressionCallMessageSendExpr(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionCallEmissionOptions &options) {
  return EmitObjc3IRMessageSendExpr(
      expr, ctx,
      Objc3IRMessageSendEmissionOptions{
          options.selector_pool_globals,
          options.runtime_string_pool_globals,
          options.class_receiver_constants,
          options.direct_dispatch_symbols_by_key,
          options.direct_dispatch_signatures_by_key,
          options.runtime_dispatch_return_types_by_key,
          options.runtime_dispatch_return_value_optional_carriers_by_key,
          options.runtime_dispatch_superclass_by_name,
          options.runtime_dispatch_arg_slots,
          options.runtime_dispatch_symbol,
          options.runtime_dispatch_call_state,
          options.arc_mode_enabled},
      Objc3IRMessageSendEmissionCallbacks{
          [&options](const Expr *arg_expr, FunctionContext &callback_ctx) {
            return EmitObjc3IRExpressionCallImpl(arg_expr, callback_ctx,
                                                 options);
          },
          options.services.emit_identifier_value,
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

bool TryResolveObjc3IRExpressionCallMessageSendSignature(
    const Expr *expr, const FunctionContext &ctx,
    const Objc3IRExpressionCallEmissionOptions &options,
    LoweredFunctionSignature &signature_out) {
  Objc3IRDirectDispatchSignature direct_signature;
  if (TryResolveObjc3IRDirectDispatchSignature(
          expr, ctx,
          Objc3IRMessageSendEmissionOptions{
              options.selector_pool_globals,
              options.runtime_string_pool_globals,
              options.class_receiver_constants,
              options.direct_dispatch_symbols_by_key,
              options.direct_dispatch_signatures_by_key,
              options.runtime_dispatch_return_types_by_key,
              options.runtime_dispatch_return_value_optional_carriers_by_key,
              options.runtime_dispatch_superclass_by_name,
              options.runtime_dispatch_arg_slots,
              options.runtime_dispatch_symbol,
              options.runtime_dispatch_call_state,
              options.arc_mode_enabled},
          &direct_signature, nullptr)) {
    signature_out =
        BuildObjc3IRLoweredSignatureFromDirectDispatch(direct_signature);
    return true;
  }
  const LoweredFunctionSignature *selector_signature =
      options.services.lookup_function_signature(expr != nullptr
                                                     ? expr->selector
                                                     : std::string{});
  if (selector_signature == nullptr) {
    return false;
  }
  signature_out = *selector_signature;
  return true;
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
          [&options](const Expr *message_expr,
                     const FunctionContext &callback_ctx,
                     LoweredFunctionSignature &signature_out) {
            return TryResolveObjc3IRExpressionCallMessageSendSignature(
                message_expr, callback_ctx, options, signature_out);
          },
          [&options](const Expr *call_expr,
                     const LoweredFunctionSignature *signature,
                     FunctionContext &callback_ctx,
                     const std::string &throws_error_slot_ptr,
                     bool *bridge_failed_out,
                     std::string *bridge_error_value_out,
                     std::string *bridge_failure_condition_out) {
            return EmitObjc3IRExpressionCallDirectFunctionCall(
                call_expr, signature, callback_ctx, throws_error_slot_ptr,
                bridge_failed_out, bridge_error_value_out,
                bridge_failure_condition_out, options);
          },
          BuildObjc3IRThrowsErrorSlotAlloca,
          EmitObjc3IRLoadThrownError,
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
