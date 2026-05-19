#include "ir/objc3_ir_concurrency_runtime_call_emission.h"

#include <string>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_runtime_helper_calls.h"
#include "lower/contracts/concurrency_actor_contracts.h"
#include "lower/contracts/concurrency_task_runtime_helper_contracts.h"
#include "support/objc3_string_predicates.h"

namespace {

std::string EmitObjc3IRRuntimeCallTemp(
    FunctionContext &ctx,
    const Objc3IRConcurrencyRuntimeCallEmissionCallbacks &callbacks) {
  return callbacks.new_temp(ctx);
}

void InvalidateObjc3IRRuntimeCallProofState(
    FunctionContext &ctx,
    const Objc3IRConcurrencyRuntimeCallEmissionCallbacks &callbacks) {
  callbacks.invalidate_global_proof_state(ctx);
}

std::string EmitObjc3IRRuntimeCallFirstArgOrZero(
    const Expr *expr,
    const Objc3IRConcurrencyRuntimeCallEmissionCallbacks &callbacks) {
  return expr->args.empty() ? "0" : callbacks.emit_expr(expr->args.front().get());
}

}  // namespace

bool TryEmitObjc3IRConcurrencyTaskRuntimeLoweringCall(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRConcurrencyRuntimeCallEmissionCallbacks &callbacks,
    std::string &result_out) {
  if (expr == nullptr || !ctx.async_runtime_helper_enabled) {
    return false;
  }

  const std::string lowered = objc3c::support::LowercaseAscii(expr->ident);
  const auto emit_unary_runtime_call = [&](const char *symbol) {
    const std::string out = EmitObjc3IRRuntimeCallTemp(ctx, callbacks);
    ctx.code_lines.push_back(BuildObjc3IRRuntimeI32CallLine(
        out, symbol, {std::to_string(ctx.async_executor_tag)}));
    InvalidateObjc3IRRuntimeCallProofState(ctx, callbacks);
    result_out = out;
  };

  if (lowered == "task_spawn_child" || lowered == "spawn_task") {
    const std::string out = EmitObjc3IRRuntimeCallTemp(ctx, callbacks);
    ctx.code_lines.push_back(BuildObjc3IRRuntimeI32CallLine(
        out, kObjc3RuntimeSpawnTaskI32Symbol,
        {"1", std::to_string(ctx.async_executor_tag)}));
    InvalidateObjc3IRRuntimeCallProofState(ctx, callbacks);
    result_out = out;
    return true;
  }
  if (lowered == "detached_task_create") {
    const std::string out = EmitObjc3IRRuntimeCallTemp(ctx, callbacks);
    ctx.code_lines.push_back(BuildObjc3IRRuntimeI32CallLine(
        out, kObjc3RuntimeSpawnTaskI32Symbol,
        {"2", std::to_string(ctx.async_executor_tag)}));
    InvalidateObjc3IRRuntimeCallProofState(ctx, callbacks);
    result_out = out;
    return true;
  }
  if (lowered == "with_task_group_scope") {
    emit_unary_runtime_call(kObjc3RuntimeEnterTaskGroupScopeI32Symbol);
    return true;
  }
  if (lowered == "task_group_add_task") {
    emit_unary_runtime_call(kObjc3RuntimeAddTaskGroupTaskI32Symbol);
    return true;
  }
  if (lowered == "task_group_cancel_all") {
    emit_unary_runtime_call(kObjc3RuntimeCancelTaskGroupI32Symbol);
    return true;
  }
  if (lowered == "task_runtime_cancelled_value") {
    emit_unary_runtime_call(kObjc3RuntimeTaskIsCancelledI32Symbol);
    return true;
  }
  if (lowered == "task_runtime_on_cancel") {
    emit_unary_runtime_call(kObjc3RuntimeTaskOnCancelI32Symbol);
    return true;
  }
  if (lowered == "task_group_wait_next" || lowered == "wait_next") {
    const std::string waited = EmitObjc3IRRuntimeCallTemp(ctx, callbacks);
    ctx.code_lines.push_back(BuildObjc3IRRuntimeI32CallLine(
        waited, kObjc3RuntimeWaitTaskGroupNextI32Symbol,
        {std::to_string(ctx.async_executor_tag)}));
    const std::string hopped = EmitObjc3IRRuntimeCallTemp(ctx, callbacks);
    ctx.code_lines.push_back(BuildObjc3IRRuntimeI32CallLine(
        hopped, kObjc3RuntimeExecutorHopI32Symbol,
        {waited, std::to_string(ctx.async_executor_tag)}));
    InvalidateObjc3IRRuntimeCallProofState(ctx, callbacks);
    result_out = hopped;
    return true;
  }
  return false;
}

bool TryEmitObjc3IRConcurrencyActorLoweringCall(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRConcurrencyRuntimeCallEmissionCallbacks &callbacks,
    std::string &result_out) {
  if (expr == nullptr || !ctx.actor_runtime_helper_enabled) {
    return false;
  }

  const std::string lowered = objc3c::support::LowercaseAscii(expr->ident);
  if (lowered == "actor_enter_isolation_thunk") {
    const std::string out = EmitObjc3IRRuntimeCallTemp(ctx, callbacks);
    ctx.code_lines.push_back(BuildObjc3IRRuntimeI32CallLine(
        out, kObjc3RuntimeActorEnterIsolationThunkI32Symbol,
        {std::to_string(ctx.async_executor_tag)}));
    InvalidateObjc3IRRuntimeCallProofState(ctx, callbacks);
    result_out = out;
    return true;
  }
  if (lowered == "actor_nonisolated_entry" &&
      ctx.actor_nonisolated_entry_enabled) {
    const std::string value =
        EmitObjc3IRRuntimeCallFirstArgOrZero(expr, callbacks);
    const std::string out = EmitObjc3IRRuntimeCallTemp(ctx, callbacks);
    ctx.code_lines.push_back(BuildObjc3IRRuntimeI32CallLine(
        out, kObjc3RuntimeActorEnterNonisolatedI32Symbol,
        {value, std::to_string(ctx.async_executor_tag)}));
    InvalidateObjc3IRRuntimeCallProofState(ctx, callbacks);
    result_out = out;
    return true;
  }
  if (lowered == "actor_hop_to_executor") {
    const std::string value =
        EmitObjc3IRRuntimeCallFirstArgOrZero(expr, callbacks);
    const std::string out = EmitObjc3IRRuntimeCallTemp(ctx, callbacks);
    ctx.code_lines.push_back(BuildObjc3IRRuntimeI32CallLine(
        out, kObjc3RuntimeActorHopToExecutorI32Symbol,
        {value, std::to_string(ctx.async_executor_tag)}));
    InvalidateObjc3IRRuntimeCallProofState(ctx, callbacks);
    result_out = out;
    return true;
  }
  if (lowered == "actor_bind_executor") {
    const std::string actor_handle =
        EmitObjc3IRRuntimeCallFirstArgOrZero(expr, callbacks);
    const std::string out = EmitObjc3IRRuntimeCallTemp(ctx, callbacks);
    ctx.code_lines.push_back(BuildObjc3IRRuntimeI32CallLine(
        out, kObjc3RuntimeActorBindExecutorI32Symbol,
        {actor_handle, std::to_string(ctx.async_executor_tag)}));
    InvalidateObjc3IRRuntimeCallProofState(ctx, callbacks);
    result_out = out;
    return true;
  }
  if (lowered == "actor_mailbox_enqueue") {
    const std::string actor_handle =
        EmitObjc3IRRuntimeCallFirstArgOrZero(expr, callbacks);
    const std::string value =
        expr->args.size() < 2u ? "0" : callbacks.emit_expr(expr->args[1].get());
    const std::string out = EmitObjc3IRRuntimeCallTemp(ctx, callbacks);
    ctx.code_lines.push_back(BuildObjc3IRRuntimeI32CallLine(
        out, kObjc3RuntimeActorMailboxEnqueueI32Symbol,
        {actor_handle, value, std::to_string(ctx.async_executor_tag)}));
    InvalidateObjc3IRRuntimeCallProofState(ctx, callbacks);
    result_out = out;
    return true;
  }
  if (lowered == "actor_mailbox_drain_next") {
    const std::string actor_handle =
        EmitObjc3IRRuntimeCallFirstArgOrZero(expr, callbacks);
    const std::string out = EmitObjc3IRRuntimeCallTemp(ctx, callbacks);
    ctx.code_lines.push_back(BuildObjc3IRRuntimeI32CallLine(
        out, kObjc3RuntimeActorMailboxDrainNextI32Symbol,
        {actor_handle, std::to_string(ctx.async_executor_tag)}));
    InvalidateObjc3IRRuntimeCallProofState(ctx, callbacks);
    result_out = out;
    return true;
  }
  if (lowered == "replay_proof_step") {
    const std::string out = EmitObjc3IRRuntimeCallTemp(ctx, callbacks);
    ctx.code_lines.push_back(BuildObjc3IRRuntimeI32CallLine(
        out, kObjc3RuntimeActorRecordReplayProofI32Symbol,
        {std::to_string(ctx.async_executor_tag)}));
    InvalidateObjc3IRRuntimeCallProofState(ctx, callbacks);
    result_out = out;
    return true;
  }
  if (lowered == "race_guard_lock") {
    const std::string out = EmitObjc3IRRuntimeCallTemp(ctx, callbacks);
    ctx.code_lines.push_back(BuildObjc3IRRuntimeI32CallLine(
        out, kObjc3RuntimeActorRecordRaceGuardI32Symbol,
        {std::to_string(ctx.async_executor_tag)}));
    InvalidateObjc3IRRuntimeCallProofState(ctx, callbacks);
    result_out = out;
    return true;
  }

  return false;
}
