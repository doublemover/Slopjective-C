    case Stmt::Kind::Return:
      if (stmt->return_stmt != nullptr) {
        const ReturnStmt *ret = stmt->return_stmt.get();
        if (inside_defer_body) {
          diagnostics.push_back(MakeDiag(
              ret->line,
              ret->column,
              "O3S222",
              "control-flow misuse: 'return' is not allowed inside defer body"));
          if (ret->value != nullptr) {
            (void)ValidateExpr(ret->value.get(), scopes, globals, functions,
                               diagnostics, max_message_send_args,
                               message_send_context);
          }
          return;
        }
        if (ret->value == nullptr) {
          if (!(IsScalarSemanticType(expected_return_type) && expected_return_type.type == ValueType::Void)) {
            diagnostics.push_back(MakeDiag(ret->line, ret->column, "O3S211",
                                           "type mismatch: function '" + function_name + "' must return '" +
                                               SemanticTypeName(expected_return_type) + "'"));
          }
          return;
        }

        if (IsScalarSemanticType(expected_return_type) && expected_return_type.type == ValueType::Void) {
          diagnostics.push_back(MakeDiag(ret->line, ret->column, "O3S211",
                                         "type mismatch: void function '" + function_name +
                                             "' must use 'return;'"));
          (void)ValidateExpr(ret->value.get(), scopes, globals, functions, diagnostics,
                             max_message_send_args, message_send_context);
          return;
        }

        const SemanticTypeInfo return_type = ValidateExpr(
            ret->value.get(), scopes, globals, functions, diagnostics,
            max_message_send_args, message_send_context);
        if (IsEscapingBlockRuntimeHandleCompatible(expected_return_type,
                                                   return_type)) {
          DiagnoseEscapingBlockRuntimeHandleCaptureLegality(
              ret->line, ret->column, return_type, diagnostics);
        }
        if (IsUnsafeNullableToNonnullFlow(expected_return_type, return_type)) {
          diagnostics.push_back(MakeDiag(
              ret->line, ret->column, "O3S227",
              "nullability mismatch: return expression in function '" +
                  function_name +
                  "' may flow nullable value into nonnull Objective-C reference return"));
          return;
        }
        const bool return_matches = IsSameSemanticType(return_type, expected_return_type) ||
            AreScalarI32AliasCompatible(expected_return_type, return_type) ||
            (IsScalarSemanticType(expected_return_type) && IsScalarSemanticType(return_type) &&
             expected_return_type.type == ValueType::I32 && return_type.type == ValueType::Bool) ||
            (IsScalarSemanticType(expected_return_type) && IsScalarSemanticType(return_type) &&
             expected_return_type.type == ValueType::Bool && return_type.type == ValueType::I32 &&
             IsBoolLikeI32Literal(ret->value.get())) ||
            AreObjCReferenceTypesAssignmentCompatible(expected_return_type, return_type) ||
            IsEscapingBlockRuntimeHandleCompatible(expected_return_type, return_type);
        if (!return_matches && !IsUnknownSemanticType(return_type) &&
            !(IsScalarSemanticType(return_type) && return_type.type == ValueType::Function)) {
          diagnostics.push_back(MakeDiag(ret->line, ret->column, "O3S211",
                                         "type mismatch: return expression in function '" + function_name +
                                             "' must be '" + SemanticTypeName(expected_return_type) +
                                             "', got '" + SemanticTypeName(return_type) + "'"));
        }
      }
      return;
