#include "ir/objc3_ir_statement_switch_emission.h"

#include <cstddef>
#include <string>
#include <vector>

#include "ast/objc3_ast.h"

void EmitObjc3IRSwitchStatement(
    const SwitchStmt *switch_stmt, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
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
}
