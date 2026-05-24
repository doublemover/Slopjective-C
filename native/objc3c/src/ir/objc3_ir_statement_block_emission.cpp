#include "ir/objc3_ir_statement_block_emission.h"

#include <cctype>
#include <cstddef>
#include <string>
#include <vector>

#include "ast/objc3_ast.h"
#include "lower/contracts/error_handling_runtime_bridge_contracts.h"
#include "lower/contracts/ownership_runtime_memory_management_contracts.h"
#include "sema/objc3_typed_throws_effect_contract.h"

namespace {

int Objc3IRErrorHandlingCatchKindForTypeSpelling(
    const std::string &spelling) {
  std::string normalized;
  normalized.reserve(spelling.size());
  for (unsigned char ch : spelling) {
    if (!std::isspace(ch)) {
      normalized.push_back(
          static_cast<char>(std::tolower(static_cast<unsigned char>(ch))));
    }
  }
  if (normalized == "nserror" || normalized == "nserror*" ||
      normalized == "nserror**") {
    return 1;
  }
  if (normalized == "id<error>" || normalized == "id<error>*") {
    return 2;
  }
  return 0;
}

void CollectObjc3IRTypedThrowsPayloadsFromExpr(
    const Expr *expr,
    const Objc3IRStatementEmissionCallbacks &callbacks,
    std::vector<std::string> &typed_payloads,
    bool &saw_untyped_or_bridge) {
  if (expr == nullptr) {
    return;
  }
  if (expr->try_expression_enabled) {
    const Expr *operand =
        !expr->args.empty() ? expr->args.front().get() : expr->left.get();
    if (operand != nullptr && operand->kind == Expr::Kind::Call) {
      const LoweredFunctionSignature *signature =
          callbacks.lookup_function_signature
              ? callbacks.lookup_function_signature(operand->ident)
              : nullptr;
      if (signature != nullptr && signature->typed_throws_declared &&
          !signature->typed_throws_error_type_spelling.empty()) {
        typed_payloads.push_back(signature->typed_throws_error_type_spelling);
      } else if (signature != nullptr &&
                 (signature->throws_declared ||
                  signature->objc_nserror_declared ||
                  signature->objc_status_code_declared)) {
        saw_untyped_or_bridge = true;
      }
    }
  }
  CollectObjc3IRTypedThrowsPayloadsFromExpr(expr->receiver.get(), callbacks,
                                            typed_payloads,
                                            saw_untyped_or_bridge);
  CollectObjc3IRTypedThrowsPayloadsFromExpr(expr->left.get(), callbacks,
                                            typed_payloads,
                                            saw_untyped_or_bridge);
  CollectObjc3IRTypedThrowsPayloadsFromExpr(expr->right.get(), callbacks,
                                            typed_payloads,
                                            saw_untyped_or_bridge);
  CollectObjc3IRTypedThrowsPayloadsFromExpr(expr->third.get(), callbacks,
                                            typed_payloads,
                                            saw_untyped_or_bridge);
  for (const auto &key : expr->collection_keys) {
    CollectObjc3IRTypedThrowsPayloadsFromExpr(key.get(), callbacks,
                                              typed_payloads,
                                              saw_untyped_or_bridge);
  }
  for (const auto &value : expr->collection_values) {
    CollectObjc3IRTypedThrowsPayloadsFromExpr(value.get(), callbacks,
                                              typed_payloads,
                                              saw_untyped_or_bridge);
  }
  for (const auto &arg : expr->args) {
    CollectObjc3IRTypedThrowsPayloadsFromExpr(arg.get(), callbacks,
                                              typed_payloads,
                                              saw_untyped_or_bridge);
  }
}

void CollectObjc3IRTypedThrowsPayloadsFromStmt(
    const Stmt *stmt,
    const Objc3IRStatementEmissionCallbacks &callbacks,
    std::vector<std::string> &typed_payloads,
    bool &saw_untyped_or_bridge) {
  if (stmt == nullptr) {
    return;
  }
  switch (stmt->kind) {
  case Stmt::Kind::Let:
    CollectObjc3IRTypedThrowsPayloadsFromExpr(
        stmt->let_stmt != nullptr ? stmt->let_stmt->value.get() : nullptr,
        callbacks, typed_payloads, saw_untyped_or_bridge);
    return;
  case Stmt::Kind::Assign:
    CollectObjc3IRTypedThrowsPayloadsFromExpr(
        stmt->assign_stmt != nullptr ? stmt->assign_stmt->value.get() : nullptr,
        callbacks, typed_payloads, saw_untyped_or_bridge);
    return;
  case Stmt::Kind::Return:
    CollectObjc3IRTypedThrowsPayloadsFromExpr(
        stmt->return_stmt != nullptr ? stmt->return_stmt->value.get()
                                     : nullptr,
        callbacks, typed_payloads, saw_untyped_or_bridge);
    return;
  case Stmt::Kind::Expr:
    CollectObjc3IRTypedThrowsPayloadsFromExpr(
        stmt->expr_stmt != nullptr ? stmt->expr_stmt->value.get() : nullptr,
        callbacks, typed_payloads, saw_untyped_or_bridge);
    return;
  case Stmt::Kind::If:
    if (stmt->if_stmt != nullptr) {
      CollectObjc3IRTypedThrowsPayloadsFromExpr(
          stmt->if_stmt->condition.get(), callbacks, typed_payloads,
          saw_untyped_or_bridge);
      for (const auto &nested_stmt : stmt->if_stmt->then_body) {
        CollectObjc3IRTypedThrowsPayloadsFromStmt(
            nested_stmt.get(), callbacks, typed_payloads,
            saw_untyped_or_bridge);
      }
      for (const auto &nested_stmt : stmt->if_stmt->else_body) {
        CollectObjc3IRTypedThrowsPayloadsFromStmt(
            nested_stmt.get(), callbacks, typed_payloads,
            saw_untyped_or_bridge);
      }
    }
    return;
  case Stmt::Kind::DoWhile:
    if (stmt->do_while_stmt != nullptr) {
      for (const auto &nested_stmt : stmt->do_while_stmt->body) {
        CollectObjc3IRTypedThrowsPayloadsFromStmt(
            nested_stmt.get(), callbacks, typed_payloads,
            saw_untyped_or_bridge);
      }
      CollectObjc3IRTypedThrowsPayloadsFromExpr(
          stmt->do_while_stmt->condition.get(), callbacks, typed_payloads,
          saw_untyped_or_bridge);
    }
    return;
  case Stmt::Kind::For:
    if (stmt->for_stmt != nullptr) {
      CollectObjc3IRTypedThrowsPayloadsFromExpr(
          stmt->for_stmt->init.value.get(), callbacks, typed_payloads,
          saw_untyped_or_bridge);
      CollectObjc3IRTypedThrowsPayloadsFromExpr(
          stmt->for_stmt->condition.get(), callbacks, typed_payloads,
          saw_untyped_or_bridge);
      CollectObjc3IRTypedThrowsPayloadsFromExpr(
          stmt->for_stmt->step.value.get(), callbacks, typed_payloads,
          saw_untyped_or_bridge);
      for (const auto &nested_stmt : stmt->for_stmt->body) {
        CollectObjc3IRTypedThrowsPayloadsFromStmt(
            nested_stmt.get(), callbacks, typed_payloads,
            saw_untyped_or_bridge);
      }
    }
    return;
  case Stmt::Kind::ForIn:
    if (stmt->for_in_stmt != nullptr) {
      CollectObjc3IRTypedThrowsPayloadsFromExpr(
          stmt->for_in_stmt->collection.get(), callbacks, typed_payloads,
          saw_untyped_or_bridge);
      for (const auto &nested_stmt : stmt->for_in_stmt->body) {
        CollectObjc3IRTypedThrowsPayloadsFromStmt(
            nested_stmt.get(), callbacks, typed_payloads,
            saw_untyped_or_bridge);
      }
    }
    return;
  case Stmt::Kind::Switch:
    if (stmt->switch_stmt != nullptr) {
      CollectObjc3IRTypedThrowsPayloadsFromExpr(
          stmt->switch_stmt->condition.get(), callbacks, typed_payloads,
          saw_untyped_or_bridge);
      for (const auto &switch_case : stmt->switch_stmt->cases) {
        for (const auto &nested_stmt : switch_case.body) {
          CollectObjc3IRTypedThrowsPayloadsFromStmt(
              nested_stmt.get(), callbacks, typed_payloads,
              saw_untyped_or_bridge);
        }
      }
    }
    return;
  case Stmt::Kind::While:
    if (stmt->while_stmt != nullptr) {
      CollectObjc3IRTypedThrowsPayloadsFromExpr(
          stmt->while_stmt->condition.get(), callbacks, typed_payloads,
          saw_untyped_or_bridge);
      for (const auto &nested_stmt : stmt->while_stmt->body) {
        CollectObjc3IRTypedThrowsPayloadsFromStmt(
            nested_stmt.get(), callbacks, typed_payloads,
            saw_untyped_or_bridge);
      }
    }
    return;
  case Stmt::Kind::CollectionMutation:
    if (stmt->collection_mutation_stmt != nullptr) {
      CollectObjc3IRTypedThrowsPayloadsFromExpr(
          stmt->collection_mutation_stmt->key_or_index.get(), callbacks,
          typed_payloads, saw_untyped_or_bridge);
      CollectObjc3IRTypedThrowsPayloadsFromExpr(
          stmt->collection_mutation_stmt->value.get(), callbacks,
          typed_payloads, saw_untyped_or_bridge);
    }
    return;
  case Stmt::Kind::Block:
  case Stmt::Kind::Defer:
    if (stmt->block_stmt != nullptr) {
      for (const auto &nested_stmt : stmt->block_stmt->body) {
        CollectObjc3IRTypedThrowsPayloadsFromStmt(
            nested_stmt.get(), callbacks, typed_payloads,
            saw_untyped_or_bridge);
      }
    }
    return;
  case Stmt::Kind::Break:
  case Stmt::Kind::Continue:
  case Stmt::Kind::Empty:
    return;
  default:
    return;
  }
}

std::string SingleObjc3IRTypedThrowsPayloadForDoCatchBody(
    const BlockStmt &block_stmt,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  std::vector<std::string> typed_payloads;
  bool saw_untyped_or_bridge = false;
  for (const auto &stmt : block_stmt.body) {
    CollectObjc3IRTypedThrowsPayloadsFromStmt(
        stmt.get(), callbacks, typed_payloads, saw_untyped_or_bridge);
  }
  if (typed_payloads.empty() || saw_untyped_or_bridge) {
    return "";
  }
  const std::string first_identity =
      Objc3TypedThrowsPayloadIdentity(typed_payloads.front());
  for (const std::string &payload : typed_payloads) {
    if (Objc3TypedThrowsPayloadIdentity(payload) != first_identity) {
      return "";
    }
  }
  return typed_payloads.front();
}

}  // namespace

void EmitObjc3IRBlockStatement(
    const BlockStmt *block_stmt, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  if (block_stmt == nullptr) {
    return;
  }
  if (block_stmt->is_do_catch_scope) {
    const std::size_t autoreleasepool_depth =
        ctx.autoreleasepool_scope_symbols.size();
    const std::string dispatch_label =
        callbacks.new_label(ctx, "do_catch_dispatch_");
    const std::string merge_label =
        callbacks.new_label(ctx, "do_catch_end_");
    const std::string error_slot =
        callbacks.build_throws_error_slot_alloca(ctx, "do_catch");
    ctx.code_lines.push_back("  store i32 0, ptr " + error_slot +
                             ", align 4");
    callbacks.push_scope(ctx);
    ctx.error_handler_stack.push_back(
        {error_slot,
         dispatch_label,
         ctx.scopes.size() - 1u,
         ctx.autoreleasepool_scope_symbols.size(),
         ctx.pending_block_dispose_calls.size(),
         ctx.pending_ownership_cleanup_calls.size(),
         ctx.arc_owned_cleanup_ptrs.size()});
    for (const auto &nested_stmt : block_stmt->body) {
      EmitObjc3IRStatement(nested_stmt.get(), ctx, callbacks);
      if (ctx.terminated) {
        break;
      }
    }
    const bool body_terminated = ctx.terminated;
    ctx.error_handler_stack.pop_back();
    callbacks.pop_scope(ctx, !body_terminated);
    if (!body_terminated) {
      callbacks.emit_autoreleasepool_unwind_to_depth(
          ctx, autoreleasepool_depth);
      ctx.code_lines.push_back("  br label %" + merge_label);
    }
    ctx.code_lines.push_back(dispatch_label + ":");
    const std::string loaded_error =
        callbacks.emit_load_thrown_error(error_slot, ctx);
    const std::string typed_throw_payload =
        SingleObjc3IRTypedThrowsPayloadForDoCatchBody(*block_stmt, callbacks);
    const std::string no_match_label =
        callbacks.new_label(ctx, "do_catch_no_match_");
    for (std::size_t clause_index = 0;
         clause_index < block_stmt->catch_clauses.size(); ++clause_index) {
      const auto &clause = block_stmt->catch_clauses[clause_index];
      const std::string catch_label =
          callbacks.new_label(ctx,
                              clause.catch_all ? "catch_all_" : "catch_");
      const std::string next_label =
          clause_index + 1u < block_stmt->catch_clauses.size()
              ? callbacks.new_label(ctx, "catch_next_")
              : no_match_label;
      const std::string catches = callbacks.new_temp(ctx);
      const std::string catches_bool = callbacks.new_temp(ctx);
      int catch_kind =
          Objc3IRErrorHandlingCatchKindForTypeSpelling(
              clause.binding_type_spelling);
      if (!typed_throw_payload.empty() && !clause.catch_all) {
        catch_kind = Objc3TypedThrowsRuntimeCatchKind(
            typed_throw_payload, clause.binding_type_spelling, true);
      }
      ctx.code_lines.push_back(
          "  " + catches + " = call i32 @" +
          std::string(kObjc3RuntimeCatchMatchesErrorI32Symbol) + "(i32 " +
          loaded_error + ", i32 " +
          std::to_string(catch_kind) +
          ", i32 " + (clause.catch_all ? "1" : "0") + ")");
      ctx.code_lines.push_back("  " + catches_bool + " = icmp ne i32 " +
                               catches + ", 0");
      ctx.code_lines.push_back("  br i1 " + catches_bool + ", label %" +
                               catch_label + ", label %" + next_label);
      ctx.code_lines.push_back(catch_label + ":");
      callbacks.push_scope(ctx);
      if (clause.has_binding && !clause.binding_name.empty()) {
        const std::string ptr =
            "%" + clause.binding_name + ".addr." +
            std::to_string(ctx.temp_counter++);
        ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
        ctx.code_lines.push_back("  store i32 " + loaded_error +
                                 ", ptr " + ptr + ", align 4");
        ctx.scopes.back()[clause.binding_name] = ptr;
      }
      ctx.terminated = false;
      for (const auto &catch_stmt : clause.body) {
        EmitObjc3IRStatement(catch_stmt.get(), ctx, callbacks);
        if (ctx.terminated) {
          break;
        }
      }
      const bool catch_terminated = ctx.terminated;
      callbacks.pop_scope(ctx, !catch_terminated);
      if (!catch_terminated) {
        ctx.code_lines.push_back("  br label %" + merge_label);
      }
      ctx.code_lines.push_back(next_label + ":");
    }
    callbacks.emit_propagate_thrown_error(loaded_error, ctx);
    ctx.code_lines.push_back(merge_label + ":");
    ctx.terminated = false;
    return;
  }
  const std::size_t autoreleasepool_depth =
      ctx.autoreleasepool_scope_symbols.size();
  if (block_stmt->is_autoreleasepool_scope) {
    // integration anchor: the current non-suspending async
    // slice reuses the existing autoreleasepool scope hooks and later
    // composes them with deferred cleanup emission on scope exit. There
    // is still no separate suspension-frame cleanup runtime here.
    ctx.code_lines.push_back(
        "  call void @" +
        std::string(kObjc3RuntimePushAutoreleasepoolScopeSymbol) + "()");
    ctx.autoreleasepool_scope_symbols.push_back(
        block_stmt->autoreleasepool_scope_symbol);
  }
  callbacks.push_scope(ctx);
  for (const auto &nested_stmt : block_stmt->body) {
    EmitObjc3IRStatement(nested_stmt.get(), ctx, callbacks);
  }
  callbacks.pop_scope(ctx, !ctx.terminated);
  if (!ctx.terminated) {
    callbacks.emit_autoreleasepool_unwind_to_depth(
        ctx, autoreleasepool_depth);
  }
}
