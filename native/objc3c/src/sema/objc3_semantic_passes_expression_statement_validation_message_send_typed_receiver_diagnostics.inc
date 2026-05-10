  if (!receiver_type.object_pointer_type_name.empty() &&
      message_send_context.surface != nullptr) {
    const Objc3ResolvedMessageSendMethod typed_receiver_resolution =
        ResolveConcreteOwnerMethod(*message_send_context.surface,
                                   receiver_type.object_pointer_type_name,
                                   selector, false);
    if (typed_receiver_resolution.status ==
        Objc3ResolvedMessageSendMethod::Status::MissingConcreteSelector) {
      const Objc3ResolvedMessageSendPropertyAccessor property_resolution =
          ResolveConcreteOwnerPropertyAccessor(*message_send_context.surface,
                                               receiver_type.object_pointer_type_name,
                                               selector, false);
      if (property_resolution.property != nullptr) {
        const std::size_t expected_arity =
            property_resolution.is_setter ? 1u : 0u;
        if (expr->args.size() != expected_arity) {
          diagnostics.push_back(MakeDiag(
              expr->line, expr->column, "O3S216",
              "selector resolution failed: property accessor selector '" +
                  selector + "' resolves to arity " +
                  std::to_string(expected_arity) + " for typed receiver '" +
                  receiver_type.object_pointer_type_name + "'"));
          return MakeScalarSemanticType(ValueType::Unknown);
        }
        if (property_resolution.is_setter) {
          const SemanticTypeInfo arg_type = ValidateExpr(
              expr->args[0].get(), scopes, globals, functions, diagnostics,
              max_message_send_args, message_send_context);
          const SemanticTypeInfo expected = SubstituteGenericReceiverType(
              MakeSemanticTypeFromPropertyInfo(*property_resolution.property),
              receiver_type.canonical_type, *message_send_context.surface);
          const bool bool_coercion =
              !expected.is_vector && expected.type == ValueType::Bool &&
              !arg_type.is_vector && arg_type.type == ValueType::I32;
          const bool i32_alias_coercion =
              AreScalarI32AliasCompatible(expected, arg_type);
          const bool objc_reference_coercion =
              AreObjCReferenceTypesAssignmentCompatible(expected, arg_type);
          if (!IsUnknownSemanticType(arg_type) &&
              !IsUnknownSemanticType(expected) &&
              !IsSameSemanticType(arg_type, expected) && !bool_coercion &&
              !i32_alias_coercion && !objc_reference_coercion) {
            diagnostics.push_back(MakeDiag(
                expr->args[0]->line, expr->args[0]->column, "O3S206",
                "type mismatch: property accessor selector '" + selector +
                    "' on typed receiver '" +
                    receiver_type.object_pointer_type_name + "' expects '" +
                    SemanticTypeName(expected) + "', got '" +
                    SemanticTypeName(arg_type) + "'"));
          }
          return MakeScalarSemanticType(ValueType::Void);
        }
        return SubstituteGenericReceiverType(
            MakeSemanticTypeFromPropertyInfo(*property_resolution.property),
            receiver_type.canonical_type, *message_send_context.surface);
      }
      const unsigned diag_line =
          expr->receiver != nullptr ? expr->receiver->line : expr->line;
      const unsigned diag_column =
          expr->receiver != nullptr ? expr->receiver->column : expr->column;
      diagnostics.push_back(MakeDiag(
          diag_line, diag_column, "O3S216",
          "selector resolution failed: typed receiver '" +
              receiver_type.object_pointer_type_name +
              "' does not provide selector '" + selector + "'"));
      return MakeScalarSemanticType(ValueType::Unknown);
    }
    if (typed_receiver_resolution.status ==
        Objc3ResolvedMessageSendMethod::Status::AmbiguousConcreteSelector) {
      const unsigned diag_line =
          expr->receiver != nullptr ? expr->receiver->line : expr->line;
      const unsigned diag_column =
          expr->receiver != nullptr ? expr->receiver->column : expr->column;
      const std::string detail =
          typed_receiver_resolution.cycle_detected
              ? "super-chain cycle detected during selector resolution"
              : (typed_receiver_resolution.missing_base
                     ? "super-chain lookup requires a missing base interface"
                     : "incompatible declarations produce no unique concrete target");
      diagnostics.push_back(MakeDiag(
          diag_line, diag_column, "O3S217",
          "selector resolution is ambiguous for typed receiver '" +
              receiver_type.object_pointer_type_name + "' and selector '" +
              selector + "': " + detail));
      return MakeScalarSemanticType(ValueType::Unknown);
    }
    if (typed_receiver_resolution.status ==
            Objc3ResolvedMessageSendMethod::Status::Resolved &&
        typed_receiver_resolution.method != nullptr) {
      const Objc3MethodInfo &method = *typed_receiver_resolution.method;
      if (expr->args.size() != method.arity) {
        diagnostics.push_back(MakeDiag(
            expr->line, expr->column, "O3S216",
            "selector resolution failed: selector '" + selector +
                "' resolves to arity " + std::to_string(method.arity) +
                " for typed receiver '" +
                receiver_type.object_pointer_type_name + "'"));
        return MakeScalarSemanticType(ValueType::Unknown);
      }
      for (std::size_t i = 0; i < expr->args.size(); ++i) {
        const SemanticTypeInfo arg_type = ValidateExpr(
            expr->args[i].get(), scopes, globals, functions, diagnostics,
            max_message_send_args, message_send_context);
        const SemanticTypeInfo expected = SubstituteGenericReceiverType(
            MakeSemanticTypeFromMethodInfoParam(method, i),
            receiver_type.canonical_type, *message_send_context.surface);
        const bool bool_coercion =
            !expected.is_vector && expected.type == ValueType::Bool &&
            !arg_type.is_vector && arg_type.type == ValueType::I32;
        const bool i32_alias_coercion =
            AreScalarI32AliasCompatible(expected, arg_type);
        const bool objc_reference_coercion =
            AreObjCReferenceTypesAssignmentCompatible(expected, arg_type);
        if (!IsUnknownSemanticType(arg_type) &&
            !IsUnknownSemanticType(expected) &&
            !IsSameSemanticType(arg_type, expected) && !bool_coercion &&
            !i32_alias_coercion && !objc_reference_coercion) {
          diagnostics.push_back(MakeDiag(
              expr->args[i]->line, expr->args[i]->column, "O3S206",
              "type mismatch: selector '" + selector +
                  "' on typed receiver '" +
                  receiver_type.object_pointer_type_name + "' expects '" +
                  SemanticTypeName(expected) + "' for argument " +
                  std::to_string(i) + ", got '" + SemanticTypeName(arg_type) +
                  "'"));
        }
      }
      return SubstituteGenericReceiverType(
          MakeSemanticTypeFromMethodInfoReturn(method),
          receiver_type.canonical_type, *message_send_context.surface);
    }
  }
