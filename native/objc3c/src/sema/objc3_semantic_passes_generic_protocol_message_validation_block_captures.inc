static bool ResolveBlockCaptureSemanticType(
    const std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    const std::string &name,
    SemanticTypeInfo &resolved_type) {
  if (const SemanticTypeInfo *local_type = ScopeLookupTypeOrNull(scopes, name)) {
    resolved_type = *local_type;
    return true;
  }
  const auto global_it = globals.find(name);
  if (global_it != globals.end()) {
    resolved_type = MakeSemanticTypeFromGlobal(global_it->second);
    return true;
  }
  return false;
}

static void ValidateBlockLiteralCaptureLegality(
    const Expr *expr,
    const std::vector<SemanticScope> &scopes,
    const std::unordered_map<std::string, ValueType> &globals,
    std::vector<std::string> &diagnostics) {
  if (expr == nullptr || expr->kind != Expr::Kind::BlockLiteral) {
    return;
  }

  Expr *mutable_expr = const_cast<Expr *>(expr);

#include "sema/objc3_semantic_passes_generic_protocol_message_validation_block_captures_capture_collection.inc"

#include "sema/objc3_semantic_passes_generic_protocol_message_validation_block_captures_message_interaction_diagnostics.inc"

#include "sema/objc3_semantic_passes_generic_protocol_message_validation_block_captures_generic_protocol_relation_checks.inc"

#include "sema/objc3_semantic_passes_generic_protocol_message_validation_block_captures_readiness_replay.inc"
}
