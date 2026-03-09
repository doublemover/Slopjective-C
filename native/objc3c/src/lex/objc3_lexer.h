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

struct Objc3LexerLanguageVersionPragmaContract {
  bool seen = false;
  std::size_t directive_count = 0;
  bool duplicate = false;
  bool non_leading = false;
  unsigned first_line = 0;
  unsigned first_column = 0;
  unsigned last_line = 0;
  unsigned last_column = 0;
};

struct Objc3LexerNamedIdentifierPragmaContract {
  bool seen = false;
  std::size_t directive_count = 0;
  bool duplicate = false;
  bool non_leading = false;
  unsigned first_line = 0;
  unsigned first_column = 0;
  unsigned last_line = 0;
  unsigned last_column = 0;
  std::string identifier;
};

struct Objc3LexerBootstrapRegistrationSourceContract {
  Objc3LexerNamedIdentifierPragmaContract registration_descriptor;
  Objc3LexerNamedIdentifierPragmaContract image_root;
};

class Objc3Lexer {
 public:
  explicit Objc3Lexer(const std::string &source, const Objc3LexerOptions &options = Objc3LexerOptions{});

  std::vector<Objc3LexToken> Run(std::vector<std::string> &diagnostics);
  const Objc3LexerMigrationHints &MigrationHints() const;
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
  Objc3LexerMigrationHints migration_hints_;
  Objc3LexerLanguageVersionPragmaContract language_version_pragma_contract_;
  Objc3LexerBootstrapRegistrationSourceContract
      bootstrap_registration_source_contract_;
  std::size_t index_ = 0;
  unsigned line_ = 1;
  unsigned column_ = 1;
};
