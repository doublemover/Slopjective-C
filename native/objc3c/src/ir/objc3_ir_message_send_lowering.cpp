#include "ir/objc3_ir_message_send_lowering.h"

#include "lower/objc3_lowering_contract.h"

Objc3IRMessageSendLoweringPlan BuildObjc3IRMessageSendLoweringPlan(
    const std::string &selector, const std::string &dispatch_surface_family,
    const std::string &dispatch_symbol, const std::string &direct_call_symbol,
    const Objc3IRReceiverDispatchFacts &receiver_facts) {
  Objc3IRMessageSendLoweringPlan plan;
  plan.selector = selector;
  plan.dispatch_symbol = dispatch_symbol;
  plan.direct_call_symbol = direct_call_symbol;
  plan.uses_canonical_runtime_entrypoint =
      UsesCanonicalObjc3RuntimeDispatchEntrypoint(dispatch_surface_family);

  if (!plan.direct_call_symbol.empty()) {
    plan.emits_direct_dispatch = true;
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
  return plan;
}

Objc3IRReceiverDispatchPolicy BuildObjc3IROptionalMessageSendReceiverPolicy(
    const Objc3IRReceiverDispatchFacts &receiver_facts) {
  return BuildObjc3IRReceiverDispatchPolicy(receiver_facts, false);
}
