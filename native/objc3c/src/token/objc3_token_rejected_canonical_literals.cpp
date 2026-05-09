#include "token/objc3_token_kind.h"

#include "token/objc3_token_rejected_canonical_literal_table.h"

Objc3RejectedCanonicalLiteralKind ClassifyObjc3RejectedCanonicalLiteral(
    const std::string &text) {
  std::size_t count = 0;
  const Objc3RejectedCanonicalLiteralEntry *entries =
      Objc3RejectedCanonicalLiteralEntries(count);
  for (std::size_t index = 0; index < count; ++index) {
    if (text == entries[index].rejected_spelling) {
      return entries[index].kind;
    }
  }
  return Objc3RejectedCanonicalLiteralKind::None;
}

const char *Objc3RejectedCanonicalLiteralDiagnosticSpelling(
    Objc3RejectedCanonicalLiteralKind literal) {
  const Objc3RejectedCanonicalLiteralEntry *entry =
      FindObjc3RejectedCanonicalLiteralEntry(literal);
  if (entry != nullptr) {
    return entry->rejected_spelling;
  }
  return "";
}

const char *Objc3RejectedCanonicalLiteralReplacementSpelling(
    Objc3RejectedCanonicalLiteralKind literal) {
  const Objc3RejectedCanonicalLiteralEntry *entry =
      FindObjc3RejectedCanonicalLiteralEntry(literal);
  if (entry != nullptr) {
    return entry->canonical_spelling;
  }
  return "";
}
