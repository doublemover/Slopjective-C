#pragma once

#include <cstddef>

#include "token/objc3_token_kind.h"

struct Objc3RejectedCanonicalLiteralEntry {
  const char *rejected_spelling = "";
  const char *canonical_spelling = "";
  Objc3RejectedCanonicalLiteralKind kind =
      Objc3RejectedCanonicalLiteralKind::None;
};

const Objc3RejectedCanonicalLiteralEntry *
Objc3RejectedCanonicalLiteralEntries(std::size_t &count);

const Objc3RejectedCanonicalLiteralEntry *
FindObjc3RejectedCanonicalLiteralEntry(Objc3RejectedCanonicalLiteralKind kind);
