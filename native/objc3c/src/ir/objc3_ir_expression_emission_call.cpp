#include "ir/objc3_ir_expression_emission_call.h"

#include <cstddef>
#include <string>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_expression_emission.h"
#include "ir/objc3_ir_function_signature_model.h"

std::string EmitObjc3IRCallExpression(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionEmissionCallbacks &callbacks,
    const Objc3IRExpressionChildEmitter &emit_child_expr) {
  if (expr->ident == "__objc3_throw_stmt") {
    const std::string error_value =
        expr->args.empty()
            ? "1"
            : emit_child_expr(expr->args.front().get(), ctx, callbacks);
    callbacks.emit_propagate_thrown_error(error_value, ctx);
    return "0";
  }
  if (expr->ident == "__objc3_try_expr") {
    const Expr *operand =
        !expr->args.empty() ? expr->args.front().get() : expr->left.get();
    if (operand == nullptr || operand->kind != Expr::Kind::Call) {
      return callbacks.emit_unsupported_i32_value(
          "try lowering expected callable operand");
    }
    const LoweredFunctionSignature *operand_signature =
        callbacks.lookup_function_signature(operand->ident);
    if (operand_signature == nullptr) {
      return callbacks.emit_unsupported_i32_value(
          "try lowering requires declared callable signature");
    }
    const std::string result_ptr =
        "%try.result.addr." + std::to_string(ctx.temp_counter++);
    const std::string error_slot =
        callbacks.build_throws_error_slot_alloca(ctx, "try");
    ctx.entry_lines.push_back("  " + result_ptr +
                              " = alloca i32, align 4");
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
    std::string result = callbacks.emit_direct_function_call(
        operand, operand_signature, ctx, error_slot, &bridge_failed,
        &bridge_error_value);
    std::string actual_result = result;
    std::string failure_cond;
    if (bridge_failed) {
      const std::size_t marker = result.rfind('|');
      actual_result = result.substr(0, marker);
      failure_cond = result.substr(marker + 1);
    } else if (operand_signature->throws_declared) {
      const std::string loaded_error = callbacks.new_temp(ctx);
      const std::string has_error = callbacks.new_temp(ctx);
      ctx.code_lines.push_back("  " + loaded_error +
                               " = load i32, ptr " + error_slot +
                               ", align 4");
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
    ctx.code_lines.push_back("  store i32 " + actual_result + ", ptr " +
                             result_ptr + ", align 4");
    ctx.code_lines.push_back("  br label %" + merged_label);
    ctx.code_lines.push_back(failure_label + ":");
    switch (expr->try_operator_kind) {
      case Expr::TryOperatorKind::Optional:
        ctx.code_lines.push_back("  store i32 0, ptr " + result_ptr +
                                 ", align 4");
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
        ctx.code_lines.push_back("  store i32 " + actual_result +
                                 ", ptr " + result_ptr + ", align 4");
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
    ctx.code_lines.push_back("  " + loaded + " = load i32, ptr " +
                             result_ptr + ", align 4");
    return loaded;
  }
  const auto local_block_it = ctx.block_bindings.find(expr->ident);
  if (local_block_it != ctx.block_bindings.end()) {
    return callbacks.emit_block_invoke_call(local_block_it->second, expr,
                                            ctx);
  }
  const LoweredFunctionSignature *signature =
      callbacks.lookup_function_signature(expr->ident);
  if (signature != nullptr && signature->throws_declared) {
    const std::string ignored_error_slot =
        callbacks.build_throws_error_slot_alloca(ctx, "ignored");
    ctx.code_lines.push_back("  store i32 0, ptr " + ignored_error_slot +
                             ", align 4");
    return callbacks.emit_direct_function_call(
        expr, signature, ctx, ignored_error_slot, nullptr, nullptr);
  }
  // implementation anchor: supported await-marked expressions
  // currently reach native IR through the operand's direct-call lowering
  // path. This emits runnable IR/object code for the non-suspending happy
  // slice without materializing continuation allocation or a state
  // machine; those surfaces remain later work.
  return callbacks.emit_direct_function_call(expr, signature, ctx, "",
                                             nullptr, nullptr);
}
