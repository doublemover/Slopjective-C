#pragma once

#include <string>
#include <vector>

#include "lex/objc3_lexer_contract.h"
#include "token/objc3_token_contract.h"

class Objc3Lexer {
 public:
  static constexpr std::uint8_t kDefaultLanguageVersion =
      objc3c::config::kCanonicalLanguageVersion;

  explicit Objc3Lexer(const std::string &source, const Objc3LexerOptions &options = Objc3LexerOptions{});

  std::vector<Objc3LexToken> Run(std::vector<std::string> &diagnostics);
  const Objc3LexerCanonicalLiteralRejectionCounts
      &CanonicalLiteralRejectionCounts() const;
  const Objc3LexerLanguageVersionPragmaContract &LanguageVersionPragmaContract() const;
  const Objc3LexerBootstrapRegistrationSourceContract &BootstrapRegistrationSourceContract() const;

 private:
  enum class LanguageVersionPragmaPlacement {
    kPrelude,
    kNonLeading,
  };
  enum class NamedIdentifierPragmaPlacement {
    kPrelude,
    kNonLeading,
  };

  void ConsumePreludePragmas(std::vector<std::string> &diagnostics);
  bool ConsumeLanguageVersionPragmaDirective(std::vector<std::string> &diagnostics,
                                             LanguageVersionPragmaPlacement placement,
                                             bool strict_pragma_matching);
  bool ConsumeBootstrapRegistrationPragmaDirective(
      std::vector<std::string> &diagnostics,
      NamedIdentifierPragmaPlacement placement);
  bool ConsumeNamedIdentifierPragmaDirective(
      std::vector<std::string> &diagnostics,
      NamedIdentifierPragmaPlacement placement,
      const char *directive_name,
      Objc3LexerNamedIdentifierPragmaContract &contract,
      const char *malformed_message,
      const char *duplicate_message,
      const char *non_leading_message);
  bool MatchLiteralAt(std::size_t cursor, const char *literal) const;
  void RecordLanguageVersionPragmaObservation(unsigned line, unsigned column, LanguageVersionPragmaPlacement placement);
  void RecordNamedIdentifierPragmaObservation(
      Objc3LexerNamedIdentifierPragmaContract &contract,
      unsigned line,
      unsigned column,
      NamedIdentifierPragmaPlacement placement,
      const std::string &identifier);
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
  Objc3LexerCanonicalLiteralRejectionCounts
      canonical_literal_rejection_counts_;
  Objc3LexerLanguageVersionPragmaContract language_version_pragma_contract_;
  Objc3LexerBootstrapRegistrationSourceContract
      bootstrap_registration_source_contract_;
  std::size_t index_ = 0;
  unsigned line_ = 1;
  unsigned column_ = 1;
};
