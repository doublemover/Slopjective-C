#pragma once

#include <cstddef>
#include <string>

#include "token/objc3_token_kind_contract.h"

struct Objc3PunctuationTokenClassification {
  bool recognized = false;
  Objc3LexTokenKind kind = Objc3LexTokenKind::Eof;
  const char *text = "";
  std::size_t width = 0;
  bool stray_block_comment_terminator = false;
};

Objc3PunctuationTokenClassification ClassifyObjc3PunctuationToken(
    const std::string &source,
    std::size_t index);
