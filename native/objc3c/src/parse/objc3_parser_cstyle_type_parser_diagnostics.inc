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

}  // namespace
