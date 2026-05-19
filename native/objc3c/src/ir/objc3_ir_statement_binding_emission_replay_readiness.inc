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
