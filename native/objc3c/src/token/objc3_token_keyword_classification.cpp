#include "token/objc3_token_kind.h"

#include "token/objc3_token_keyword_data.h"

namespace {

Objc3TokenKindClassification Keyword(Objc3LexTokenKind kind) {
  return Objc3TokenKindClassification{kind, true};
}

}  // namespace

Objc3TokenKindClassification ClassifyObjc3IdentifierToken(
    const std::string &text) {
  std::size_t count = 0;
  const Objc3KeywordTokenEntry *keywords =
      Objc3IdentifierKeywordTokenEntries(count);
  for (std::size_t index = 0; index < count; ++index) {
    if (text == keywords[index].spelling) {
      return Keyword(keywords[index].kind);
    }
  }
  return Objc3TokenKindClassification{};
}

Objc3TokenKindClassification ClassifyObjc3AtDirectiveToken(
    const std::string &directive) {
  std::size_t count = 0;
  const Objc3KeywordTokenEntry *keywords =
      Objc3AtDirectiveKeywordTokenEntries(count);
  for (std::size_t index = 0; index < count; ++index) {
    if (directive == keywords[index].spelling) {
      return Keyword(keywords[index].kind);
    }
  }
  return Objc3TokenKindClassification{};
}
