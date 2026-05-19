bool ClassifyObjc3CStyleBuiltinTypeToken(
    const Objc3LexToken &token,
    Objc3CStyleTypeTokenClassification &classification) {
  classification = Objc3CStyleTypeTokenClassification{};
  switch (token.kind) {
  case Objc3LexTokenKind::KwI32:
    classification.recognized = true;
    classification.type = ValueType::I32;
    return true;
  case Objc3LexTokenKind::KwBool:
  case Objc3LexTokenKind::KwBOOL:
    classification.recognized = true;
    classification.type = ValueType::Bool;
    return true;
  case Objc3LexTokenKind::KwVoid:
    classification.recognized = true;
    classification.type = ValueType::Void;
    return true;
  case Objc3LexTokenKind::KwId:
    classification.recognized = true;
    classification.type = ValueType::ObjCId;
    classification.id_spelling = true;
    return true;
  case Objc3LexTokenKind::KwClass:
    classification.recognized = true;
    classification.type = ValueType::ObjCClass;
    classification.class_spelling = true;
    return true;
  case Objc3LexTokenKind::KwSEL:
    classification.recognized = true;
    classification.type = ValueType::ObjCSel;
    classification.sel_spelling = true;
    return true;
  case Objc3LexTokenKind::KwProtocol:
    classification.recognized = true;
    classification.type = ValueType::ObjCProtocol;
    return true;
  case Objc3LexTokenKind::KwInstancetype:
    classification.recognized = true;
    classification.type = ValueType::ObjCInstancetype;
    classification.instancetype_spelling = true;
    return true;
  case Objc3LexTokenKind::KwNSInteger:
  case Objc3LexTokenKind::KwNSUInteger:
    classification.recognized = true;
    classification.type = ValueType::I32;
    return true;
  case Objc3LexTokenKind::Identifier:
    return ClassifyObjc3CStyleBuiltinTypeSpelling(token.text, classification);
  default:
    return false;
  }
}
