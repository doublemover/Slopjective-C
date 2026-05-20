#include "ir/objc3_ir_block_lowering_helper_emission.h"

#include <algorithm>
#include <cstddef>
#include <sstream>
#include <string>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_block_lowering.h"
#include "ir/objc3_ir_block_lowering_internal.h"
#include "ir/objc3_ir_block_runtime_contracts.h"
#include "lower/contracts/ownership_runtime_memory_management_contracts.h"

void EmitObjc3IRBlockDescriptor(
    const Expr &expr, const Objc3IRBlockLoweringContext &context) {
  const std::string symbol = BuildBlockDescriptorSymbol(expr);
  if (symbol.empty() ||
      !context.state.emitted_block_descriptor_symbols->insert(symbol)
           .second) {
    return;
  }

  std::ostringstream out;
  const std::string descriptor_type = BuildBlockDescriptorType();
  out << "@" << symbol << " = internal constant " << descriptor_type << " { ";
  out << "i64 " << BlockStorageStaticSizeBytes(expr) << ", ";
  out << "i64 " << expr.block_capture_names_lexicographic.size() << ", ";
  out << "i32 " << expr.block_parameter_count << ", ";
  out << "i32 " << BuildBlockDescriptorFlags(expr) << ", ";
  out << "i32 0, ";
  out << "ptr @" << BuildBlockInvokeSymbol(expr) << " }, align 8\n";
  context.state.block_function_definitions->push_back(out.str());
}

void EmitObjc3IRBlockCopyHelper(
    const Expr &expr, const Objc3IRBlockLoweringContext &context) {
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
    if (!Objc3IRBlockSortedStringListContains(
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

void EmitObjc3IRBlockDisposeHelper(
    const Expr &expr, const FunctionContext &ctx,
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
    if (!Objc3IRBlockSortedStringListContains(
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
    if (Objc3IRBlockSortedStringListContains(
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

void EmitObjc3IRBlockInvokeThunk(
    const Expr &expr, const Objc3IRBlockLoweringContext &context) {
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
    const std::string slot_ptr = NewObjc3IRBlockTemp(ctx);
    const std::size_t capture_field_index = pointer_capture_storage ? 3u : 1u;
    ctx.entry_lines.push_back(
        "  " + slot_ptr + " = getelementptr inbounds " + block_storage_type +
        ", ptr %block, i32 0, i32 " +
        std::to_string(capture_field_index) + ", i32 " + std::to_string(i));
    if (pointer_capture_storage) {
      const std::string capture_ptr = NewObjc3IRBlockTemp(ctx);
      ctx.entry_lines.push_back("  " + capture_ptr + " = load ptr, ptr " +
                                slot_ptr + ", align 8");
      ctx.scopes.back()[capture_name] = capture_ptr;
      continue;
    }
    const std::string ptr =
        "%" + capture_name + ".addr." + std::to_string(ctx.temp_counter++);
    const std::string value = NewObjc3IRBlockTemp(ctx);
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
    EmitObjc3IRTerminalCleanupToDepth(
        ctx, 0u, 0u, 0u, 0u, 0u, context.scope_cleanup_callbacks);
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
