    case Expr::Kind::Binary: {
      const SemanticTypeInfo lhs = ValidateExpr(
          expr->left.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      const SemanticTypeInfo rhs = ValidateExpr(
          expr->right.get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);

      if (expr->op == "??") {
        const bool lhs_reference_like =
            IsObjCReferenceSemanticType(lhs) || IsCallableSemanticType(lhs);
        const bool rhs_reference_like =
            IsObjCReferenceSemanticType(rhs) || IsCallableSemanticType(rhs);
        if (!IsUnknownSemanticType(lhs) && !lhs_reference_like) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: nil-coalescing lhs must be ObjC-reference-compatible"));
        }
        if (!IsUnknownSemanticType(rhs) && !rhs_reference_like) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: nil-coalescing rhs must be ObjC-reference-compatible"));
        }
        if (IsUnknownSemanticType(lhs)) {
          return rhs;
        }
        if (IsUnknownSemanticType(rhs)) {
          return MakeNonnullRefinedSemanticType(lhs);
        }
        if (AreObjCReferenceTypesAssignmentCompatible(lhs, rhs)) {
          if (lhs.type == rhs.type) {
            return rhs.has_nullability_suffix ? lhs : MakeNonnullRefinedSemanticType(lhs);
          }
          return MakeNonnullRefinedSemanticType(MakeScalarSemanticType(ValueType::ObjCId));
        }
        if (IsCallableSemanticType(lhs) && IsCallableSemanticType(rhs) &&
            lhs.callable_param_types == rhs.callable_param_types &&
            lhs.callable_return_type == rhs.callable_return_type) {
          return lhs;
        }
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                       "type mismatch: nil-coalescing operands must be type-compatible"));
        return MakeScalarSemanticType(ValueType::Unknown);
      }

      if (expr->op == "+" || expr->op == "-" || expr->op == "*" || expr->op == "/" || expr->op == "%") {
        if (!IsUnknownSemanticType(lhs) && !IsScalarI32CompatibleType(lhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for arithmetic lhs, got '" +
                                             SemanticTypeName(lhs) + "'"));
        }
        if (!IsUnknownSemanticType(rhs) && !IsScalarI32CompatibleType(rhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for arithmetic rhs, got '" +
                                             SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::I32);
      }

      if (expr->op == "&" || expr->op == "|" || expr->op == "^" || expr->op == "<<" || expr->op == ">>") {
        if (!IsUnknownSemanticType(lhs) && !IsScalarI32CompatibleType(lhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for bitwise lhs, got '" +
                                             SemanticTypeName(lhs) + "'"));
        }
        if (!IsUnknownSemanticType(rhs) && !IsScalarI32CompatibleType(rhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for bitwise rhs, got '" +
                                             SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::I32);
      }

      if (expr->op == "==" || expr->op == "!=") {
        if (lhs.is_vector || rhs.is_vector) {
          if (!IsUnknownSemanticType(lhs) && !IsUnknownSemanticType(rhs) && !IsSameSemanticType(lhs, rhs)) {
            diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                           "type mismatch: equality compares '" + SemanticTypeName(lhs) +
                                               "' with '" + SemanticTypeName(rhs) + "'"));
          }
          return MakeScalarSemanticType(ValueType::Bool);
        }

        const bool bool_to_i32_literal =
            (lhs.type == ValueType::Bool && rhs.type == ValueType::I32 && IsBoolLikeI32Literal(expr->right.get())) ||
            (rhs.type == ValueType::Bool && lhs.type == ValueType::I32 && IsBoolLikeI32Literal(expr->left.get()));
        const bool i32_alias_coercion = AreScalarI32AliasCompatible(lhs, rhs);
        if (!IsUnknownSemanticType(lhs) && !IsUnknownSemanticType(rhs) && lhs.type != rhs.type &&
            !bool_to_i32_literal && !i32_alias_coercion) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: equality compares '" + SemanticTypeName(lhs) +
                                             "' with '" + SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::Bool);
      }

      if (expr->op == "<" || expr->op == "<=" || expr->op == ">" || expr->op == ">=") {
        if (!IsUnknownSemanticType(lhs) && !IsScalarI32CompatibleType(lhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for relational lhs, got '" +
                                             SemanticTypeName(lhs) + "'"));
        }
        if (!IsUnknownSemanticType(rhs) && !IsScalarI32CompatibleType(rhs)) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected i32 for relational rhs, got '" +
                                             SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::Bool);
      }

      if (expr->op == "&&" || expr->op == "||") {
        if (!IsUnknownSemanticType(lhs) && (lhs.is_vector || (lhs.type != ValueType::Bool && lhs.type != ValueType::I32))) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected bool for logical lhs, got '" +
                                             SemanticTypeName(lhs) + "'"));
        }
        if (!IsUnknownSemanticType(rhs) && (rhs.is_vector || (rhs.type != ValueType::Bool && rhs.type != ValueType::I32))) {
          diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S206",
                                         "type mismatch: expected bool for logical rhs, got '" +
                                             SemanticTypeName(rhs) + "'"));
        }
        return MakeScalarSemanticType(ValueType::Bool);
      }

      return MakeScalarSemanticType(ValueType::Unknown);
    }
