#pragma once

struct Objc3IRReceiverDispatchFacts {
  bool compile_time_nil_receiver = false;
  bool compile_time_nonzero_receiver = false;
};

struct Objc3IRReceiverDispatchPolicy {
  bool elide_to_nil_result = false;
  bool emit_dispatch_without_nil_branch = false;
  bool emit_nil_checked_dispatch = false;
};

Objc3IRReceiverDispatchFacts Objc3IRKnownNonNilReceiverFacts();
Objc3IRReceiverDispatchPolicy BuildObjc3IRReceiverDispatchPolicy(
    const Objc3IRReceiverDispatchFacts &facts,
    bool uses_canonical_runtime_entrypoint);
