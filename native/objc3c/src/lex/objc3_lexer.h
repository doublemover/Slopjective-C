#pragma once

#include <string>
#include <vector>

#include "token/objc3_token_contract.h"

class Objc3Lexer {
 public:
  explicit Objc3Lexer(const std::string &source);

  std::vector<Objc3LexToken> Run(std::vector<std::string> &diagnostics);

 private:
  void SkipTrivia(std::vector<std::string> &diagnostics);
  std::string ConsumeIdentifier();
  std::string ConsumeNumber();
  void Advance();
  bool MatchChar(char expected);

  const std::string &source_;
  std::size_t index_ = 0;
  unsigned line_ = 1;
  unsigned column_ = 1;
};
