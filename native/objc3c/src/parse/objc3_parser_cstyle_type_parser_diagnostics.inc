namespace {

bool RejectRemovedOptionalTemplateAliasLead(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index,
    std::vector<std::string> &diagnostics) {
  if (!IsObjc3RemovedOptionalTemplateAliasLead(tokens, index)) {
    return false;
  }
  diagnostics.push_back(
      BuildObjc3RemovedOptionalTemplateAliasDiagnostic(PeekToken(tokens, index)));
  return true;
}

void EmitExpectedCStyleTypeDiagnostic(
    unsigned diag_line,
    unsigned diag_column,
    const char *diagnostic_context,
    std::vector<std::string> &diagnostics) {
  diagnostics.push_back(MakeDiag(
      diag_line,
      diag_column,
      "O3P114",
      std::string("expected ") + diagnostic_context + " type"));
}

void EmitUnsupportedCStylePointerTypeDiagnostic(
    ValueType type,
    unsigned diag_line,
    unsigned diag_column,
    std::vector<std::string> &diagnostics) {
  const char *pointer_type = type == ValueType::I32 ? "i32*" : "bool*";
  diagnostics.push_back(MakeDiag(
      diag_line,
      diag_column,
      "O3P114",
      std::string("unsupported pointer type in C-style declaration: unsupported pointer type '") +
          pointer_type + "' in C-style declaration"));
}

void EmitCStyleVoidParameterDiagnostic(
    unsigned diag_line,
    unsigned diag_column,
    std::vector<std::string> &diagnostics) {
  diagnostics.push_back(MakeDiag(
      diag_line,
      diag_column,
      "O3P108",
      "void is only allowed as '(void)' in C-style parameter lists"));
}

bool FinalizeObjc3CStyleValueOptionalType(
    Objc3CStyleFunctionTypeParseResult &result,
    std::vector<std::string> &diagnostics) {
  if (!result.value_optional.present) {
    return true;
  }
  if (!result.has_generic_suffix || !result.generic_suffix_terminated) {
    diagnostics.push_back(MakeDiag(
        result.value_optional.line, result.value_optional.column, "O3P114",
        "Optional<T> value optional type requires a terminated payload suffix"));
    return false;
  }
  result.value_optional = BuildObjc3ValueOptionalDescriptor(
      result.generic_suffix_text, result.value_optional.line,
      result.value_optional.column);
  if (result.value_optional.payload_type_spelling.empty()) {
    diagnostics.push_back(MakeDiag(
        result.value_optional.line, result.value_optional.column, "O3P114",
        "Optional<T> value optional type requires exactly one payload type"));
    return false;
  }
  if (result.value_optional.payload_lowercase_optional_alias) {
    diagnostics.push_back(MakeDiag(
        result.value_optional.line, result.value_optional.column, "O3C004",
        "optional<T> aliases are rejected inside Optional<T> payloads; use canonical Optional<T> spelling"));
    return false;
  }
  return true;
}

}  // namespace
