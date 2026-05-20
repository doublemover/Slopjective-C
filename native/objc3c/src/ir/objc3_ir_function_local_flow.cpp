#include "ir/objc3_ir_function_local_flow.h"

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_function_signature_model.h"
#include "lower/contracts/concurrency_continuation_runtime_contracts.h"
#include "lower/contracts/error_handling_runtime_bridge_contracts.h"
#include "lower/contracts/ownership_runtime_memory_management_contracts.h"

namespace {

std::string NewFunctionLocalTemp(FunctionContext &ctx) {
  return "%t" + std::to_string(ctx.temp_counter++);
}

std::string EmitObjc3IRAsyncReturnContinuationHandoff(
    const std::string &returned_value, FunctionContext &ctx) {
  if (!ctx.async_runtime_helper_enabled) {
    return returned_value;
  }

  const std::string continuation_handle = NewFunctionLocalTemp(ctx);
  ctx.code_lines.push_back(
      "  " + continuation_handle + " = call i32 @" +
      std::string(kObjc3RuntimeAllocateAsyncContinuationI32Symbol) +
      "(i32 " + std::to_string(ctx.async_resume_entry_tag) + ", i32 " +
      std::to_string(ctx.async_executor_tag) + ")");
  const std::string handed_off_handle = NewFunctionLocalTemp(ctx);
  ctx.code_lines.push_back(
      "  " + handed_off_handle + " = call i32 @" +
      std::string(kObjc3RuntimeHandoffAsyncContinuationToExecutorI32Symbol) +
      "(i32 " + continuation_handle + ", i32 " +
      std::to_string(ctx.async_executor_tag) + ")");
  const std::string resumed_value = NewFunctionLocalTemp(ctx);
  ctx.code_lines.push_back(
      "  " + resumed_value + " = call i32 @" +
      std::string(kObjc3RuntimeResumeAsyncContinuationI32Symbol) + "(i32 " +
      handed_off_handle + ", i32 " + returned_value + ")");
  ctx.global_proofs_invalidated = true;
  return resumed_value;
}

}  // namespace

std::string CoerceObjc3IRI32ToBoolI1(const std::string &i32_value,
                                     FunctionContext &ctx) {
  const std::string bool_i1 = NewFunctionLocalTemp(ctx);
  ctx.code_lines.push_back("  " + bool_i1 + " = icmp ne i32 " + i32_value +
                           ", 0");
  return bool_i1;
}

std::string CoerceObjc3IRValueToI32(const std::string &value,
                                    ValueType value_type,
                                    FunctionContext &ctx) {
  if (value_type != ValueType::Bool) {
    return value;
  }
  const std::string widened = NewFunctionLocalTemp(ctx);
  ctx.code_lines.push_back("  " + widened + " = zext i1 " + value +
                           " to i32");
  return widened;
}

std::string BuildObjc3IRThrowsErrorSlotAlloca(FunctionContext &ctx,
                                              const std::string &prefix) {
  const std::string slot =
      "%" + prefix + ".error.addr." + std::to_string(ctx.temp_counter++);
  ctx.entry_lines.push_back("  " + slot + " = alloca i32, align 4");
  return slot;
}

void EmitObjc3IRStoreThrownError(const std::string &error_value,
                                 const std::string &slot,
                                 FunctionContext &ctx) {
  if (slot.empty()) {
    return;
  }
  ctx.code_lines.push_back(
      "  call void @" +
      std::string(kObjc3RuntimeStoreThrownErrorI32Symbol) + "(ptr " + slot +
      ", i32 " + error_value + ")");
}

std::string EmitObjc3IRLoadThrownError(const std::string &slot,
                                       FunctionContext &ctx) {
  if (slot.empty()) {
    return "0";
  }
  const std::string loaded = NewFunctionLocalTemp(ctx);
  ctx.code_lines.push_back(
      "  " + loaded + " = call i32 @" +
      std::string(kObjc3RuntimeLoadThrownErrorI32Symbol) + "(ptr " + slot +
      ")");
  return loaded;
}

void EmitObjc3IRFunctionLocalTerminalCleanupToDepth(
    FunctionContext &ctx, std::size_t scope_depth,
    std::size_t autoreleasepool_depth, std::size_t pending_block_dispose_depth,
    std::size_t ownership_cleanup_depth, std::size_t arc_cleanup_depth,
    const Objc3IRFunctionLocalFlowContext &flow_context) {
  EmitObjc3IRTerminalCleanupToDepth(
      ctx, scope_depth, autoreleasepool_depth, pending_block_dispose_depth,
      ownership_cleanup_depth, arc_cleanup_depth,
      flow_context.scope_cleanup_callbacks);
}

void EmitObjc3IRFunctionLocalTypedReturn(
    const std::string &i32_value, FunctionContext &ctx,
    const Objc3IRFunctionLocalFlowContext &flow_context) {
  const bool cleanup_already_emitted =
      ctx.return_await_cleanup_before_handoff_emitted;
  if (ctx.return_type == ValueType::Void) {
    if (!cleanup_already_emitted) {
      EmitObjc3IRDeferredCleanupTerminalToDepth(
          ctx, 0u, flow_context.scope_cleanup_callbacks);
      EmitObjc3IROwnershipCleanupTerminalCleanupToDepth(
          ctx, 0u, ctx.code_lines, ctx.temp_counter);
      EmitObjc3IRPendingBlockDisposeTerminalCleanupToDepth(
          ctx, 0u, ctx.code_lines);
      EmitObjc3IRArcOwnedTerminalCleanupToDepth(
          ctx, 0u, ctx.code_lines, ctx.temp_counter);
      EmitObjc3IRAutoreleasepoolUnwindToDepth(ctx, 0u);
    }
    ctx.code_lines.push_back("  ret void");
    return;
  }

  std::string returned_value = i32_value;
  if (ctx.arc_return_insert_retain || ctx.arc_return_insert_autorelease) {
    const std::string retained_value = NewFunctionLocalTemp(ctx);
    ctx.code_lines.push_back("  " + retained_value + " = call i32 @" +
                             std::string(kObjc3RuntimeRetainI32Symbol) +
                             "(i32 " + returned_value + ")");
    returned_value = retained_value;
  }
  if (!cleanup_already_emitted) {
    EmitObjc3IRDeferredCleanupTerminalToDepth(
        ctx, 0u, flow_context.scope_cleanup_callbacks);
    EmitObjc3IROwnershipCleanupTerminalCleanupToDepth(
        ctx, 0u, ctx.code_lines, ctx.temp_counter);
    EmitObjc3IRPendingBlockDisposeTerminalCleanupToDepth(
        ctx, 0u, ctx.code_lines);
    EmitObjc3IRArcOwnedTerminalCleanupToDepth(
        ctx, 0u, ctx.code_lines, ctx.temp_counter);
    EmitObjc3IRAutoreleasepoolUnwindToDepth(ctx, 0u);
  }
  if (ctx.arc_return_insert_autorelease) {
    const std::string autoreleased_value = NewFunctionLocalTemp(ctx);
    ctx.code_lines.push_back("  " + autoreleased_value + " = call i32 @" +
                             std::string(kObjc3RuntimeAutoreleaseI32Symbol) +
                             "(i32 " + returned_value + ")");
    returned_value = autoreleased_value;
  }
  if (ctx.return_type == ValueType::Bool) {
    returned_value = EmitObjc3IRAsyncReturnContinuationHandoff(returned_value,
                                                              ctx);
    const std::string bool_i1 = CoerceObjc3IRI32ToBoolI1(returned_value, ctx);
    ctx.code_lines.push_back("  ret i1 " + bool_i1);
    return;
  }
  returned_value = EmitObjc3IRAsyncReturnContinuationHandoff(returned_value,
                                                            ctx);
  ctx.code_lines.push_back("  ret i32 " + returned_value);
}

void EmitObjc3IRPropagateThrownError(
    const std::string &error_value, FunctionContext &ctx,
    const Objc3IRFunctionLocalFlowContext &flow_context) {
  if (!ctx.error_handler_stack.empty()) {
    const auto &handler = ctx.error_handler_stack.back();
    EmitObjc3IRStoreThrownError(error_value, handler.error_slot_ptr, ctx);
    EmitObjc3IRFunctionLocalTerminalCleanupToDepth(
        ctx, handler.scope_depth, handler.autoreleasepool_depth,
        handler.pending_block_dispose_depth, handler.ownership_cleanup_depth,
        handler.arc_cleanup_depth, flow_context);
    ctx.code_lines.push_back("  br label %" + handler.dispatch_label);
    ctx.terminated = true;
    return;
  }
  if (!ctx.function_error_out_param.empty()) {
    EmitObjc3IRStoreThrownError(error_value, ctx.function_error_out_param, ctx);
    EmitObjc3IRFunctionLocalTypedReturn("0", ctx, flow_context);
    ctx.terminated = true;
    return;
  }
  ctx.code_lines.push_back("  call void @abort()");
  ctx.code_lines.push_back("  unreachable");
  ctx.terminated = true;
}

void EmitObjc3IRFunctionLocalTypedParamStore(
    const FuncParam &param, std::size_t index, const std::string &ptr,
    FunctionContext &ctx,
    const Objc3IRFunctionLocalFlowContext &flow_context) {
  if (param.type == ValueType::Bool) {
    const std::string widened = "%arg" + std::to_string(index) + ".zext." +
                                std::to_string(ctx.temp_counter++);
    ctx.entry_lines.push_back("  " + widened + " = zext i1 %arg" +
                              std::to_string(index) + " to i32");
    ctx.entry_lines.push_back("  store i32 " + widened + ", ptr " + ptr +
                              ", align 4");
    return;
  }

  std::string stored_value = "%arg" + std::to_string(index);
  if (EffectiveArcParamInsertRetain(param, flow_context.arc_mode_enabled)) {
    const std::string retained_value = "%arg" + std::to_string(index) +
                                       ".retained." +
                                       std::to_string(ctx.temp_counter++);
    ctx.entry_lines.push_back("  " + retained_value + " = call i32 @" +
                              std::string(kObjc3RuntimeRetainI32Symbol) +
                              "(i32 " + stored_value + ")");
    stored_value = retained_value;
  }
  ctx.entry_lines.push_back("  store i32 " + stored_value + ", ptr " + ptr +
                            ", align 4");
  if (EffectiveArcParamInsertRelease(param, flow_context.arc_mode_enabled)) {
    RegisterObjc3IRArcOwnedCleanupPtr(ptr, ctx);
  }
}
