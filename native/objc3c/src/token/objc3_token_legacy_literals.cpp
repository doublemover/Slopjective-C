#include "token/objc3_token_kind.h"

Objc3LegacyLiteralAliasKind ClassifyObjc3LegacyLiteralAlias(
    const std::string &text) {
  if (text == "YES") {
    return Objc3LegacyLiteralAliasKind::Yes;
  }
  if (text == "NO") {
    return Objc3LegacyLiteralAliasKind::No;
  }
  if (text == "NULL") {
    return Objc3LegacyLiteralAliasKind::Null;
  }
  return Objc3LegacyLiteralAliasKind::None;
}

const char *Objc3LegacyLiteralAliasDiagnosticSpelling(
    Objc3LegacyLiteralAliasKind alias) {
  switch (alias) {
  case Objc3LegacyLiteralAliasKind::Yes:
    return "YES";
  case Objc3LegacyLiteralAliasKind::No:
    return "NO";
  case Objc3LegacyLiteralAliasKind::Null:
    return "NULL";
  case Objc3LegacyLiteralAliasKind::None:
  default:
    return "";
  }
}

const char *Objc3LegacyLiteralAliasCanonicalSpelling(
    Objc3LegacyLiteralAliasKind alias) {
  switch (alias) {
  case Objc3LegacyLiteralAliasKind::Yes:
    return "true";
  case Objc3LegacyLiteralAliasKind::No:
    return "false";
  case Objc3LegacyLiteralAliasKind::Null:
    return "nil";
  case Objc3LegacyLiteralAliasKind::None:
  default:
    return "";
  }
}
