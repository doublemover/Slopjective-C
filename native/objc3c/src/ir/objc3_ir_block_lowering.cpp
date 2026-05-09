#include "ir/objc3_ir_block_lowering.h"

#include <algorithm>
#include <array>
#include <sstream>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_block_runtime_contracts.h"
#include "lower/contracts/block_runtime_helper_contracts.h"
#include "lower/contracts/ownership_runtime_memory_management_contracts.h"

namespace {

std::string NewBlockTemp(FunctionContext &ctx) {
  return "%t" + std::to_string(ctx.temp_counter++);
}

bool SortedStringListContains(const std::vector<std::string> &entries,
                              const std::string &needle) {
  return std::binary_search(entries.begin(), entries.end(), needle);
}

void EmitBlockCopyHelper(const Expr &expr,
                         const Objc3IRBlockLoweringContext &context) {
  if (!BlockLiteralUsesPointerCaptureStorage(expr) ||
      !expr.block_runtime_copy_helper_required) {
    return;
  }
  const std::string symbol = BuildBlockCopyHelperSymbol(expr);
  if (symbol.empty() ||
      !context.state.emitted_block_copy_helper_symbols->insert(symbol)
           .second) {
    return;
  }

  const std::string storage_type = BuildBlockStorageType(expr);
  std::ostringstream out;
  out << "define internal void @" << symbol << "(ptr %block) {\n";
  out << "entry:\n";
  int temp_counter = 0;
  for (std::size_t i = 0; i < expr.block_capture_names_lexicographic.size();
       ++i) {
    const std::string &capture_name =
        expr.block_capture_names_lexicographic[i];
    if (!SortedStringListContains(
            expr.block_runtime_owned_object_capture_names_lexicographic,
            capture_name)) {
      continue;
    }
    const std::string slot_ptr =
        "%block.copy.slot." + std::to_string(temp_counter++);
    const std::string capture_ptr =
        "%block.copy.capture." + std::to_string(temp_counter++);
    const std::string loaded_value =
        "%block.copy.value." + std::to_string(temp_counter++);
    const std::string retained_value =
        "%block.copy.retained." + std::to_string(temp_counter++);
    out << "  " << slot_ptr << " = getelementptr inbounds " << storage_type
        << ", ptr %block, i32 0, i32 3, i32 " << i << "\n";
    out << "  " << capture_ptr << " = load ptr, ptr " << slot_ptr
        << ", align 8\n";
    out << "  " << loaded_value << " = load i32, ptr " << capture_ptr
        << ", align 4\n";
    out << "  " << retained_value << " = call i32 @"
        << kObjc3RuntimeRetainI32Symbol << "(i32 " << loaded_value << ")\n";
    out << "  store i32 " << retained_value << ", ptr " << capture_ptr
        << ", align 4\n";
  }
  out << "  ret void\n";
  out << "}\n";
  context.state.block_function_definitions->push_back(out.str());
}

void EmitBlockDisposeHelper(const Expr &expr, const FunctionContext &ctx,
                            const Objc3IRBlockLoweringContext &context) {
  if (!BlockLiteralUsesPointerCaptureStorage(expr) ||
      !expr.block_runtime_dispose_helper_required) {
    return;
  }
  const std::string symbol = BuildBlockDisposeHelperSymbol(expr);
  if (symbol.empty() ||
      !context.state.emitted_block_dispose_helper_symbols->insert(symbol)
           .second) {
    return;
  }

  const std::string storage_type = BuildBlockStorageType(expr);
  std::ostringstream out;
  out << "define internal void @" << symbol << "(ptr %block) {\n";
  out << "entry:\n";
  int temp_counter = 0;
  for (std::size_t i = 0; i < expr.block_capture_names_lexicographic.size();
       ++i) {
    const std::string &capture_name =
        expr.block_capture_names_lexicographic[i];
    const auto moved_capture_it = std::find_if(
        expr.block_explicit_capture_items_source_order.begin(),
        expr.block_explicit_capture_items_source_order.end(),
        [&capture_name](const Expr::ExplicitBlockCaptureItem &item) {
          return item.name == capture_name && item.mode == "move";
        });
    const bool moved_capture =
        moved_capture_it !=
        expr.block_explicit_capture_items_source_order.end();
    const auto cleanup_it =
        ctx.ownership_cleanup_call_indices.find(capture_name);
    if (!SortedStringListContains(
            expr.block_runtime_owned_object_capture_names_lexicographic,
            capture_name) &&
        (!moved_capture ||
         cleanup_it == ctx.ownership_cleanup_call_indices.end())) {
      continue;
    }
    const std::string slot_ptr =
        "%block.dispose.slot." + std::to_string(temp_counter++);
    const std::string capture_ptr =
        "%block.dispose.capture." + std::to_string(temp_counter++);
    const std::string loaded_value =
        "%block.dispose.value." + std::to_string(temp_counter++);
    const std::string released_value =
        "%block.dispose.released." + std::to_string(temp_counter++);
    out << "  " << slot_ptr << " = getelementptr inbounds " << storage_type
        << ", ptr %block, i32 0, i32 3, i32 " << i << "\n";
    out << "  " << capture_ptr << " = load ptr, ptr " << slot_ptr
        << ", align 8\n";
    out << "  " << loaded_value << " = load i32, ptr " << capture_ptr
        << ", align 4\n";
    if (SortedStringListContains(
            expr.block_runtime_owned_object_capture_names_lexicographic,
            capture_name)) {
      out << "  " << released_value << " = call i32 @"
          << kObjc3RuntimeReleaseI32Symbol << "(i32 " << loaded_value
          << ")\n";
    }
    if (moved_capture &&
        cleanup_it != ctx.ownership_cleanup_call_indices.end()) {
      const PendingOwnershipCleanupCall &cleanup_call =
          ctx.pending_ownership_cleanup_calls[cleanup_it->second];
      if (!cleanup_call.resource_close_symbol.empty()) {
        if (cleanup_call.has_resource_invalid_value) {
          const std::string resource_live =
              "%block.dispose.resource.live." + std::to_string(temp_counter++);
          const std::string resource_close_label =
              "block.dispose.resource.close." +
              std::to_string(temp_counter++);
          const std::string resource_skip_label =
              "block.dispose.resource.skip." + std::to_string(temp_counter++);
          out << "  " << resource_live << " = icmp ne i32 " << loaded_value
              << ", " << cleanup_call.resource_invalid_value << "\n";
          out << "  br i1 " << resource_live << ", label %"
              << resource_close_label << ", label %" << resource_skip_label
              << "\n";
          out << resource_close_label << ":\n";
          out << "  call void @" << cleanup_call.resource_close_symbol
              << "(i32 " << loaded_value << ")\n";
          out << "  br label %" << resource_skip_label << "\n";
          out << resource_skip_label << ":\n";
        } else {
          out << "  call void @" << cleanup_call.resource_close_symbol
              << "(i32 " << loaded_value << ")\n";
        }
      }
      if (!cleanup_call.cleanup_function_symbol.empty()) {
        out << "  call void @" << cleanup_call.cleanup_function_symbol
            << "(i32 " << loaded_value << ")\n";
      }
    }
    (void)released_value;
  }
  out << "  ret void\n";
  out << "}\n";
  context.state.block_function_definitions->push_back(out.str());
}

void EmitBlockInvokeThunk(const Expr &expr,
                          const Objc3IRBlockLoweringContext &context) {
  const std::string symbol = BuildBlockInvokeSymbol(expr);
  if (symbol.empty() ||
      !context.state.emitted_block_invoke_symbols->insert(symbol).second) {
    return;
  }

  std::ostringstream out;
  out << "define internal i32 @" << symbol
      << "(ptr %block, i32 %arg0, i32 %arg1, i32 %arg2, i32 %arg3) {\n";
  out << "entry:\n";

  FunctionContext ctx;
  ctx.return_type = ValueType::I32;
  PushObjc3IRScope(ctx);

  const std::string block_storage_type = BuildBlockStorageType(expr);
  const bool pointer_capture_storage =
      BlockLiteralUsesPointerCaptureStorage(expr);
  for (std::size_t i = 0; i < expr.block_capture_names_lexicographic.size();
       ++i) {
    const std::string &capture_name =
        expr.block_capture_names_lexicographic[i];
    const std::string slot_ptr = NewBlockTemp(ctx);
    const std::size_t capture_field_index = pointer_capture_storage ? 3u : 1u;
    ctx.entry_lines.push_back(
        "  " + slot_ptr + " = getelementptr inbounds " + block_storage_type +
        ", ptr %block, i32 0, i32 " +
        std::to_string(capture_field_index) + ", i32 " + std::to_string(i));
    if (pointer_capture_storage) {
      const std::string capture_ptr = NewBlockTemp(ctx);
      ctx.entry_lines.push_back("  " + capture_ptr + " = load ptr, ptr " +
                                slot_ptr + ", align 8");
      ctx.scopes.back()[capture_name] = capture_ptr;
      continue;
    }
    const std::string ptr =
        "%" + capture_name + ".addr." + std::to_string(ctx.temp_counter++);
    const std::string value = NewBlockTemp(ctx);
    ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
    ctx.scopes.back()[capture_name] = ptr;
    ctx.entry_lines.push_back("  " + value + " = load i32, ptr " + slot_ptr +
                              ", align 4");
    ctx.entry_lines.push_back("  store i32 " + value + ", ptr " + ptr +
                              ", align 4");
  }

  for (std::size_t i = 0; i < expr.block_parameters_source_order.size() &&
                          i < 4u;
       ++i) {
    const auto &parameter = expr.block_parameters_source_order[i];
    const std::string ptr =
        "%" + parameter.name + ".addr." + std::to_string(ctx.temp_counter++);
    ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
    ctx.entry_lines.push_back("  store i32 %arg" + std::to_string(i) +
                              ", ptr " + ptr + ", align 4");
    ctx.scopes.back()[parameter.name] = ptr;
  }

  for (const auto &stmt : expr.block_body) {
    context.callbacks.emit_statement(stmt.get(), ctx);
    if (ctx.terminated) {
      break;
    }
  }

  if (!ctx.terminated) {
    EmitObjc3IRAutoreleasepoolUnwindToDepth(ctx, 0u);
    EmitObjc3IROwnershipCleanupUnwindToDepth(
        ctx, 0u, context.scope_cleanup_callbacks);
    EmitObjc3IRPendingBlockDisposeUnwindToDepth(ctx, 0u);
    EmitObjc3IRArcOwnedCleanupReleases(
        ctx, context.scope_cleanup_callbacks);
    ctx.code_lines.push_back("  ret i32 0");
  }

  for (const auto &line : ctx.entry_lines) {
    out << line << "\n";
  }
  for (const auto &line : ctx.code_lines) {
    out << line << "\n";
  }
  out << "}\n";
  context.state.block_function_definitions->push_back(out.str());
}

}  // namespace

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
  const std::string promoted = NewBlockTemp(ctx);
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
    const std::string loaded = NewBlockTemp(ctx);
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

  EmitBlockInvokeThunk(expr, lowering_context);
  EmitBlockCopyHelper(expr, lowering_context);
  EmitBlockDisposeHelper(expr, ctx, lowering_context);

  const std::string storage_type = BuildBlockStorageType(expr);
  const bool pointer_capture_storage =
      BlockLiteralUsesPointerCaptureStorage(expr);
  const std::string storage_ptr =
      "%block.literal.addr." + std::to_string(ctx.temp_counter++);
  ctx.entry_lines.push_back("  " + storage_ptr + " = alloca " +
                            storage_type + ", align 8");

  const std::string invoke_ptr_slot = NewBlockTemp(ctx);
  ctx.code_lines.push_back("  " + invoke_ptr_slot +
                           " = getelementptr inbounds " + storage_type +
                           ", ptr " + storage_ptr + ", i32 0, i32 0");
  ctx.code_lines.push_back("  store ptr @" + BuildBlockInvokeSymbol(expr) +
                           ", ptr " + invoke_ptr_slot + ", align 8");

  if (pointer_capture_storage) {
    const std::string copy_helper_slot = NewBlockTemp(ctx);
    const std::string dispose_helper_slot = NewBlockTemp(ctx);
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
    const std::string capture_slot = NewBlockTemp(ctx);
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
      } else if (SortedStringListContains(
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
    const std::string handle = NewBlockTemp(ctx);
    ctx.code_lines.push_back("  " + handle + " = load i32, ptr " +
                             binding.promoted_handle_ptr + ", align 4");
    const std::string out = NewBlockTemp(ctx);
    ctx.code_lines.push_back(
        "  " + out + " = call i32 @" +
        std::string(kObjc3RuntimeInvokeBlockI32Symbol) + "(i32 " + handle +
        ", i32 " + args[0] + ", i32 " + args[1] + ", i32 " + args[2] +
        ", i32 " + args[3] + ")");
    return out;
  }

  const std::string storage_type = BuildBlockStorageType(*binding.literal);
  const std::string invoke_ptr_slot = NewBlockTemp(ctx);
  const std::string invoke_ptr = NewBlockTemp(ctx);
  ctx.code_lines.push_back("  " + invoke_ptr_slot +
                           " = getelementptr inbounds " + storage_type +
                           ", ptr " + binding.storage_ptr +
                           ", i32 0, i32 0");
  ctx.code_lines.push_back("  " + invoke_ptr + " = load ptr, ptr " +
                           invoke_ptr_slot + ", align 8");

  std::array<std::string, 4> args{"0", "0", "0", "0"};
  for (std::size_t i = 0; i < call_expr->args.size() && i < args.size();
       ++i) {
    args[i] = lowering_context.callbacks.emit_expr(call_expr->args[i].get(),
                                                   ctx);
  }

  const std::string out = NewBlockTemp(ctx);
  ctx.code_lines.push_back("  " + out + " = call i32 " + invoke_ptr +
                           "(ptr " + binding.storage_ptr + ", i32 " +
                           args[0] + ", i32 " + args[1] + ", i32 " +
                           args[2] + ", i32 " + args[3] + ")");
  return out;
}
