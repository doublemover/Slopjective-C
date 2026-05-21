#include "ir/objc3_ir_message_send_emission.h"

#include <cstddef>
#include <string>
#include <unordered_set>
#include <vector>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_control_flow_ops.h"
#include "ir/objc3_ir_function_local_flow.h"
#include "ir/objc3_ir_message_send_lowering.h"
#include "ir/objc3_ir_receiver_dispatch_policy.h"
#include "ir/objc3_ir_runtime_dispatch_calls.h"
#include "ir/objc3_ir_scope_cleanup_emission.h"
#include "ir/objc3_ir_symbol_model.h"
#include "lower/contracts/runtime_dispatch_boundary_contracts.h"

namespace {

LoweredMessageSend LowerObjc3IRMessageSendHeader(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRMessageSendEmissionOptions &options,
    const Objc3IRMessageSendEmissionCallbacks &callbacks);

std::string EmitObjc3IRRuntimeDispatch(
    const LoweredMessageSend &lowered, FunctionContext &ctx,
    const Objc3IRMessageSendEmissionOptions &options,
    const Objc3IRMessageSendEmissionCallbacks &callbacks);

bool Objc3IRMessageSendIsEligibleForCacheAwareDispatch(
    const LoweredMessageSend &lowered,
    const Objc3IRMessageSendLoweringPlan &plan);

std::string ApplyObjc3IRMethodFamilyArcResultCleanup(
    const LoweredMessageSend &lowered, const std::string &value,
    FunctionContext &ctx, const Objc3IRMessageSendEmissionOptions &options);

void DisarmObjc3IRRelatedResultReceiverCleanup(
    const LoweredMessageSend &lowered, FunctionContext &ctx);

struct Objc3IRResolvedDirectDispatch {
  std::string symbol;
  Objc3IRDirectDispatchSignature signature;
};

Objc3IRResolvedDirectDispatch TryResolveObjc3IRDirectDispatch(
    const Expr *expr, const FunctionContext &ctx,
    const Objc3IRMessageSendEmissionOptions &options) {
  Objc3IRResolvedDirectDispatch resolved;
  if (expr == nullptr || expr->receiver == nullptr || expr->selector.empty()) {
    return resolved;
  }

  std::string owner_name;
  bool is_class_method = false;
  if (expr->receiver->kind == Expr::Kind::Identifier &&
      expr->receiver->ident == "self" &&
      !ctx.current_implementation_name.empty()) {
    owner_name = ctx.current_implementation_name;
    is_class_method = ctx.current_method_is_class_method;
  } else if (expr->receiver->kind == Expr::Kind::Identifier &&
             options.class_receiver_constants.find(expr->receiver->ident) !=
                 options.class_receiver_constants.end()) {
    owner_name = expr->receiver->ident;
    is_class_method = true;
  } else {
    return resolved;
  }

  const std::string key =
      BuildDirectDispatchMethodKey(owner_name, expr->selector,
                                   is_class_method);
  const auto symbol_it = options.direct_dispatch_symbols_by_key.find(key);
  if (symbol_it == options.direct_dispatch_symbols_by_key.end()) {
    return resolved;
  }
  const auto signature_it = options.direct_dispatch_signatures_by_key.find(key);
  if (signature_it == options.direct_dispatch_signatures_by_key.end()) {
    return resolved;
  }
  resolved.symbol = symbol_it->second;
  resolved.signature = signature_it->second;
  return resolved;
}

bool Objc3IRValueTypeUsesTypedDispatch(ValueType type) {
  switch (type) {
    case ValueType::Bool:
    case ValueType::Void:
    case ValueType::ObjCId:
    case ValueType::ObjCClass:
    case ValueType::ObjCSel:
    case ValueType::ObjCProtocol:
    case ValueType::ObjCInstancetype:
    case ValueType::ObjCObjectPtr:
      return true;
    case ValueType::Unknown:
    case ValueType::I32:
    case ValueType::Function:
    case ValueType::TextHandle:
      return false;
  }
  return false;
}

bool Objc3IRMessageSendIsEligibleForCacheAwareDispatch(
    const LoweredMessageSend &lowered,
    const Objc3IRMessageSendLoweringPlan &plan) {
  return plan.emits_cache_aware_dispatch &&
         !lowered.uses_from_class_dispatch &&
         !Objc3IRValueTypeUsesTypedDispatch(lowered.runtime_return_type) &&
         lowered.dispatch_symbol ==
             kObjc3RuntimeDispatchLoweringCanonicalEntrypointSymbol &&
         lowered.args.size() == kObjc3RuntimeDispatchDefaultArgs &&
         lowered.explicit_arg_count <= kObjc3RuntimeDispatchDefaultArgs &&
         lowered.runtime_return_type == ValueType::I32;
}

int Objc3IRRuntimeDispatchReturnKindForValueType(ValueType type) {
  switch (type) {
    case ValueType::Bool:
      return kObjc3RuntimeDispatchReturnKindBool;
    case ValueType::Void:
      return kObjc3RuntimeDispatchReturnKindVoid;
    case ValueType::ObjCId:
    case ValueType::ObjCInstancetype:
    case ValueType::ObjCObjectPtr:
      return kObjc3RuntimeDispatchReturnKindObjectReference;
    case ValueType::ObjCClass:
      return kObjc3RuntimeDispatchReturnKindClassReference;
    case ValueType::ObjCSel:
      return kObjc3RuntimeDispatchReturnKindSelectorReference;
    case ValueType::ObjCProtocol:
      return kObjc3RuntimeDispatchReturnKindProtocolReference;
    case ValueType::I32:
    case ValueType::Unknown:
    case ValueType::Function:
    case ValueType::TextHandle:
      break;
  }
  return kObjc3RuntimeDispatchReturnKindI32;
}

std::string ResolveObjc3IRMessageSendOwnerName(
    const Expr *expr, const FunctionContext &ctx,
    const Objc3IRMessageSendEmissionOptions &options,
    bool &is_class_method) {
  is_class_method = false;
  if (expr == nullptr || expr->receiver == nullptr ||
      expr->receiver->kind != Expr::Kind::Identifier) {
    return {};
  }
  const std::string &receiver_name = expr->receiver->ident;
  if (receiver_name == "super" && !ctx.current_superclass_name.empty()) {
    is_class_method = ctx.current_method_is_class_method;
    return ctx.current_superclass_name;
  }
  if (receiver_name == "self" && !ctx.current_implementation_name.empty()) {
    is_class_method = ctx.current_method_is_class_method;
    return ctx.current_implementation_name;
  }
  if (options.class_receiver_constants.find(receiver_name) !=
      options.class_receiver_constants.end()) {
    is_class_method = true;
    return receiver_name;
  }
  return {};
}

bool IsObjc3IRSuperMessageSend(const Expr *expr) {
  return expr != nullptr && expr->receiver != nullptr &&
         expr->receiver->kind == Expr::Kind::Identifier &&
         expr->receiver->ident == "super";
}

bool Objc3IRMessageFamilyProducesObjectReference(const Expr *expr) {
  if (expr == nullptr) {
    return false;
  }
  if (expr->method_family_returns_related_result) {
    return true;
  }
  return expr->method_family_name == "alloc" ||
         expr->method_family_name == "copy" ||
         expr->method_family_name == "mutableCopy" ||
         expr->method_family_name == "new" ||
         expr->method_family_name == "init";
}

ValueType ResolveObjc3IRRuntimeDispatchReturnType(
    const Expr *expr, const FunctionContext &ctx,
    const Objc3IRMessageSendEmissionOptions &options) {
  bool is_class_method = false;
  std::string owner_name =
      ResolveObjc3IRMessageSendOwnerName(expr, ctx, options, is_class_method);
  std::unordered_set<std::string> visited;
  while (!owner_name.empty() && visited.insert(owner_name).second) {
    const auto return_type_it = options.runtime_dispatch_return_types_by_key.find(
        BuildDirectDispatchMethodKey(owner_name, expr != nullptr ? expr->selector
                                                                 : "",
                                     is_class_method));
    if (return_type_it != options.runtime_dispatch_return_types_by_key.end()) {
      return return_type_it->second;
    }
    const auto superclass_it =
        options.runtime_dispatch_superclass_by_name.find(owner_name);
    owner_name = superclass_it == options.runtime_dispatch_superclass_by_name.end()
                     ? std::string{}
                     : superclass_it->second;
  }
  if (Objc3IRMessageFamilyProducesObjectReference(expr)) {
    return ValueType::ObjCId;
  }
  return ValueType::I32;
}

LoweredMessageSend LowerObjc3IRMessageSendHeader(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRMessageSendEmissionOptions &options,
    const Objc3IRMessageSendEmissionCallbacks &callbacks) {
  LoweredMessageSend lowered;
  lowered.args.assign(options.runtime_dispatch_arg_slots, "0");
  if (expr == nullptr) {
    return lowered;
  }

  bool lowered_super_receiver = false;
  if (IsObjc3IRSuperMessageSend(expr) && !ctx.current_superclass_name.empty()) {
    if (callbacks.emit_identifier_value != nullptr) {
      lowered.receiver = callbacks.emit_identifier_value("self", ctx);
      lowered.receiver_dispatch_facts = Objc3IRKnownNonNilReceiverFacts();
      lowered.uses_from_class_dispatch = true;
      lowered.lookup_start_class_name = ctx.current_superclass_name;
      lowered_super_receiver = true;
    }
  }
  if (!lowered_super_receiver) {
    lowered.receiver_dispatch_facts.compile_time_nil_receiver =
        callbacks.is_compile_time_nil_receiver_expr(expr->receiver.get(), ctx);
    lowered.receiver_dispatch_facts.compile_time_nonzero_receiver =
        callbacks.is_compile_time_known_non_nil_expr(expr->receiver.get(), ctx);
    lowered.receiver = callbacks.emit_expr(expr->receiver.get(), ctx);
  }
  lowered.selector = expr->selector;
  lowered.source_line = expr->line;
  lowered.source_column = expr->column;
  lowered.method_family_name = expr->method_family_name;
  lowered.method_family_returns_retained_result =
      expr->method_family_returns_retained_result;
  lowered.method_family_returns_related_result =
      expr->method_family_returns_related_result;
  lowered.dispatch_surface_family = expr->dispatch_surface_family_symbol;
  lowered.dispatch_surface_entrypoint_family =
      expr->dispatch_surface_entrypoint_family_symbol;
  lowered.dispatch_symbol =
      UsesCanonicalObjc3RuntimeDispatchEntrypoint(
          lowered.dispatch_surface_family)
          ? options.runtime_dispatch_symbol
          : Objc3DispatchSurfaceRuntimeEntrypointSymbol(
                lowered.dispatch_surface_family);
  lowered.runtime_return_type =
      ResolveObjc3IRRuntimeDispatchReturnType(expr, ctx, options);
  const Objc3IRResolvedDirectDispatch direct_dispatch =
      TryResolveObjc3IRDirectDispatch(expr, ctx, options);
  lowered.direct_call_symbol = direct_dispatch.symbol;
  lowered.direct_call_return_type = direct_dispatch.signature.return_type;
  lowered.direct_call_param_types = direct_dispatch.signature.param_types;
  return lowered;
}

void MaterializeObjc3IRMessageSendArgs(
    const Expr *expr, LoweredMessageSend &lowered, FunctionContext &ctx,
    const Objc3IRMessageSendEmissionCallbacks &callbacks) {
  if (expr == nullptr) {
    return;
  }
  lowered.explicit_arg_count = expr->args.size();
  for (std::size_t i = 0; i < expr->args.size() && i < lowered.args.size();
       ++i) {
    lowered.args[i] = callbacks.emit_expr(expr->args[i].get(), ctx);
  }
}

LoweredMessageSend LowerObjc3IRMessageSendExpr(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRMessageSendEmissionOptions &options,
    const Objc3IRMessageSendEmissionCallbacks &callbacks) {
  LoweredMessageSend lowered =
      LowerObjc3IRMessageSendHeader(expr, ctx, options, callbacks);
  MaterializeObjc3IRMessageSendArgs(expr, lowered, ctx, callbacks);
  return lowered;
}

std::string EmitObjc3IRRuntimeDispatch(
    const LoweredMessageSend &lowered, FunctionContext &ctx,
    const Objc3IRMessageSendEmissionOptions &options,
    const Objc3IRMessageSendEmissionCallbacks &callbacks) {
  const Objc3IRMessageSendLoweringPlan plan =
      BuildObjc3IRMessageSendLoweringPlan(
          lowered.selector, lowered.dispatch_surface_family,
          lowered.dispatch_symbol, lowered.direct_call_symbol,
          lowered.receiver_dispatch_facts,
          lowered.method_family_returns_retained_result,
          options.arc_mode_enabled);
  if (plan.emits_direct_dispatch) {
    // dispatch-control lowering anchor: concrete self/known-class
    // sends that target effective objc_direct methods now lower as exact LLVM
    // direct calls instead of routing through the runtime dispatch entrypoint.
    Objc3IRDirectDispatchCallRequest request;
    if (lowered.explicit_arg_count > lowered.direct_call_param_types.size()) {
      return callbacks.emit_unsupported_i32_value(
          "direct dispatch signature is missing explicit argument types");
    }
    const bool direct_returns_void =
        lowered.direct_call_return_type == ValueType::Void;
    const std::string direct_value =
        direct_returns_void ? std::string{} : callbacks.new_temp(ctx);
    request.result_value = direct_value;
    request.callee_symbol = plan.direct_call_symbol;
    request.return_type = lowered.direct_call_return_type;
    request.explicit_arg_count = lowered.explicit_arg_count;
    request.args.reserve(lowered.explicit_arg_count);
    request.arg_types.reserve(lowered.explicit_arg_count);
    for (std::size_t i = 0; i < lowered.explicit_arg_count; ++i) {
      ValueType arg_type = lowered.direct_call_param_types[i];
      std::string arg_value = i < lowered.args.size() ? lowered.args[i] : "0";
      if (arg_type == ValueType::Bool) {
        arg_value = CoerceObjc3IRI32ToBoolI1(arg_value, ctx);
      }
      request.args.push_back(arg_value);
      request.arg_types.push_back(arg_type);
    }
    if (!Objc3IRDirectDispatchCallRequestOwnsResult(request)) {
      return callbacks.emit_unsupported_i32_value(
          "direct dispatch result is missing explicit IR ownership");
    }
    ctx.code_lines.push_back(BuildObjc3IRDirectDispatchCall(request));
    options.runtime_dispatch_call_state.NoteDirectDispatchCall();
    callbacks.invalidate_global_proof_state(ctx);
    if (direct_returns_void) {
      return "0";
    }
    return CoerceObjc3IRValueToI32(direct_value,
                                   lowered.direct_call_return_type, ctx);
  }

  if (plan.fail_closed) {
    return callbacks.emit_unsupported_i32_value(plan.failure_reason);
  }

  if (plan.elides_to_nil_result) {
    return "0";
  }

  auto selector_it = options.selector_pool_globals.find(lowered.selector);
  if (selector_it == options.selector_pool_globals.end()) {
    return callbacks.emit_unsupported_i32_value(
        "missing selector global for message send selector '" +
        lowered.selector + "'");
  }

  const std::size_t selector_len = lowered.selector.size() + 1;
  const std::string selector_ptr = callbacks.new_temp(ctx);
  ctx.code_lines.push_back(
      "  " + selector_ptr + " = getelementptr inbounds [" +
      std::to_string(selector_len) + " x i8], ptr " + selector_it->second +
      ", i32 0, i32 0");
  options.runtime_dispatch_call_state.NoteSelectorPoolGep();

  std::string lookup_start_class_ptr;
  if (lowered.uses_from_class_dispatch) {
    const auto lookup_start_it =
        options.runtime_string_pool_globals.find(lowered.lookup_start_class_name);
    if (lookup_start_it == options.runtime_string_pool_globals.end()) {
      return callbacks.emit_unsupported_i32_value(
          "missing runtime string global for lookup-start class '" +
          lowered.lookup_start_class_name + "'");
    }
    const std::size_t lookup_start_len =
        lowered.lookup_start_class_name.size() + 1;
    lookup_start_class_ptr = callbacks.new_temp(ctx);
    ctx.code_lines.push_back(
        "  " + lookup_start_class_ptr + " = getelementptr inbounds [" +
        std::to_string(lookup_start_len) + " x i8], ptr " +
        lookup_start_it->second + ", i32 0, i32 0");
  }

  const auto emit_dispatch_call = [&](const std::string &dispatch_value,
                                      std::string &failure_reason) {
    // dispatch-surface classification anchor: instance/class/super/dynamic
    // message sends that survive folding all route through the live runtime family;
    // direct dispatch remains an explicit non-goal for this freeze.
    // dispatch legality/selector-resolution freeze anchor: lowering
    // consumes normalized selector text only, preserves explicit receiver
    // legality, and does not attempt overload or ambiguity recovery beyond the
    // fail-closed frontend contract.
    // selector-resolution implementation anchor: once lane-B sema
    // resolves concrete self/super/known-class receivers, lowering still
    // emits the same live runtime entrypoint family and relies on the
    // fail-closed exact-signature result instead of performing its own
    // overload recovery.
    // super/direct/dynamic legality expansion anchor: lowering
    // continues to route admitted super/dynamic sites through the live
    // runtime family, preserves their normalized method-family metadata, and
    // never synthesizes a reserved direct-dispatch entrypoint.
    // dispatch lowering ABI anchor: lowering emits the canonical runtime
    // entrypoint directly while selector lookup, receiver/result ABI, and
    // the fixed four-slot argument vector remain stable.
    // runtime call ABI generation anchor: normalized instance/class/super
    // and dynamic sends all call public runtime dispatch entrypoints directly.
    // live-dispatch cutover anchor: supported dynamic sends now
    // join instance/class/super on public runtime dispatch entrypoints, nil
    // semantics for canonical surfaces stay runtime-owned, and reserved direct
    // dispatch surfaces still fail closed before IR emission.
    // live-dispatch gate anchor: supported live sends emit only public runtime
    // dispatch calls here; any missing owner or non-canonical target fails
    // closed before IR call construction.
    // live-dispatch smoke/replay closeout anchor: smoke and replay
    // now treat canonical runtime dispatch evidence as authoritative, so
    // emitted live sends must continue to surface public runtime dispatch calls
    // even for nil-result paths that return 0 through the runtime.
    // lookup/dispatch runtime freeze anchor: emitted IR still
    // targets only the canonical lookup/dispatch boundary and does not
    // materialize runtime selector-table, method-cache, or slow-path helper
    // symbols. Runtime-owned details stay behind the explicit
    // objc3_runtime_lookup_selector / runtime-dispatch boundary.
    Objc3IRRuntimeDispatchCallRequest request;
    request.result_value = dispatch_value;
    request.result_owner = plan.dispatch_result_owner;
    request.result_owner_model = plan.dispatch_result_owner_model;
    const bool uses_typed_dispatch =
        Objc3IRValueTypeUsesTypedDispatch(lowered.runtime_return_type);
    if (uses_typed_dispatch && lowered.uses_from_class_dispatch) {
      request.dispatch_symbol = kObjc3RuntimeTypedDispatchValueFromClassSymbol;
    } else if (uses_typed_dispatch) {
      request.dispatch_symbol = kObjc3RuntimeTypedDispatchValueSymbol;
    } else if (lowered.uses_from_class_dispatch) {
      request.dispatch_symbol = kObjc3RuntimeDispatchFromClassSymbol;
    } else {
      request.dispatch_symbol = plan.dispatch_symbol;
    }
    request.receiver = lowered.receiver;
    request.lookup_start_class_ptr = lookup_start_class_ptr;
    request.selector_ptr = selector_ptr;
    request.args = lowered.args;
    request.uses_typed_value_dispatch = uses_typed_dispatch;
    request.uses_from_class_dispatch = lowered.uses_from_class_dispatch;
    request.expected_return_kind =
        Objc3IRRuntimeDispatchReturnKindForValueType(
            lowered.runtime_return_type);
    request.strict_no_retired_route = plan.strict_no_retired_route;
    request.strict_no_compatibility = plan.strict_no_compatibility;
    if (Objc3IRMessageSendIsEligibleForCacheAwareDispatch(lowered, plan)) {
      Objc3IRCacheAwareDispatchCallRequest cache_request;
      cache_request.result_value = dispatch_value;
      cache_request.result_envelope_value = callbacks.new_temp(ctx);
      cache_request.prepare_status_value = callbacks.new_temp(ctx);
      cache_request.prepare_status_ok_value = callbacks.new_temp(ctx);
      cache_request.status_value = callbacks.new_temp(ctx);
      cache_request.status_ok_value = callbacks.new_temp(ctx);
      cache_request.descriptor_ptr =
          "%objc3.cache_aware.dispatch.descriptor." +
          std::to_string(ctx.temp_counter++);
      cache_request.selector_ptr = selector_ptr;
      cache_request.receiver = lowered.receiver;
      cache_request.args = lowered.args;
      cache_request.source_line = lowered.source_line;
      cache_request.source_column = lowered.source_column;
      cache_request.strict_no_retired_route = plan.strict_no_retired_route;
      cache_request.strict_no_compatibility = plan.strict_no_compatibility;
      if (!Objc3IRCacheAwareDispatchCallRequestOwnsResult(cache_request)) {
        failure_reason =
            "cache-aware dispatch result is missing explicit IR ownership";
        return false;
      }
      const std::string strict_failure_label =
          callbacks.new_label(ctx, "cache_dispatch_strict_fail_");
      const std::string value_label =
          callbacks.new_label(ctx, "cache_dispatch_value_");
      const std::vector<std::string> cache_lines =
          BuildObjc3IRCacheAwareDispatchCall(cache_request,
                                             strict_failure_label, value_label);
      ctx.code_lines.insert(ctx.code_lines.end(), cache_lines.begin(),
                            cache_lines.end());
      options.runtime_dispatch_call_state.NoteCacheAwareDispatchCall();
      return true;
    }
    if (!Objc3IRRuntimeDispatchCallRequestOwnsResult(request)) {
      failure_reason =
          "runtime dispatch result is missing explicit IR ownership";
      return false;
    }
    ctx.code_lines.push_back(BuildObjc3IRRuntimeDispatchCall(request));
    options.runtime_dispatch_call_state.NoteRuntimeDispatchCall(
        request.dispatch_symbol);
    return true;
  };

  if (plan.receiver_dispatch_policy.emit_dispatch_without_nil_branch) {
    const std::string dispatch_value = callbacks.new_temp(ctx);
    std::string failure_reason;
    if (!emit_dispatch_call(dispatch_value, failure_reason)) {
      return callbacks.emit_unsupported_i32_value(failure_reason);
    }
    callbacks.invalidate_global_proof_state(ctx);
    return dispatch_value;
  }

  const std::string is_nil = callbacks.new_temp(ctx);
  const std::string nil_label = callbacks.new_label(ctx, "msg_nil_");
  const std::string dispatch_label =
      callbacks.new_label(ctx, "msg_dispatch_");
  const std::string merge_label = callbacks.new_label(ctx, "msg_merge_");
  const std::string dispatch_value = callbacks.new_temp(ctx);
  const std::string out = callbacks.new_temp(ctx);
  ctx.code_lines.push_back(BuildObjc3IRI32IsZeroLine(is_nil, lowered.receiver));
  ctx.code_lines.push_back(
      BuildObjc3IRConditionalBranchLine(is_nil, nil_label, dispatch_label));
  ctx.code_lines.push_back(BuildObjc3IRLabelLine(nil_label));
  ctx.code_lines.push_back(BuildObjc3IRBranchLine(merge_label));
  ctx.code_lines.push_back(BuildObjc3IRLabelLine(dispatch_label));
  std::string failure_reason;
  if (!emit_dispatch_call(dispatch_value, failure_reason)) {
    return callbacks.emit_unsupported_i32_value(failure_reason);
  }
  ctx.code_lines.push_back(BuildObjc3IRBranchLine(merge_label));
  ctx.code_lines.push_back(BuildObjc3IRLabelLine(merge_label));
  ctx.code_lines.push_back(BuildObjc3IRI32PhiLine(
      out, "0", nil_label, dispatch_value, dispatch_label));
  callbacks.invalidate_global_proof_state(ctx);
  return out;
}

std::string ApplyObjc3IRMethodFamilyArcResultCleanup(
    const LoweredMessageSend &lowered, const std::string &value,
    FunctionContext &ctx, const Objc3IRMessageSendEmissionOptions &options) {
  if (!options.arc_mode_enabled ||
      !lowered.method_family_returns_retained_result || value == "0") {
    return value;
  }
  DisarmObjc3IRRelatedResultReceiverCleanup(lowered, ctx);
  const std::string storage_ptr =
      "%objc3.arc.methodfamily.result.addr." +
      std::to_string(ctx.temp_counter++);
  ctx.entry_lines.push_back("  " + storage_ptr + " = alloca i32, align 4");
  ctx.entry_lines.push_back("  store i32 0, ptr " + storage_ptr +
                            ", align 4");
  ctx.code_lines.push_back(
      "  ; objc3_arc_method_family_retained_result_cleanup = " +
      lowered.method_family_name);
  ctx.code_lines.push_back("  store i32 " + value + ", ptr " + storage_ptr +
                           ", align 4");
  RegisterObjc3IRArcOwnedCleanupPtr(storage_ptr, ctx);
  ctx.arc_method_family_cleanup_ptr_by_value[value] = storage_ptr;
  return value;
}

void DisarmObjc3IRRelatedResultReceiverCleanup(
    const LoweredMessageSend &lowered, FunctionContext &ctx) {
  if (!lowered.method_family_returns_related_result) {
    return;
  }
  const auto receiver_cleanup =
      ctx.arc_method_family_cleanup_ptr_by_value.find(lowered.receiver);
  if (receiver_cleanup == ctx.arc_method_family_cleanup_ptr_by_value.end()) {
    return;
  }
  ctx.code_lines.push_back(
      "  ; objc3_arc_method_family_related_result_consumes_receiver_cleanup = " +
      lowered.method_family_name);
  ctx.code_lines.push_back("  store i32 0, ptr " + receiver_cleanup->second +
                           ", align 4");
  ctx.arc_method_family_cleanup_ptr_by_value.erase(receiver_cleanup);
}

}  // namespace

std::string EmitObjc3IRMessageSendExpr(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRMessageSendEmissionOptions &options,
    const Objc3IRMessageSendEmissionCallbacks &callbacks) {
  if (expr != nullptr && expr->optional_send_enabled) {
    LoweredMessageSend lowered =
        LowerObjc3IRMessageSendHeader(expr, ctx, options, callbacks);
    const Objc3IRReceiverDispatchPolicy receiver_dispatch_policy =
        BuildObjc3IROptionalMessageSendReceiverPolicy(
            lowered.receiver_dispatch_facts);
    if (receiver_dispatch_policy.elide_to_nil_result) {
      return "0";
    }
    if (receiver_dispatch_policy.emit_dispatch_without_nil_branch) {
      MaterializeObjc3IRMessageSendArgs(expr, lowered, ctx, callbacks);
      const std::string dispatch_value =
          EmitObjc3IRRuntimeDispatch(lowered, ctx, options, callbacks);
      return ApplyObjc3IRMethodFamilyArcResultCleanup(
          lowered, dispatch_value, ctx, options);
    }

    const std::string is_nil = callbacks.new_temp(ctx);
    const std::string nil_label = callbacks.new_label(ctx, "opt_send_nil_");
    const std::string dispatch_label =
        callbacks.new_label(ctx, "opt_send_dispatch_");
    const std::string merge_label =
        callbacks.new_label(ctx, "opt_send_merge_");
    const std::string out = callbacks.new_temp(ctx);

    ctx.code_lines.push_back(BuildObjc3IRI32IsZeroLine(is_nil, lowered.receiver));
    ctx.code_lines.push_back(
        BuildObjc3IRConditionalBranchLine(is_nil, nil_label, dispatch_label));
    ctx.code_lines.push_back(BuildObjc3IRLabelLine(nil_label));
    ctx.code_lines.push_back(BuildObjc3IRBranchLine(merge_label));
    ctx.code_lines.push_back(BuildObjc3IRLabelLine(dispatch_label));
    lowered.receiver_dispatch_facts = Objc3IRKnownNonNilReceiverFacts();
    MaterializeObjc3IRMessageSendArgs(expr, lowered, ctx, callbacks);
    const std::string dispatch_value =
        EmitObjc3IRRuntimeDispatch(lowered, ctx, options, callbacks);
    ctx.code_lines.push_back(BuildObjc3IRBranchLine(merge_label));
    ctx.code_lines.push_back(BuildObjc3IRLabelLine(merge_label));
    ctx.code_lines.push_back(BuildObjc3IRI32PhiLine(
        out, "0", nil_label, dispatch_value, dispatch_label));
    return ApplyObjc3IRMethodFamilyArcResultCleanup(
        lowered, out, ctx, options);
  }
  const LoweredMessageSend lowered =
      LowerObjc3IRMessageSendExpr(expr, ctx, options, callbacks);
  const std::string dispatch_value =
      EmitObjc3IRRuntimeDispatch(lowered, ctx, options, callbacks);
  return ApplyObjc3IRMethodFamilyArcResultCleanup(
      lowered, dispatch_value, ctx, options);
}
