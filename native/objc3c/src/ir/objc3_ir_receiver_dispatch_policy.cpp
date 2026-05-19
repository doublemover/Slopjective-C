#include "ir/objc3_ir_receiver_dispatch_policy.h"

Objc3IRReceiverDispatchFacts Objc3IRKnownNonNilReceiverFacts() {
  Objc3IRReceiverDispatchFacts facts;
  facts.compile_time_nonzero_receiver = true;
  return facts;
}

Objc3IRReceiverDispatchPolicy BuildObjc3IRReceiverDispatchPolicy(
    const Objc3IRReceiverDispatchFacts &facts,
    bool uses_canonical_runtime_entrypoint) {
  Objc3IRReceiverDispatchPolicy policy;
  policy.elide_to_nil_result = facts.compile_time_nil_receiver;
  policy.emit_dispatch_without_nil_branch =
      !policy.elide_to_nil_result &&
      (uses_canonical_runtime_entrypoint ||
       facts.compile_time_nonzero_receiver);
  policy.emit_nil_checked_dispatch =
      !policy.elide_to_nil_result &&
      !policy.emit_dispatch_without_nil_branch;
  return policy;
}
