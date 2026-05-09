#pragma once

#include <functional>
#include <map>
#include <string>
#include <unordered_set>

#include "ir/objc3_ir_block_lowering.h"
#include "ir/objc3_ir_emitter_context.h"

struct Expr;

struct Objc3IRValueMaterializationContext {
  const std::unordered_set<std::string> &globals;
  const std::map<std::string, TypedKeyPathArtifact> &typed_keypath_artifacts;
  std::function<std::string(FunctionContext &ctx)> new_temp;
  std::function<std::string(const std::string &reason)>
      emit_unsupported_i32_value;
  std::function<Objc3IRBlockLoweringContext()> build_block_lowering_context;
};

std::string LookupObjc3IRVarPtr(
    const FunctionContext &ctx, const std::string &name,
    const Objc3IRValueMaterializationContext &materialization_context);

std::string EmitObjc3IRIdentifierValue(
    const std::string &name, FunctionContext &ctx,
    const Objc3IRValueMaterializationContext &materialization_context);

std::string EmitObjc3IRTypedKeyPathLiteralValue(
    const Expr &expr,
    const Objc3IRValueMaterializationContext &materialization_context);
