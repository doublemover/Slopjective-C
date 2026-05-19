#include "lower/contracts/block_arc_lowering_plan.h"

#include "ast/objc3_ast_block_surface.h"

#include <sstream>

Objc3BlockArcLoweringPlan Objc3BuildBlockArcLoweringPlan(const Expr &expr) {
  Objc3BlockArcLoweringPlan plan;
  plan.block_literal = Objc3ExprIsBlockLiteral(expr);
  plan.normalized = expr.block_literal_is_normalized;
  plan.stack_invoke_lowering = Objc3BlockCanUseStackInvokeLowering(expr);
  plan.byref_storage_required = Objc3BlockRequiresByrefStorage(expr);
  plan.copy_helper_required = expr.block_copy_helper_required ||
                              expr.block_runtime_copy_helper_required;
  plan.dispose_helper_required = expr.block_dispose_helper_required ||
                                 expr.block_runtime_dispose_helper_required;
  plan.runtime_promotion_required = Objc3BlockRequiresRuntimePromotion(expr);
  plan.ownership_runtime_required =
      expr.block_runtime_owned_object_capture_count > 0 ||
      expr.block_runtime_weak_object_capture_count > 0 ||
      expr.block_runtime_unowned_object_capture_count > 0;
  plan.parameter_count = expr.block_parameter_count;
  plan.capture_count = expr.block_capture_count;
  plan.byref_capture_count = expr.block_byref_capture_count;
  plan.owned_capture_count = expr.block_runtime_owned_object_capture_count;
  plan.descriptor_symbol = expr.block_abi_descriptor_symbol;
  plan.invoke_symbol = expr.block_invoke_trampoline_symbol;
  plan.copy_helper_symbol = expr.block_copy_helper_symbol;
  plan.dispose_helper_symbol = expr.block_dispose_helper_symbol;
  plan.replay_key = Objc3BlockArcLoweringPlanReplayKey(plan);
  return plan;
}

bool Objc3BlockArcLoweringPlanIsRunnable(
    const Objc3BlockArcLoweringPlan &plan) {
  return plan.block_literal && plan.normalized &&
         (plan.stack_invoke_lowering || plan.runtime_promotion_required) &&
         !plan.invoke_symbol.empty();
}

std::string Objc3BlockArcLoweringPlanReplayKey(
    const Objc3BlockArcLoweringPlan &plan) {
  std::ostringstream out;
  out << "block=" << (plan.block_literal ? "true" : "false")
      << ";normalized=" << (plan.normalized ? "true" : "false")
      << ";stack_invoke=" << (plan.stack_invoke_lowering ? "true" : "false")
      << ";byref=" << (plan.byref_storage_required ? "true" : "false")
      << ";copy=" << (plan.copy_helper_required ? "true" : "false")
      << ";dispose=" << (plan.dispose_helper_required ? "true" : "false")
      << ";promotion="
      << (plan.runtime_promotion_required ? "true" : "false")
      << ";ownership_runtime="
      << (plan.ownership_runtime_required ? "true" : "false")
      << ";params=" << plan.parameter_count
      << ";captures=" << plan.capture_count
      << ";byref_captures=" << plan.byref_capture_count
      << ";owned_captures=" << plan.owned_capture_count
      << ";descriptor=" << plan.descriptor_symbol
      << ";invoke=" << plan.invoke_symbol
      << ";copy_helper=" << plan.copy_helper_symbol
      << ";dispose_helper=" << plan.dispose_helper_symbol;
  return out.str();
}
