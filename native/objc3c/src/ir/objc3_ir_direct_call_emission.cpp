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
    ValueType expected_type,
    const Objc3IRValueOptionalCarrierMetadata *expected_optional_carrier,
    FunctionContext &ctx,
    const Objc3IRDirectCallEmissionCallbacks &callbacks) {
  if (expected_type == ValueType::Bool) {
    const std::string arg_i1 = callbacks.coerce_i32_to_bool_i1(arg_i32, ctx);
    args.push_back("i1 " + arg_i1);
    return;
  }
  if (expected_type == ValueType::Optional) {
    const Objc3IRValueOptionalCarrierMetadata default_carrier;
    const Objc3IRValueOptionalCarrierMetadata &carrier =
        expected_optional_carrier != nullptr ? *expected_optional_carrier
                                             : default_carrier;
    args.push_back(
        std::string(LLVMScalarTypeForValueOptionalCarrier(expected_type,
                                                          carrier)) +
        " " + arg_i32);
    return;
  }
  args.push_back("i32 " + arg_i32);
}

const Objc3IRValueOptionalCarrierMetadata *Objc3IRSignatureParamCarrier(
    const LoweredFunctionSignature *signature, std::size_t index) {
  if (signature == nullptr ||
      index >= signature->param_value_optional_carriers.size()) {
    return nullptr;
  }
  return &signature->param_value_optional_carriers[index];
}

bool Objc3IRValueOptionalArgCarrierMatches(
    const std::string &arg_value,
    const Objc3IRValueOptionalCarrierMetadata *expected_optional_carrier,
    const FunctionContext &ctx) {
  if (expected_optional_carrier == nullptr ||
      !expected_optional_carrier->present) {
    return true;
  }
  const auto actual_carrier = ctx.value_optional_carrier_by_value.find(arg_value);
  return actual_carrier != ctx.value_optional_carrier_by_value.end() &&
         actual_carrier->second ==
             Objc3IRValueOptionalCarrierKindFor(*expected_optional_carrier);
}

bool IsObjc3IRFullI64OptionalRuntimeHelper(const std::string &ident) {
  return ident == kObjc3RuntimeOptionalAbsentFullI64Symbol ||
         ident == kObjc3RuntimeOptionalPresentFullI64Symbol ||
         ident == kObjc3RuntimeOptionalHasValueFullI64Symbol;
}

std::string NewObjc3IRFullI64CarrierSlot(FunctionContext &ctx) {
  const std::string slot =
      "%value_optional.full_i64.addr." + std::to_string(ctx.temp_counter++);
  ctx.entry_lines.push_back("  " + slot + " = alloca { i8, i64 }, align 8");
  return slot;
}

std::string EmitObjc3IRLoadFullI64CarrierFromSlot(
    const std::string &slot, FunctionContext &ctx,
    const Objc3IRDirectCallEmissionCallbacks &callbacks) {
  const std::string value = callbacks.new_temp(ctx);
  ctx.code_lines.push_back("  " + value +
                           " = load { i8, i64 }, ptr " + slot +
                           ", align 8");
  ctx.value_optional_carrier_by_value[value] =
      Objc3IRValueOptionalCarrierKind::FullI64;
  return value;
}

bool Objc3IRValueUsesFullI64OptionalCarrier(const std::string &value,
                                            const FunctionContext &ctx) {
  const auto carrier = ctx.value_optional_carrier_by_value.find(value);
  return carrier != ctx.value_optional_carrier_by_value.end() &&
         carrier->second == Objc3IRValueOptionalCarrierKind::FullI64;
}

std::string EmitObjc3IRFullI64OptionalRuntimeHelperCall(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRDirectCallEmissionCallbacks &callbacks) {
  if (expr == nullptr || !IsObjc3IRFullI64OptionalRuntimeHelper(expr->ident)) {
    return "";
  }
  if (expr->ident == kObjc3RuntimeOptionalAbsentFullI64Symbol) {
#if defined(_WIN32)
    const std::string slot = NewObjc3IRFullI64CarrierSlot(ctx);
    ctx.code_lines.push_back("  call void @" +
                             std::string(
                                 kObjc3RuntimeOptionalAbsentFullI64Symbol) +
                             "(ptr sret({ i8, i64 }) align 8 " + slot +
                             ")");
    return EmitObjc3IRLoadFullI64CarrierFromSlot(slot, ctx, callbacks);
#else
    const std::string value = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + value + " = call { i8, i64 } @" +
                             std::string(
                                 kObjc3RuntimeOptionalAbsentFullI64Symbol) +
                             "()");
    ctx.value_optional_carrier_by_value[value] =
        Objc3IRValueOptionalCarrierKind::FullI64;
    return value;
#endif
  }
  if (expr->ident == kObjc3RuntimeOptionalPresentFullI64Symbol &&
      expr->args.size() != 1u) {
    return callbacks.emit_unsupported_i32_value(
        "Optional<i64> present helper lowering requires exactly one payload");
  }
  if (expr->ident == kObjc3RuntimeOptionalPresentFullI64Symbol) {
#if defined(_WIN32)
    const std::string slot = NewObjc3IRFullI64CarrierSlot(ctx);
#endif
    std::string payload = callbacks.emit_expr(expr->args.front().get());
    if (ctx.terminated) {
      return "zeroinitializer";
    }
    if (expr->args.front()->kind != Expr::Kind::Number) {
      const std::string widened_payload = callbacks.new_temp(ctx);
      ctx.code_lines.push_back("  " + widened_payload + " = sext i32 " +
                               payload + " to i64");
      payload = widened_payload;
    }
#if defined(_WIN32)
    ctx.code_lines.push_back("  call void @" +
                             std::string(
                                 kObjc3RuntimeOptionalPresentFullI64Symbol) +
                             "(ptr sret({ i8, i64 }) align 8 " + slot +
                             ", i64 " + payload + ")");
    return EmitObjc3IRLoadFullI64CarrierFromSlot(slot, ctx, callbacks);
#else
    const std::string value = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + value + " = call { i8, i64 } @" +
                             std::string(
                                 kObjc3RuntimeOptionalPresentFullI64Symbol) +
                             "(i64 " + payload + ")");
    ctx.value_optional_carrier_by_value[value] =
        Objc3IRValueOptionalCarrierKind::FullI64;
    return value;
#endif
  }
  if (expr->ident == kObjc3RuntimeOptionalHasValueFullI64Symbol) {
    if (expr->args.size() != 1u) {
      return callbacks.emit_unsupported_i32_value(
          "Optional<i64> has-value helper lowering requires exactly one value");
    }
    const std::string value = callbacks.emit_expr(expr->args.front().get());
    if (ctx.terminated) {
      return "0";
    }
    if (!Objc3IRValueUsesFullI64OptionalCarrier(value, ctx)) {
      return callbacks.emit_unsupported_i32_value(
          "Optional<i64> has-value helper requires a full-width carrier value");
    }
#if defined(_WIN32)
    const std::string slot = NewObjc3IRFullI64CarrierSlot(ctx);
    ctx.code_lines.push_back("  store { i8, i64 } " + value + ", ptr " +
                             slot + ", align 8");
    const std::string has_value = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + has_value + " = call i1 @" +
                             std::string(
                                 kObjc3RuntimeOptionalHasValueFullI64Symbol) +
                             "(ptr " + slot + ")");
#else
    const std::string has_value = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + has_value + " = call i1 @" +
                             std::string(
                                 kObjc3RuntimeOptionalHasValueFullI64Symbol) +
                             "({ i8, i64 } " + value + ")");
#endif
    const std::string result = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + result + " = zext i1 " + has_value +
                             " to i32");
    return result;
  }
  return callbacks.emit_unsupported_i32_value(
      "unsupported Optional<i64> runtime helper lowering");
}

}  // namespace

std::string EmitObjc3IRDirectFunctionCall(
    const Expr *expr, const LoweredFunctionSignature *signature,
    FunctionContext &ctx,
    const Objc3IRDirectCallEmissionCallbacks &callbacks,
    const std::string &throws_error_slot_ptr, bool *bridge_failed_out,
    std::string *bridge_error_value_out,
    std::string *bridge_failure_condition_out) {
  const std::string full_i64_optional_helper =
      EmitObjc3IRFullI64OptionalRuntimeHelperCall(expr, ctx, callbacks);
  if (!full_i64_optional_helper.empty()) {
    return full_i64_optional_helper;
  }
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
    const Objc3IRValueOptionalCarrierMetadata *expected_optional_carrier =
        Objc3IRSignatureParamCarrier(signature, i);
    if (expected_type == ValueType::Optional &&
        !Objc3IRValueOptionalArgCarrierMatches(
            arg_i32, expected_optional_carrier, ctx)) {
      return callbacks.emit_unsupported_i32_value(
          "direct call Optional argument carrier does not match declared "
          "function signature");
    }
    AppendObjc3IRLoweredCallArg(
        args, arg_i32, expected_type, expected_optional_carrier, ctx,
        callbacks);
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
  const std::string llvm_return_type =
      signature != nullptr
          ? LLVMScalarTypeForValueOptionalCarrier(
                return_type, signature->return_value_optional_carrier)
          : LLVMScalarType(return_type);
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
    if (return_type == ValueType::Optional) {
      if (signature != nullptr) {
        ctx.value_optional_carrier_by_value[tmp] =
            Objc3IRValueOptionalCarrierKindFor(
                signature->return_value_optional_carrier);
      }
      out = tmp;
    } else {
      out = callbacks.coerce_value_to_i32(tmp, return_type, ctx);
    }
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
