#include "ir/objc3_ir_direct_call_emission.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_concurrency_runtime_call_emission.h"
#include "ir/objc3_ir_type_model.h"
#include "lower/contracts/concurrency_continuation_runtime_contracts.h"
#include "lower/contracts/error_handling_runtime_bridge_contracts.h"
#include "lower/contracts/ownership_runtime_memory_management_contracts.h"

namespace {

void AppendObjc3IRLoweredCallArg(
    std::vector<std::string> &args, const std::string &arg_i32,
    ValueType expected_type, FunctionContext &ctx,
    const Objc3IRDirectCallEmissionCallbacks &callbacks) {
  if (expected_type == ValueType::Bool) {
    const std::string arg_i1 = callbacks.coerce_i32_to_bool_i1(arg_i32, ctx);
    args.push_back("i1 " + arg_i1);
    return;
  }
  args.push_back("i32 " + arg_i32);
}

}  // namespace

std::string EmitObjc3IRDirectFunctionCall(
    const Expr *expr, const LoweredFunctionSignature *signature,
    FunctionContext &ctx,
    const Objc3IRDirectCallEmissionCallbacks &callbacks,
    const std::string &throws_error_slot_ptr, bool *bridge_failed_out,
    std::string *bridge_error_value_out,
    std::string *bridge_failure_condition_out) {
  if (expr != nullptr && expr->await_expression_enabled &&
      !ctx.async_runtime_helper_enabled) {
    return callbacks.emit_unsupported_i32_value(
        "await lowering requires async objc_executor affinity for continuation handoff");
  }
  if (expr != nullptr &&
      IsObjc3IRConcurrencyTaskRuntimeHelperName(expr->ident) &&
      !ctx.async_runtime_helper_enabled) {
    return callbacks.emit_unsupported_i32_value(
        "concurrency task runtime helper lowering requires async objc_executor affinity");
  }
  if (signature != nullptr && signature->has_value_optional_type_signature &&
      !signature->value_optional_lowering_supported) {
    const std::string payload =
        signature->value_optional_payload_type_spelling.empty()
            ? "unknown"
            : signature->value_optional_payload_type_spelling;
    return callbacks.emit_unsupported_i32_value(
        "value optional lowering is deferred for payload " + payload +
        "; no ABI emission or nullable-pointer erasure is allowed");
  }

  std::vector<std::string> args;
  std::vector<std::string> post_call_release_values;
  args.reserve(expr->args.size() +
               (signature != nullptr && signature->throws_error_out_abi_ready
                    ? 1u
                    : 0u));
  post_call_release_values.reserve(expr->args.size());
  for (std::size_t i = 0; i < expr->args.size(); ++i) {
    std::string arg_i32 = callbacks.emit_expr(expr->args[i].get());
    if (signature != nullptr && i < signature->param_insert_retain.size()) {
      if (signature->param_insert_retain[i]) {
        const std::string retained_value = callbacks.new_temp(ctx);
        ctx.code_lines.push_back("  " + retained_value + " = call i32 @" +
                                 std::string(kObjc3RuntimeRetainI32Symbol) +
                                 "(i32 " + arg_i32 + ")");
        arg_i32 = retained_value;
      }
      if (signature->param_insert_autorelease[i]) {
        const std::string autoreleased_value = callbacks.new_temp(ctx);
        ctx.code_lines.push_back(
            "  " + autoreleased_value + " = call i32 @" +
            std::string(kObjc3RuntimeAutoreleaseI32Symbol) + "(i32 " +
            arg_i32 + ")");
        arg_i32 = autoreleased_value;
      }
      if (signature->param_insert_release[i]) {
        post_call_release_values.push_back(arg_i32);
      }
    }
    const ValueType expected_type =
        signature != nullptr && i < signature->param_types.size()
            ? signature->param_types[i]
            : ValueType::I32;
    AppendObjc3IRLoweredCallArg(args, arg_i32, expected_type, ctx, callbacks);
  }
  if (signature != nullptr && signature->throws_error_out_abi_ready) {
    args.push_back("ptr " + throws_error_slot_ptr);
  }
  std::ostringstream arglist;
  for (std::size_t i = 0; i < args.size(); ++i) {
    if (i != 0) {
      arglist << ", ";
    }
    arglist << args[i];
  }
  const ValueType return_type =
      signature != nullptr ? signature->return_type : ValueType::I32;
  const std::string llvm_return_type = LLVMScalarType(return_type);
  const bool call_may_have_global_side_effects =
      callbacks.function_may_have_global_side_effects(expr->ident);
  std::string out = "0";
  if (callbacks.try_emit_concurrency_actor_lowering_call(expr, ctx, out)) {
    // lowering anchor: actor helper spellings inside actor methods
    // now lower through the private runtime helper slice rather than staying
    // as ordinary direct-call placeholders.
  } else if (callbacks.try_emit_concurrency_task_runtime_lowering_call(
                 expr, ctx, out)) {
    // lowering anchor: supported task/executor/cancellation
    // symbols now route through the private Part 7 runtime helper cluster
    // rather than remaining ordinary extern-call placeholders.
  } else if (return_type == ValueType::Void) {
    ctx.code_lines.push_back("  call " + llvm_return_type + " @" +
                             expr->ident + "(" + arglist.str() + ")");
  } else {
    const std::string tmp = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + tmp + " = call " + llvm_return_type +
                             " @" + expr->ident + "(" + arglist.str() + ")");
    out = callbacks.coerce_value_to_i32(tmp, return_type, ctx);
  }
  if (call_may_have_global_side_effects) {
    callbacks.invalidate_global_proof_state(ctx);
  }
  for (const auto &release_value : post_call_release_values) {
    const std::string released_value = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + released_value + " = call i32 @" +
                             std::string(kObjc3RuntimeReleaseI32Symbol) +
                             "(i32 " + release_value + ")");
    (void)released_value;
  }

  if (bridge_failed_out != nullptr) {
    *bridge_failed_out = false;
  }
  if (bridge_error_value_out != nullptr) {
    *bridge_error_value_out = "0";
  }
  if (bridge_failure_condition_out != nullptr) {
    *bridge_failure_condition_out = "";
  }

  const bool observe_bridge_failure = bridge_failed_out != nullptr ||
                                      bridge_error_value_out != nullptr ||
                                      bridge_failure_condition_out != nullptr;

  if (observe_bridge_failure && signature != nullptr &&
      signature->objc_status_code_declared) {
    const std::string is_success = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + is_success + " = icmp eq i32 " + out +
                             ", " +
                             std::to_string(
                                 signature->objc_status_code_success_literal));
    const std::string bridge_failed = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + bridge_failed + " = xor i1 " +
                             is_success + ", true");
    if (bridge_failed_out != nullptr) {
      *bridge_failed_out = true;
    }
    if (bridge_failure_condition_out != nullptr) {
      *bridge_failure_condition_out = bridge_failed;
    }
    if (bridge_error_value_out != nullptr) {
      std::string raw_bridge_error = out;
      if (!signature->objc_status_code_mapping_symbol.empty()) {
        const LoweredFunctionSignature *mapping_signature =
            callbacks.lookup_function_signature(
                signature->objc_status_code_mapping_symbol);
        if (mapping_signature != nullptr &&
            mapping_signature->return_type == ValueType::Void) {
          ctx.code_lines.push_back("  call void @" +
                                   signature->objc_status_code_mapping_symbol +
                                   "(i32 " + out + ")");
        } else {
          const std::string mapped = callbacks.new_temp(ctx);
          ctx.code_lines.push_back(
              "  " + mapped + " = call i32 @" +
              signature->objc_status_code_mapping_symbol + "(i32 " + out +
              ")");
          raw_bridge_error = mapped;
        }
      }
      const std::string bridged_error = callbacks.new_temp(ctx);
      ctx.code_lines.push_back("  " + bridged_error + " = call i32 @" +
                               std::string(
                                   kObjc3RuntimeBridgeStatusErrorI32Symbol) +
                               "(i32 " + out + ", i32 " + raw_bridge_error +
                               ")");
      *bridge_error_value_out = bridged_error;
    }
  }

  if (observe_bridge_failure && signature != nullptr &&
      signature->objc_nserror_declared) {
    const std::string is_success = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + is_success + " = icmp ne i32 " + out +
                             ", 0");
    const std::string bridge_failed = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + bridge_failed + " = xor i1 " +
                             is_success + ", true");
    if (bridge_failed_out != nullptr) {
      *bridge_failed_out = true;
    }
    if (bridge_failure_condition_out != nullptr) {
      *bridge_failure_condition_out = bridge_failed;
    }
    if (bridge_error_value_out != nullptr) {
      std::string raw_bridge_error = "1";
      if (signature->ns_error_out_param_index < expr->args.size()) {
        raw_bridge_error =
            callbacks.emit_expr(expr->args[signature->ns_error_out_param_index]
                                    .get());
      }
      const std::string bridged_error = callbacks.new_temp(ctx);
      ctx.code_lines.push_back("  " + bridged_error + " = call i32 @" +
                               std::string(
                                   kObjc3RuntimeBridgeNSErrorErrorI32Symbol) +
                               "(i32 " + raw_bridge_error + ")");
      *bridge_error_value_out = bridged_error;
    }
  }

  if (expr != nullptr && expr->await_expression_enabled &&
      ctx.async_runtime_helper_enabled) {
    const std::string continuation_handle = callbacks.new_temp(ctx);
    ctx.code_lines.push_back(
        "  " + continuation_handle + " = call i32 @" +
        std::string(kObjc3RuntimeAllocateAsyncContinuationI32Symbol) +
        "(i32 " + std::to_string(ctx.async_resume_entry_tag) + ", i32 " +
        std::to_string(ctx.async_executor_tag) + ")");
    if (ctx.return_await_cleanup_before_handoff_enabled &&
        !ctx.return_await_cleanup_before_handoff_emitted &&
        callbacks.emit_return_await_cleanup_before_handoff) {
      callbacks.emit_return_await_cleanup_before_handoff(ctx);
      ctx.return_await_cleanup_before_handoff_emitted = true;
    }
    const std::string handed_off_handle = callbacks.new_temp(ctx);
    ctx.code_lines.push_back(
        "  " + handed_off_handle + " = call i32 @" +
        std::string(kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol) +
        "(i32 " + continuation_handle + ", i32 " +
        std::to_string(ctx.async_executor_tag) + ")");
    const std::string resumed_value = callbacks.new_temp(ctx);
    ctx.code_lines.push_back(
        "  " + resumed_value + " = call i32 @" +
        std::string(kObjc3RuntimeResumeAsyncContinuationI32Symbol) + "(i32 " +
        handed_off_handle + ", i32 " + out + ")");
    callbacks.invalidate_global_proof_state(ctx);
    return resumed_value;
  }
  if (signature != nullptr && signature->return_insert_retain) {
    const std::string retained_value = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + retained_value + " = call i32 @" +
                             std::string(kObjc3RuntimeRetainI32Symbol) +
                             "(i32 " + out + ")");
    out = retained_value;
  }
  if (signature != nullptr && signature->return_insert_autorelease) {
    const std::string autoreleased_value = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + autoreleased_value + " = call i32 @" +
                             std::string(kObjc3RuntimeAutoreleaseI32Symbol) +
                             "(i32 " + out + ")");
    out = autoreleased_value;
  }
  if (signature != nullptr && signature->return_insert_release) {
    const std::string released_value = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + released_value + " = call i32 @" +
                             std::string(kObjc3RuntimeReleaseI32Symbol) +
                             "(i32 " + out + ")");
    (void)released_value;
  }
  return out;
}
