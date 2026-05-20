#pragma once

#include "lower/contracts/runtime_dispatch_lowering_contracts.h"
#include "ir/objc3_ir_receiver_dispatch_policy.h"

#include <string>

struct Objc3IRMessageSendLoweringPlan {
  std::string selector;
  std::string dispatch_symbol;
  std::string direct_call_symbol;
  Objc3IRReceiverDispatchPolicy receiver_dispatch_policy;
  std::string dispatch_result_owner = kObjc3IRRuntimeDispatchResultOwner;
  std::string dispatch_result_owner_model = kObjc3LoweringNoRetiredRouteOwnerModel;
  bool uses_canonical_runtime_entrypoint = false;
  bool owns_dispatch_result = false;
  bool hard_cutover_dispatch_target = false;
  bool emits_direct_dispatch = false;
  bool emits_runtime_dispatch = false;
  bool elides_to_nil_result = false;
  bool emits_nil_checked_dispatch = false;
  bool method_family_retained_result_cleanup_required = false;
  bool method_family_returns_retained_result = false;
  bool arc_mode_enabled = false;
  bool fail_closed = false;
  bool strict_no_retired_route = true;
  bool strict_no_compatibility = true;
  std::string failure_reason;
};

Objc3IRMessageSendLoweringPlan BuildObjc3IRMessageSendLoweringPlan(
    const std::string &selector, const std::string &dispatch_surface_family,
    const std::string &dispatch_symbol, const std::string &direct_call_symbol,
    const Objc3IRReceiverDispatchFacts &receiver_facts,
    bool method_family_returns_retained_result, bool arc_mode_enabled);

Objc3IRReceiverDispatchPolicy BuildObjc3IROptionalMessageSendReceiverPolicy(
    const Objc3IRReceiverDispatchFacts &receiver_facts);
