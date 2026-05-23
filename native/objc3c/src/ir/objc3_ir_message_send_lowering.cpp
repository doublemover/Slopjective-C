#include "ir/objc3_ir_message_send_lowering.h"

#include "lower/objc3_lowering_contract.h"

Objc3IRMessageSendLoweringPlan BuildObjc3IRMessageSendLoweringPlan(
    const std::string &selector, const std::string &dispatch_surface_family,
    const std::string &dispatch_symbol, const std::string &direct_call_symbol,
    const Objc3IRReceiverDispatchFacts &receiver_facts,
    bool method_family_returns_retained_result, bool arc_mode_enabled) {
  Objc3IRMessageSendLoweringPlan plan;
  plan.selector = selector;
  plan.dispatch_symbol = dispatch_symbol;
  plan.direct_call_symbol = direct_call_symbol;
  plan.method_family_returns_retained_result =
      method_family_returns_retained_result;
  plan.arc_mode_enabled = arc_mode_enabled;
  plan.method_family_retained_result_cleanup_required =
      arc_mode_enabled && method_family_returns_retained_result;
  plan.cache_aware_abi_available =
      kObjc3RuntimeCacheAwareDispatchAbiAvailable;
  plan.uses_canonical_runtime_entrypoint =
      UsesCanonicalObjc3RuntimeDispatchEntrypoint(dispatch_surface_family);
  plan.owns_dispatch_result = Objc3LoweringStrictOwnerModelIsReady(
      plan.dispatch_result_owner, plan.dispatch_result_owner_model,
      plan.strict_no_retired_route, plan.strict_no_compatibility);
  plan.hard_cutover_dispatch_target =
      plan.uses_canonical_runtime_entrypoint &&
      plan.dispatch_symbol ==
          kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol;

  if (!plan.direct_call_symbol.empty()) {
    plan.emits_direct_dispatch = true;
    plan.hard_cutover_dispatch_target = true;
    return plan;
  }

  if (!plan.hard_cutover_dispatch_target) {
    plan.fail_closed = true;
    plan.failure_reason =
        "runtime dispatch lowering requires the canonical hard-cutover entrypoint";
    return plan;
  }

  if (!plan.owns_dispatch_result) {
    plan.fail_closed = true;
    plan.failure_reason =
        "runtime dispatch result is missing explicit IR ownership";
    return plan;
  }

  if (RequiresFailClosedObjc3RuntimeDispatchError(dispatch_surface_family)) {
    plan.fail_closed = true;
    plan.failure_reason =
        "direct dispatch lowering remains unsupported on the live runtime path";
    return plan;
  }

  plan.receiver_dispatch_policy = BuildObjc3IRReceiverDispatchPolicy(
      receiver_facts, plan.uses_canonical_runtime_entrypoint);
  plan.elides_to_nil_result = plan.receiver_dispatch_policy.elide_to_nil_result;
  plan.emits_nil_checked_dispatch =
      plan.receiver_dispatch_policy.emit_nil_checked_dispatch;
  plan.emits_runtime_dispatch = !plan.elides_to_nil_result;
  plan.emits_cache_aware_dispatch =
      plan.cache_aware_abi_available && plan.emits_runtime_dispatch;
  return plan;
}

Objc3IRReceiverDispatchPolicy BuildObjc3IROptionalMessageSendReceiverPolicy(
    const Objc3IRReceiverDispatchFacts &receiver_facts) {
  return BuildObjc3IRReceiverDispatchPolicy(receiver_facts, false);
}
