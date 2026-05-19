#pragma once

#include <cstddef>
#include <string>

#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_scope_cleanup_emission.h"

struct FuncParam;

struct Objc3IRFunctionLocalFlowContext {
  bool arc_mode_enabled = false;
  Objc3IRScopeCleanupEmissionCallbacks scope_cleanup_callbacks;
};

std::string CoerceObjc3IRI32ToBoolI1(const std::string &i32_value,
                                     FunctionContext &ctx);

std::string CoerceObjc3IRValueToI32(const std::string &value,
                                    ValueType value_type,
                                    FunctionContext &ctx);

std::string BuildObjc3IRThrowsErrorSlotAlloca(FunctionContext &ctx,
                                              const std::string &prefix);

void EmitObjc3IRStoreThrownError(const std::string &error_value,
                                 const std::string &slot,
                                 FunctionContext &ctx);

std::string EmitObjc3IRLoadThrownError(const std::string &slot,
                                       FunctionContext &ctx);

void EmitObjc3IRFunctionLocalTerminalCleanupToDepth(
    FunctionContext &ctx, std::size_t scope_depth,
    std::size_t autoreleasepool_depth, std::size_t pending_block_dispose_depth,
    std::size_t ownership_cleanup_depth, std::size_t arc_cleanup_depth,
    const Objc3IRFunctionLocalFlowContext &flow_context);

void EmitObjc3IRFunctionLocalTypedReturn(
    const std::string &i32_value, FunctionContext &ctx,
    const Objc3IRFunctionLocalFlowContext &flow_context);

void EmitObjc3IRPropagateThrownError(
    const std::string &error_value, FunctionContext &ctx,
    const Objc3IRFunctionLocalFlowContext &flow_context);

void EmitObjc3IRFunctionLocalTypedParamStore(
    const FuncParam &param, std::size_t index, const std::string &ptr,
    FunctionContext &ctx,
    const Objc3IRFunctionLocalFlowContext &flow_context);
