#include "ir/objc3_ir_function_definition_emission.h"

#include <cstddef>
#include <memory>
#include <sstream>
#include <vector>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_concurrency_identity.h"
#include "ir/objc3_ir_function_signature_model.h"
#include "ir/objc3_ir_receiver_identity_contracts.h"
#include "ir/objc3_ir_type_model.h"

namespace {

std::string BuildObjc3IRFunctionDefinitionSignature(
    const std::vector<FuncParam> &params, bool throws_declared) {
  std::ostringstream signature;
  for (std::size_t i = 0; i < params.size(); ++i) {
    if (i != 0) {
      signature << ", ";
    }
    signature << LLVMScalarType(params[i].type) << " %arg" << i;
  }
  if (throws_declared) {
    if (!params.empty()) {
      signature << ", ";
    }
    signature << "ptr %error_out";
  }
  return signature.str();
}

void EmitObjc3IRParameterStores(
    const std::vector<FuncParam> &params,
    const Objc3IRFunctionDefinitionEmissionCallbacks &callbacks,
    FunctionContext &ctx) {
  for (std::size_t i = 0; i < params.size(); ++i) {
    const auto &param = params[i];
    const std::string ptr =
        "%" + param.name + ".addr." + std::to_string(ctx.temp_counter++);
    ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
    callbacks.emit_typed_param_store(param, i, ptr, ctx);
    ctx.scopes.back()[param.name] = ptr;
  }
}

void EmitObjc3IRStatementBody(
    const std::vector<std::unique_ptr<Stmt>> &body,
    const Objc3IRFunctionDefinitionEmissionCallbacks &callbacks,
    FunctionContext &ctx) {
  for (const auto &stmt : body) {
    callbacks.emit_statement(stmt.get(), ctx);
    if (ctx.terminated) {
      break;
    }
  }
}

void EmitObjc3IRDefinitionContextLines(const FunctionContext &ctx,
                                       std::ostringstream &out) {
  for (const auto &line : ctx.entry_lines) {
    out << line << "\n";
  }
  for (const auto &line : ctx.code_lines) {
    out << line << "\n";
  }
}

}  // namespace

void EmitObjc3IRFunctionDefinition(
    const FunctionDecl &fn, bool arc_mode_enabled,
    const Objc3IRFunctionDefinitionEmissionCallbacks &callbacks,
    std::ostringstream &out) {
  out << "define " << LLVMScalarType(fn.return_type) << " @" << fn.name << "("
      << BuildObjc3IRFunctionDefinitionSignature(fn.params, fn.throws_declared)
      << ") {\n";
  out << "entry:\n";

  FunctionContext ctx;
  ctx.return_type = fn.return_type;
  ctx.async_runtime_helper_enabled =
      fn.async_declared && Objc3IRExecutorAffinityTag(fn) != 0;
  ctx.async_resume_entry_tag = Objc3IRAsyncResumeEntryTag(fn);
  ctx.async_executor_tag = Objc3IRExecutorAffinityTag(fn);
  if (fn.throws_declared) {
    ctx.function_error_out_param = "%error_out";
  }
  ctx.arc_return_insert_retain =
      EffectiveArcReturnInsertRetain(fn, arc_mode_enabled);
  ctx.arc_return_insert_autorelease = EffectiveArcReturnInsertAutorelease(fn);
  callbacks.push_scope(ctx);
  callbacks.seed_known_class_receiver_bindings(ctx);

  EmitObjc3IRParameterStores(fn.params, callbacks, ctx);
  EmitObjc3IRStatementBody(fn.body, callbacks, ctx);

  if (!ctx.terminated) {
    callbacks.emit_autoreleasepool_unwind_to_depth(ctx, 0u);
    callbacks.emit_typed_return("0", ctx);
  }

  EmitObjc3IRDefinitionContextLines(ctx, out);
  out << "}\n";
}

void EmitObjc3IRMethodDefinition(
    const Objc3IRMethodDefinition &method_def, bool arc_mode_enabled,
    const Objc3IRFunctionDefinitionEmissionCallbacks &callbacks,
    std::ostringstream &out) {
  if (method_def.method == nullptr) {
    callbacks.emit_synthetic_method(method_def, out);
    return;
  }
  const Objc3MethodDecl &method = *method_def.method;
  out << "define " << LLVMScalarType(method.return_type) << " @"
      << method_def.symbol << "("
      << BuildObjc3IRFunctionDefinitionSignature(method.params,
                                                 method.throws_declared)
      << ") {\n";
  out << "entry:\n";

  FunctionContext ctx;
  ctx.return_type = method.return_type;
  ctx.async_runtime_helper_enabled =
      method.async_declared && Objc3IRExecutorAffinityTag(method) != 0;
  ctx.actor_runtime_helper_enabled =
      callbacks.is_actor_implementation(method_def.implementation_name);
  ctx.actor_nonisolated_entry_enabled =
      ctx.actor_runtime_helper_enabled && method.objc_nonisolated_declared;
  ctx.current_implementation_name = method_def.implementation_name;
  ctx.current_superclass_name = method_def.superclass_name;
  ctx.current_method_is_class_method = method.is_class_method;
  ctx.async_resume_entry_tag = Objc3IRAsyncResumeEntryTag(method_def);
  ctx.async_executor_tag = Objc3IRExecutorAffinityTag(method);
  if (method.throws_declared) {
    ctx.function_error_out_param = "%error_out";
  }
  ctx.arc_return_insert_retain =
      EffectiveArcReturnInsertRetain(method, arc_mode_enabled);
  ctx.arc_return_insert_autorelease =
      EffectiveArcReturnInsertAutorelease(method);
  callbacks.push_scope(ctx);
  callbacks.seed_known_class_receiver_bindings(ctx);
  const int implementation_class_identity =
      callbacks.lookup_class_receiver_identity_value(
          method_def.implementation_name);
  const int self_identity =
      method.is_class_method
          ? BuildClassReceiverIdentityValue(implementation_class_identity)
          : BuildInstanceReceiverIdentityValue(implementation_class_identity);
  if (self_identity != 0) {
    ctx.immediate_identifiers["self"] = self_identity;
  }
  const int super_class_identity =
      callbacks.lookup_class_receiver_identity_value(method_def.superclass_name);
  const int super_identity =
      method.is_class_method
          ? BuildClassReceiverIdentityValue(super_class_identity)
          : BuildInstanceReceiverIdentityValue(super_class_identity);
  if (super_identity != 0) {
    ctx.immediate_identifiers["super"] = super_identity;
  }

  EmitObjc3IRParameterStores(method.params, callbacks, ctx);
  EmitObjc3IRStatementBody(method.body, callbacks, ctx);

  if (!ctx.terminated) {
    callbacks.emit_autoreleasepool_unwind_to_depth(ctx, 0u);
    callbacks.emit_typed_return("0", ctx);
  }

  EmitObjc3IRDefinitionContextLines(ctx, out);
  out << "}\n";
}
