#include "ir/objc3_ir_scope_cleanup_emission.h"

#include <string>
#include <utility>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_scope_cleanup_emission_ownership.h"
#include "lower/contracts/ownership_runtime_memory_management_contracts.h"

namespace {

void EmitObjc3IRDeferredCleanupBlock(
    const BlockStmt *block_stmt, FunctionContext &ctx,
    const Objc3IRScopeCleanupEmissionCallbacks &callbacks);

void EmitObjc3IRDeferredCleanupForScopeBucket(
    const std::vector<const BlockStmt *> &bucket, FunctionContext &ctx,
    const Objc3IRScopeCleanupEmissionCallbacks &callbacks) {
  // Defer cleanup execution pushes/pops lexical scopes while replaying each
  // deferred block. Copy the bucket first so those scope-vector mutations do
  // not invalidate the source bucket reference mid-iteration when one scope
  // owns multiple defer blocks.
  const std::vector<const BlockStmt *> stable_bucket = bucket;
  for (std::size_t index = stable_bucket.size(); index > 0; --index) {
    EmitObjc3IRDeferredCleanupBlock(stable_bucket[index - 1u], ctx, callbacks);
  }
}

void EmitObjc3IRDeferredCleanupBlock(
    const BlockStmt *block_stmt, FunctionContext &ctx,
    const Objc3IRScopeCleanupEmissionCallbacks &callbacks) {
  // defer/guard lowering anchor: defer bodies now lower into
  // explicit LIFO scope cleanups that execute inside the same lexical cleanup
  // pipeline as existing block-dispose and ARC-owned teardown, rather than
  // running eagerly at the original statement site.
  if (block_stmt == nullptr) {
    return;
  }
  const bool outer_terminated = ctx.terminated;
  ctx.terminated = false;
  const std::size_t autoreleasepool_depth =
      ctx.autoreleasepool_scope_symbols.size();
  if (block_stmt->is_autoreleasepool_scope) {
    ctx.code_lines.push_back(
        "  call void @" +
        std::string(kObjc3RuntimePushAutoreleasepoolScopeSymbol) + "()");
    ctx.autoreleasepool_scope_symbols.push_back(
        block_stmt->autoreleasepool_scope_symbol);
  }
  PushObjc3IRScope(ctx);
  for (const auto &nested_stmt : block_stmt->body) {
    callbacks.emit_statement(nested_stmt.get(), ctx);
  }
  const bool block_terminated = ctx.terminated;
  PopObjc3IRScope(ctx, !block_terminated, callbacks);
  if (!block_terminated) {
    EmitObjc3IRAutoreleasepoolUnwindToDepth(ctx, autoreleasepool_depth);
  }
  ctx.terminated = outer_terminated;
}

}  // namespace

void EmitObjc3IRAutoreleasepoolUnwindToDepth(FunctionContext &ctx,
                                             std::size_t target_depth) {
  while (ctx.autoreleasepool_scope_symbols.size() > target_depth) {
    ctx.code_lines.push_back("  call void @" +
                             std::string(
                                 kObjc3RuntimePopAutoreleasepoolScopeSymbol) +
                             "()");
    ctx.autoreleasepool_scope_symbols.pop_back();
  }
}

void PushObjc3IRScope(FunctionContext &ctx) {
  ctx.scopes.push_back({});
  ctx.pending_defer_scope_blocks.push_back({});
  ctx.pending_block_dispose_scope_depths.push_back(
      ctx.pending_block_dispose_calls.size());
  ctx.pending_ownership_cleanup_scope_depths.push_back(
      ctx.pending_ownership_cleanup_calls.size());
  ctx.arc_cleanup_scope_depths.push_back(ctx.arc_owned_cleanup_ptrs.size());
}

void EmitObjc3IRDeferredCleanupTerminalToDepth(
    FunctionContext &ctx, std::size_t target_scope_depth,
    const Objc3IRScopeCleanupEmissionCallbacks &callbacks) {
  for (std::size_t index = ctx.pending_defer_scope_blocks.size();
       index > target_scope_depth; --index) {
    EmitObjc3IRDeferredCleanupForScopeBucket(
        ctx.pending_defer_scope_blocks[index - 1u], ctx, callbacks);
  }
}

void EmitObjc3IRPendingBlockDisposeUnwindToDepth(FunctionContext &ctx,
                                                 std::size_t target_depth) {
  while (ctx.pending_block_dispose_calls.size() > target_depth) {
    const PendingBlockDisposeCall call = ctx.pending_block_dispose_calls.back();
    ctx.pending_block_dispose_calls.pop_back();
    if (call.helper_symbol.empty() || call.storage_ptr.empty()) {
      continue;
    }
    ctx.code_lines.push_back("  call void @" + call.helper_symbol + "(ptr " +
                             call.storage_ptr + ")");
  }
}

void EmitObjc3IRPendingBlockDisposeTerminalCleanupToDepth(
    const FunctionContext &ctx, std::size_t target_depth,
    std::vector<std::string> &out_lines) {
  for (std::size_t index = ctx.pending_block_dispose_calls.size();
       index > target_depth; --index) {
    const PendingBlockDisposeCall &call =
        ctx.pending_block_dispose_calls[index - 1u];
    if (call.helper_symbol.empty() || call.storage_ptr.empty()) {
      continue;
    }
    out_lines.push_back("  call void @" + call.helper_symbol + "(ptr " +
                        call.storage_ptr + ")");
  }
}

void DiscardObjc3IRPendingBlockDisposeToDepth(FunctionContext &ctx,
                                              std::size_t target_depth) {
  while (ctx.pending_block_dispose_calls.size() > target_depth) {
    ctx.pending_block_dispose_calls.pop_back();
  }
}

void PopObjc3IRScope(FunctionContext &ctx, bool emit_cleanup,
                     const Objc3IRScopeCleanupEmissionCallbacks &callbacks) {
  if (ctx.scopes.empty()) {
    return;
  }
  const auto scope_bindings = std::move(ctx.scopes.back());
  ctx.scopes.pop_back();
  const auto deferred_blocks =
      ctx.pending_defer_scope_blocks.empty()
          ? std::vector<const BlockStmt *>{}
          : std::move(ctx.pending_defer_scope_blocks.back());
  if (!ctx.pending_defer_scope_blocks.empty()) {
    ctx.pending_defer_scope_blocks.pop_back();
  }
  const std::size_t target_block_dispose_depth =
      ctx.pending_block_dispose_scope_depths.empty()
          ? 0u
          : ctx.pending_block_dispose_scope_depths.back();
  if (!ctx.pending_block_dispose_scope_depths.empty()) {
    ctx.pending_block_dispose_scope_depths.pop_back();
  }
  const std::size_t target_ownership_cleanup_depth =
      ctx.pending_ownership_cleanup_scope_depths.empty()
          ? 0u
          : ctx.pending_ownership_cleanup_scope_depths.back();
  if (!ctx.pending_ownership_cleanup_scope_depths.empty()) {
    ctx.pending_ownership_cleanup_scope_depths.pop_back();
  }
  const std::size_t target_arc_cleanup_depth =
      ctx.arc_cleanup_scope_depths.empty() ? 0u
                                           : ctx.arc_cleanup_scope_depths.back();
  if (!ctx.arc_cleanup_scope_depths.empty()) {
    ctx.arc_cleanup_scope_depths.pop_back();
  }
  if (emit_cleanup) {
    EmitObjc3IRDeferredCleanupForScopeBucket(deferred_blocks, ctx, callbacks);
    EmitObjc3IROwnershipCleanupUnwindToDepth(
        ctx, target_ownership_cleanup_depth, callbacks);
    EmitObjc3IRPendingBlockDisposeUnwindToDepth(ctx,
                                                target_block_dispose_depth);
    EmitObjc3IRArcOwnedCleanupUnwindToDepth(ctx, target_arc_cleanup_depth,
                                            callbacks);
  } else {
    DiscardObjc3IROwnershipCleanupToDepth(ctx,
                                          target_ownership_cleanup_depth);
    DiscardObjc3IRPendingBlockDisposeToDepth(ctx,
                                             target_block_dispose_depth);
    DiscardObjc3IRArcOwnedCleanupToDepth(ctx, target_arc_cleanup_depth);
  }
  for (const auto &binding : scope_bindings) {
    ctx.block_bindings.erase(binding.first);
    ctx.ownership_cleanup_call_indices.erase(binding.first);
  }
}

void EmitObjc3IRArcOwnedCleanupUnwindToDepth(
    FunctionContext &ctx, std::size_t target_depth,
    const Objc3IRScopeCleanupEmissionCallbacks &callbacks) {
  while (ctx.arc_owned_cleanup_ptrs.size() > target_depth) {
    const std::string ptr = ctx.arc_owned_cleanup_ptrs.back();
    ctx.arc_owned_cleanup_ptrs.pop_back();
    ctx.arc_owned_cleanup_ptr_set.erase(ptr);
    ctx.arc_owned_storage_ptrs.erase(ptr);
    for (auto it = ctx.arc_method_family_cleanup_ptr_by_value.begin();
         it != ctx.arc_method_family_cleanup_ptr_by_value.end();) {
      if (it->second == ptr) {
        it = ctx.arc_method_family_cleanup_ptr_by_value.erase(it);
      } else {
        ++it;
      }
    }
    if (ptr.empty()) {
      continue;
    }
    const std::string loaded_value = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + loaded_value + " = load i32, ptr " + ptr +
                             ", align 4");
    const std::string released_value = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + released_value + " = call i32 @" +
                             std::string(kObjc3RuntimeReleaseI32Symbol) +
                             "(i32 " + loaded_value + ")");
    (void)released_value;
  }
}

void DiscardObjc3IRArcOwnedCleanupToDepth(FunctionContext &ctx,
                                          std::size_t target_depth) {
  while (ctx.arc_owned_cleanup_ptrs.size() > target_depth) {
    const std::string ptr = ctx.arc_owned_cleanup_ptrs.back();
    ctx.arc_owned_cleanup_ptrs.pop_back();
    ctx.arc_owned_cleanup_ptr_set.erase(ptr);
    ctx.arc_owned_storage_ptrs.erase(ptr);
    for (auto it = ctx.arc_method_family_cleanup_ptr_by_value.begin();
         it != ctx.arc_method_family_cleanup_ptr_by_value.end();) {
      if (it->second == ptr) {
        it = ctx.arc_method_family_cleanup_ptr_by_value.erase(it);
      } else {
        ++it;
      }
    }
  }
}

void EmitObjc3IRArcOwnedTerminalCleanupToDepth(
    const FunctionContext &ctx, std::size_t target_depth,
    std::vector<std::string> &out_lines, int &temp_counter) {
  for (std::size_t index = ctx.arc_owned_cleanup_ptrs.size();
       index > target_depth; --index) {
    const std::string &ptr = ctx.arc_owned_cleanup_ptrs[index - 1u];
    if (ptr.empty()) {
      continue;
    }
    const std::string loaded_value = "%t" + std::to_string(temp_counter++);
    out_lines.push_back("  " + loaded_value + " = load i32, ptr " + ptr +
                        ", align 4");
    const std::string released_value = "%t" + std::to_string(temp_counter++);
    out_lines.push_back("  " + released_value + " = call i32 @" +
                        std::string(kObjc3RuntimeReleaseI32Symbol) + "(i32 " +
                        loaded_value + ")");
    (void)released_value;
  }
}

void RegisterObjc3IRArcOwnedCleanupPtr(const std::string &ptr,
                                       FunctionContext &ctx) {
  if (ptr.empty() || !ctx.arc_owned_cleanup_ptr_set.insert(ptr).second) {
    return;
  }
  ctx.arc_owned_cleanup_ptrs.push_back(ptr);
  ctx.arc_owned_storage_ptrs.insert(ptr);
}

void EmitObjc3IRArcOwnedCleanupReleases(
    FunctionContext &ctx,
    const Objc3IRScopeCleanupEmissionCallbacks &callbacks) {
  EmitObjc3IRArcOwnedCleanupUnwindToDepth(ctx, 0u, callbacks);
}

void EmitObjc3IRTerminalCleanupToDepth(
    FunctionContext &ctx, std::size_t scope_depth,
    std::size_t autoreleasepool_depth,
    std::size_t pending_block_dispose_depth,
    std::size_t ownership_cleanup_depth, std::size_t arc_cleanup_depth,
    const Objc3IRScopeCleanupEmissionCallbacks &callbacks) {
  // Keep all terminal exits on the same source-construct cleanup order:
  // deferred statements first, then explicit ownership cleanup, block dispose,
  // ARC-owned storage release, and finally autoreleasepool draining.
  EmitObjc3IRDeferredCleanupTerminalToDepth(ctx, scope_depth, callbacks);
  EmitObjc3IROwnershipCleanupTerminalCleanupToDepth(
      ctx, ownership_cleanup_depth, ctx.code_lines, ctx.temp_counter);
  EmitObjc3IRPendingBlockDisposeTerminalCleanupToDepth(
      ctx, pending_block_dispose_depth, ctx.code_lines);
  EmitObjc3IRArcOwnedTerminalCleanupToDepth(
      ctx, arc_cleanup_depth, ctx.code_lines, ctx.temp_counter);
  EmitObjc3IRAutoreleasepoolUnwindToDepth(ctx, autoreleasepool_depth);
}
