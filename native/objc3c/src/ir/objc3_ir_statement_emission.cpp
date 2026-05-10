#include "ir/objc3_ir_statement_emission.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_literal_parsing.h"
#include "ir/objc3_ir_statement_block_emission.h"
#include "ir/objc3_ir_statement_binding_emission.h"
#include "ir/objc3_ir_type_model.h"

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
        callbacks.emit_autoreleasepool_unwind_to_depth(ctx, 0u);
        callbacks.emit_typed_return("0", ctx);
      } else {
        const std::string value = callbacks.emit_expr(ret->value.get(), ctx);
        if (ctx.terminated) {
          return;
        }
        callbacks.emit_autoreleasepool_unwind_to_depth(ctx, 0u);
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
      EmitObjc3IRAssignmentStore(ptr, assign->op, assign->value.get(), ctx,
                                 callbacks);
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
      const WhileStmt *while_stmt = stmt->while_stmt.get();
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
      return;
    }
    case Stmt::Kind::DoWhile: {
      const DoWhileStmt *do_while_stmt = stmt->do_while_stmt.get();
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
      return;
    }
    case Stmt::Kind::For: {
      const ForStmt *for_stmt = stmt->for_stmt.get();
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
      return;
    }
    case Stmt::Kind::Switch: {
      const SwitchStmt *switch_stmt = stmt->switch_stmt.get();
      if (switch_stmt == nullptr) {
        return;
      }

      const std::string condition_value =
          callbacks.emit_expr(switch_stmt->condition.get(), ctx);
      const std::string end_label = callbacks.new_label(ctx, "switch_end_");

      if (switch_stmt->match_surface_enabled) {
        // match lowering anchor: statement-form match now lowers
        // literal/default/wildcard/binding arms as a distinct control-flow
        // carrier with case-local binding storage and no switch-style
        // fallthrough, while result-case payload matching remains fail-closed
        // until a runtime Result ABI exists.
        std::vector<std::string> arm_labels;
        arm_labels.reserve(switch_stmt->cases.size());
        for (std::size_t i = 0; i < switch_stmt->cases.size(); ++i) {
          const SwitchCase &case_stmt = switch_stmt->cases[i];
          arm_labels.push_back(case_stmt.is_default
                                   ? callbacks.new_label(ctx, "match_default_")
                                   : callbacks.new_label(ctx, "match_case_"));
        }

        if (!switch_stmt->cases.empty()) {
          std::vector<std::string> test_labels;
          test_labels.reserve(switch_stmt->cases.size());
          for (std::size_t i = 0; i < switch_stmt->cases.size(); ++i) {
            test_labels.push_back(callbacks.new_label(ctx, "match_test_"));
          }
          ctx.code_lines.push_back("  br label %" + test_labels.front());
          for (std::size_t case_index = 0;
               case_index < switch_stmt->cases.size(); ++case_index) {
            const SwitchCase &case_stmt = switch_stmt->cases[case_index];
            const std::string next_label =
                case_index + 1u < switch_stmt->cases.size()
                    ? test_labels[case_index + 1u]
                    : end_label;
            ctx.code_lines.push_back(test_labels[case_index] + ":");
            if (case_stmt.is_default ||
                case_stmt.match_pattern_kind == MatchPatternKind::Wildcard ||
                case_stmt.match_pattern_kind == MatchPatternKind::Binding) {
              ctx.code_lines.push_back("  br label %" + arm_labels[case_index]);
              continue;
            }
            if (case_stmt.match_pattern_kind ==
                    MatchPatternKind::LiteralInteger ||
                case_stmt.match_pattern_kind == MatchPatternKind::LiteralBool ||
                case_stmt.match_pattern_kind == MatchPatternKind::LiteralNil) {
              const std::string cmp = callbacks.new_temp(ctx);
              ctx.code_lines.push_back("  " + cmp + " = icmp eq i32 " +
                                       condition_value + ", " +
                                       std::to_string(case_stmt.value));
              ctx.code_lines.push_back("  br i1 " + cmp + ", label %" +
                                       arm_labels[case_index] + ", label %" +
                                       next_label);
              continue;
            }
            if (case_stmt.match_pattern_kind == MatchPatternKind::ResultCase) {
              callbacks.emit_unsupported_i32_value(
                  "result-case match lowering remains fail-closed until a runtime Result payload ABI lands");
              return;
            }
            callbacks.emit_unsupported_i32_value(
                "unsupported match pattern reached IR lowering");
            return;
          }
        } else {
          ctx.code_lines.push_back("  br label %" + end_label);
        }

        for (std::size_t arm_index = 0;
             arm_index < switch_stmt->cases.size(); ++arm_index) {
          const SwitchCase &case_stmt = switch_stmt->cases[arm_index];
          ctx.code_lines.push_back(arm_labels[arm_index] + ":");
          callbacks.push_scope(ctx);
          if (!case_stmt.match_binding_name.empty() &&
              (case_stmt.match_pattern_kind == MatchPatternKind::Binding)) {
            const std::string ptr =
                "%" + case_stmt.match_binding_name + ".addr." +
                std::to_string(ctx.temp_counter++);
            ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
            ctx.code_lines.push_back("  store i32 " + condition_value +
                                     ", ptr " + ptr + ", align 4");
            ctx.scopes.back()[case_stmt.match_binding_name] = ptr;
          }
          ctx.control_stack.push_back(
              {"", end_label, false, ctx.scopes.size() - 1u,
               ctx.autoreleasepool_scope_symbols.size(),
               ctx.pending_block_dispose_calls.size(),
               ctx.pending_ownership_cleanup_calls.size(),
               ctx.arc_owned_cleanup_ptrs.size()});
          ctx.terminated = false;
          for (const auto &case_body_stmt : case_stmt.body) {
            EmitObjc3IRStatement(case_body_stmt.get(), ctx, callbacks);
          }
          const bool arm_terminated = ctx.terminated;
          ctx.control_stack.pop_back();
          callbacks.pop_scope(ctx, !arm_terminated);

          if (!arm_terminated) {
            ctx.code_lines.push_back("  br label %" + end_label);
          }
        }

        ctx.code_lines.push_back(end_label + ":");
        ctx.terminated = false;
        return;
      }

      std::vector<std::string> arm_labels;
      arm_labels.reserve(switch_stmt->cases.size());
      std::vector<std::size_t> case_clause_indices;
      case_clause_indices.reserve(switch_stmt->cases.size());
      std::size_t default_index = switch_stmt->cases.size();

      for (std::size_t i = 0; i < switch_stmt->cases.size(); ++i) {
        const SwitchCase &case_stmt = switch_stmt->cases[i];
        if (case_stmt.is_default) {
          arm_labels.push_back(callbacks.new_label(ctx, "switch_default_"));
          if (default_index == switch_stmt->cases.size()) {
            default_index = i;
          }
        } else {
          arm_labels.push_back(callbacks.new_label(ctx, "switch_case_"));
          case_clause_indices.push_back(i);
        }
      }

      const std::string default_label =
          default_index < switch_stmt->cases.size() ? arm_labels[default_index]
                                                    : end_label;

      if (!case_clause_indices.empty()) {
        std::vector<std::string> test_labels;
        test_labels.reserve(case_clause_indices.size());
        for (std::size_t i = 0; i < case_clause_indices.size(); ++i) {
          test_labels.push_back(callbacks.new_label(ctx, "switch_test_"));
        }

        ctx.code_lines.push_back("  br label %" + test_labels[0]);
        for (std::size_t test_index = 0;
             test_index < case_clause_indices.size(); ++test_index) {
          const std::size_t case_index = case_clause_indices[test_index];
          const std::string next_label =
              (test_index + 1 < case_clause_indices.size())
                  ? test_labels[test_index + 1]
                  : default_label;

          ctx.code_lines.push_back(test_labels[test_index] + ":");
          const std::string cmp = callbacks.new_temp(ctx);
          ctx.code_lines.push_back("  " + cmp + " = icmp eq i32 " +
                                   condition_value + ", " +
                                   std::to_string(
                                       switch_stmt->cases[case_index].value));
          ctx.code_lines.push_back("  br i1 " + cmp + ", label %" +
                                   arm_labels[case_index] + ", label %" +
                                   next_label);
        }
      } else {
        ctx.code_lines.push_back("  br label %" + default_label);
      }

      for (std::size_t arm_index = 0; arm_index < switch_stmt->cases.size();
           ++arm_index) {
        const SwitchCase &case_stmt = switch_stmt->cases[arm_index];
        ctx.code_lines.push_back(arm_labels[arm_index] + ":");
        callbacks.push_scope(ctx);
        ctx.control_stack.push_back(
            {"", end_label, false, ctx.scopes.size() - 1u,
             ctx.autoreleasepool_scope_symbols.size(),
             ctx.pending_block_dispose_calls.size(),
             ctx.pending_ownership_cleanup_calls.size(),
             ctx.arc_owned_cleanup_ptrs.size()});
        ctx.terminated = false;
        for (const auto &case_body_stmt : case_stmt.body) {
          EmitObjc3IRStatement(case_body_stmt.get(), ctx, callbacks);
        }
        const bool arm_terminated = ctx.terminated;
        ctx.control_stack.pop_back();
        callbacks.pop_scope(ctx, !arm_terminated);

        if (!arm_terminated) {
          if (arm_index + 1 < switch_stmt->cases.size()) {
            ctx.code_lines.push_back("  br label %" +
                                     arm_labels[arm_index + 1]);
          } else {
            ctx.code_lines.push_back("  br label %" + end_label);
          }
        }
      }

      ctx.code_lines.push_back(end_label + ":");
      ctx.terminated = false;
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
