#include "ir/objc3_ir_value_materialization.h"

#include <string>

#include "ast/objc3_ast.h"

std::string LookupObjc3IRVarPtr(
    const FunctionContext &ctx, const std::string &name,
    const Objc3IRValueMaterializationContext &materialization_context) {
  for (auto it = ctx.scopes.rbegin(); it != ctx.scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      return found->second;
    }
  }
  if (materialization_context.globals.find(name) !=
      materialization_context.globals.end()) {
    return "@" + name;
  }
  return "";
}

std::string EmitObjc3IRIdentifierValue(
    const std::string &name, FunctionContext &ctx,
    const Objc3IRValueMaterializationContext &materialization_context) {
  auto block_it = ctx.block_bindings.find(name);
  if (block_it != ctx.block_bindings.end()) {
    return EmitObjc3IRPromotedBlockHandleLoad(
        block_it->second, ctx,
        materialization_context.build_block_lowering_context());
  }
  const std::string ptr =
      LookupObjc3IRVarPtr(ctx, name, materialization_context);
  if (!ptr.empty()) {
    const std::string tmp = materialization_context.new_temp(ctx);
    ctx.code_lines.push_back("  " + tmp + " = load i32, ptr " + ptr +
                             ", align 4");
    return tmp;
  }
  if (materialization_context.globals.find(name) !=
      materialization_context.globals.end()) {
    const std::string tmp = materialization_context.new_temp(ctx);
    ctx.code_lines.push_back("  " + tmp + " = load i32, ptr @" + name +
                             ", align 4");
    return tmp;
  }
  const auto immediate_it = ctx.immediate_identifiers.find(name);
  if (immediate_it != ctx.immediate_identifiers.end()) {
    return std::to_string(immediate_it->second);
  }
  // Match bindings are now materialized by the executable Part 5 lowering
  // path when a live match arm captures the condition value. Remaining
  // unresolved identifiers still fail closed here.
  return materialization_context.emit_unsupported_i32_value(
      "unresolved identifier '" + name + "' during IR lowering");
}

std::string EmitObjc3IRTypedKeyPathLiteralValue(
    const Expr &expr,
    const Objc3IRValueMaterializationContext &materialization_context) {
  const std::string profile =
      expr.typed_keypath_literal_profile.empty()
          ? std::string("typed-keypath:root=") + expr.typed_keypath_root_name
          : expr.typed_keypath_literal_profile;
  const auto artifact_it =
      materialization_context.typed_keypath_artifacts.find(profile);
  if (artifact_it == materialization_context.typed_keypath_artifacts.end()) {
    return materialization_context.emit_unsupported_i32_value(
        "typed key-path artifact '" + profile +
        "' was not registered before IR lowering");
  }
  const TypedKeyPathArtifact &artifact = artifact_it->second;
  if (artifact.fallback_interpretation_allowed ||
      artifact.component_owner_identity_path.empty() ||
      artifact.component_member_identity_path.empty() ||
      artifact.component_type_identity_path.empty() ||
      artifact.source_span_id.empty() || artifact.root_type_identity.empty() ||
      artifact.value_type_identity.empty() ||
      artifact.object_model_owner_identity.empty() ||
      artifact.object_model_member_identity.empty() ||
      artifact.debug_source_map_key.empty() ||
      artifact.diagnostic_anchor_key.empty()) {
    return materialization_context.emit_unsupported_i32_value(
        "typed key-path artifact '" + profile +
        "' lacks debugger-grade type/source/object-model metadata");
  }
  return std::to_string(
      static_cast<unsigned long long>(artifact.ordinal + 1u));
}
