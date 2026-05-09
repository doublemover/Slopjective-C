#pragma once

#include "ir/objc3_ir_receiver_dispatch_policy.h"

#include <string>

struct Objc3IRMessageSendLoweringPlan {
  std::string selector;
  std::string dispatch_symbol;
  std::string direct_call_symbol;
  Objc3IRReceiverDispatchPolicy receiver_dispatch_policy;
  bool uses_canonical_runtime_entrypoint = false;
  bool emits_direct_dispatch = false;
  bool emits_runtime_dispatch = false;
  bool elides_to_nil_result = false;
  bool emits_nil_checked_dispatch = false;
  bool fail_closed = false;
  std::string failure_reason;
};

Objc3IRMessageSendLoweringPlan BuildObjc3IRMessageSendLoweringPlan(
    const std::string &selector, const std::string &dispatch_surface_family,
    const std::string &dispatch_symbol, const std::string &direct_call_symbol,
    const Objc3IRReceiverDispatchFacts &receiver_facts);

Objc3IRReceiverDispatchPolicy BuildObjc3IROptionalMessageSendReceiverPolicy(
    const Objc3IRReceiverDispatchFacts &receiver_facts);
