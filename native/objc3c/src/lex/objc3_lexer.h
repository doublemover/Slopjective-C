#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "token/objc3_token_contract.h"

enum class Objc3LexerCompatibilityMode {
  kCanonical,
  kLegacy,
};

struct Objc3LexerOptions {
  std::uint8_t language_version = 3u;
  Objc3LexerCompatibilityMode compatibility_mode = Objc3LexerCompatibilityMode::kCanonical;
  bool migration_assist = false;
};

struct Objc3LexerMigrationHints {
  std::size_t legacy_yes_count = 0;
  std::size_t legacy_no_count = 0;
  std::size_t legacy_null_count = 0;

  std::size_t LegacyLiteralTotal() const { return legacy_yes_count + legacy_no_count + legacy_null_count; }
};

class Objc3Lexer {
 public:
  explicit Objc3Lexer(const std::string &source, const Objc3LexerOptions &options = Objc3LexerOptions{});

  std::vector<Objc3LexToken> Run(std::vector<std::string> &diagnostics);
  const Objc3LexerMigrationHints &MigrationHints() const;

 private:
  void ConsumeLanguageVersionPragmas(std::vector<std::string> &diagnostics);
  void SkipHorizontalWhitespace();
  bool MatchLiteral(const char *literal);
  void ConsumeToEndOfLine();
  void SkipTrivia(std::vector<std::string> &diagnostics);
  std::string ConsumeIdentifier();
  std::string ConsumeNumber();
  void Advance();
  bool MatchChar(char expected);

  const std::string &source_;
  Objc3LexerOptions options_;
  Objc3LexerMigrationHints migration_hints_;
  std::size_t index_ = 0;
  unsigned line_ = 1;
  unsigned column_ = 1;
};
