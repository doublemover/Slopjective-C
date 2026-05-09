#pragma once

#include <string>

#include "token/objc3_token_kind_contract.h"

struct Objc3TokenKindClassification {
  Objc3LexTokenKind kind = Objc3LexTokenKind::Identifier;
  bool recognized = false;
};

enum class Objc3LegacyLiteralAliasKind {
  None,
  Yes,
  No,
  Null,
};

Objc3TokenKindClassification ClassifyObjc3IdentifierToken(const std::string &text);
Objc3TokenKindClassification ClassifyObjc3AtDirectiveToken(const std::string &directive);
Objc3LegacyLiteralAliasKind ClassifyObjc3LegacyLiteralAlias(const std::string &text);
const char *Objc3LegacyLiteralAliasDiagnosticSpelling(Objc3LegacyLiteralAliasKind alias);
const char *Objc3LegacyLiteralAliasCanonicalSpelling(Objc3LegacyLiteralAliasKind alias);
const char *Objc3LexTokenKindName(Objc3LexTokenKind kind);
bool Objc3LexTokenKindIsKeyword(Objc3LexTokenKind kind);
