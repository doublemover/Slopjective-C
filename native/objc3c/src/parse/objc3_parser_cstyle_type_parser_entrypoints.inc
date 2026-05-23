Objc3CStyleFunctionTypeParseResult ParseObjc3CStyleFunctionType(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    bool allow_void,
    unsigned diag_line,
    unsigned diag_column,
    const char *diagnostic_context,
    std::vector<std::string> &diagnostics) {
  Objc3CStyleFunctionTypeParseResult result;

  if (RejectRemovedOptionalTemplateAliasLead(tokens, index, diagnostics)) {
    return result;
  }

  if (!ParseObjc3CStyleTypeSpelling(tokens, index, result, diagnostics)) {
    EmitExpectedCStyleTypeDiagnostic(
        diag_line, diag_column, diagnostic_context, diagnostics);
    return result;
  }

  ParseObjc3CStylePointerDepth(tokens, index, result);

  if (!ApplyObjc3CStylePointerSemantics(
          tokens, index, diag_line, diag_column, result, diagnostics)) {
    return result;
  }

  if (!ValidateObjc3CStyleVoidUse(
          allow_void, diag_line, diag_column, result, diagnostics)) {
    return result;
  }

  result.ok = true;
  return result;
}
