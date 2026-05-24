namespace {

ValueType Objc3IRStorageTypeForPtr(const std::string &ptr,
                                   const FunctionContext &ctx) {
  const auto found_type = ctx.value_type_by_ptr.find(ptr);
  if (found_type == ctx.value_type_by_ptr.end()) {
    return ValueType::I32;
  }
  return found_type->second;
}

const char *Objc3IRLocalStorageTypeForCarrier(
    ValueType value_type, Objc3IRValueOptionalCarrierKind carrier) {
  return LLVMLocalStorageTypeForValueOptionalCarrier(value_type, carrier);
}

unsigned Objc3IRLocalStorageAlignmentForCarrier(
    ValueType value_type, Objc3IRValueOptionalCarrierKind carrier) {
  return LLVMLocalStorageAlignmentForValueOptionalCarrier(value_type, carrier);
}

Objc3IRValueOptionalCarrierKind Objc3IRCarrierForPtr(
    const std::string &ptr, const FunctionContext &ctx) {
  const auto found_carrier = ctx.value_optional_carrier_by_ptr.find(ptr);
  if (found_carrier == ctx.value_optional_carrier_by_ptr.end()) {
    return Objc3IRValueOptionalCarrierKind::PackedI64;
  }
  return found_carrier->second;
}

bool IsObjc3IRPackedOptionalConstructorHelper(const std::string &ident) {
  return ident == kObjc3RuntimeOptionalAbsentI64Symbol ||
         ident == kObjc3RuntimeOptionalAbsentBoolSymbol ||
         ident == kObjc3RuntimeOptionalAbsentIdSymbol ||
         ident == kObjc3RuntimeOptionalPresentI32Symbol ||
         ident == kObjc3RuntimeOptionalPresentBoolSymbol ||
         ident == kObjc3RuntimeOptionalPresentIdSymbol;
}

bool IsObjc3IRFullI64OptionalConstructorHelper(const std::string &ident) {
  return ident == kObjc3RuntimeOptionalAbsentFullI64Symbol ||
         ident == kObjc3RuntimeOptionalPresentFullI64Symbol;
}

std::string EmitObjc3IRFullI64OptionalConstructor(
    const Expr *expr, FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  if (expr == nullptr || expr->kind != Expr::Kind::Call ||
      !IsObjc3IRFullI64OptionalConstructorHelper(expr->ident)) {
    return "";
  }
  const std::string value = callbacks.new_temp(ctx);
  if (expr->ident == kObjc3RuntimeOptionalAbsentFullI64Symbol) {
    ctx.code_lines.push_back("  " + value + " = call { i8, i64 } @" +
                             std::string(
                                 kObjc3RuntimeOptionalAbsentFullI64Symbol) +
                             "()");
    ctx.value_optional_carrier_by_value[value] =
        Objc3IRValueOptionalCarrierKind::FullI64;
    return value;
  }
  if (expr->args.size() != 1u) {
    return callbacks.emit_unsupported_i32_value(
        "Optional<i64> present helper lowering requires exactly one payload");
  }
  std::string payload = callbacks.emit_expr(expr->args.front().get(), ctx);
  if (ctx.terminated) {
    return value;
  }
  if (expr->args.front()->kind != Expr::Kind::Number) {
    const std::string widened_payload = callbacks.new_temp(ctx);
    ctx.code_lines.push_back("  " + widened_payload + " = sext i32 " +
                             payload + " to i64");
    payload = widened_payload;
  }
  ctx.code_lines.push_back("  " + value + " = call { i8, i64 } @" +
                           std::string(
                               kObjc3RuntimeOptionalPresentFullI64Symbol) +
                           "(i64 " + payload + ")");
  ctx.value_optional_carrier_by_value[value] =
      Objc3IRValueOptionalCarrierKind::FullI64;
  return value;
}

bool TryInferObjc3IRBindingOptionalCarrier(
    const Expr *expr, const FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks,
    Objc3IRValueOptionalCarrierKind &carrier_out) {
  if (expr == nullptr) {
    return false;
  }
  (void)callbacks;
  switch (expr->kind) {
    case Expr::Kind::Identifier:
      for (auto it = ctx.scopes.rbegin(); it != ctx.scopes.rend(); ++it) {
        const auto found_ptr = it->find(expr->ident);
        if (found_ptr == it->end()) {
          continue;
        }
        const auto found_carrier =
            ctx.value_optional_carrier_by_ptr.find(found_ptr->second);
        if (found_carrier != ctx.value_optional_carrier_by_ptr.end()) {
          carrier_out = found_carrier->second;
          return true;
        }
        break;
      }
      return false;
    case Expr::Kind::Call:
      if (IsObjc3IRFullI64OptionalConstructorHelper(expr->ident)) {
        carrier_out = Objc3IRValueOptionalCarrierKind::FullI64;
        return true;
      }
      if (IsObjc3IRPackedOptionalConstructorHelper(expr->ident)) {
        carrier_out = Objc3IRValueOptionalCarrierKind::PackedI64;
        return true;
      }
      return false;
    default:
      return false;
  }
}

Objc3IRValueOptionalCarrierKind InferObjc3IRBindingOptionalCarrierOrPacked(
    const Expr *expr, const FunctionContext &ctx,
    const Objc3IRStatementEmissionCallbacks &callbacks) {
  Objc3IRValueOptionalCarrierKind carrier =
      Objc3IRValueOptionalCarrierKind::PackedI64;
  (void)TryInferObjc3IRBindingOptionalCarrier(expr, ctx, callbacks, carrier);
  return carrier;
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
      if (IsObjc3IRPackedOptionalConstructorHelper(expr->ident) ||
          IsObjc3IRFullI64OptionalConstructorHelper(expr->ident)) {
        return ValueType::Optional;
      }
      if (expr->ident == kObjc3RuntimeOptionalPayloadOrIdSymbol ||
          expr->ident == kObjc3RuntimeOptionalUnwrapIdSymbol) {
        return ValueType::ObjCId;
      }
      if (expr->ident == kObjc3RuntimeOptionalHasValueFullI64Symbol) {
        return ValueType::Bool;
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
  const Objc3IRValueOptionalCarrierKind storage_carrier =
      Objc3IRCarrierForPtr(ptr, ctx);
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
    std::string value =
        storage_type == ValueType::Optional
            ? EmitObjc3IRFullI64OptionalConstructor(value_expr, ctx, callbacks)
            : "";
    if (value.empty()) {
      value = callbacks.emit_expr(value_expr, ctx);
    }
    if (ctx.terminated) {
      return;
    }
    if (storage_type == ValueType::Optional) {
      Objc3IRValueOptionalCarrierKind source_carrier =
          Objc3IRValueOptionalCarrierKind::PackedI64;
      bool has_source_carrier = TryInferObjc3IRBindingOptionalCarrier(
          value_expr, ctx, callbacks, source_carrier);
      const auto emitted_carrier = ctx.value_optional_carrier_by_value.find(value);
      if (emitted_carrier != ctx.value_optional_carrier_by_value.end()) {
        source_carrier = emitted_carrier->second;
        has_source_carrier = true;
      }
      if (has_source_carrier && source_carrier != storage_carrier) {
        (void)callbacks.emit_unsupported_i32_value(
            "assignment between packed and full-width Optional carriers requires explicit optional carrier conversion");
        return;
      }
      ctx.value_optional_carrier_by_value[value] = storage_carrier;
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
          "  store " +
          std::string(
              Objc3IRLocalStorageTypeForCarrier(storage_type, storage_carrier)) +
          " " +
          value + ", ptr " + ptr + ", align " +
          std::to_string(
              Objc3IRLocalStorageAlignmentForCarrier(storage_type,
                                                     storage_carrier)));
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
    std::string value =
        storage_type == ValueType::Optional
            ? EmitObjc3IRFullI64OptionalConstructor(value_expr, ctx, callbacks)
            : "";
    if (value.empty()) {
      value = callbacks.emit_expr(value_expr, ctx);
    }
    if (ctx.terminated) {
      return;
    }
    if (storage_type == ValueType::Optional) {
      ctx.value_optional_carrier_by_value[value] = storage_carrier;
    }
    ctx.code_lines.push_back(
        "  store " +
        std::string(
            Objc3IRLocalStorageTypeForCarrier(storage_type, storage_carrier)) +
        " " + value +
        ", ptr " + ptr + ", align " +
        std::to_string(Objc3IRLocalStorageAlignmentForCarrier(storage_type,
                                                              storage_carrier)));
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
      const ValueType storage_type =
          InferObjc3IRBindingStorageType(clause.value.get(), ctx, callbacks);
      Objc3IRValueOptionalCarrierKind storage_carrier =
          InferObjc3IRBindingOptionalCarrierOrPacked(clause.value.get(), ctx,
                                                     callbacks);
      std::string value =
          storage_type == ValueType::Optional
              ? EmitObjc3IRFullI64OptionalConstructor(clause.value.get(), ctx,
                                                      callbacks)
              : "";
      if (value.empty()) {
        value = callbacks.emit_expr(clause.value.get(), ctx);
      }
      if (ctx.terminated) {
        return;
      }
      if (storage_type == ValueType::Optional) {
        const auto emitted_carrier =
            ctx.value_optional_carrier_by_value.find(value);
        if (emitted_carrier != ctx.value_optional_carrier_by_value.end()) {
          storage_carrier = emitted_carrier->second;
        }
      }
      const std::string ptr =
          "%" + clause.name + ".addr." + std::to_string(ctx.temp_counter++);
      const Objc3IRLocalI32StorageMetadata metadata =
          CollectObjc3IRLocalI32StorageMetadata(clause.value.get(), ctx,
                                                callbacks);
      ctx.entry_lines.push_back(
          "  " + ptr + " = alloca " +
          std::string(Objc3IRLocalStorageTypeForCarrier(storage_type,
                                                        storage_carrier)) +
          ", align " +
          std::to_string(Objc3IRLocalStorageAlignmentForCarrier(
              storage_type, storage_carrier)));
      ctx.scopes.back()[clause.name] = ptr;
      ctx.value_type_by_ptr[ptr] = storage_type;
      if (storage_type == ValueType::Optional) {
        ctx.value_optional_carrier_by_ptr[ptr] = storage_carrier;
        ctx.value_optional_carrier_by_value[value] = storage_carrier;
      }
      if (clause.value->kind == Expr::Kind::CollectionLiteral) {
        ctx.collection_kind_by_ptr[ptr] =
            clause.value->collection_literal_kind;
        if (clause.mutable_binding) {
          ctx.mutable_collection_ptrs.insert(ptr);
        }
      }
      RecordObjc3IRLocalI32StorageMetadata(ptr, metadata, ctx, false);
      ctx.code_lines.push_back(
          "  store " +
          std::string(Objc3IRLocalStorageTypeForCarrier(storage_type,
                                                        storage_carrier)) +
          " " +
          value + ", ptr " + ptr + ", align " +
          std::to_string(Objc3IRLocalStorageAlignmentForCarrier(
              storage_type, storage_carrier)));
      return;
    }
  }
}
