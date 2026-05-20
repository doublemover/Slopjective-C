#include "ir/objc3_ir_scope_cleanup_emission_ownership.h"

#include <cstddef>
#include <string>
#include <vector>

namespace {

void EmitObjc3IROwnershipCleanupCall(
    const PendingOwnershipCleanupCall &call, FunctionContext &ctx,
    const Objc3IRScopeCleanupEmissionCallbacks &callbacks) {
  if (!call.active || call.storage_ptr.empty()) {
    return;
  }
  const std::string loaded_value = callbacks.new_temp(ctx);
  ctx.code_lines.push_back("  " + loaded_value + " = load i32, ptr " +
                           call.storage_ptr + ", align 4");
  if (!call.resource_close_symbol.empty()) {
    if (call.has_resource_invalid_value) {
      const std::string resource_live = callbacks.new_temp(ctx);
      const std::string resource_close_label =
          callbacks.new_label(ctx, "ownership_resource_close_");
      const std::string resource_skip_label =
          callbacks.new_label(ctx, "ownership_resource_skip_");
      ctx.code_lines.push_back("  " + resource_live + " = icmp ne i32 " +
                               loaded_value + ", " +
                               std::to_string(call.resource_invalid_value));
      ctx.code_lines.push_back("  br i1 " + resource_live + ", label %" +
                               resource_close_label + ", label %" +
                               resource_skip_label);
      ctx.code_lines.push_back(resource_close_label + ":");
      ctx.code_lines.push_back("  call void @" + call.resource_close_symbol +
                               "(i32 " + loaded_value + ")");
      ctx.code_lines.push_back("  br label %" + resource_skip_label);
      ctx.code_lines.push_back(resource_skip_label + ":");
    } else {
      ctx.code_lines.push_back("  call void @" + call.resource_close_symbol +
                               "(i32 " + loaded_value + ")");
    }
  }
  if (!call.cleanup_function_symbol.empty()) {
    ctx.code_lines.push_back("  call void @" + call.cleanup_function_symbol +
                             "(i32 " + loaded_value + ")");
  }
}

void EmitObjc3IROwnershipCleanupTerminalCall(
    const PendingOwnershipCleanupCall &call, std::vector<std::string> &out_lines,
    int &temp_counter) {
  if (!call.active || call.storage_ptr.empty()) {
    return;
  }
  const std::string loaded_value = "%t" + std::to_string(temp_counter++);
  out_lines.push_back("  " + loaded_value + " = load i32, ptr " +
                      call.storage_ptr + ", align 4");
  if (!call.resource_close_symbol.empty()) {
    if (call.has_resource_invalid_value) {
      const std::string resource_live = "%t" + std::to_string(temp_counter++);
      const std::string resource_close_label =
          "ownership_resource_close_" + std::to_string(temp_counter++);
      const std::string resource_skip_label =
          "ownership_resource_skip_" + std::to_string(temp_counter++);
      out_lines.push_back("  " + resource_live + " = icmp ne i32 " +
                          loaded_value + ", " +
                          std::to_string(call.resource_invalid_value));
      out_lines.push_back("  br i1 " + resource_live + ", label %" +
                          resource_close_label + ", label %" +
                          resource_skip_label);
      out_lines.push_back(resource_close_label + ":");
      out_lines.push_back("  call void @" + call.resource_close_symbol +
                          "(i32 " + loaded_value + ")");
      out_lines.push_back("  br label %" + resource_skip_label);
      out_lines.push_back(resource_skip_label + ":");
    } else {
      out_lines.push_back("  call void @" + call.resource_close_symbol +
                          "(i32 " + loaded_value + ")");
    }
  }
  if (!call.cleanup_function_symbol.empty()) {
    out_lines.push_back("  call void @" + call.cleanup_function_symbol +
                        "(i32 " + loaded_value + ")");
  }
}

}  // namespace

void EmitObjc3IROwnershipCleanupUnwindToDepth(
    FunctionContext &ctx, std::size_t target_depth,
    const Objc3IRScopeCleanupEmissionCallbacks &callbacks) {
  while (ctx.pending_ownership_cleanup_calls.size() > target_depth) {
    const PendingOwnershipCleanupCall call =
        ctx.pending_ownership_cleanup_calls.back();
    if (!call.binding_name.empty()) {
      ctx.ownership_cleanup_call_indices.erase(call.binding_name);
    }
    ctx.pending_ownership_cleanup_calls.pop_back();
    EmitObjc3IROwnershipCleanupCall(call, ctx, callbacks);
  }
}

void DiscardObjc3IROwnershipCleanupToDepth(FunctionContext &ctx,
                                           std::size_t target_depth) {
  while (ctx.pending_ownership_cleanup_calls.size() > target_depth) {
    const PendingOwnershipCleanupCall call =
        ctx.pending_ownership_cleanup_calls.back();
    if (!call.binding_name.empty()) {
      ctx.ownership_cleanup_call_indices.erase(call.binding_name);
    }
    ctx.pending_ownership_cleanup_calls.pop_back();
  }
}

void EmitObjc3IROwnershipCleanupTerminalCleanupToDepth(
    const FunctionContext &ctx, std::size_t target_depth,
    std::vector<std::string> &out_lines, int &temp_counter) {
  for (std::size_t index = ctx.pending_ownership_cleanup_calls.size();
       index > target_depth; --index) {
    EmitObjc3IROwnershipCleanupTerminalCall(
        ctx.pending_ownership_cleanup_calls[index - 1u], out_lines,
        temp_counter);
  }
}
