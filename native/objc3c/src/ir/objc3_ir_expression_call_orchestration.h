#pragma once

#include <cstddef>
#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "ir/objc3_ir_block_lowering.h"
#include "ir/objc3_ir_compile_time_proof_analysis.h"
#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_function_local_flow.h"
#include "ir/objc3_ir_function_signature_model.h"
#include "ir/objc3_ir_runtime_dispatch_state.h"

struct Expr;

struct Objc3IRExpressionCallEmissionServices {
  std::function<std::string(FunctionContext &ctx)> new_temp;
  std::function<std::string(FunctionContext &ctx, const std::string &prefix)>
      new_label;
  std::function<std::string(const std::string &reason)>
      emit_unsupported_i32_value;
  std::function<void(FunctionContext &ctx)> invalidate_global_proof_state;
  std::function<std::string(const std::string &name, FunctionContext &ctx)>
      emit_identifier_value;
  std::function<std::string(const Expr &expr)> emit_typed_keypath_literal_value;
  std::function<const LoweredFunctionSignature *(const std::string &name)>
      lookup_function_signature;
  std::function<Objc3IRBlockLoweringContext()> build_block_lowering_context;
  std::function<Objc3IRFunctionLocalFlowContext()>
      build_function_local_flow_context;
  std::function<Objc3IRCompileTimeProofAnalysisContext()>
      build_compile_time_proof_analysis_context;
};

struct Objc3IRExpressionCallEmissionOptions {
  const std::map<std::string, std::string> &selector_pool_globals;
  const std::unordered_map<std::string, int> &class_receiver_constants;
  const std::unordered_map<std::string, std::string>
      &direct_dispatch_symbols_by_key;
  std::size_t runtime_dispatch_arg_slots = 0;
  std::string runtime_dispatch_symbol;
  Objc3IRRuntimeDispatchCallState &runtime_dispatch_call_state;
  bool arc_mode_enabled = false;
  const std::unordered_set<std::string> &defined_functions;
  const std::unordered_set<std::string> &declared_pure_functions;
  const std::unordered_set<std::string> &impure_functions;
  Objc3IRExpressionCallEmissionServices services;
};

std::string EmitObjc3IRExpressionCall(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRExpressionCallEmissionOptions &options);
