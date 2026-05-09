#pragma once

#include <cstddef>
#include <functional>
#include <iosfwd>
#include <string>

#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_method_definition_plan.h"

struct FuncParam;
struct FunctionDecl;
struct Stmt;

struct Objc3IRFunctionDefinitionEmissionCallbacks {
  std::function<void(FunctionContext &ctx)> push_scope;
  std::function<void(FunctionContext &ctx)> seed_known_class_receiver_bindings;
  std::function<void(const FuncParam &param, std::size_t index,
                     const std::string &ptr, FunctionContext &ctx)>
      emit_typed_param_store;
  std::function<void(const Stmt *stmt, FunctionContext &ctx)> emit_statement;
  std::function<void(FunctionContext &ctx, std::size_t depth)>
      emit_autoreleasepool_unwind_to_depth;
  std::function<void(const std::string &i32_value, FunctionContext &ctx)>
      emit_typed_return;
  std::function<bool(const std::string &name)> is_actor_implementation;
  std::function<int(const std::string &class_name)>
      lookup_class_receiver_identity_value;
  std::function<void(const Objc3IRMethodDefinition &method_def,
                     std::ostringstream &out)>
      emit_synthetic_method;
};

void EmitObjc3IRFunctionDefinition(
    const FunctionDecl &fn, bool arc_mode_enabled,
    const Objc3IRFunctionDefinitionEmissionCallbacks &callbacks,
    std::ostringstream &out);

void EmitObjc3IRMethodDefinition(
    const Objc3IRMethodDefinition &method_def, bool arc_mode_enabled,
    const Objc3IRFunctionDefinitionEmissionCallbacks &callbacks,
    std::ostringstream &out);
