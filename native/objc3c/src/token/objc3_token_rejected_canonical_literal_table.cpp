#include "token/objc3_token_rejected_canonical_literal_table.h"

namespace {

constexpr Objc3RejectedCanonicalLiteralEntry kRejectedCanonicalLiterals[] = {
    {"YES", "true", Objc3RejectedCanonicalLiteralKind::Yes},
    {"NO", "false", Objc3RejectedCanonicalLiteralKind::No},
    {"NULL", "nil", Objc3RejectedCanonicalLiteralKind::Null},
};

}  // namespace

const Objc3RejectedCanonicalLiteralEntry *
Objc3RejectedCanonicalLiteralEntries(std::size_t &count) {
  count = sizeof(kRejectedCanonicalLiterals) /
          sizeof(kRejectedCanonicalLiterals[0]);
  return kRejectedCanonicalLiterals;
}

const Objc3RejectedCanonicalLiteralEntry *
FindObjc3RejectedCanonicalLiteralEntry(Objc3RejectedCanonicalLiteralKind kind) {
  std::size_t count = 0;
  const Objc3RejectedCanonicalLiteralEntry *entries =
      Objc3RejectedCanonicalLiteralEntries(count);
  for (std::size_t index = 0; index < count; ++index) {
    if (entries[index].kind == kind) {
      return &entries[index];
    }
  }
  return nullptr;
}
