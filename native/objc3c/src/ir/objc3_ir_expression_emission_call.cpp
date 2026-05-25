#include "ir/objc3_ir_expression_emission_call.h"

#include <cstddef>
#include <string>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_expression_emission.h"
#include "ir/objc3_ir_function_signature_model.h"
#include "ir/objc3_ir_type_model.h"

std::string EmitObjc3IRCallExpression(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionEmissionCallbacks &callbacks,
    const Objc3IRExpressionChildEmitter &emit_child_expr) {
  if (expr->kind == Expr::Kind::Throw) {
    const std::string error_value =
        expr->args.empty()
            ? "1"
            : emit_child_expr(expr->args.front().get(), ctx, callbacks);
    callbacks.emit_propagate_thrown_error(error_value, ctx);
    return "0";
  }
  if (expr->kind == Expr::Kind::Try) {
    const Expr *operand =
        !expr->args.empty() ? expr->args.front().get() : expr->left.get();
    if (operand == nullptr ||
        (operand->kind != Expr::Kind::Call &&
         operand->kind != Expr::Kind::MessageSend)) {
      return callbacks.emit_unsupported_i32_value(
          "try lowering expected callable operand");
    }
    LoweredFunctionSignature resolved_message_send_signature;
    const LoweredFunctionSignature *operand_signature = nullptr;
    if (operand->kind == Expr::Kind::MessageSend) {
      if (callbacks.resolve_message_send_signature &&
          callbacks.resolve_message_send_signature(
              operand, ctx, resolved_message_send_signature)) {
        operand_signature = &resolved_message_send_signature;
      }
    } else {
      operand_signature = callbacks.lookup_function_signature(operand->ident);
    }
    if (operand_signature == nullptr) {
      return callbacks.emit_unsupported_i32_value(
          "try lowering requires declared callable signature");
    }
    if (operand_signature->has_value_optional_type_signature &&
        !operand_signature->value_optional_lowering_supported) {
      const std::string payload =
          operand_signature->value_optional_payload_type_spelling.empty()
              ? "unknown"
              : operand_signature->value_optional_payload_type_spelling;
      return callbacks.emit_unsupported_i32_value(
          "try lowering for value optional payload " + payload +
          " requires value optional ABI support");
    }
    const ValueType result_type = operand_signature->return_type;
    if (result_type == ValueType::Void) {
      return callbacks.emit_unsupported_i32_value(
          "try expression lowering requires a non-void result");
    }
    const Objc3IRValueOptionalCarrierKind result_optional_carrier =
        Objc3IRValueOptionalCarrierKindFor(
            operand_signature->return_value_optional_carrier);
    const std::string result_storage_type =
        LLVMLocalStorageTypeForValueOptionalCarrier(result_type,
                                                   result_optional_carrier);
    const unsigned result_storage_alignment =
        LLVMLocalStorageAlignmentForValueOptionalCarrier(
            result_type, result_optional_carrier);
    const std::string result_ptr =
        "%try.result.addr." + std::to_string(ctx.temp_counter++);
    const std::string error_slot =
        callbacks.build_throws_error_slot_alloca(ctx, "try");
    ctx.entry_lines.push_back("  " + result_ptr + " = alloca " +
                              result_storage_type + ", align " +
                              std::to_string(result_storage_alignment));
    const std::string merged_label =
        callbacks.new_label(ctx, "try_merge_");
    const std::string failure_label =
        callbacks.new_label(ctx, "try_fail_");
    const std::string success_label =
        callbacks.new_label(ctx, "try_success_");
    ctx.code_lines.push_back("  store i32 0, ptr " + error_slot +
                             ", align 4");
    bool bridge_failed = false;
    std::string bridge_error_value = "0";
    std::string bridge_failure_condition;
    std::string result;
    if (operand->kind == Expr::Kind::MessageSend) {
      const std::string previous_error_slot =
          ctx.active_message_send_error_out_slot;
      const Expr *previous_error_expr =
          ctx.active_message_send_error_out_expr;
      if (operand_signature->throws_error_out_abi_ready) {
        ctx.active_message_send_error_out_slot = error_slot;
        ctx.active_message_send_error_out_expr = operand;
      }
      result = callbacks.emit_message_send_expr(operand, ctx);
      ctx.active_message_send_error_out_slot = previous_error_slot;
      ctx.active_message_send_error_out_expr = previous_error_expr;
    } else {
      result = callbacks.emit_direct_function_call(
          operand, operand_signature, ctx, error_slot, &bridge_failed,
          &bridge_error_value, &bridge_failure_condition);
    }
    std::string actual_result = result;
    std::string failure_cond;
    if (bridge_failed) {
      if (bridge_failure_condition.empty()) {
        return callbacks.emit_unsupported_i32_value(
            "try lowering received bridged operand without failure condition");
      }
      failure_cond = bridge_failure_condition;
    } else if (operand_signature->throws_error_out_abi_ready) {
      const std::string has_error = callbacks.new_temp(ctx);
      const std::string loaded_error =
          callbacks.emit_load_thrown_error(error_slot, ctx);
      ctx.code_lines.push_back("  " + has_error + " = icmp ne i32 " +
                               loaded_error + ", 0");
      failure_cond = has_error;
      bridge_error_value = loaded_error;
    } else {
      return callbacks.emit_unsupported_i32_value(
          "try lowering requires throwing or bridged operand");
    }
    ctx.code_lines.push_back("  br i1 " + failure_cond + ", label %" +
                             failure_label + ", label %" + success_label);
    ctx.code_lines.push_back(success_label + ":");
    ctx.code_lines.push_back("  store " + result_storage_type + " " +
                             actual_result + ", ptr " + result_ptr +
                             ", align " +
                             std::to_string(result_storage_alignment));
    ctx.code_lines.push_back("  br label %" + merged_label);
    ctx.code_lines.push_back(failure_label + ":");
    switch (expr->try_operator_kind) {
      case Expr::TryOperatorKind::Optional:
        ctx.code_lines.push_back("  store " + result_storage_type +
                                 " " +
                                 std::string(LLVMZeroValueForValueOptionalCarrier(
                                     result_type, result_optional_carrier)) +
                                 ", ptr " + result_ptr + ", align " +
                                 std::to_string(result_storage_alignment));
        ctx.code_lines.push_back("  br label %" + merged_label);
        break;
      case Expr::TryOperatorKind::Forced:
        ctx.code_lines.push_back("  call void @abort()");
        ctx.code_lines.push_back("  unreachable");
        break;
      case Expr::TryOperatorKind::Propagate:
        callbacks.emit_propagate_thrown_error(bridge_error_value, ctx);
        break;
      case Expr::TryOperatorKind::None:
        ctx.code_lines.push_back("  store " + result_storage_type + " " +
                                 actual_result + ", ptr " + result_ptr +
                                 ", align " +
                                 std::to_string(result_storage_alignment));
        ctx.code_lines.push_back("  br label %" + merged_label);
        break;
    }
    if (expr->try_operator_kind == Expr::TryOperatorKind::Forced ||
        expr->try_operator_kind == Expr::TryOperatorKind::Propagate) {
      // The failure arm terminates, but the success arm still flows
      // through the merged result block.
      ctx.terminated = false;
    }
    ctx.code_lines.push_back(merged_label + ":");
    const std::string loaded = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + loaded + " = load " +
                             result_storage_type + ", ptr " + result_ptr +
                             ", align " +
                             std::to_string(result_storage_alignment));
    if (result_type == ValueType::Optional) {
      ctx.value_optional_carrier_by_value[loaded] = result_optional_carrier;
    }
    return loaded;
  }
  const auto local_block_it = ctx.block_bindings.find(expr->ident);
  if (local_block_it != ctx.block_bindings.end()) {
    return callbacks.emit_block_invoke_call(local_block_it->second, expr,
                                            ctx);
  }
  const LoweredFunctionSignature *signature =
      callbacks.lookup_function_signature(expr->ident);
  if (signature != nullptr && signature->has_value_optional_type_signature &&
      !signature->value_optional_lowering_supported) {
    const std::string payload =
        signature->value_optional_payload_type_spelling.empty()
            ? "unknown"
            : signature->value_optional_payload_type_spelling;
    return callbacks.emit_unsupported_i32_value(
        "value optional call lowering for payload " + payload +
        " requires value optional ABI support");
  }
  if (signature != nullptr && signature->throws_error_out_abi_ready) {
    const std::string ignored_error_slot =
        callbacks.build_throws_error_slot_alloca(ctx, "ignored");
    ctx.code_lines.push_back("  store i32 0, ptr " + ignored_error_slot +
                             ", align 4");
    return callbacks.emit_direct_function_call(
        expr, signature, ctx, ignored_error_slot, nullptr, nullptr, nullptr);
  }
  // implementation anchor: supported await-marked expressions reach native IR
  // through direct-call lowering, where executor-affined async contexts
  // materialize the private continuation helper handoff. Unsupported await
  // surfaces fail closed from that direct-call path.
  return callbacks.emit_direct_function_call(expr, signature, ctx, "",
                                             nullptr, nullptr, nullptr);
}
