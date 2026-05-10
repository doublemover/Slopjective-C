#include "ir/objc3_ir_statement_loop_emission.h"

#include <string>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_statement_binding_emission.h"

void EmitObjc3IRWhileStatement(
    const WhileStmt *while_stmt, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  if (while_stmt == nullptr) {
    return;
  }

  const std::string cond_label =
      callbacks.new_label(ctx, "while_cond_");
  const std::string body_label =
      callbacks.new_label(ctx, "while_body_");
  const std::string end_label = callbacks.new_label(ctx, "while_end_");
  ctx.code_lines.push_back("  br label %" + cond_label);

  ctx.code_lines.push_back(cond_label + ":");
  const std::string cond =
      callbacks.emit_expr(while_stmt->condition.get(), ctx);
  const std::string cond_i1 = callbacks.new_temp(ctx);
  ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond +
                           ", 0");
  ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" +
                           body_label + ", label %" + end_label);

  ctx.code_lines.push_back(body_label + ":");
  callbacks.push_scope(ctx);
  ctx.control_stack.push_back(
      {cond_label, end_label, true, ctx.scopes.size() - 1u,
       ctx.autoreleasepool_scope_symbols.size(),
       ctx.pending_block_dispose_calls.size(),
       ctx.pending_ownership_cleanup_calls.size(),
       ctx.arc_owned_cleanup_ptrs.size()});
  ctx.terminated = false;
  for (const auto &s : while_stmt->body) {
    EmitObjc3IRStatement(s.get(), ctx, callbacks);
  }
  const bool body_terminated = ctx.terminated;
  ctx.control_stack.pop_back();
  callbacks.pop_scope(ctx, !body_terminated);
  if (!body_terminated) {
    ctx.code_lines.push_back("  br label %" + cond_label);
  }
  ctx.code_lines.push_back(end_label + ":");
  ctx.terminated = false;
}

void EmitObjc3IRDoWhileStatement(
    const DoWhileStmt *do_while_stmt, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  if (do_while_stmt == nullptr) {
    return;
  }

  const std::string body_label = callbacks.new_label(ctx, "do_body_");
  const std::string cond_label = callbacks.new_label(ctx, "do_cond_");
  const std::string end_label = callbacks.new_label(ctx, "do_end_");
  ctx.code_lines.push_back("  br label %" + body_label);

  ctx.code_lines.push_back(body_label + ":");
  callbacks.push_scope(ctx);
  ctx.control_stack.push_back(
      {cond_label, end_label, true, ctx.scopes.size() - 1u,
       ctx.autoreleasepool_scope_symbols.size(),
       ctx.pending_block_dispose_calls.size(),
       ctx.pending_ownership_cleanup_calls.size(),
       ctx.arc_owned_cleanup_ptrs.size()});
  ctx.terminated = false;
  for (const auto &s : do_while_stmt->body) {
    EmitObjc3IRStatement(s.get(), ctx, callbacks);
  }
  const bool body_terminated = ctx.terminated;
  ctx.control_stack.pop_back();
  callbacks.pop_scope(ctx, !body_terminated);
  if (!body_terminated) {
    ctx.code_lines.push_back("  br label %" + cond_label);
  }

  ctx.code_lines.push_back(cond_label + ":");
  const std::string cond =
      callbacks.emit_expr(do_while_stmt->condition.get(), ctx);
  const std::string cond_i1 = callbacks.new_temp(ctx);
  ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond +
                           ", 0");
  ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" +
                           body_label + ", label %" + end_label);

  ctx.code_lines.push_back(end_label + ":");
  ctx.terminated = false;
}

void EmitObjc3IRForStatement(
    const ForStmt *for_stmt, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  if (for_stmt == nullptr) {
    return;
  }

  callbacks.push_scope(ctx);
  EmitObjc3IRForClause(for_stmt->init, ctx, callbacks);

  const std::string cond_label = callbacks.new_label(ctx, "for_cond_");
  const std::string body_label = callbacks.new_label(ctx, "for_body_");
  const std::string step_label = callbacks.new_label(ctx, "for_step_");
  const std::string end_label = callbacks.new_label(ctx, "for_end_");

  ctx.code_lines.push_back("  br label %" + cond_label);
  ctx.code_lines.push_back(cond_label + ":");
  if (for_stmt->condition == nullptr) {
    ctx.code_lines.push_back("  br label %" + body_label);
  } else {
    const std::string cond =
        callbacks.emit_expr(for_stmt->condition.get(), ctx);
    const std::string cond_i1 = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond +
                             ", 0");
    ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" +
                             body_label + ", label %" + end_label);
  }

  ctx.code_lines.push_back(body_label + ":");
  callbacks.push_scope(ctx);
  ctx.control_stack.push_back(
      {step_label, end_label, true, ctx.scopes.size() - 1u,
       ctx.autoreleasepool_scope_symbols.size(),
       ctx.pending_block_dispose_calls.size(),
       ctx.pending_ownership_cleanup_calls.size(),
       ctx.arc_owned_cleanup_ptrs.size()});
  ctx.terminated = false;
  for (const auto &s : for_stmt->body) {
    EmitObjc3IRStatement(s.get(), ctx, callbacks);
  }
  const bool body_terminated = ctx.terminated;
  ctx.control_stack.pop_back();
  callbacks.pop_scope(ctx, !body_terminated);
  if (!body_terminated) {
    ctx.code_lines.push_back("  br label %" + step_label);
  }

  ctx.code_lines.push_back(step_label + ":");
  EmitObjc3IRForClause(for_stmt->step, ctx, callbacks);
  ctx.code_lines.push_back("  br label %" + cond_label);

  ctx.code_lines.push_back(end_label + ":");
  callbacks.pop_scope(ctx, true);
  ctx.terminated = false;
}
