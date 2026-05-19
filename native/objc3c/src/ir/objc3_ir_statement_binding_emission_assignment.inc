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
    RecordObjc3IRAssignmentStorageMetadata(
        ptr, has_assigned_nil_value, has_assigned_const_value,
        assigned_const_value, ctx);
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
      const Objc3IRLocalI32StorageMetadata metadata =
          CollectObjc3IRLocalI32StorageMetadata(clause.value.get(), ctx,
                                                callbacks);
      ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
      ctx.scopes.back()[clause.name] = ptr;
      RecordObjc3IRLocalI32StorageMetadata(ptr, metadata, ctx, false);
      ctx.code_lines.push_back("  store i32 " + value + ", ptr " + ptr +
                               ", align 4");
      return;
    }
  }
}
