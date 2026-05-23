#include "ir/objc3_ir_statement_loop_emission.h"

#include <string>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_statement_binding_emission.h"

namespace {

constexpr const char *kObjc3RuntimeCollectionsArrayIteratorI32Symbol =
    "objc3_runtime_stdlib_collections_array_iterator_i32";
constexpr const char *kObjc3RuntimeCollectionsSetIteratorI32Symbol =
    "objc3_runtime_stdlib_collections_set_iterator_i32";
constexpr const char *kObjc3RuntimeCollectionsMapKeyIteratorI32Symbol =
    "objc3_runtime_stdlib_collections_map_key_iterator_i32";
constexpr const char *kObjc3RuntimeCollectionsMapValueIteratorI32Symbol =
    "objc3_runtime_stdlib_collections_map_value_iterator_i32";
constexpr const char *kObjc3RuntimeCollectionsIteratorNextOrI32Symbol =
    "objc3_runtime_stdlib_collections_iterator_next_or_i32";
constexpr const char *kObjc3RuntimeCollectionsLastStatusI32Symbol =
    "objc3_runtime_stdlib_collections_last_status_i32";
constexpr int kObjc3RuntimeCollectionsStatusOk = 0;

std::string LookupObjc3IRForInLocalPtr(const FunctionContext &ctx,
                                       const std::string &name) {
  for (auto it = ctx.scopes.rbegin(); it != ctx.scopes.rend(); ++it) {
    const auto found = it->find(name);
    if (found != it->end()) {
      return found->second;
    }
  }
  return "";
}

Expr::CollectionLiteralKind Objc3IRForInCollectionKind(
    const Expr *expr, const FunctionContext &ctx) {
  if (expr == nullptr) {
    return Expr::CollectionLiteralKind::None;
  }
  if (expr->kind == Expr::Kind::CollectionLiteral) {
    return expr->collection_literal_kind;
  }
  if (expr->kind == Expr::Kind::Identifier) {
    const std::string ptr = LookupObjc3IRForInLocalPtr(ctx, expr->ident);
    const auto found = ctx.collection_kind_by_ptr.find(ptr);
    if (found != ctx.collection_kind_by_ptr.end()) {
      return found->second;
    }
  }
  return Expr::CollectionLiteralKind::None;
}

}  // namespace

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

void EmitObjc3IRForInStatement(
    const ForInStmt *for_in_stmt, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  if (for_in_stmt == nullptr) {
    return;
  }
  const Expr::CollectionLiteralKind kind =
      Objc3IRForInCollectionKind(for_in_stmt->collection.get(), ctx);
  if (kind == Expr::CollectionLiteralKind::None ||
      (kind == Expr::CollectionLiteralKind::Map &&
       !for_in_stmt->has_key_binding) ||
      (kind != Expr::CollectionLiteralKind::Map &&
       for_in_stmt->has_key_binding)) {
    (void)callbacks.emit_unsupported_i32_value(
        "for-in lowering requires array/set value binding or map key,value binding with parser-visible collection origin");
    return;
  }

  const std::string collection =
      callbacks.emit_expr(for_in_stmt->collection.get(), ctx);
  const std::string iterator = callbacks.new_temp(ctx);
  std::string value_iterator;
  if (kind == Expr::CollectionLiteralKind::Array) {
    ctx.code_lines.push_back("  " + iterator + " = call i32 @" +
                             std::string(
                                 kObjc3RuntimeCollectionsArrayIteratorI32Symbol) +
                             "(i32 " + collection + ")");
  } else if (kind == Expr::CollectionLiteralKind::Set) {
    ctx.code_lines.push_back("  " + iterator + " = call i32 @" +
                             std::string(
                                 kObjc3RuntimeCollectionsSetIteratorI32Symbol) +
                             "(i32 " + collection + ")");
  } else {
    ctx.code_lines.push_back("  " + iterator + " = call i32 @" +
                             std::string(
                                 kObjc3RuntimeCollectionsMapKeyIteratorI32Symbol) +
                             "(i32 " + collection + ")");
    value_iterator = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + value_iterator + " = call i32 @" +
                             std::string(
                                 kObjc3RuntimeCollectionsMapValueIteratorI32Symbol) +
                             "(i32 " + collection + ")");
  }

  callbacks.push_scope(ctx);
  const std::string value_ptr =
      "%" + for_in_stmt->value_name + ".addr." +
      std::to_string(ctx.temp_counter++);
  ctx.entry_lines.push_back("  " + value_ptr + " = alloca i32, align 4");
  ctx.scopes.back()[for_in_stmt->value_name] = value_ptr;
  std::string key_ptr;
  if (for_in_stmt->has_key_binding) {
    key_ptr = "%" + for_in_stmt->key_name + ".addr." +
              std::to_string(ctx.temp_counter++);
    ctx.entry_lines.push_back("  " + key_ptr + " = alloca i32, align 4");
    ctx.scopes.back()[for_in_stmt->key_name] = key_ptr;
  }

  const std::string next_label = callbacks.new_label(ctx, "for_in_next_");
  const std::string body_label = callbacks.new_label(ctx, "for_in_body_");
  const std::string end_label = callbacks.new_label(ctx, "for_in_end_");
  ctx.code_lines.push_back("  br label %" + next_label);
  ctx.code_lines.push_back(next_label + ":");
  const std::string next_value = callbacks.new_temp(ctx);
  ctx.code_lines.push_back("  " + next_value + " = call i32 @" +
                           std::string(
                               kObjc3RuntimeCollectionsIteratorNextOrI32Symbol) +
                           "(i32 " + iterator + ", i32 0)");
  const std::string status = callbacks.new_temp(ctx);
  ctx.code_lines.push_back("  " + status + " = call i32 @" +
                           std::string(
                               kObjc3RuntimeCollectionsLastStatusI32Symbol) +
                           "()");
  const std::string ok = callbacks.new_temp(ctx);
  ctx.code_lines.push_back("  " + ok + " = icmp eq i32 " + status + ", " +
                           std::to_string(kObjc3RuntimeCollectionsStatusOk));
  ctx.code_lines.push_back("  br i1 " + ok + ", label %" + body_label +
                           ", label %" + end_label);

  ctx.code_lines.push_back(body_label + ":");
  if (for_in_stmt->has_key_binding) {
    ctx.code_lines.push_back("  store i32 " + next_value + ", ptr " +
                             key_ptr + ", align 4");
    const std::string value = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + value + " = call i32 @" +
                             std::string(
                                 kObjc3RuntimeCollectionsIteratorNextOrI32Symbol) +
                             "(i32 " + value_iterator + ", i32 0)");
    ctx.code_lines.push_back("  store i32 " + value + ", ptr " + value_ptr +
                             ", align 4");
  } else {
    ctx.code_lines.push_back("  store i32 " + next_value + ", ptr " +
                             value_ptr + ", align 4");
  }
  ctx.control_stack.push_back(
      {next_label, end_label, true, ctx.scopes.size() - 1u,
       ctx.autoreleasepool_scope_symbols.size(),
       ctx.pending_block_dispose_calls.size(),
       ctx.pending_ownership_cleanup_calls.size(),
       ctx.arc_owned_cleanup_ptrs.size()});
  ctx.terminated = false;
  for (const auto &s : for_in_stmt->body) {
    EmitObjc3IRStatement(s.get(), ctx, callbacks);
  }
  const bool body_terminated = ctx.terminated;
  ctx.control_stack.pop_back();
  if (!body_terminated) {
    ctx.code_lines.push_back("  br label %" + next_label);
  }
  ctx.code_lines.push_back(end_label + ":");
  callbacks.pop_scope(ctx, true);
  ctx.terminated = false;
}
