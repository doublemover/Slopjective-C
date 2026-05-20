#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ir/objc3_ir_scope_cleanup_emission.h"

void EmitObjc3IROwnershipCleanupUnwindToDepth(
    FunctionContext &ctx, std::size_t target_depth,
    const Objc3IRScopeCleanupEmissionCallbacks &callbacks);

void DiscardObjc3IROwnershipCleanupToDepth(FunctionContext &ctx,
                                           std::size_t target_depth);

void EmitObjc3IROwnershipCleanupAtIndex(
    FunctionContext &ctx, std::size_t index,
    const Objc3IRScopeCleanupEmissionCallbacks &callbacks);

void EmitObjc3IROwnershipCleanupTerminalCleanupToDepth(
    const FunctionContext &ctx, std::size_t target_depth,
    std::vector<std::string> &out_lines, int &temp_counter);

void EmitObjc3IROwnershipCleanupTerminalAtIndex(
    const FunctionContext &ctx, std::size_t index,
    std::vector<std::string> &out_lines, int &temp_counter);
