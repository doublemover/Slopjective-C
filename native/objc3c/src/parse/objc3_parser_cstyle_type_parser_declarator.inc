namespace {

void ParseObjc3CStylePointerDepth(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    Objc3CStyleFunctionTypeParseResult &result) {
  while (MatchTokenKind(tokens, index, Objc3LexTokenKind::Star)) {
    ++result.pointer_depth;
  }
}

bool ApplyObjc3CStylePointerSemantics(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index,
    unsigned diag_line,
    unsigned diag_column,
    Objc3CStyleFunctionTypeParseResult &result,
    std::vector<std::string> &diagnostics) {
  if (result.pointer_depth == 0u) {
    return true;
  }

  if (result.type == ValueType::Void) {
    result.type = ValueType::ObjCObjectPtr;
    result.object_pointer_spelling = true;
    if (result.object_pointer_name.empty()) {
      result.object_pointer_name = "void";
    }
  } else if (result.type == ValueType::I32 || result.type == ValueType::Bool) {
    EmitUnsupportedCStylePointerTypeDiagnostic(
        result.type, diag_line, diag_column, diagnostics);
    return false;
  }

  if (!result.object_pointer_spelling &&
      !result.id_spelling &&
      !result.class_spelling &&
      !result.sel_spelling &&
      !result.instancetype_spelling) {
    result.object_pointer_spelling = true;
  }
  if (result.object_pointer_name.empty() &&
      AtTokenKind(tokens, index, Objc3LexTokenKind::Identifier)) {
    result.object_pointer_name = PeekToken(tokens, index).text;
  }

  return true;
}

bool ValidateObjc3CStyleVoidUse(
    bool allow_void,
    unsigned diag_line,
    unsigned diag_column,
    Objc3CStyleFunctionTypeParseResult &result,
    std::vector<std::string> &diagnostics) {
  if (allow_void || result.type != ValueType::Void) {
    return true;
  }
  EmitCStyleVoidParameterDiagnostic(diag_line, diag_column, diagnostics);
  return false;
}

}  // namespace
