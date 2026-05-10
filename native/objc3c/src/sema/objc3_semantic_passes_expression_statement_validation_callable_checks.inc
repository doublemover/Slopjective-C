    case Expr::Kind::Call: {
      if (expr->try_expression_enabled) {
        SemanticTypeInfo operand_type = MakeScalarSemanticType(ValueType::Unknown);
        if (!expr->args.empty()) {
          operand_type = ValidateExpr(expr->args.front().get(), scopes, globals,
                                      functions, diagnostics,
                                      max_message_send_args,
                                      message_send_context);
        }
        return operand_type;
      }
      if (expr->throw_statement_enabled) {
        if (!expr->args.empty()) {
          (void)ValidateExpr(expr->args.front().get(), scopes, globals, functions,
                             diagnostics, max_message_send_args,
                             message_send_context);
        }
        return MakeScalarSemanticType(ValueType::Void);
      }
      const SemanticTypeInfo local_type = ScopeLookupType(scopes, expr->ident);
      if (!IsUnknownSemanticType(local_type)) {
        if (!IsCallableSemanticType(local_type)) {
          diagnostics.push_back(
              MakeDiag(expr->line,
                       expr->column,
                       "O3S206",
                       "type mismatch: symbol '" + expr->ident + "' is not callable"));
          for (const auto &arg : expr->args) {
            (void)ValidateExpr(arg.get(), scopes, globals, functions, diagnostics,
                               max_message_send_args, message_send_context);
          }
          return MakeScalarSemanticType(ValueType::Unknown);
        }
        if (local_type.callable_param_types.size() != expr->args.size()) {
          diagnostics.push_back(
              MakeDiag(expr->line,
                       expr->column,
                       "O3S204",
                       "arity mismatch for callable '" + expr->ident + "'"));
        }
        for (std::size_t i = 0; i < expr->args.size(); ++i) {
          const SemanticTypeInfo arg_type = ValidateExpr(
              expr->args[i].get(), scopes, globals, functions, diagnostics,
              max_message_send_args, message_send_context);
          if (i < local_type.callable_param_types.size()) {
            const SemanticTypeInfo expected =
                MakeScalarSemanticType(local_type.callable_param_types[i]);
            const bool bool_coercion =
                !expected.is_vector && expected.type == ValueType::Bool &&
                !arg_type.is_vector && arg_type.type == ValueType::I32;
            const bool i32_alias_coercion =
                AreScalarI32AliasCompatible(expected, arg_type);
            const bool objc_reference_coercion =
                AreObjCReferenceTypesAssignmentCompatible(
                    expected, arg_type, message_send_context.surface);
            const bool same_non_objc_semantic_type =
                IsSameSemanticType(arg_type, expected) &&
                !(IsObjCReferenceSemanticType(expected) &&
                  IsObjCReferenceSemanticType(arg_type));
            const std::optional<bool> generic_variance_compatibility =
                AreGenericSpecializationsVarianceAssignmentCompatible(
                    expected, arg_type, message_send_context.surface);
            const bool block_handle_coercion =
                IsEscapingBlockRuntimeHandleCompatible(expected, arg_type);
            if (block_handle_coercion) {
              DiagnoseEscapingBlockRuntimeHandleCaptureLegality(
                  expr->args[i]->line, expr->args[i]->column, arg_type,
                  diagnostics);
            }
            if (IsUnsafeNullableToNonnullFlow(
                    expected, arg_type, message_send_context.surface)) {
              diagnostics.push_back(
                  MakeDiag(expr->args[i]->line,
                           expr->args[i]->column,
                           "O3S227",
                           "nullability mismatch: parameter " +
                               std::to_string(i) + " of callable '" +
                               expr->ident +
                               "' requires a nonnull Objective-C reference"));
              continue;
            }
            if (generic_variance_compatibility.has_value() &&
                !*generic_variance_compatibility) {
              diagnostics.push_back(
                  MakeDiag(expr->args[i]->line,
                           expr->args[i]->column,
                           "O3S206",
                           "type mismatch: generic variance policy rejects argument for parameter " +
                               std::to_string(i) + " of callable '" +
                               expr->ident + "'"));
              continue;
            }
            if (!IsUnknownSemanticType(expected) &&
                !IsUnknownSemanticType(arg_type) &&
                !same_non_objc_semantic_type &&
                !bool_coercion &&
                !i32_alias_coercion &&
                !objc_reference_coercion &&
                !block_handle_coercion) {
              diagnostics.push_back(
                  MakeDiag(expr->args[i]->line,
                           expr->args[i]->column,
                           "O3S206",
                           "type mismatch: expected '" + SemanticTypeName(expected) +
                               "' argument for parameter " + std::to_string(i) +
                               " of callable '" + expr->ident + "', got '" +
                               SemanticTypeName(arg_type) + "'"));
            }
          }
        }
        return MakeScalarSemanticType(local_type.callable_return_type);
      }

      auto fn_it = functions.find(expr->ident);
      const auto global_it = globals.find(expr->ident);
      if (fn_it == functions.end() && global_it != globals.end()) {
        diagnostics.push_back(
            MakeDiag(expr->line,
                     expr->column,
                     "O3S206",
                     "type mismatch: global symbol '" + expr->ident + "' is not callable"));
      } else if (fn_it == functions.end()) {
        diagnostics.push_back(
            MakeDiag(expr->line, expr->column, "O3S203", "unknown function '" + expr->ident + "'"));
      } else if (fn_it->second.arity != expr->args.size()) {
        diagnostics.push_back(MakeDiag(expr->line, expr->column, "O3S204",
                                       "arity mismatch for function '" + expr->ident + "'"));
      }

      for (std::size_t i = 0; i < expr->args.size(); ++i) {
        const SemanticTypeInfo arg_type = ValidateExpr(
            expr->args[i].get(), scopes, globals, functions, diagnostics,
            max_message_send_args, message_send_context);
        if (fn_it != functions.end() && i < fn_it->second.param_types.size()) {
          if (i < fn_it->second.param_has_invalid_type_suffix.size() &&
              fn_it->second.param_has_invalid_type_suffix[i]) {
            continue;
          }
          const SemanticTypeInfo expected = MakeSemanticTypeFromFunctionInfoParam(fn_it->second, i);
          const bool bool_coercion =
              !expected.is_vector && expected.type == ValueType::Bool && !arg_type.is_vector && arg_type.type == ValueType::I32;
          const bool i32_alias_coercion = AreScalarI32AliasCompatible(expected, arg_type);
          const bool objc_reference_coercion =
              AreObjCReferenceTypesAssignmentCompatible(
                  expected, arg_type, message_send_context.surface);
          const bool same_non_objc_semantic_type =
              IsSameSemanticType(arg_type, expected) &&
              !(IsObjCReferenceSemanticType(expected) &&
                IsObjCReferenceSemanticType(arg_type));
          const std::optional<bool> generic_variance_compatibility =
              AreGenericSpecializationsVarianceAssignmentCompatible(
                  expected, arg_type, message_send_context.surface);
          const bool block_handle_coercion =
              IsEscapingBlockRuntimeHandleCompatible(expected, arg_type);
          if (block_handle_coercion) {
            DiagnoseEscapingBlockRuntimeHandleCaptureLegality(
                expr->args[i]->line, expr->args[i]->column, arg_type,
                diagnostics);
          }
          if (IsUnsafeNullableToNonnullFlow(
                  expected, arg_type, message_send_context.surface)) {
            diagnostics.push_back(
                MakeDiag(expr->args[i]->line, expr->args[i]->column, "O3S227",
                         "nullability mismatch: parameter " +
                             std::to_string(i) + " of '" + expr->ident +
                             "' requires a nonnull Objective-C reference"));
            continue;
          }
          if (generic_variance_compatibility.has_value() &&
              !*generic_variance_compatibility) {
            diagnostics.push_back(MakeDiag(
                expr->args[i]->line, expr->args[i]->column, "O3S206",
                "type mismatch: generic variance policy rejects argument for parameter " +
                    std::to_string(i) + " of '" + expr->ident + "'"));
            continue;
          }
          if (!IsUnknownSemanticType(arg_type) && !IsUnknownSemanticType(expected) &&
              !same_non_objc_semantic_type &&
              !bool_coercion &&
              !i32_alias_coercion &&
              !objc_reference_coercion &&
              !block_handle_coercion) {
            diagnostics.push_back(MakeDiag(expr->args[i]->line, expr->args[i]->column, "O3S206",
                                           "type mismatch: expected '" + SemanticTypeName(expected) +
                                               "' argument for parameter " + std::to_string(i) + " of '" +
                                               expr->ident + "', got '" + SemanticTypeName(arg_type) + "'"));
          }
        }
      }
      if (fn_it != functions.end()) {
        return MakeSemanticTypeFromFunctionInfoReturn(fn_it->second);
      }
      return MakeScalarSemanticType(ValueType::Unknown);
    }
