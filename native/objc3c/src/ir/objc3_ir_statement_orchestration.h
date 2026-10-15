#pragma once

#include <functional>
#include <string>

#include "ir/objc3_ir_block_lowering.h"
#include "ir/objc3_ir_compile_time_proof_analysis.h"
#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_function_signature_model.h"
#include "ir/objc3_ir_function_local_flow.h"
#include "ir/objc3_ir_scope_cleanup_emission.h"
#include "ir/objc3_ir_value_materialization.h"

struct Expr;
struct Stmt;

struct Objc3IRStatementOrchestrationServices {
  std::function<std::string(const Expr *expr, FunctionContext &ctx)> emit_expr;
  std::function<std::string(FunctionContext &ctx)> new_temp;
  std::function<std::string(FunctionContext &ctx, const std::string &prefix)>
      new_label;
  std::function<std::string(const std::string &reason)>
      emit_unsupported_i32_value;
  std::function<Objc3IRBlockLoweringContext()> build_block_lowering_context;
  std::function<Objc3IRValueMaterializationContext()>
      build_value_materialization_context;
  std::function<Objc3IRCompileTimeProofAnalysisContext()>
      build_compile_time_proof_analysis_context;
  std::function<const LoweredFunctionSignature *(const std::string &name)>
      lookup_function_signature;
};

struct Objc3IRStatementOrchestrationOptions {
  bool arc_mode_enabled = false;
  Objc3IRStatementOrchestrationServices services;
};

Objc3IRScopeCleanupEmissionCallbacks
BuildObjc3IRStatementOrchestrationScopeCleanupCallbacks(
    const Objc3IRStatementOrchestrationOptions &options);

Objc3IRFunctionLocalFlowContext
BuildObjc3IRStatementOrchestrationFunctionLocalContext(
    const Objc3IRStatementOrchestrationOptions &options);

void EmitObjc3IRStatementOrchestration(
    const Stmt *stmt, FunctionContext &ctx,
    const Objc3IRStatementOrchestrationOptions &options);
