#include "token/objc3_token_kind.h"

Objc3RejectedCanonicalLiteralKind ClassifyObjc3RejectedCanonicalLiteral(
    const std::string &text) {
  if (text == "YES") {
    return Objc3RejectedCanonicalLiteralKind::Yes;
  }
  if (text == "NO") {
    return Objc3RejectedCanonicalLiteralKind::No;
  }
  if (text == "NULL") {
    return Objc3RejectedCanonicalLiteralKind::Null;
  }
  return Objc3RejectedCanonicalLiteralKind::None;
}

const char *Objc3RejectedCanonicalLiteralDiagnosticSpelling(
    Objc3RejectedCanonicalLiteralKind literal) {
  switch (literal) {
  case Objc3RejectedCanonicalLiteralKind::Yes:
    return "YES";
  case Objc3RejectedCanonicalLiteralKind::No:
    return "NO";
  case Objc3RejectedCanonicalLiteralKind::Null:
    return "NULL";
  case Objc3RejectedCanonicalLiteralKind::None:
  default:
    return "";
  }
}

const char *Objc3RejectedCanonicalLiteralReplacementSpelling(
    Objc3RejectedCanonicalLiteralKind literal) {
  switch (literal) {
  case Objc3RejectedCanonicalLiteralKind::Yes:
    return "true";
  case Objc3RejectedCanonicalLiteralKind::No:
    return "false";
  case Objc3RejectedCanonicalLiteralKind::Null:
    return "nil";
  case Objc3RejectedCanonicalLiteralKind::None:
  default:
    return "";
  }
}
