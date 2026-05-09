#include "ir/objc3_ir_expression_emission.h"

#include <cstddef>
#include <string>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_block_runtime_contracts.h"

namespace {

std::string EmitObjc3IRExprImpl(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionEmissionCallbacks &callbacks) {
  if (expr == nullptr) {
    return callbacks.emit_unsupported_i32_value(
        "null expression reached IR lowering");
  }
  switch (expr->kind) {
    case Expr::Kind::Number:
      return std::to_string(expr->number);
    case Expr::Kind::BoolLiteral:
      return expr->bool_value ? "1" : "0";
    case Expr::Kind::NilLiteral:
      return "0";
    case Expr::Kind::BlockLiteral:
      if (BlockLiteralSupportsEscapingRuntimeHookLowering(*expr)) {
        const std::string storage_ptr =
            callbacks.emit_block_literal_storage(*expr, ctx);
        if (storage_ptr == "poison") {
          return storage_ptr;
        }
        return callbacks.emit_promoted_block_handle(*expr, storage_ptr, ctx);
      }
      return callbacks.emit_unsupported_i32_value(
          "block literal values must be bound to a local name before use");
    case Expr::Kind::Identifier: {
      if (expr->typed_keypath_literal_enabled) {
        return callbacks.emit_typed_keypath_literal_value(*expr);
      }
      return callbacks.emit_identifier_value(expr->ident, ctx);
    }
    case Expr::Kind::Binary: {
      if (expr->op == "&&" || expr->op == "||") {
        const std::string lhs = EmitObjc3IRExprImpl(expr->left.get(), ctx,
                                                    callbacks);
        const std::string lhs_i1 = callbacks.new_temp(ctx);
        const std::string rhs_label = callbacks.new_label(
            ctx, expr->op == "&&" ? "and_rhs_" : "or_rhs_");
        const std::string rhs_done_label = callbacks.new_label(
            ctx, expr->op == "&&" ? "and_rhs_done_" : "or_rhs_done_");
        const std::string short_label = callbacks.new_label(
            ctx, expr->op == "&&" ? "and_short_" : "or_short_");
        const std::string merge_label = callbacks.new_label(
            ctx, expr->op == "&&" ? "and_merge_" : "or_merge_");
        const std::string rhs_i1 = callbacks.new_temp(ctx);
        const std::string logical_i1 = callbacks.new_temp(ctx);
        const std::string out_i32 = callbacks.new_temp(ctx);
        const std::string short_value = expr->op == "&&" ? "0" : "1";

        ctx.code_lines.push_back("  " + lhs_i1 + " = icmp ne i32 " + lhs +
                                 ", 0");
        if (expr->op == "&&") {
          ctx.code_lines.push_back("  br i1 " + lhs_i1 + ", label %" +
                                   rhs_label + ", label %" + short_label);
        } else {
          ctx.code_lines.push_back("  br i1 " + lhs_i1 + ", label %" +
                                   short_label + ", label %" + rhs_label);
        }

        ctx.code_lines.push_back(rhs_label + ":");
        const std::string rhs = EmitObjc3IRExprImpl(expr->right.get(), ctx,
                                                    callbacks);
        ctx.code_lines.push_back("  br label %" + rhs_done_label);
        ctx.code_lines.push_back(rhs_done_label + ":");
        ctx.code_lines.push_back("  " + rhs_i1 + " = icmp ne i32 " + rhs +
                                 ", 0");
        ctx.code_lines.push_back("  br label %" + merge_label);

        ctx.code_lines.push_back(short_label + ":");
        ctx.code_lines.push_back("  br label %" + merge_label);

        ctx.code_lines.push_back(merge_label + ":");
        ctx.code_lines.push_back("  " + logical_i1 + " = phi i1 [" +
                                 short_value + ", %" + short_label + "], [" +
                                 rhs_i1 + ", %" + rhs_done_label + "]");
        ctx.code_lines.push_back("  " + out_i32 + " = zext i1 " +
                                 logical_i1 + " to i32");
        return out_i32;
      }
      if (expr->op == "??") {
        const std::string lhs =
            EmitObjc3IRExprImpl(expr->left.get(), ctx, callbacks);
        const std::string lhs_i1 = callbacks.new_temp(ctx);
        const std::string rhs_label =
            callbacks.new_label(ctx, "coalesce_rhs_");
        const std::string lhs_label =
            callbacks.new_label(ctx, "coalesce_lhs_");
        const std::string merge_label =
            callbacks.new_label(ctx, "coalesce_merge_");
        const std::string rhs_value_name = callbacks.new_temp(ctx);
        const std::string out_value = callbacks.new_temp(ctx);
        ctx.code_lines.push_back("  " + lhs_i1 + " = icmp ne i32 " + lhs +
                                 ", 0");
        ctx.code_lines.push_back("  br i1 " + lhs_i1 + ", label %" +
                                 lhs_label + ", label %" + rhs_label);
        ctx.code_lines.push_back(rhs_label + ":");
        const std::string rhs =
            EmitObjc3IRExprImpl(expr->right.get(), ctx, callbacks);
        ctx.code_lines.push_back("  " + rhs_value_name + " = add i32 " +
                                 rhs + ", 0");
        ctx.code_lines.push_back("  br label %" + merge_label);
        ctx.code_lines.push_back(lhs_label + ":");
        ctx.code_lines.push_back("  br label %" + merge_label);
        ctx.code_lines.push_back(merge_label + ":");
        ctx.code_lines.push_back("  " + out_value + " = phi i32 [" + lhs +
                                 ", %" + lhs_label + "], [" +
                                 rhs_value_name + ", %" + rhs_label + "]");
        return out_value;
      }

      const std::string lhs =
          EmitObjc3IRExprImpl(expr->left.get(), ctx, callbacks);
      const std::string rhs =
          EmitObjc3IRExprImpl(expr->right.get(), ctx, callbacks);
      if (expr->op == "+" || expr->op == "-" || expr->op == "*" ||
          expr->op == "/" || expr->op == "%") {
        const std::string tmp = callbacks.new_temp(ctx);
        std::string op = "add";
        if (expr->op == "+") {
          op = "add";
        } else if (expr->op == "-") {
          op = "sub";
        } else if (expr->op == "*") {
          op = "mul";
        } else if (expr->op == "/") {
          op = "sdiv";
        } else if (expr->op == "%") {
          op = "srem";
        }
        ctx.code_lines.push_back("  " + tmp + " = " + op + " i32 " + lhs +
                                 ", " + rhs);
        return tmp;
      }

      if (expr->op == "&" || expr->op == "|" || expr->op == "^" ||
          expr->op == "<<" || expr->op == ">>") {
        const std::string tmp = callbacks.new_temp(ctx);
        std::string op = "and";
        if (expr->op == "&") {
          op = "and";
        } else if (expr->op == "|") {
          op = "or";
        } else if (expr->op == "^") {
          op = "xor";
        } else if (expr->op == "<<") {
          op = "shl";
        } else if (expr->op == ">>") {
          op = "ashr";
        }
        ctx.code_lines.push_back("  " + tmp + " = " + op + " i32 " + lhs +
                                 ", " + rhs);
        return tmp;
      }

      std::string pred;
      if (expr->op == "==") {
        pred = "eq";
      } else if (expr->op == "!=") {
        pred = "ne";
      } else if (expr->op == "<") {
        pred = "slt";
      } else if (expr->op == "<=") {
        pred = "sle";
      } else if (expr->op == ">") {
        pred = "sgt";
      } else if (expr->op == ">=") {
        pred = "sge";
      } else {
        return callbacks.emit_unsupported_i32_value(
            "unsupported binary operator '" + expr->op + "'");
      }
      const std::string cmp_i1 = callbacks.new_temp(ctx);
      const std::string out_i32 = callbacks.new_temp(ctx);
      ctx.code_lines.push_back("  " + cmp_i1 + " = icmp " + pred +
                               " i32 " + lhs + ", " + rhs);
      ctx.code_lines.push_back("  " + out_i32 + " = zext i1 " + cmp_i1 +
                               " to i32");
      return out_i32;
    }
    case Expr::Kind::Conditional: {
      const std::string cond_value =
          EmitObjc3IRExprImpl(expr->left.get(), ctx, callbacks);
      const std::string cond_i1 = callbacks.new_temp(ctx);
      const std::string true_label = callbacks.new_label(ctx, "cond_true_");
      const std::string false_label = callbacks.new_label(ctx, "cond_false_");
      const std::string merge_label = callbacks.new_label(ctx, "cond_merge_");
      const std::string result_ptr =
          "%cond.addr." + std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + result_ptr + " = alloca i32, align 4");
      ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " +
                               cond_value + ", 0");
      ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" +
                               true_label + ", label %" + false_label);

      ctx.code_lines.push_back(true_label + ":");
      const std::string true_value =
          EmitObjc3IRExprImpl(expr->right.get(), ctx, callbacks);
      ctx.code_lines.push_back("  store i32 " + true_value + ", ptr " +
                               result_ptr + ", align 4");
      ctx.code_lines.push_back("  br label %" + merge_label);

      ctx.code_lines.push_back(false_label + ":");
      const std::string false_value =
          EmitObjc3IRExprImpl(expr->third.get(), ctx, callbacks);
      ctx.code_lines.push_back("  store i32 " + false_value + ", ptr " +
                               result_ptr + ", align 4");
      ctx.code_lines.push_back("  br label %" + merge_label);

      ctx.code_lines.push_back(merge_label + ":");
      const std::string out_value = callbacks.new_temp(ctx);
      ctx.code_lines.push_back("  " + out_value + " = load i32, ptr " +
                               result_ptr + ", align 4");
      return out_value;
    }
    case Expr::Kind::Call: {
      if (expr->ident == "__objc3_throw_stmt") {
        const std::string error_value =
            expr->args.empty()
                ? "1"
                : EmitObjc3IRExprImpl(expr->args.front().get(), ctx,
                                      callbacks);
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
    case Expr::Kind::MessageSend: {
      return callbacks.emit_message_send_expr(expr, ctx);
    }
  }
  return callbacks.emit_unsupported_i32_value(
      "unsupported expression kind reached IR lowering");
}

}  // namespace

std::string EmitObjc3IRExpr(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionEmissionCallbacks &callbacks) {
  return EmitObjc3IRExprImpl(expr, ctx, callbacks);
}
