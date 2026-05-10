namespace {

bool ParseObjc3CStyleTypeSpelling(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    Objc3CStyleFunctionTypeParseResult &result) {
  Objc3CStyleTypeTokenClassification classification;
  if (ClassifyObjc3CStyleBuiltinTypeToken(PeekToken(tokens, index), classification)) {
    result.type = classification.type;
    result.id_spelling = classification.id_spelling;
    result.class_spelling = classification.class_spelling;
    result.sel_spelling = classification.sel_spelling;
    result.instancetype_spelling = classification.instancetype_spelling;
    AdvanceToken(tokens, index);
    return true;
  }

  if (!AtTokenKind(tokens, index, Objc3LexTokenKind::Identifier)) {
    return false;
  }

  const Objc3LexToken object_name = AdvanceToken(tokens, index);
  result.type = ValueType::ObjCObjectPtr;
  result.object_pointer_spelling = true;
  result.object_pointer_name = object_name.text;
  return true;
}

}  // namespace
