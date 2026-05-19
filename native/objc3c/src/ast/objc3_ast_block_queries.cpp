#include "ast/objc3_ast_block_queries.h"

#include <sstream>

bool Objc3ExprIsBlockLiteral(const Expr &expr) {
  return expr.kind == Expr::Kind::BlockLiteral;
}

bool Objc3BlockRequiresByrefStorage(const Expr &expr) {
  return Objc3ExprIsBlockLiteral(expr) &&
         (expr.block_storage_requires_byref_cells ||
          expr.block_byref_capture_count > 0 ||
          expr.block_storage_byref_slot_count > 0);
}

bool Objc3BlockRequiresRuntimeCopyDispose(const Expr &expr) {
  return Objc3ExprIsBlockLiteral(expr) &&
         (expr.block_copy_helper_required || expr.block_dispose_helper_required ||
          expr.block_runtime_copy_helper_required ||
          expr.block_runtime_dispose_helper_required ||
          expr.block_runtime_owned_object_capture_count > 0);
}

bool Objc3BlockRequiresRuntimePromotion(const Expr &expr) {
  return Objc3ExprIsBlockLiteral(expr) &&
         (expr.block_storage_escape_to_heap ||
          expr.block_escape_shape_promotes_to_heap_candidate);
}

bool Objc3BlockCanUseStackInvokeLowering(const Expr &expr) {
  if (!Objc3ExprIsBlockLiteral(expr) || !expr.block_literal_is_normalized) {
    return false;
  }
  if (expr.block_storage_escape_to_heap || Objc3BlockRequiresRuntimePromotion(expr)) {
    return false;
  }
  return expr.block_abi_has_invoke_trampoline;
}

std::string Objc3BlockLoweringReplayKey(const Expr &expr) {
  std::ostringstream out;
  out << "kind=" << (Objc3ExprIsBlockLiteral(expr) ? "block" : "non-block")
      << ";normalized=" << (expr.block_literal_is_normalized ? "true" : "false")
      << ";params=" << expr.block_parameter_count
      << ";captures=" << expr.block_capture_count
      << ";byref=" << expr.block_byref_capture_count
      << ";owned=" << expr.block_runtime_owned_object_capture_count
      << ";weak=" << expr.block_runtime_weak_object_capture_count
      << ";unowned=" << expr.block_runtime_unowned_object_capture_count
      << ";requires_byref="
      << (Objc3BlockRequiresByrefStorage(expr) ? "true" : "false")
      << ";requires_copy_dispose="
      << (Objc3BlockRequiresRuntimeCopyDispose(expr) ? "true" : "false")
      << ";requires_promotion="
      << (Objc3BlockRequiresRuntimePromotion(expr) ? "true" : "false")
      << ";stack_invoke="
      << (Objc3BlockCanUseStackInvokeLowering(expr) ? "true" : "false")
      << ";descriptor=" << expr.block_abi_descriptor_symbol
      << ";invoke=" << expr.block_invoke_trampoline_symbol
      << ";source_replay=" << expr.block_source_model_replay_key;
  return out.str();
}
