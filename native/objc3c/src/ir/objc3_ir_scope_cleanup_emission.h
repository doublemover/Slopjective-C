#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

#include "ir/objc3_ir_emitter_context.h"

struct Stmt;

struct Objc3IRScopeCleanupEmissionCallbacks {
  std::function<std::string(FunctionContext &ctx)> new_temp;
  std::function<std::string(FunctionContext &ctx, const std::string &prefix)>
      new_label;
  std::function<void(const Stmt *stmt, FunctionContext &ctx)> emit_statement;
};

void EmitObjc3IRAutoreleasepoolUnwindToDepth(FunctionContext &ctx,
                                             std::size_t target_depth);

void PushObjc3IRScope(FunctionContext &ctx);

void EmitObjc3IRDeferredCleanupTerminalToDepth(
    FunctionContext &ctx, std::size_t target_scope_depth,
    const Objc3IRScopeCleanupEmissionCallbacks &callbacks);

void EmitObjc3IRDeferredAndOwnershipCleanupTerminalToDepth(
    FunctionContext &ctx, std::size_t target_scope_depth,
    std::size_t target_ownership_cleanup_depth,
    const Objc3IRScopeCleanupEmissionCallbacks &callbacks);

void EmitObjc3IRPendingBlockDisposeUnwindToDepth(FunctionContext &ctx,
                                                 std::size_t target_depth);

void EmitObjc3IRPendingBlockDisposeTerminalCleanupToDepth(
    const FunctionContext &ctx, std::size_t target_depth,
    std::vector<std::string> &out_lines);

void DiscardObjc3IRPendingBlockDisposeToDepth(FunctionContext &ctx,
                                              std::size_t target_depth);

void EmitObjc3IROwnershipCleanupUnwindToDepth(
    FunctionContext &ctx, std::size_t target_depth,
    const Objc3IRScopeCleanupEmissionCallbacks &callbacks);

void EmitObjc3IROwnershipCleanupTerminalCleanupToDepth(
    const FunctionContext &ctx, std::size_t target_depth,
    std::vector<std::string> &out_lines, int &temp_counter);

void PopObjc3IRScope(FunctionContext &ctx, bool emit_cleanup,
                     const Objc3IRScopeCleanupEmissionCallbacks &callbacks);

void EmitObjc3IRArcOwnedCleanupUnwindToDepth(
    FunctionContext &ctx, std::size_t target_depth,
    const Objc3IRScopeCleanupEmissionCallbacks &callbacks);

void DiscardObjc3IRArcOwnedCleanupToDepth(FunctionContext &ctx,
                                          std::size_t target_depth);

void EmitObjc3IRArcOwnedTerminalCleanupToDepth(
    const FunctionContext &ctx, std::size_t target_depth,
    std::vector<std::string> &out_lines, int &temp_counter);

void RegisterObjc3IRArcOwnedCleanupPtr(const std::string &ptr,
                                       FunctionContext &ctx);

void EmitObjc3IRArcOwnedCleanupReleases(
    FunctionContext &ctx,
    const Objc3IRScopeCleanupEmissionCallbacks &callbacks);

void EmitObjc3IRTerminalCleanupToDepth(
    FunctionContext &ctx, std::size_t scope_depth,
    std::size_t autoreleasepool_depth,
    std::size_t pending_block_dispose_depth,
    std::size_t ownership_cleanup_depth, std::size_t arc_cleanup_depth,
    const Objc3IRScopeCleanupEmissionCallbacks &callbacks);
