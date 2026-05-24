  bool AtRemovedOptionalTemplateAlias() const {
    return IsObjc3RemovedOptionalTemplateAliasLead(tokens_, index_);
  }

  bool AtReservedValueOptionalType() const {
    return IsObjc3ReservedValueOptionalTypeLead(tokens_, index_);
  }

  bool RejectRemovedOptionalTemplateAlias(const Token &token) {
    diagnostics_.push_back(
        BuildObjc3RemovedOptionalTemplateAliasDiagnostic(token));
    return false;
  }

  bool RejectReservedValueOptionalType(const Token &token) {
    diagnostics_.push_back(
        BuildObjc3ReservedValueOptionalTypeDiagnostic(token));
    return false;
  }

  Token ValueOptionalDiagnosticToken(
      const Objc3ValueOptionalTypeDescriptor &descriptor) const {
    Token token;
    token.kind = TokenKind::Identifier;
    token.text = "Optional";
    token.line = descriptor.line;
    token.column = descriptor.column;
    return token;
  }

  bool ValidateValueOptionalDescriptor(
      const Objc3ValueOptionalTypeDescriptor &descriptor,
      bool has_pointer_declarator,
      bool has_nullability_suffix,
      bool has_ownership_qualifier) {
    if (descriptor.payload_type_spelling.empty()) {
      diagnostics_.push_back(MakeDiag(
          descriptor.line, descriptor.column, "O3P114",
          "Optional<T> value optional type requires exactly one payload type"));
      return false;
    }
    if (descriptor.payload_lowercase_optional_alias) {
      diagnostics_.push_back(MakeDiag(
          descriptor.line, descriptor.column, "O3C004",
          "optional<T> aliases are rejected inside Optional<T> payloads; use canonical Optional<T> spelling"));
      return false;
    }
    if (has_pointer_declarator || has_nullability_suffix ||
        has_ownership_qualifier) {
      diagnostics_.push_back(BuildObjc3ReservedValueOptionalTypeDiagnostic(
          ValueOptionalDiagnosticToken(descriptor)));
      return false;
    }
    return true;
  }

  bool FinalizeParameterValueOptionalType(FuncParam &param) {
    if (!param.value_optional.present) {
      return true;
    }
    if (!param.has_generic_suffix || !param.generic_suffix_terminated) {
      diagnostics_.push_back(MakeDiag(
          param.value_optional.line, param.value_optional.column, "O3P114",
          "Optional<T> value optional type requires a terminated payload suffix"));
      return false;
    }
    param.value_optional = BuildObjc3ValueOptionalDescriptor(
        param.generic_suffix_text, param.value_optional.line,
        param.value_optional.column);
    return ValidateValueOptionalDescriptor(
        param.value_optional, param.has_pointer_declarator,
        !param.nullability_suffix_tokens.empty(), param.has_ownership_qualifier);
  }

  bool FinalizeFunctionReturnValueOptionalType(FunctionDecl &fn) {
    if (!fn.return_value_optional.present) {
      return true;
    }
    if (!fn.has_return_generic_suffix ||
        !fn.return_generic_suffix_terminated) {
      diagnostics_.push_back(MakeDiag(
          fn.return_value_optional.line, fn.return_value_optional.column,
          "O3P114",
          "Optional<T> value optional return type requires a terminated payload suffix"));
      return false;
    }
    fn.return_value_optional = BuildObjc3ValueOptionalDescriptor(
        fn.return_generic_suffix_text, fn.return_value_optional.line,
        fn.return_value_optional.column);
    return ValidateValueOptionalDescriptor(
        fn.return_value_optional, fn.has_return_pointer_declarator,
        !fn.return_nullability_suffix_tokens.empty(),
        fn.has_return_ownership_qualifier);
  }

  const Objc3ValueOptionalTypeDescriptor *FirstUnsupportedValueOptionalType(
      const FunctionDecl &fn) const {
    if (fn.return_value_optional.present &&
        !Objc3ValueOptionalRuntimeAbiReady(fn.return_value_optional)) {
      return &fn.return_value_optional;
    }
    for (const FuncParam &param : fn.params) {
      if (param.value_optional.present &&
          !Objc3ValueOptionalRuntimeAbiReady(param.value_optional)) {
        return &param.value_optional;
      }
    }
    return nullptr;
  }

  const Objc3ValueOptionalTypeDescriptor *FirstUnsupportedValueOptionalType(
      const Objc3MethodDecl &method) const {
    if (method.return_value_optional.present &&
        !Objc3ValueOptionalRuntimeAbiReady(method.return_value_optional)) {
      return &method.return_value_optional;
    }
    for (const FuncParam &param : method.params) {
      if (param.value_optional.present &&
          !Objc3ValueOptionalRuntimeAbiReady(param.value_optional)) {
        return &param.value_optional;
      }
    }
    return nullptr;
  }

  bool RejectUnsupportedValueOptionalExecutableFunction(const FunctionDecl &fn) {
    const Objc3ValueOptionalTypeDescriptor *descriptor =
        FirstUnsupportedValueOptionalType(fn);
    if (descriptor == nullptr) {
      return false;
    }
    diagnostics_.push_back(BuildObjc3ReservedValueOptionalTypeDiagnostic(
        ValueOptionalDiagnosticToken(*descriptor)));
    return true;
  }

  bool RejectUnsupportedValueOptionalExecutableMethod(const Objc3MethodDecl &method) {
    const Objc3ValueOptionalTypeDescriptor *descriptor =
        FirstUnsupportedValueOptionalType(method);
    if (descriptor == nullptr) {
      return false;
    }
    diagnostics_.push_back(BuildObjc3ReservedValueOptionalTypeDiagnostic(
        ValueOptionalDiagnosticToken(*descriptor)));
    return true;
  }
