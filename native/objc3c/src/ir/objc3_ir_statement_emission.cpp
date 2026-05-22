#include "ir/objc3_ir_statement_emission.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_literal_parsing.h"
#include "ir/objc3_ir_statement_block_emission.h"
#include "ir/objc3_ir_statement_binding_emission.h"
#include "ir/objc3_ir_statement_loop_emission.h"
#include "ir/objc3_ir_statement_switch_emission.h"
#include "ir/objc3_ir_type_model.h"

namespace {

constexpr const char *kObjc3RuntimeCollectionsMutableArrayAppendI32Symbol =
    "objc3_runtime_stdlib_collections_mutable_array_append_i32";
constexpr const char *kObjc3RuntimeCollectionsMutableArraySetI32Symbol =
    "objc3_runtime_stdlib_collections_mutable_array_set_i32";
constexpr const char *kObjc3RuntimeCollectionsMutableArrayRemoveAtI32Symbol =
    "objc3_runtime_stdlib_collections_mutable_array_remove_at_i32";
constexpr const char *kObjc3RuntimeCollectionsMapInsertI32Symbol =
    "objc3_runtime_stdlib_collections_map_insert_i32";
constexpr const char *kObjc3RuntimeCollectionsMapDeleteI32Symbol =
    "objc3_runtime_stdlib_collections_map_delete_i32";
constexpr const char *kObjc3RuntimeCollectionsSetInsertI32Symbol =
    "objc3_runtime_stdlib_collections_set_insert_i32";
constexpr const char *kObjc3RuntimeCollectionsSetDeleteI32Symbol =
    "objc3_runtime_stdlib_collections_set_delete_i32";

bool IsTerminalReturnAwaitDirectCall(const Expr *expr) {
  return expr != nullptr && expr->kind == Expr::Kind::Call &&
         expr->await_expression_enabled;
}

void RecordCollectionBinding(const LetStmt &let, const std::string &ptr,
                             FunctionContext &ctx) {
  if (let.value == nullptr) {
    return;
  }
  Expr::CollectionLiteralKind kind = Expr::CollectionLiteralKind::None;
  if (let.value->kind == Expr::Kind::CollectionLiteral) {
    kind = let.value->collection_literal_kind;
  } else if (let.value->kind == Expr::Kind::Identifier) {
    for (auto it = ctx.scopes.rbegin(); it != ctx.scopes.rend(); ++it) {
      const auto found_ptr = it->find(let.value->ident);
      if (found_ptr == it->end()) {
        continue;
      }
      const auto found_kind = ctx.collection_kind_by_ptr.find(found_ptr->second);
      if (found_kind != ctx.collection_kind_by_ptr.end()) {
        kind = found_kind->second;
      }
      break;
    }
  }
  if (kind != Expr::CollectionLiteralKind::None) {
    ctx.collection_kind_by_ptr[ptr] = kind;
    if (let.mutable_binding) {
      ctx.mutable_collection_ptrs.insert(ptr);
    }
  }
}

ValueType InferObjc3IRLocalBindingValueType(const Expr *expr,
                                            const FunctionContext &ctx) {
  if (expr == nullptr) {
    return ValueType::Unknown;
  }
  switch (expr->kind) {
    case Expr::Kind::Number:
      return ValueType::I32;
    case Expr::Kind::BoolLiteral:
      return ValueType::Bool;
    case Expr::Kind::StringLiteral:
    case Expr::Kind::StringInterpolation:
      return ValueType::TextHandle;
    case Expr::Kind::Identifier:
      for (auto it = ctx.scopes.rbegin(); it != ctx.scopes.rend(); ++it) {
        const auto found_ptr = it->find(expr->ident);
        if (found_ptr == it->end()) {
          continue;
        }
        const auto found_type = ctx.value_type_by_ptr.find(found_ptr->second);
        if (found_type != ctx.value_type_by_ptr.end()) {
          return found_type->second;
        }
        break;
      }
      return ValueType::Unknown;
    case Expr::Kind::NilLiteral:
      return ValueType::ObjCId;
    case Expr::Kind::Conditional:
      if (expr->right != nullptr && expr->third != nullptr) {
        const ValueType then_type =
            InferObjc3IRLocalBindingValueType(expr->right.get(), ctx);
        const ValueType else_type =
            InferObjc3IRLocalBindingValueType(expr->third.get(), ctx);
        if (then_type == else_type) {
          return then_type;
        }
      }
      return ValueType::Unknown;
    case Expr::Kind::Binary:
      if (expr->op == "==" || expr->op == "!=" || expr->op == "<" ||
          expr->op == "<=" || expr->op == ">" || expr->op == ">=" ||
          expr->op == "&&" || expr->op == "||") {
        return ValueType::Bool;
      }
      if (expr->op == "+" || expr->op == "-" || expr->op == "*" ||
          expr->op == "/" || expr->op == "%" || expr->op == "&" ||
          expr->op == "|" || expr->op == "^" || expr->op == "<<" ||
          expr->op == ">>") {
        return ValueType::I32;
      }
      return ValueType::Unknown;
    default:
      return ValueType::Unknown;
  }
}

}  // namespace

void EmitObjc3IRStatement(
    const Stmt *stmt, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  if (stmt == nullptr || ctx.terminated) {
    return;
  }

  switch (stmt->kind) {
    case Stmt::Kind::Let: {
      const LetStmt *let = stmt->let_stmt.get();
      if (let == nullptr || ctx.scopes.empty()) {
        return;
      }
      if (let->value != nullptr &&
          let->value->kind == Expr::Kind::BlockLiteral) {
        const std::string storage_ptr =
            callbacks.emit_block_literal_storage(*let->value, ctx);
        if (storage_ptr == "poison") {
          return;
        }
        ctx.block_bindings[let->name] =
            BlockBinding{storage_ptr, let->value.get(), ""};
        return;
      }
      if (let->value != nullptr &&
          let->value->kind == Expr::Kind::Identifier) {
        const auto existing_block_it =
            ctx.block_bindings.find(let->value->ident);
        if (existing_block_it != ctx.block_bindings.end()) {
          ctx.block_bindings[let->name] = existing_block_it->second;
          return;
        }
      }
      if (let->value != nullptr &&
          let->value->kind == Expr::Kind::CollectionLiteral &&
          let->value->collection_literal_kind ==
              Expr::CollectionLiteralKind::Array) {
        let->value->collection_literal_mutable = let->mutable_binding;
      }
      // Evaluate the initializer against the currently visible scope first so
      // shadowing declarations can read the previous binding deterministically.
      const std::string value = callbacks.emit_expr(let->value.get(), ctx);
      if (ctx.terminated) {
        return;
      }
      int let_const_value = 0;
      const bool has_let_const_value =
          callbacks.try_get_compile_time_i32_expr(let->value.get(), ctx,
                                                  let_const_value);
      const bool has_let_nil_value =
          callbacks.is_compile_time_nil_receiver_expr(let->value.get(), ctx);
      const std::string ptr =
          "%" + let->name + ".addr." + std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
      ctx.scopes.back()[let->name] = ptr;
      ctx.value_type_by_ptr[ptr] =
          InferObjc3IRLocalBindingValueType(let->value.get(), ctx);
      RecordCollectionBinding(*let, ptr, ctx);
      if (has_let_nil_value) {
        ctx.nil_bound_ptrs.insert(ptr);
      }
      if (has_let_const_value) {
        ctx.const_value_ptrs[ptr] = let_const_value;
      }
      if (has_let_const_value && let_const_value != 0) {
        ctx.nonzero_bound_ptrs.insert(ptr);
      }
      ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr +
                               ", align 4");
      if (let->cleanup_attribute_declared || let->cleanup_sugar_declared ||
          let->resource_attribute_declared || let->resource_sugar_declared) {
        PendingOwnershipCleanupCall cleanup_call;
        cleanup_call.binding_name = let->name;
        cleanup_call.storage_ptr = ptr;
        cleanup_call.cleanup_function_symbol = let->cleanup_function_symbol;
        cleanup_call.resource_close_symbol = let->resource_close_symbol;
        if (!let->resource_close_symbol.empty()) {
          int invalid_value = 0;
          if (!let->resource_invalid_expression.empty() &&
              !ParseOwnershipResourceInvalidLiteral(
                  let->resource_invalid_expression, invalid_value)) {
            (void)callbacks.emit_unsupported_i32_value(
                "resource invalid expression for '" + let->name +
                "' must stay an integer literal in the current Part 8 lowering slice");
            return;
          }
          cleanup_call.has_resource_invalid_value =
              !let->resource_invalid_expression.empty();
          cleanup_call.resource_invalid_value = invalid_value;
        }
        ctx.ownership_cleanup_call_indices[let->name] =
            ctx.pending_ownership_cleanup_calls.size();
        if (!ctx.pending_scope_cleanup_actions.empty()) {
          ctx.pending_scope_cleanup_actions.back().push_back(
              {PendingScopeCleanupActionKind::Ownership,
               ctx.pending_ownership_cleanup_calls.size()});
        }
        ctx.pending_ownership_cleanup_calls.push_back(std::move(cleanup_call));
      }
      return;
    }
    case Stmt::Kind::Return: {
      const ReturnStmt *ret = stmt->return_stmt.get();
      if (ret == nullptr) {
        return;
      }
      if (ret->value == nullptr) {
        callbacks.emit_typed_return("0", ctx);
      } else {
        const bool previous_return_await_cleanup_enabled =
            ctx.return_await_cleanup_before_handoff_enabled;
        const bool previous_return_await_cleanup_emitted =
            ctx.return_await_cleanup_before_handoff_emitted;
        const bool terminal_return_await =
            IsTerminalReturnAwaitDirectCall(ret->value.get());
        ctx.return_await_cleanup_before_handoff_enabled =
            terminal_return_await;
        ctx.return_await_cleanup_before_handoff_emitted = false;
        const std::string value = callbacks.emit_expr(ret->value.get(), ctx);
        const bool return_await_cleanup_emitted =
            terminal_return_await &&
            ctx.return_await_cleanup_before_handoff_emitted;
        ctx.return_await_cleanup_before_handoff_enabled =
            previous_return_await_cleanup_enabled;
        ctx.return_await_cleanup_before_handoff_emitted =
            previous_return_await_cleanup_emitted ||
            return_await_cleanup_emitted;
        if (ctx.terminated) {
          return;
        }
        callbacks.emit_typed_return(value, ctx);
      }
      ctx.terminated = true;
      return;
    }
    case Stmt::Kind::Assign: {
      const AssignStmt *assign = stmt->assign_stmt.get();
      if (assign == nullptr) {
        return;
      }
      const std::string ptr = callbacks.lookup_var_ptr(ctx, assign->name);
      if (ptr.empty() &&
          ctx.block_bindings.find(assign->name) != ctx.block_bindings.end()) {
        (void)callbacks.emit_unsupported_i32_value(
            "reassigning block values is not yet runnable in Objective-C 3 native mode");
        return;
      }
      const auto collection_kind = ctx.collection_kind_by_ptr.find(ptr);
      if (collection_kind != ctx.collection_kind_by_ptr.end() &&
          assign->op == "+=" && assign->value != nullptr) {
        if (ctx.mutable_collection_ptrs.find(ptr) ==
            ctx.mutable_collection_ptrs.end()) {
          (void)callbacks.emit_unsupported_i32_value(
              "collection mutation requires a 'var' binding");
          return;
        }
        const std::string handle = callbacks.new_temp(ctx);
        ctx.code_lines.push_back("  " + handle + " = load i32, ptr " + ptr +
                                 ", align 4");
        const std::string value = callbacks.emit_expr(assign->value.get(), ctx);
        const std::string ignored = callbacks.new_temp(ctx);
        if (collection_kind->second == Expr::CollectionLiteralKind::Array) {
          ctx.code_lines.push_back("  " + ignored + " = call i32 @" +
                                   std::string(
                                       kObjc3RuntimeCollectionsMutableArrayAppendI32Symbol) +
                                   "(i32 " + handle + ", i32 " + value + ")");
          return;
        }
        if (collection_kind->second == Expr::CollectionLiteralKind::Set) {
          ctx.code_lines.push_back("  " + ignored + " = call i32 @" +
                                   std::string(
                                       kObjc3RuntimeCollectionsSetInsertI32Symbol) +
                                   "(i32 " + handle + ", i32 " + value + ")");
          return;
        }
      }
      EmitObjc3IRAssignmentStore(ptr, assign->op, assign->value.get(), ctx,
                                 callbacks);
      return;
    }
    case Stmt::Kind::CollectionMutation: {
      const CollectionMutationStmt *mutation =
          stmt->collection_mutation_stmt.get();
      if (mutation == nullptr) {
        return;
      }
      const std::string ptr =
          callbacks.lookup_var_ptr(ctx, mutation->collection_name);
      const auto collection_kind = ctx.collection_kind_by_ptr.find(ptr);
      if (collection_kind == ctx.collection_kind_by_ptr.end()) {
        (void)callbacks.emit_unsupported_i32_value(
            "collection mutation requires a parser-visible collection binding");
        return;
      }
      if (ctx.mutable_collection_ptrs.find(ptr) ==
          ctx.mutable_collection_ptrs.end()) {
        (void)callbacks.emit_unsupported_i32_value(
            "collection mutation requires a 'var' binding");
        return;
      }
      const std::string handle = callbacks.new_temp(ctx);
      ctx.code_lines.push_back("  " + handle + " = load i32, ptr " + ptr +
                               ", align 4");
      const std::string key_or_index =
          callbacks.emit_expr(mutation->key_or_index.get(), ctx);
      const std::string ignored = callbacks.new_temp(ctx);
      if (mutation->kind == CollectionMutationStmt::Kind::IndexSet) {
        if (mutation->value == nullptr) {
          return;
        }
        const std::string value = callbacks.emit_expr(mutation->value.get(), ctx);
        if (collection_kind->second == Expr::CollectionLiteralKind::Array) {
          ctx.code_lines.push_back("  " + ignored + " = call i32 @" +
                                   std::string(
                                       kObjc3RuntimeCollectionsMutableArraySetI32Symbol) +
                                   "(i32 " + handle + ", i32 " +
                                   key_or_index + ", i32 " + value + ")");
          return;
        }
        if (collection_kind->second == Expr::CollectionLiteralKind::Map) {
          ctx.code_lines.push_back("  " + ignored + " = call i32 @" +
                                   std::string(
                                       kObjc3RuntimeCollectionsMapInsertI32Symbol) +
                                   "(i32 " + handle + ", i32 " +
                                   key_or_index + ", i32 " + value + ")");
          return;
        }
      } else {
        if (collection_kind->second == Expr::CollectionLiteralKind::Array) {
          ctx.code_lines.push_back("  " + ignored + " = call i32 @" +
                                   std::string(
                                       kObjc3RuntimeCollectionsMutableArrayRemoveAtI32Symbol) +
                                   "(i32 " + handle + ", i32 " +
                                   key_or_index + ")");
          return;
        }
        if (collection_kind->second == Expr::CollectionLiteralKind::Map) {
          ctx.code_lines.push_back("  " + ignored + " = call i32 @" +
                                   std::string(
                                       kObjc3RuntimeCollectionsMapDeleteI32Symbol) +
                                   "(i32 " + handle + ", i32 " +
                                   key_or_index + ")");
          return;
        }
        if (collection_kind->second == Expr::CollectionLiteralKind::Set) {
          ctx.code_lines.push_back("  " + ignored + " = call i32 @" +
                                   std::string(
                                       kObjc3RuntimeCollectionsSetDeleteI32Symbol) +
                                   "(i32 " + handle + ", i32 " +
                                   key_or_index + ")");
          return;
        }
      }
      (void)callbacks.emit_unsupported_i32_value(
          "unsupported collection mutation shape");
      return;
    }
    case Stmt::Kind::Break: {
      if (ctx.control_stack.empty()) {
        callbacks.emit_terminal_cleanup_to_depth(ctx, 0u, 0u, 0u, 0u, 0u);
        ctx.code_lines.push_back(
            "  ret " + std::string(LLVMScalarType(ctx.return_type)) + " 0");
      } else {
        callbacks.emit_terminal_cleanup_to_depth(
            ctx, ctx.control_stack.back().scope_depth,
            ctx.control_stack.back().autoreleasepool_depth,
            ctx.control_stack.back().pending_block_dispose_depth,
            ctx.control_stack.back().ownership_cleanup_depth,
            ctx.control_stack.back().arc_cleanup_depth);
        ctx.code_lines.push_back(
            "  br label %" + ctx.control_stack.back().break_label);
      }
      ctx.terminated = true;
      return;
    }
    case Stmt::Kind::Continue: {
      std::string continue_label;
      std::size_t continue_autoreleasepool_depth =
          ctx.autoreleasepool_scope_symbols.size();
      for (auto it = ctx.control_stack.rbegin();
           it != ctx.control_stack.rend(); ++it) {
        if (it->continue_allowed) {
          continue_label = it->continue_label;
          continue_autoreleasepool_depth = it->autoreleasepool_depth;
          break;
        }
      }
      if (continue_label.empty()) {
        callbacks.emit_terminal_cleanup_to_depth(ctx, 0u, 0u, 0u, 0u, 0u);
        ctx.code_lines.push_back(
            "  ret " + std::string(LLVMScalarType(ctx.return_type)) + " 0");
      } else {
        const ControlLabels &target = *std::find_if(
            ctx.control_stack.rbegin(), ctx.control_stack.rend(),
            [&continue_label](const ControlLabels &labels) {
              return labels.continue_allowed &&
                     labels.continue_label == continue_label;
            });
        callbacks.emit_terminal_cleanup_to_depth(
            ctx, target.scope_depth, continue_autoreleasepool_depth,
            target.pending_block_dispose_depth, target.ownership_cleanup_depth,
            target.arc_cleanup_depth);
        ctx.code_lines.push_back("  br label %" + continue_label);
      }
      ctx.terminated = true;
      return;
    }
    case Stmt::Kind::Empty:
      return;
    case Stmt::Kind::Block: {
      EmitObjc3IRBlockStatement(stmt->block_stmt.get(), ctx, callbacks);
      return;
    }
    case Stmt::Kind::Defer: {
      const BlockStmt *block_stmt = stmt->block_stmt.get();
      if (block_stmt == nullptr || ctx.pending_defer_scope_blocks.empty()) {
        return;
      }
      if (!ctx.pending_scope_cleanup_actions.empty()) {
        ctx.pending_scope_cleanup_actions.back().push_back(
            {PendingScopeCleanupActionKind::Defer,
             ctx.pending_defer_scope_blocks.back().size()});
      }
      ctx.pending_defer_scope_blocks.back().push_back(block_stmt);
      return;
    }
    case Stmt::Kind::Expr: {
      const ExprStmt *expr_stmt = stmt->expr_stmt.get();
      if (expr_stmt != nullptr) {
        (void)callbacks.emit_expr(expr_stmt->value.get(), ctx);
      }
      return;
    }
    case Stmt::Kind::While: {
      EmitObjc3IRWhileStatement(stmt->while_stmt.get(), ctx, callbacks);
      return;
    }
    case Stmt::Kind::DoWhile: {
      EmitObjc3IRDoWhileStatement(stmt->do_while_stmt.get(), ctx, callbacks);
      return;
    }
    case Stmt::Kind::For: {
      EmitObjc3IRForStatement(stmt->for_stmt.get(), ctx, callbacks);
      return;
    }
    case Stmt::Kind::ForIn: {
      EmitObjc3IRForInStatement(stmt->for_in_stmt.get(), ctx, callbacks);
      return;
    }
    case Stmt::Kind::Switch: {
      EmitObjc3IRSwitchStatement(stmt->switch_stmt.get(), ctx, callbacks);
      return;
    }
    case Stmt::Kind::If: {
      const IfStmt *if_stmt = stmt->if_stmt.get();
      if (if_stmt == nullptr) {
        return;
      }
      if (if_stmt->optional_binding_surface_enabled ||
          if_stmt->guard_condition_list_surface_enabled) {
        EmitObjc3IROptionalBindingIfStatement(if_stmt, ctx, callbacks);
        return;
      }

      const std::string cond = callbacks.emit_expr(if_stmt->condition.get(), ctx);
      const std::string cond_i1 = callbacks.new_temp(ctx);
      const std::string then_label = callbacks.new_label(ctx, "if_then_");
      const std::string else_label = callbacks.new_label(ctx, "if_else_");
      const std::string merge_label = callbacks.new_label(ctx, "if_end_");

      ctx.code_lines.push_back("  " + cond_i1 + " = icmp ne i32 " + cond +
                               ", 0");
      ctx.code_lines.push_back("  br i1 " + cond_i1 + ", label %" +
                               then_label + ", label %" + else_label);

      ctx.code_lines.push_back(then_label + ":");
      callbacks.push_scope(ctx);
      ctx.terminated = false;
      for (const auto &s : if_stmt->then_body) {
        EmitObjc3IRStatement(s.get(), ctx, callbacks);
      }
      const bool then_terminated = ctx.terminated;
      callbacks.pop_scope(ctx, !then_terminated);
      if (!then_terminated) {
        ctx.code_lines.push_back("  br label %" + merge_label);
      }

      ctx.code_lines.push_back(else_label + ":");
      callbacks.push_scope(ctx);
      ctx.terminated = false;
      for (const auto &s : if_stmt->else_body) {
        EmitObjc3IRStatement(s.get(), ctx, callbacks);
      }
      const bool else_terminated = ctx.terminated;
      callbacks.pop_scope(ctx, !else_terminated);
      if (!else_terminated) {
        ctx.code_lines.push_back("  br label %" + merge_label);
      }

      if (then_terminated && else_terminated) {
        ctx.terminated = true;
      } else {
        ctx.code_lines.push_back(merge_label + ":");
        ctx.terminated = false;
      }
      return;
    }
  }
}
