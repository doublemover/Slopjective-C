#pragma once

#include <string>

#include "token/objc3_token_kind_contract.h"

struct Objc3TokenKindClassification {
  Objc3LexTokenKind kind = Objc3LexTokenKind::Identifier;
  bool recognized = false;
};

enum class Objc3RejectedCanonicalLiteralKind {
  None,
  Yes,
  No,
  Null,
};

Objc3TokenKindClassification ClassifyObjc3IdentifierToken(const std::string &text);
Objc3TokenKindClassification ClassifyObjc3AtDirectiveToken(const std::string &directive);
Objc3RejectedCanonicalLiteralKind ClassifyObjc3RejectedCanonicalLiteral(const std::string &text);
const char *Objc3RejectedCanonicalLiteralDiagnosticSpelling(Objc3RejectedCanonicalLiteralKind literal);
const char *Objc3RejectedCanonicalLiteralReplacementSpelling(Objc3RejectedCanonicalLiteralKind literal);
const char *Objc3LexTokenKindName(Objc3LexTokenKind kind);
bool Objc3LexTokenKindIsKeyword(Objc3LexTokenKind kind);
