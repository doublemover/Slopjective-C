#include "ir/objc3_ir_statement_binding_emission.h"

#include <algorithm>
#include <cstddef>
#include <string>

#include "ast/objc3_ast.h"
#include "lower/contracts/ownership_runtime_memory_management_contracts.h"
#include "lower/core/lowering_primitive_ops.h"

void EmitObjc3IRAssignmentStore(
    const std::string &ptr, const std::string &op, const Expr *value_expr,
    FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  if (ptr.empty()) {
    return;
  }
  int assigned_const_value = 0;
  const bool has_assigned_const_value =
      op == "=" && value_expr != nullptr &&
      callbacks.try_get_compile_time_i32_expr(value_expr, ctx,
                                              assigned_const_value);
  const bool has_assigned_nil_value =
      op == "=" && value_expr != nullptr &&
      callbacks.is_compile_time_nil_receiver_expr(value_expr, ctx);
  // Any explicit write invalidates compile-time nil binding for this storage slot.
  ctx.nil_bound_ptrs.erase(ptr);
  // Any explicit write invalidates compile-time known non-zero binding for this storage slot.
  ctx.nonzero_bound_ptrs.erase(ptr);
  // Any explicit write invalidates tracked compile-time constant value for this storage slot.
  ctx.const_value_ptrs.erase(ptr);
  if (op == "++" || op == "--") {
    const std::string lhs = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + lhs + " = load i32, ptr " + ptr +
                             ", align 4");
    const std::string out = callbacks.new_temp(ctx);
    const std::string opcode = op == "++" ? "add" : "sub";
    ctx.code_lines.push_back("  " + out + " = " + opcode + " i32 " + lhs +
                             ", 1");
    ctx.code_lines.push_back("  store i32 " + out + ", ptr " + ptr +
                             ", align 4");
    return;
  }
  if (op == "=") {
    if (value_expr == nullptr) {
      return;
    }
    const std::string value = callbacks.emit_expr(value_expr, ctx);
    if (ctx.terminated) {
      return;
    }
    if (ctx.arc_owned_storage_ptrs.find(ptr) !=
        ctx.arc_owned_storage_ptrs.end()) {
      const std::string retained_value = callbacks.new_temp(ctx);
      const std::string previous_value = callbacks.new_temp(ctx);
      const std::string released_value = callbacks.new_temp(ctx);
      ctx.code_lines.push_back("  " + retained_value + " = call i32 @" +
                               std::string(kObjc3RuntimeRetainI32Symbol) +
                               "(i32 " + value + ")");
      ctx.code_lines.push_back("  " + previous_value +
                               " = load i32, ptr " + ptr + ", align 4");
      ctx.code_lines.push_back("  store i32 " + retained_value + ", ptr " +
                               ptr + ", align 4");
      ctx.code_lines.push_back("  " + released_value + " = call i32 @" +
                               std::string(kObjc3RuntimeReleaseI32Symbol) +
                               "(i32 " + previous_value + ")");
      (void)released_value;
    } else {
      ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr +
                               ", align 4");
    }
    if (has_assigned_nil_value && ptr.rfind("@", 0) != 0) {
      ctx.nil_bound_ptrs.insert(ptr);
    }
    if (has_assigned_const_value) {
      ctx.const_value_ptrs[ptr] = assigned_const_value;
      if (assigned_const_value != 0) {
        ctx.nonzero_bound_ptrs.insert(ptr);
      }
    }
    return;
  }

  if (value_expr == nullptr) {
    return;
  }
  std::string binary_opcode;
  if (!TryGetCompoundAssignmentBinaryOpcode(op, binary_opcode)) {
    const std::string value = callbacks.emit_expr(value_expr, ctx);
    if (ctx.terminated) {
      return;
    }
    ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr +
                             ", align 4");
    return;
  }

  const std::string lhs = callbacks.new_temp(ctx);
  ctx.code_lines.push_back("  " + lhs + " = load i32, ptr " + ptr +
                           ", align 4");
  const std::string rhs = callbacks.emit_expr(value_expr, ctx);
  if (ctx.terminated) {
    return;
  }
  const std::string out = callbacks.new_temp(ctx);
  ctx.code_lines.push_back("  " + out + " = " + binary_opcode + " i32 " +
                           lhs + ", " + rhs);
  ctx.code_lines.push_back("  store i32 " + out + ", ptr " + ptr +
                           ", align 4");
}

void EmitObjc3IRForClause(
    const ForClause &clause, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  switch (clause.kind) {
    case ForClause::Kind::None:
      return;
    case ForClause::Kind::Expr:
      if (clause.value != nullptr) {
        (void)callbacks.emit_expr(clause.value.get(), ctx);
      }
      return;
    case ForClause::Kind::Assign: {
      const std::string ptr = callbacks.lookup_var_ptr(ctx, clause.name);
      if (ptr.empty() &&
          ctx.block_bindings.find(clause.name) != ctx.block_bindings.end()) {
        (void)callbacks.emit_unsupported_i32_value(
            "reassigning block values is not yet runnable in Objective-C 3 native mode");
        return;
      }
      EmitObjc3IRAssignmentStore(ptr, clause.op, clause.value.get(), ctx,
                                 callbacks);
      return;
    }
    case ForClause::Kind::Let: {
      if (ctx.scopes.empty() || clause.value == nullptr) {
        return;
      }
      const std::string value = callbacks.emit_expr(clause.value.get(), ctx);
      if (ctx.terminated) {
        return;
      }
      const std::string ptr =
          "%" + clause.name + ".addr." + std::to_string(ctx.temp_counter++);
      int clause_const_value = 0;
      const bool has_clause_const_value =
          callbacks.try_get_compile_time_i32_expr(
              clause.value.get(), ctx, clause_const_value);
      const bool has_clause_nil_value =
          callbacks.is_compile_time_nil_receiver_expr(clause.value.get(), ctx);
      ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
      ctx.scopes.back()[clause.name] = ptr;
      if (has_clause_nil_value) {
        ctx.nil_bound_ptrs.insert(ptr);
      }
      if (has_clause_const_value) {
        ctx.const_value_ptrs[ptr] = clause_const_value;
      }
      if (has_clause_const_value && clause_const_value != 0) {
        ctx.nonzero_bound_ptrs.insert(ptr);
      }
      ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr +
                               ", align 4");
      return;
    }
  }
}

std::string EmitObjc3IRLocalI32Binding(
    const std::string &name, const Expr *value_expr, FunctionContext &ctx,
    bool mark_runtime_nonnull,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  const std::string value = callbacks.emit_expr(value_expr, ctx);
  if (ctx.terminated) {
    return value;
  }
  int const_value = 0;
  const bool has_const_value =
      callbacks.try_get_compile_time_i32_expr(value_expr, ctx, const_value);
  const bool has_nil_value =
      callbacks.is_compile_time_nil_receiver_expr(value_expr, ctx);
  const std::string ptr =
      "%" + name + ".addr." + std::to_string(ctx.temp_counter++);
  ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
  ctx.scopes.back()[name] = ptr;
  if (has_nil_value) {
    ctx.nil_bound_ptrs.insert(ptr);
  }
  if (has_const_value) {
    ctx.const_value_ptrs[ptr] = const_value;
  }
  if ((has_const_value && const_value != 0) || mark_runtime_nonnull) {
    ctx.nonzero_bound_ptrs.insert(ptr);
  }
  ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr +
                           ", align 4");
  return value;
}

void EmitObjc3IROptionalBindingIfStatement(
    const IfStmt *if_stmt, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  if (if_stmt == nullptr) {
    return;
  }

  const bool is_guard = if_stmt->guard_binding_surface_enabled ||
                        if_stmt->guard_condition_list_surface_enabled;
  const std::size_t binding_count =
      std::min(if_stmt->optional_binding_clause_count, if_stmt->then_body.size());
  if (binding_count == 0u && !is_guard) {
    return;
  }
  const std::string success_label =
      callbacks.new_label(ctx, is_guard ? "guard_success_"
                                        : "if_bind_success_");
  const std::string failure_label =
      callbacks.new_label(ctx, is_guard ? "guard_else_" : "if_bind_else_");
  const std::string merge_label =
      callbacks.new_label(ctx, is_guard ? "guard_end_" : "if_bind_end_");

  callbacks.push_scope(ctx);
  for (std::size_t index = 0; index < binding_count; ++index) {
    const Stmt *binding_stmt = if_stmt->then_body[index].get();
    if (binding_stmt == nullptr || binding_stmt->kind != Stmt::Kind::Let ||
        binding_stmt->let_stmt == nullptr) {
      (void)callbacks.emit_unsupported_i32_value(
          "optional binding lowering expected synthetic let clauses");
      callbacks.pop_scope(ctx, false);
      return;
    }
    const LetStmt *binding = binding_stmt->let_stmt.get();
    const std::string value = EmitObjc3IRLocalI32Binding(
        binding->name, binding->value.get(), ctx, true, callbacks);
    const std::string is_present = callbacks.new_temp(ctx);
    const std::string next_label =
        (index + 1u == binding_count)
            ? success_label
            : callbacks.new_label(ctx, "if_bind_clause_");
    ctx.code_lines.push_back("  " + is_present + " = icmp ne i32 " + value +
                             ", 0");
    ctx.code_lines.push_back("  br i1 " + is_present + ", label %" +
                             next_label + ", label %" + failure_label);
    if (next_label != success_label) {
      ctx.code_lines.push_back(next_label + ":");
    }
  }

  if (is_guard) {
    const std::string guard_ready_label =
        if_stmt->guard_condition_exprs.empty()
            ? success_label
            : callbacks.new_label(ctx, "guard_ready_");
    if (binding_count == 0u) {
      ctx.code_lines.push_back("  br label %" + success_label);
    }
    ctx.code_lines.push_back(success_label + ":");
    for (const auto &guard_condition : if_stmt->guard_condition_exprs) {
      const std::string condition_value =
          callbacks.emit_expr(guard_condition.get(), ctx);
      const std::string condition_i1 = callbacks.new_temp(ctx);
      const bool is_last_condition =
          guard_condition.get() == if_stmt->guard_condition_exprs.back().get();
      const std::string next_label =
          is_last_condition ? guard_ready_label
                            : callbacks.new_label(ctx, "guard_clause_");
      ctx.code_lines.push_back("  " + condition_i1 + " = icmp ne i32 " +
                               condition_value + ", 0");
      ctx.code_lines.push_back("  br i1 " + condition_i1 + ", label %" +
                               next_label + ", label %" + failure_label);
      if (!is_last_condition) {
        ctx.code_lines.push_back(next_label + ":");
      }
    }
    if (guard_ready_label != success_label) {
      ctx.code_lines.push_back(guard_ready_label + ":");
    }
    const auto promoted_bindings = ctx.scopes.back();
    callbacks.pop_scope(ctx, false);
    for (const auto &binding : promoted_bindings) {
      ctx.scopes.back()[binding.first] = binding.second;
    }
    ctx.code_lines.push_back("  br label %" + merge_label);

    ctx.code_lines.push_back(failure_label + ":");
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

    ctx.code_lines.push_back(merge_label + ":");
    ctx.terminated = false;
    return;
  }

  ctx.code_lines.push_back(success_label + ":");
  ctx.terminated = false;
  for (std::size_t index = binding_count; index < if_stmt->then_body.size();
       ++index) {
    EmitObjc3IRStatement(if_stmt->then_body[index].get(), ctx, callbacks);
  }
  const bool then_terminated = ctx.terminated;
  callbacks.pop_scope(ctx, !then_terminated);
  if (!then_terminated) {
    ctx.code_lines.push_back("  br label %" + merge_label);
  }

  ctx.code_lines.push_back(failure_label + ":");
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
    return;
  }
  ctx.code_lines.push_back(merge_label + ":");
  ctx.terminated = false;
}
