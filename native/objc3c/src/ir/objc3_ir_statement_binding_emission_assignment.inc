namespace {

ValueType Objc3IRStorageTypeForPtr(const std::string &ptr,
                                   const FunctionContext &ctx) {
  const auto found_type = ctx.value_type_by_ptr.find(ptr);
  if (found_type == ctx.value_type_by_ptr.end()) {
    return ValueType::I32;
  }
  return found_type->second;
}

ValueType InferObjc3IRBindingStorageType(
    const Expr *expr, const FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  if (expr == nullptr) {
    return ValueType::I32;
  }
  switch (expr->kind) {
    case Expr::Kind::BoolLiteral:
      return ValueType::Bool;
    case Expr::Kind::StringLiteral:
    case Expr::Kind::StringInterpolation:
      return ValueType::TextHandle;
    case Expr::Kind::NilLiteral:
      return ValueType::ObjCId;
    case Expr::Kind::Identifier:
      for (auto it = ctx.scopes.rbegin(); it != ctx.scopes.rend(); ++it) {
        const auto found_ptr = it->find(expr->ident);
        if (found_ptr == it->end()) {
          continue;
        }
        return Objc3IRStorageTypeForPtr(found_ptr->second, ctx);
      }
      return ValueType::I32;
    case Expr::Kind::Call:
      if (expr->ident == kObjc3RuntimeOptionalAbsentI64Symbol ||
          expr->ident == kObjc3RuntimeOptionalPresentI32Symbol) {
        return ValueType::Optional;
      }
      if (callbacks.lookup_function_signature) {
        const LoweredFunctionSignature *signature =
            callbacks.lookup_function_signature(expr->ident);
        if (signature != nullptr) {
          return signature->return_type;
        }
      }
      return ValueType::I32;
    case Expr::Kind::Conditional:
      if (expr->right != nullptr && expr->third != nullptr) {
        const ValueType then_type =
            InferObjc3IRBindingStorageType(expr->right.get(), ctx, callbacks);
        const ValueType else_type =
            InferObjc3IRBindingStorageType(expr->third.get(), ctx, callbacks);
        if (then_type == else_type) {
          return then_type;
        }
      }
      return ValueType::I32;
    case Expr::Kind::MatchExpression:
      if (expr->match_expression_result_type_spelling == "Text") {
        return ValueType::TextHandle;
      }
      if (expr->match_expression_result_type_spelling == "bool") {
        return ValueType::Bool;
      }
      if (expr->match_expression_result_type_spelling == "id") {
        return ValueType::ObjCId;
      }
      return ValueType::I32;
    case Expr::Kind::Binary:
      if (expr->op == "==" || expr->op == "!=" || expr->op == "<" ||
          expr->op == "<=" || expr->op == ">" || expr->op == ">=" ||
          expr->op == "&&" || expr->op == "||") {
        return ValueType::Bool;
      }
      return ValueType::I32;
    default:
      return ValueType::I32;
  }
}

}  // namespace

void EmitObjc3IRAssignmentStore(
    const std::string &ptr, const std::string &op, const Expr *value_expr,
    FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  if (ptr.empty()) {
    return;
  }
  const ValueType storage_type = Objc3IRStorageTypeForPtr(ptr, ctx);
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
    if (storage_type != ValueType::I32) {
      (void)callbacks.emit_unsupported_i32_value(
          "increment and decrement assignment require i32 storage");
      return;
    }
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
      if (storage_type != ValueType::I32) {
        (void)callbacks.emit_unsupported_i32_value(
            "ARC assignment storage requires i32 object handles");
        return;
      }
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
      ctx.code_lines.push_back(
          "  store " + std::string(LLVMLocalStorageType(storage_type)) + " " +
          value + ", ptr " + ptr + ", align " +
          std::to_string(LLVMLocalStorageAlignment(storage_type)));
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
    ctx.code_lines.push_back(
        "  store " + std::string(LLVMLocalStorageType(storage_type)) + " " + value +
        ", ptr " + ptr + ", align " +
        std::to_string(LLVMLocalStorageAlignment(storage_type)));
    return;
  }

  if (storage_type != ValueType::I32) {
    (void)callbacks.emit_unsupported_i32_value(
        "compound assignment requires i32 storage");
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
      if (clause.value->kind == Expr::Kind::CollectionLiteral &&
          clause.value->collection_literal_kind ==
              Expr::CollectionLiteralKind::Array) {
        clause.value->collection_literal_mutable = clause.mutable_binding;
      }
      const std::string value = callbacks.emit_expr(clause.value.get(), ctx);
      if (ctx.terminated) {
        return;
      }
      const std::string ptr =
          "%" + clause.name + ".addr." + std::to_string(ctx.temp_counter++);
      const ValueType storage_type =
          InferObjc3IRBindingStorageType(clause.value.get(), ctx, callbacks);
      const Objc3IRLocalI32StorageMetadata metadata =
          CollectObjc3IRLocalI32StorageMetadata(clause.value.get(), ctx,
                                                callbacks);
      ctx.entry_lines.push_back(
          "  " + ptr + " = alloca " +
          std::string(LLVMLocalStorageType(storage_type)) + ", align " +
          std::to_string(LLVMLocalStorageAlignment(storage_type)));
      ctx.scopes.back()[clause.name] = ptr;
      ctx.value_type_by_ptr[ptr] = storage_type;
      if (clause.value->kind == Expr::Kind::CollectionLiteral) {
        ctx.collection_kind_by_ptr[ptr] =
            clause.value->collection_literal_kind;
        if (clause.mutable_binding) {
          ctx.mutable_collection_ptrs.insert(ptr);
        }
      }
      RecordObjc3IRLocalI32StorageMetadata(ptr, metadata, ctx, false);
      ctx.code_lines.push_back(
          "  store " + std::string(LLVMLocalStorageType(storage_type)) + " " +
          value + ", ptr " + ptr + ", align " +
          std::to_string(LLVMLocalStorageAlignment(storage_type)));
      return;
    }
  }
}
