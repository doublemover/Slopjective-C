  const Objc3ProtocolQualifiedMessageRequirementResolution protocol_resolution =
      ResolveProtocolQualifiedMessageRequirement(
          receiver_type.canonical_type, selector, false, message_send_context);
  if (protocol_resolution.status ==
      Objc3ProtocolQualifiedMessageRequirementResolution::Status::
          MissingRequirementClosure) {
    const unsigned diag_line =
        expr->receiver != nullptr ? expr->receiver->line : expr->line;
    const unsigned diag_column =
        expr->receiver != nullptr ? expr->receiver->column : expr->column;
    diagnostics.push_back(MakeDiag(
        diag_line, diag_column, "O3S216",
        "selector resolution failed: protocol-qualified receiver '" +
            receiver_type.canonical_type.canonical_spelling +
            "' cannot resolve selector '" + selector +
            "' because protocol '" + protocol_resolution.failing_protocol_name +
            "' requirements are unavailable"));
    return MakeScalarSemanticType(ValueType::Unknown);
  }
  if (protocol_resolution.status ==
      Objc3ProtocolQualifiedMessageRequirementResolution::Status::
          MissingSelector) {
    const unsigned diag_line =
        expr->receiver != nullptr ? expr->receiver->line : expr->line;
    const unsigned diag_column =
        expr->receiver != nullptr ? expr->receiver->column : expr->column;
    diagnostics.push_back(MakeDiag(
        diag_line, diag_column, "O3S216",
        "selector resolution failed: protocol-qualified receiver '" +
            receiver_type.canonical_type.canonical_spelling +
            "' does not provide selector '" +
            FormatMethodSelectorForDiagnostic(selector, false) + "'"));
    return MakeScalarSemanticType(ValueType::Unknown);
  }
  if (protocol_resolution.status ==
      Objc3ProtocolQualifiedMessageRequirementResolution::Status::
          AmbiguousSelector) {
    const unsigned diag_line =
        expr->receiver != nullptr ? expr->receiver->line : expr->line;
    const unsigned diag_column =
        expr->receiver != nullptr ? expr->receiver->column : expr->column;
    diagnostics.push_back(MakeDiag(
        diag_line, diag_column, "O3S217",
        "selector resolution is ambiguous for protocol-qualified receiver '" +
            receiver_type.canonical_type.canonical_spelling +
            "' and selector '" +
            FormatMethodSelectorForDiagnostic(selector, false) +
            "': incompatible protocol requirements"));
    return MakeScalarSemanticType(ValueType::Unknown);
  }
  if (protocol_resolution.status ==
          Objc3ProtocolQualifiedMessageRequirementResolution::Status::
              Resolved &&
      protocol_resolution.property_requirement != nullptr) {
    const Objc3PropertyInfo &property =
        protocol_resolution.property_requirement->property;
    const std::size_t expected_arity =
        protocol_resolution.property_requirement_is_setter ? 1u : 0u;
    if (expr->args.size() != expected_arity) {
      diagnostics.push_back(MakeDiag(
          expr->line, expr->column, "O3S216",
          "selector resolution failed: property accessor selector '" +
              selector + "' resolves to arity " +
              std::to_string(expected_arity) +
              " for protocol-qualified receiver '" +
              receiver_type.canonical_type.canonical_spelling + "'"));
      return MakeScalarSemanticType(ValueType::Unknown);
    }
    if (protocol_resolution.property_requirement_is_setter) {
      const SemanticTypeInfo arg_type = ValidateExpr(
          expr->args[0].get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      const SemanticTypeInfo expected =
          MakeSemanticTypeFromPropertyInfo(property);
      const bool bool_coercion =
          !expected.is_vector && expected.type == ValueType::Bool &&
          !arg_type.is_vector && arg_type.type == ValueType::I32;
      const bool i32_alias_coercion =
          AreScalarI32AliasCompatible(expected, arg_type);
      const bool objc_reference_coercion =
          AreObjCReferenceTypesAssignmentCompatible(expected, arg_type);
      if (!IsUnknownSemanticType(arg_type) && !IsUnknownSemanticType(expected) &&
          !IsSameSemanticType(arg_type, expected) && !bool_coercion &&
          !i32_alias_coercion && !objc_reference_coercion) {
        diagnostics.push_back(MakeDiag(
            expr->args[0]->line, expr->args[0]->column, "O3S206",
            "type mismatch: property accessor selector '" + selector +
                "' on protocol-qualified receiver '" +
                receiver_type.canonical_type.canonical_spelling +
                "' expects '" + SemanticTypeName(expected) +
                "', got '" + SemanticTypeName(arg_type) + "'"));
      }
      return MakeScalarSemanticType(ValueType::Void);
    }
    return MakeSemanticTypeFromPropertyInfo(property);
  }
  if (protocol_resolution.status ==
          Objc3ProtocolQualifiedMessageRequirementResolution::Status::
              Resolved &&
      protocol_resolution.requirement != nullptr) {
    const Objc3MethodInfo &method = protocol_resolution.requirement->method;
    if (expr->args.size() != method.arity) {
      diagnostics.push_back(MakeDiag(
          expr->line, expr->column, "O3S216",
          "selector resolution failed: selector '" + selector +
              "' resolves to arity " + std::to_string(method.arity) +
              " for protocol-qualified receiver '" +
              receiver_type.canonical_type.canonical_spelling + "'"));
      return MakeScalarSemanticType(ValueType::Unknown);
    }
    for (std::size_t i = 0; i < expr->args.size(); ++i) {
      const SemanticTypeInfo arg_type = ValidateExpr(
          expr->args[i].get(), scopes, globals, functions, diagnostics,
          max_message_send_args, message_send_context);
      const SemanticTypeInfo expected =
          MakeSemanticTypeFromMethodInfoParam(method, i);
      const bool bool_coercion =
          !expected.is_vector && expected.type == ValueType::Bool &&
          !arg_type.is_vector && arg_type.type == ValueType::I32;
      const bool i32_alias_coercion =
          AreScalarI32AliasCompatible(expected, arg_type);
      const bool objc_reference_coercion =
          AreObjCReferenceTypesAssignmentCompatible(expected, arg_type);
      if (!IsUnknownSemanticType(arg_type) && !IsUnknownSemanticType(expected) &&
          !IsSameSemanticType(arg_type, expected) && !bool_coercion &&
          !i32_alias_coercion && !objc_reference_coercion) {
        diagnostics.push_back(MakeDiag(
            expr->args[i]->line, expr->args[i]->column, "O3S206",
            "type mismatch: selector '" + selector +
                "' on protocol-qualified receiver '" +
                receiver_type.canonical_type.canonical_spelling +
                "' expects '" + SemanticTypeName(expected) +
                "' for argument " + std::to_string(i) + ", got '" +
                SemanticTypeName(arg_type) + "'"));
      }
    }
    const SemanticTypeInfo return_type = MakeSemanticTypeFromMethodInfoReturn(method);
    DiagnoseRetainedMethodFamilyReturnTypeMismatch(
        expr, selector,
        "protocol-qualified receiver '" +
            receiver_type.canonical_type.canonical_spelling + "'",
        return_type, diagnostics);
    return return_type;
  }
