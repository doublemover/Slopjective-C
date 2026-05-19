#include "ir/objc3_ir_message_send_emission.h"

#include <cstddef>
#include <string>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_control_flow_ops.h"
#include "ir/objc3_ir_message_send_lowering.h"
#include "ir/objc3_ir_receiver_dispatch_policy.h"
#include "ir/objc3_ir_runtime_dispatch_calls.h"
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

std::string TryResolveObjc3IRDirectDispatchSymbol(
    const Expr *expr, const FunctionContext &ctx,
    const Objc3IRMessageSendEmissionOptions &options) {
  if (expr == nullptr || expr->receiver == nullptr || expr->selector.empty()) {
    return {};
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
    return {};
  }

  const auto symbol_it = options.direct_dispatch_symbols_by_key.find(
      BuildDirectDispatchMethodKey(owner_name, expr->selector,
                                   is_class_method));
  if (symbol_it == options.direct_dispatch_symbols_by_key.end()) {
    return {};
  }
  return symbol_it->second;
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

  lowered.receiver_dispatch_facts.compile_time_nil_receiver =
      callbacks.is_compile_time_nil_receiver_expr(expr->receiver.get(), ctx);
  lowered.receiver_dispatch_facts.compile_time_nonzero_receiver =
      callbacks.is_compile_time_known_non_nil_expr(expr->receiver.get(), ctx);
  lowered.receiver = callbacks.emit_expr(expr->receiver.get(), ctx);
  lowered.selector = expr->selector;
  lowered.dispatch_surface_family = expr->dispatch_surface_family_symbol;
  lowered.dispatch_surface_entrypoint_family =
      expr->dispatch_surface_entrypoint_family_symbol;
  lowered.dispatch_symbol =
      UsesCanonicalObjc3RuntimeDispatchEntrypoint(
          lowered.dispatch_surface_family)
          ? options.runtime_dispatch_symbol
          : Objc3DispatchSurfaceRuntimeEntrypointSymbol(
                lowered.dispatch_surface_family);
  lowered.direct_call_symbol =
      TryResolveObjc3IRDirectDispatchSymbol(expr, ctx, options);
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
          lowered.receiver_dispatch_facts);
  if (plan.emits_direct_dispatch) {
    // dispatch-control lowering anchor: concrete self/known-class
    // sends that target effective objc_direct methods now lower as exact LLVM
    // direct calls instead of routing through the runtime dispatch entrypoint.
    const std::string direct_value = callbacks.new_temp(ctx);
    Objc3IRDirectDispatchCallRequest request;
    request.result_value = direct_value;
    request.callee_symbol = plan.direct_call_symbol;
    request.args = lowered.args;
    request.explicit_arg_count = lowered.explicit_arg_count;
    if (!Objc3IRDirectDispatchCallRequestOwnsResult(request)) {
      return callbacks.emit_unsupported_i32_value(
          "direct dispatch result is missing explicit IR ownership");
    }
    ctx.code_lines.push_back(BuildObjc3IRDirectDispatchCall(request));
    options.runtime_dispatch_call_state.NoteDirectDispatchCall();
    callbacks.invalidate_global_proof_state(ctx);
    return direct_value;
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
    // and dynamic sends all call objc3_runtime_dispatch_i32 directly.
    // live-dispatch cutover anchor: supported dynamic sends now
    // join instance/class/super on objc3_runtime_dispatch_i32, nil semantics
    // for canonical surfaces stay runtime-owned, and reserved direct dispatch
    // surfaces still fail closed before IR emission.
    // live-dispatch gate anchor: supported live sends emit only
    // objc3_runtime_dispatch_i32 calls here; any missing owner or
    // non-canonical target fails closed before IR call construction.
    // live-dispatch smoke/replay closeout anchor: smoke and replay
    // now treat canonical runtime dispatch evidence as authoritative, so
    // emitted live sends must continue to surface objc3_runtime_dispatch_i32
    // even for nil-result paths that return 0 through the runtime.
    // lookup/dispatch runtime freeze anchor: emitted IR still
    // targets only the canonical lookup/dispatch boundary and does not
    // materialize runtime selector-table, method-cache, or slow-path helper
    // symbols. Runtime-owned details stay behind the explicit
    // objc3_runtime_lookup_selector / objc3_runtime_dispatch_i32 boundary.
    Objc3IRRuntimeDispatchCallRequest request;
    request.result_value = dispatch_value;
    request.result_owner = plan.dispatch_result_owner;
    request.result_owner_model = plan.dispatch_result_owner_model;
    request.dispatch_symbol = plan.dispatch_symbol;
    request.receiver = lowered.receiver;
    request.selector_ptr = selector_ptr;
    request.args = lowered.args;
    request.strict_no_retired_route = plan.strict_no_retired_route;
    request.strict_no_compatibility = plan.strict_no_compatibility;
    if (!Objc3IRRuntimeDispatchCallRequestOwnsResult(request)) {
      failure_reason =
          "runtime dispatch result is missing explicit IR ownership";
      return false;
    }
    ctx.code_lines.push_back(BuildObjc3IRRuntimeDispatchCall(request));
    options.runtime_dispatch_call_state.NoteRuntimeDispatchCall(
        plan.dispatch_symbol);
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
      return EmitObjc3IRRuntimeDispatch(lowered, ctx, options, callbacks);
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
    return out;
  }
  const LoweredMessageSend lowered =
      LowerObjc3IRMessageSendExpr(expr, ctx, options, callbacks);
  return EmitObjc3IRRuntimeDispatch(lowered, ctx, options, callbacks);
}
