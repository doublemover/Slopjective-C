#pragma once

#include <cstddef>

#include "token/objc3_token_kind_contract.h"

struct Objc3KeywordTokenEntry {
  const char *spelling = "";
  Objc3LexTokenKind kind = Objc3LexTokenKind::Identifier;
};

const Objc3KeywordTokenEntry *Objc3IdentifierKeywordTokenEntries(
    std::size_t &count);
const Objc3KeywordTokenEntry *Objc3AtDirectiveKeywordTokenEntries(
    std::size_t &count);
