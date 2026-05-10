bool IsObjc3UpdateOperatorToken(Objc3LexTokenKind kind) {
  return kind == Objc3LexTokenKind::PlusPlus ||
         kind == Objc3LexTokenKind::MinusMinus;
}

const char *Objc3UpdateOperatorSpelling(Objc3LexTokenKind kind) {
  switch (kind) {
  case Objc3LexTokenKind::PlusPlus:
    return "++";
  case Objc3LexTokenKind::MinusMinus:
    return "--";
  default:
    return "";
  }
}
