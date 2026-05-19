#include "ir/objc3_ir_function_orchestration.h"

#include <cstddef>
#include <sstream>
#include <string>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_class_receiver_bindings.h"
#include "ir/objc3_ir_function_definition_emission.h"
#include "ir/objc3_ir_function_local_flow.h"
#include "ir/objc3_ir_method_definition_plan.h"
#include "ir/objc3_ir_scope_cleanup_emission.h"
#include "ir/objc3_ir_synthetic_method_emission.h"

namespace {

bool IsObjc3IRFunctionOrchestrationActorImplementation(
    const Objc3Program &program, const std::string &name) {
  if (name.empty()) {
    return false;
  }
  for (const auto &interface_decl : program.interfaces) {
    if (!interface_decl.has_category && interface_decl.name == name) {
      return interface_decl.is_actor;
    }
  }
  return false;
}

Objc3IRFunctionDefinitionEmissionCallbacks
BuildObjc3IRFunctionOrchestrationCallbacks(
    const Objc3IRFunctionOrchestrationOptions &options) {
  return Objc3IRFunctionDefinitionEmissionCallbacks{
      [](FunctionContext &ctx) { PushObjc3IRScope(ctx); },
      [&options](FunctionContext &ctx) {
        SeedObjc3IRKnownClassReceiverBindings(
            options.class_receiver_constants, ctx);
      },
      [&options](const FuncParam &param, std::size_t index,
                 const std::string &ptr, FunctionContext &ctx) {
        EmitObjc3IRFunctionLocalTypedParamStore(
            param, index, ptr, ctx,
            BuildObjc3IRStatementOrchestrationFunctionLocalContext(
                options.statement_options));
      },
      [&options](const Stmt *stmt, FunctionContext &ctx) {
        EmitObjc3IRStatementOrchestration(
            stmt, ctx, options.statement_options);
      },
      EmitObjc3IRAutoreleasepoolUnwindToDepth,
      [&options](const std::string &i32_value, FunctionContext &ctx) {
        EmitObjc3IRFunctionLocalTypedReturn(
            i32_value, ctx,
            BuildObjc3IRStatementOrchestrationFunctionLocalContext(
                options.statement_options));
      },
      [&options](const std::string &name) {
        return IsObjc3IRFunctionOrchestrationActorImplementation(
            options.program, name);
      },
      [&options](const std::string &class_name) {
        return LookupObjc3IRClassReceiverIdentityValue(
            options.class_receiver_constants, class_name);
      },
      [&options](const Objc3IRMethodDefinition &method_def,
                 std::ostringstream &out) {
        EmitObjc3IRSyntheticMethod(
            method_def, out, options.synthetic_method_stats);
      }};
}

}  // namespace

void EmitObjc3IRFunctionOrchestration(
    const FunctionDecl &fn,
    const Objc3IRFunctionOrchestrationOptions &options,
    std::ostringstream &out) {
  EmitObjc3IRFunctionDefinition(
      fn, options.arc_mode_enabled,
      BuildObjc3IRFunctionOrchestrationCallbacks(options), out);
}

void EmitObjc3IRMethodOrchestration(
    const Objc3IRMethodDefinition &method_def,
    const Objc3IRFunctionOrchestrationOptions &options,
    std::ostringstream &out) {
  EmitObjc3IRMethodDefinition(
      method_def, options.arc_mode_enabled,
      BuildObjc3IRFunctionOrchestrationCallbacks(options), out);
}
