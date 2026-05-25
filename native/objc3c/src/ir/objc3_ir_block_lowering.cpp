#include "ir/objc3_ir_block_lowering.h"

#include <algorithm>
#include <array>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_block_lowering_helper_emission.h"
#include "ir/objc3_ir_block_lowering_internal.h"
#include "ir/objc3_ir_block_runtime_contracts.h"
#include "lower/contracts/block_runtime_helper_contracts.h"

std::string EmitObjc3IRPromotedBlockHandle(
    const Expr &expr, const std::string &storage_ptr, FunctionContext &ctx,
    const Objc3IRBlockLoweringContext &lowering_context,
    bool allow_nonescaping_scalar_promotion) {
  const bool supported = allow_nonescaping_scalar_promotion
                             ? BlockLiteralSupportsScalarRuntimePromotion(expr)
                             : BlockLiteralSupportsEscapingRuntimeHookLowering(
                                   expr);
  if (!supported) {
    return lowering_context.callbacks.emit_unsupported_i32_value(
        "escaping block value still requires normalized block runtime helper metadata that lands in later runtime work");
  }
  if (expr.block_explicit_capture_move_count > 0u) {
    return lowering_context.callbacks.emit_unsupported_i32_value(
        "escaping move captures for cleanup/resource-backed locals still require later Part 8 runtime ownership transfer support");
  }
  const bool pointer_capture_storage =
      BlockLiteralUsesPointerCaptureStorage(expr);
  const std::string promoted = NewObjc3IRBlockTemp(ctx);
  ctx.code_lines.push_back(
      "  " + promoted + " = call i32 @" +
      std::string(kObjc3RuntimePromoteBlockI32Symbol) + "(ptr " +
      storage_ptr + ", i64 " +
      std::to_string(BlockStorageStaticSizeBytes(expr)) + ", i32 " +
      std::string(pointer_capture_storage ? "1" : "0") + ")");
  return promoted;
}

std::string EmitObjc3IRPromotedBlockHandleLoad(
    BlockBinding &binding, FunctionContext &ctx,
    const Objc3IRBlockLoweringContext &lowering_context) {
  if (!binding.promoted_handle_ptr.empty()) {
    const std::string loaded = NewObjc3IRBlockTemp(ctx);
    ctx.code_lines.push_back("  " + loaded + " = load i32, ptr " +
                             binding.promoted_handle_ptr + ", align 4");
    return loaded;
  }
  if (binding.literal == nullptr || binding.storage_ptr.empty()) {
    return lowering_context.callbacks.emit_unsupported_i32_value(
        "missing block literal metadata for escaping block-handle lowering");
  }
  const std::string promoted = EmitObjc3IRPromotedBlockHandle(
      *binding.literal, binding.storage_ptr, ctx, lowering_context, true);
  if (promoted == "poison") {
    return promoted;
  }
  binding.promoted_handle_ptr =
      "%block.promoted.addr." + std::to_string(ctx.temp_counter++);
  ctx.entry_lines.push_back("  " + binding.promoted_handle_ptr +
                            " = alloca i32, align 4");
  ctx.code_lines.push_back("  store i32 " + promoted + ", ptr " +
                           binding.promoted_handle_ptr + ", align 4");
  return promoted;
}

std::string EmitObjc3IRBlockLiteralStorage(
    const Expr &expr, FunctionContext &ctx,
    const Objc3IRBlockLoweringContext &lowering_context) {
  if (BlockLiteralRequiresFutureRuntimeLanes(expr)) {
    return lowering_context.callbacks.emit_unsupported_i32_value(
        "block literal requires escaping heap-promotion or runtime-managed copy/dispose lowering that lands in later runtime work");
  }
  if (expr.block_parameter_count > 4u) {
    return lowering_context.callbacks.emit_unsupported_i32_value(
        "block literal exceeds current runnable invoke-thunk arity limit of 4");
  }

  Objc3IRBlockLoweringContext block_context = lowering_context;
  block_context.current_implementation_name = ctx.current_implementation_name;
  block_context.current_superclass_name = ctx.current_superclass_name;
  block_context.current_method_is_class_method =
      ctx.current_method_is_class_method;

  EmitObjc3IRBlockInvokeThunk(expr, block_context);
  EmitObjc3IRBlockDescriptor(expr, block_context);
  EmitObjc3IRBlockCopyHelper(expr, block_context);
  EmitObjc3IRBlockDisposeHelper(expr, ctx, block_context);

  const std::string storage_type = BuildBlockStorageType(expr);
  const bool pointer_capture_storage =
      BlockLiteralUsesPointerCaptureStorage(expr);
  const std::string storage_ptr =
      "%block.literal.addr." + std::to_string(ctx.temp_counter++);
  ctx.entry_lines.push_back("  " + storage_ptr + " = alloca " +
                            storage_type + ", align 8");

  const std::string descriptor_ptr_slot = NewObjc3IRBlockTemp(ctx);
  ctx.code_lines.push_back("  " + descriptor_ptr_slot +
                           " = getelementptr inbounds " + storage_type +
                           ", ptr " + storage_ptr + ", i32 0, i32 0");
  ctx.code_lines.push_back("  store ptr @" + BuildBlockDescriptorSymbol(expr) +
                           ", ptr " + descriptor_ptr_slot + ", align 8");

  if (pointer_capture_storage) {
    const std::string copy_helper_slot = NewObjc3IRBlockTemp(ctx);
    const std::string dispose_helper_slot = NewObjc3IRBlockTemp(ctx);
    ctx.code_lines.push_back("  " + copy_helper_slot +
                             " = getelementptr inbounds " + storage_type +
                             ", ptr " + storage_ptr + ", i32 0, i32 1");
    ctx.code_lines.push_back(
        "  store ptr " +
        std::string(expr.block_runtime_copy_helper_required
                        ? "@" + BuildBlockCopyHelperSymbol(expr)
                        : "null") +
        ", ptr " + copy_helper_slot + ", align 8");
    ctx.code_lines.push_back("  " + dispose_helper_slot +
                             " = getelementptr inbounds " + storage_type +
                             ", ptr " + storage_ptr + ", i32 0, i32 2");
    ctx.code_lines.push_back(
        "  store ptr " +
        std::string(expr.block_runtime_dispose_helper_required
                        ? "@" + BuildBlockDisposeHelperSymbol(expr)
                        : "null") +
        ", ptr " + dispose_helper_slot + ", align 8");
  }

  for (std::size_t i = 0; i < expr.block_capture_names_lexicographic.size();
       ++i) {
    const std::string &capture_name =
        expr.block_capture_names_lexicographic[i];
    const std::string capture_slot = NewObjc3IRBlockTemp(ctx);
    if (pointer_capture_storage) {
      std::string capture_cell_ptr;
      const auto moved_capture_it = std::find_if(
          expr.block_explicit_capture_items_source_order.begin(),
          expr.block_explicit_capture_items_source_order.end(),
          [&capture_name](const Expr::ExplicitBlockCaptureItem &item) {
            return item.name == capture_name && item.mode == "move";
          });
      if (moved_capture_it !=
          expr.block_explicit_capture_items_source_order.end()) {
        const auto cleanup_it =
            ctx.ownership_cleanup_call_indices.find(capture_name);
        if (cleanup_it == ctx.ownership_cleanup_call_indices.end()) {
          return lowering_context.callbacks.emit_unsupported_i32_value(
              "move capture '" + capture_name +
              "' was not registered as a cleanup/resource-backed local");
        }
        PendingOwnershipCleanupCall &cleanup_call =
            ctx.pending_ownership_cleanup_calls[cleanup_it->second];
        cleanup_call.active = false;
        capture_cell_ptr = cleanup_call.storage_ptr;
      } else if (Objc3IRBlockSortedStringListContains(
                     expr.block_byref_capture_names_lexicographic,
                     capture_name)) {
        capture_cell_ptr =
            lowering_context.callbacks.lookup_var_ptr(ctx, capture_name);
        if (capture_cell_ptr.empty()) {
          return lowering_context.callbacks.emit_unsupported_i32_value(
              "block literal byref capture '" + capture_name +
              "' could not be resolved during IR lowering");
        }
      } else {
        capture_cell_ptr = "%" + capture_name + ".capture.addr." +
                           std::to_string(ctx.temp_counter++);
        ctx.entry_lines.push_back("  " + capture_cell_ptr +
                                  " = alloca i32, align 4");
        const std::string capture_value =
            lowering_context.callbacks.emit_identifier_value(capture_name,
                                                             ctx);
        ctx.code_lines.push_back("  store i32 " + capture_value + ", ptr " +
                                 capture_cell_ptr + ", align 4");
      }
      ctx.code_lines.push_back("  " + capture_slot +
                               " = getelementptr inbounds " + storage_type +
                               ", ptr " + storage_ptr +
                               ", i32 0, i32 3, i32 " + std::to_string(i));
      ctx.code_lines.push_back("  store ptr " + capture_cell_ptr + ", ptr " +
                               capture_slot + ", align 8");
      continue;
    }
    const std::string capture_value =
        lowering_context.callbacks.emit_identifier_value(capture_name, ctx);
    ctx.code_lines.push_back("  " + capture_slot +
                             " = getelementptr inbounds " + storage_type +
                             ", ptr " + storage_ptr +
                             ", i32 0, i32 1, i32 " + std::to_string(i));
    ctx.code_lines.push_back("  store i32 " + capture_value + ", ptr " +
                             capture_slot + ", align 4");
  }

  if (pointer_capture_storage && expr.block_runtime_copy_helper_required) {
    ctx.code_lines.push_back("  call void @" + BuildBlockCopyHelperSymbol(expr) +
                             "(ptr " + storage_ptr + ")");
  }
  if (pointer_capture_storage && expr.block_runtime_dispose_helper_required) {
    ctx.pending_block_dispose_calls.push_back(
        PendingBlockDisposeCall{BuildBlockDisposeHelperSymbol(expr),
                                storage_ptr});
  }

  return storage_ptr;
}

std::string EmitObjc3IRBlockInvokeCall(
    const BlockBinding &binding, const Expr *call_expr, FunctionContext &ctx,
    const Objc3IRBlockLoweringContext &lowering_context) {
  if (binding.literal == nullptr) {
    return lowering_context.callbacks.emit_unsupported_i32_value(
        "missing block literal metadata for local callable invocation");
  }

  if (!binding.promoted_handle_ptr.empty()) {
    std::array<std::string, 4> args{"0", "0", "0", "0"};
    for (std::size_t i = 0; i < call_expr->args.size() && i < args.size();
         ++i) {
      args[i] = lowering_context.callbacks.emit_expr(call_expr->args[i].get(),
                                                     ctx);
    }
    const std::string handle = NewObjc3IRBlockTemp(ctx);
    ctx.code_lines.push_back("  " + handle + " = load i32, ptr " +
                             binding.promoted_handle_ptr + ", align 4");
    const std::string out = NewObjc3IRBlockTemp(ctx);
    ctx.code_lines.push_back(
        "  " + out + " = call i32 @" +
        std::string(kObjc3RuntimeInvokeBlockI32Symbol) + "(i32 " + handle +
        ", i32 " + args[0] + ", i32 " + args[1] + ", i32 " + args[2] +
        ", i32 " + args[3] + ")");
    return out;
  }

  std::array<std::string, 4> args{"0", "0", "0", "0"};
  for (std::size_t i = 0; i < call_expr->args.size() && i < args.size();
       ++i) {
    args[i] = lowering_context.callbacks.emit_expr(call_expr->args[i].get(),
                                                   ctx);
  }

  const std::string out = NewObjc3IRBlockTemp(ctx);
  ctx.code_lines.push_back("  " + out + " = call i32 @" +
                           BuildBlockInvokeSymbol(*binding.literal) +
                           "(ptr " + binding.storage_ptr + ", i32 " +
                           args[0] + ", i32 " + args[1] + ", i32 " +
                           args[2] + ", i32 " + args[3] + ")");
  return out;
}
