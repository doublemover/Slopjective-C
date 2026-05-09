#include "ir/objc3_ir_statement_orchestration.h"

#include <cstddef>
#include <string>

#include "ir/objc3_ir_block_lowering.h"
#include "ir/objc3_ir_compile_time_proof_analysis.h"
#include "ir/objc3_ir_function_local_flow.h"
#include "ir/objc3_ir_scope_cleanup_emission.h"
#include "ir/objc3_ir_statement_emission.h"
#include "ir/objc3_ir_statement_flow_context.h"

namespace {

Objc3IRStatementFlowContextServices BuildObjc3IRStatementFlowServices(
    const Objc3IRStatementOrchestrationOptions &options) {
  return Objc3IRStatementFlowContextServices{
      options.services.new_temp,
      options.services.new_label,
      [options](const Stmt *stmt, FunctionContext &ctx) {
        EmitObjc3IRStatementOrchestration(stmt, ctx, options);
      }};
}

}  // namespace

Objc3IRScopeCleanupEmissionCallbacks
BuildObjc3IRStatementOrchestrationScopeCleanupCallbacks(
    const Objc3IRStatementOrchestrationOptions &options) {
  return BuildObjc3IRStatementFlowScopeCleanupCallbacks(
      BuildObjc3IRStatementFlowServices(options));
}

Objc3IRFunctionLocalFlowContext
BuildObjc3IRStatementOrchestrationFunctionLocalContext(
    const Objc3IRStatementOrchestrationOptions &options) {
  return BuildObjc3IRStatementFlowFunctionLocalContext(
      options.arc_mode_enabled, BuildObjc3IRStatementFlowServices(options));
}

void EmitObjc3IRStatementOrchestration(
    const Stmt *stmt, FunctionContext &ctx,
    const Objc3IRStatementOrchestrationOptions &options) {
  EmitObjc3IRStatement(
      stmt, ctx,
      Objc3IRStatementEmissionCallbacks{
          options.services.emit_expr,
          [&options](const Expr &expr, FunctionContext &callback_ctx) {
            return EmitObjc3IRBlockLiteralStorage(
                expr, callback_ctx,
                options.services.build_block_lowering_context());
          },
          options.services.new_temp,
          options.services.new_label,
          [&options](const FunctionContext &callback_ctx,
                     const std::string &name) {
            return LookupObjc3IRVarPtr(
                callback_ctx, name,
                options.services.build_value_materialization_context());
          },
          [&options](const Expr *expr, const FunctionContext &callback_ctx,
                     int &value) {
            return TryGetObjc3IRCompileTimeI32ExprInContext(
                expr, callback_ctx, value,
                options.services.build_compile_time_proof_analysis_context());
          },
          [&options](const Expr *expr, const FunctionContext &callback_ctx) {
            return IsObjc3IRCompileTimeNilReceiverExprInContext(
                expr, callback_ctx,
                options.services.build_compile_time_proof_analysis_context());
          },
          options.services.emit_unsupported_i32_value,
          PushObjc3IRScope,
          [&options](FunctionContext &callback_ctx, bool emit_cleanup) {
            PopObjc3IRScope(
                callback_ctx, emit_cleanup,
                BuildObjc3IRStatementOrchestrationScopeCleanupCallbacks(
                    options));
          },
          EmitObjc3IRAutoreleasepoolUnwindToDepth,
          [&options](const std::string &i32_value,
                     FunctionContext &callback_ctx) {
            EmitObjc3IRFunctionLocalTypedReturn(
                i32_value, callback_ctx,
                BuildObjc3IRStatementOrchestrationFunctionLocalContext(
                    options));
          },
          [&options](FunctionContext &callback_ctx, std::size_t scope_depth,
                     std::size_t autoreleasepool_depth,
                     std::size_t pending_block_dispose_depth,
                     std::size_t ownership_cleanup_depth,
                     std::size_t arc_cleanup_depth) {
            EmitObjc3IRFunctionLocalTerminalCleanupToDepth(
                callback_ctx, scope_depth, autoreleasepool_depth,
                pending_block_dispose_depth, ownership_cleanup_depth,
                arc_cleanup_depth,
                BuildObjc3IRStatementOrchestrationFunctionLocalContext(
                    options));
          },
          BuildObjc3IRThrowsErrorSlotAlloca,
          EmitObjc3IRLoadThrownError,
          [&options](const std::string &error_value,
                     FunctionContext &callback_ctx) {
            EmitObjc3IRPropagateThrownError(
                error_value, callback_ctx,
                BuildObjc3IRStatementOrchestrationFunctionLocalContext(
                    options));
          }});
}
