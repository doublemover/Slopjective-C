#include "lex/objc3_lexer.h"

#include <cctype>

#include "diag/objc3_diag_utils.h"
#include "lex/objc3_lexer_char_class.h"

void Objc3Lexer::ConsumePreludePragmas(std::vector<std::string> &diagnostics) {
  // frontend-closure anchor: bootstrap registration pragmas must be
  // consumed in the file-scope prelude so the emitted registration-descriptor
  // artifact keeps one deterministic identity boundary.
  while (true) {
    SkipTrivia(diagnostics);
    if (ConsumeLanguageVersionPragmaDirective(
            diagnostics, LanguageVersionPragmaPlacement::kPrelude, false) ||
        ConsumeBootstrapRegistrationPragmaDirective(
            diagnostics, NamedIdentifierPragmaPlacement::kPrelude)) {
      continue;
    }
    return;
  }
}

bool Objc3Lexer::ConsumeBootstrapRegistrationPragmaDirective(
    std::vector<std::string> &diagnostics,
    NamedIdentifierPragmaPlacement placement) {
  static constexpr char kMalformedRegistrationDescriptorMessage[] =
      "malformed '#pragma objc_registration_descriptor' directive; expected '#pragma objc_registration_descriptor(Name)'";
  static constexpr char kDuplicateRegistrationDescriptorMessage[] =
      "duplicate '#pragma objc_registration_descriptor' directive; only one file-scope prelude directive is allowed";
  static constexpr char kNonLeadingRegistrationDescriptorMessage[] =
      "registration-descriptor pragma must stay in the file-scope prelude before declarations or tokens";
  if (ConsumeNamedIdentifierPragmaDirective(
          diagnostics, placement, kObjc3BootstrapRegistrationDescriptorPragmaName,
          bootstrap_registration_source_contract_.registration_descriptor,
          kMalformedRegistrationDescriptorMessage,
          kDuplicateRegistrationDescriptorMessage,
          kNonLeadingRegistrationDescriptorMessage)) {
    return true;
  }

  static constexpr char kMalformedImageRootMessage[] =
      "malformed '#pragma objc_image_root' directive; expected '#pragma objc_image_root(Name)'";
  static constexpr char kDuplicateImageRootMessage[] =
      "duplicate '#pragma objc_image_root' directive; only one file-scope prelude directive is allowed";
  static constexpr char kNonLeadingImageRootMessage[] =
      "image-root pragma must stay in the file-scope prelude before declarations or tokens";
  return ConsumeNamedIdentifierPragmaDirective(
      diagnostics, placement, kObjc3BootstrapImageRootPragmaName,
      bootstrap_registration_source_contract_.image_root,
      kMalformedImageRootMessage, kDuplicateImageRootMessage,
      kNonLeadingImageRootMessage);
}

bool Objc3Lexer::ConsumeNamedIdentifierPragmaDirective(
    std::vector<std::string> &diagnostics,
    NamedIdentifierPragmaPlacement placement,
    const char *directive_name,
    Objc3LexerNamedIdentifierPragmaContract &contract,
    const char *malformed_message,
    const char *duplicate_message,
    const char *non_leading_message) {
  if (index_ >= source_.size() || source_[index_] != '#') {
    return false;
  }

  std::size_t cursor = index_ + 1;
  while (cursor < source_.size() && IsObjc3HorizontalWhitespace(source_[cursor])) {
    ++cursor;
  }
  if (!MatchLiteralAt(cursor, "pragma")) {
    return false;
  }
  cursor += 6;
  while (cursor < source_.size() && IsObjc3HorizontalWhitespace(source_[cursor])) {
    ++cursor;
  }
  if (!MatchLiteralAt(cursor, directive_name)) {
    return false;
  }

  const unsigned directive_line = line_;
  const unsigned directive_column = column_;
  Advance();
  SkipHorizontalWhitespace();
  MatchLiteral("pragma");
  SkipHorizontalWhitespace();
  if (!MatchLiteral(directive_name)) {
    diagnostics.push_back(
        MakeDiag(directive_line, directive_column, "O3L009", malformed_message));
    ConsumeToEndOfLine();
    return true;
  }

  SkipHorizontalWhitespace();
  if (!MatchChar('(')) {
    diagnostics.push_back(
        MakeDiag(directive_line, directive_column, "O3L009", malformed_message));
    ConsumeToEndOfLine();
    return true;
  }

  SkipHorizontalWhitespace();
  if (index_ >= source_.size() || !IsObjc3IdentifierStart(source_[index_])) {
    diagnostics.push_back(
        MakeDiag(directive_line, directive_column, "O3L009", malformed_message));
    ConsumeToEndOfLine();
    return true;
  }
  const std::string identifier = ConsumeIdentifier();

  SkipHorizontalWhitespace();
  if (!MatchChar(')')) {
    diagnostics.push_back(
        MakeDiag(directive_line, directive_column, "O3L009", malformed_message));
    ConsumeToEndOfLine();
    return true;
  }

  SkipHorizontalWhitespace();
  if (index_ < source_.size() && source_[index_] != '\n') {
    diagnostics.push_back(
        MakeDiag(directive_line, directive_column, "O3L009", malformed_message));
    ConsumeToEndOfLine();
    return true;
  }

  RecordNamedIdentifierPragmaObservation(contract, directive_line, directive_column,
                                         placement, identifier);
  if (placement == NamedIdentifierPragmaPlacement::kNonLeading) {
    diagnostics.push_back(
        MakeDiag(directive_line, directive_column, "O3L011", non_leading_message));
  }
  if (contract.directive_count > 1) {
    diagnostics.push_back(
        MakeDiag(directive_line, directive_column, "O3L010", duplicate_message));
  }

  if (index_ < source_.size() && source_[index_] == '\n') {
    Advance();
  }
  return true;
}

void Objc3Lexer::RecordNamedIdentifierPragmaObservation(
    Objc3LexerNamedIdentifierPragmaContract &contract,
    unsigned line,
    unsigned column,
    NamedIdentifierPragmaPlacement placement,
    const std::string &identifier) {
  if (!contract.seen) {
    contract.seen = true;
    contract.first_line = line;
    contract.first_column = column;
    contract.identifier = identifier;
  }
  ++contract.directive_count;
  contract.last_line = line;
  contract.last_column = column;
  if (contract.directive_count > 1) {
    contract.duplicate = true;
  }
  if (placement == NamedIdentifierPragmaPlacement::kNonLeading) {
    contract.non_leading = true;
  }
}

bool Objc3Lexer::ConsumeLanguageVersionPragmaDirective(std::vector<std::string> &diagnostics,
                                                       LanguageVersionPragmaPlacement placement,
                                                       bool strict_pragma_matching) {
  static constexpr char kMalformedPragmaMessage[] =
      "malformed '#pragma objc_language_version' directive; expected '#pragma objc_language_version(3)'";
  static constexpr char kDuplicatePragmaMessage[] =
      "duplicate '#pragma objc_language_version' directive; only one file-scope prelude pragma is allowed";
  static constexpr char kNonLeadingPragmaMessage[] =
      "language-version pragma must stay in the file-scope prelude before declarations or tokens";

  if (index_ >= source_.size() || source_[index_] != '#') {
    return false;
  }

  std::size_t cursor = index_ + 1;
  while (cursor < source_.size() && IsObjc3HorizontalWhitespace(source_[cursor])) {
    ++cursor;
  }
  if (!MatchLiteralAt(cursor, "pragma")) {
    return false;
  }
  cursor += 6;
  while (cursor < source_.size() && IsObjc3HorizontalWhitespace(source_[cursor])) {
    ++cursor;
  }
  if (!strict_pragma_matching && !MatchLiteralAt(cursor, "objc_language_version")) {
    return false;
  }

  const unsigned directive_line = line_;
  const unsigned directive_column = column_;
  Advance();
  SkipHorizontalWhitespace();
  MatchLiteral("pragma");
  SkipHorizontalWhitespace();
  if (!MatchLiteral("objc_language_version")) {
    diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L005", kMalformedPragmaMessage));
    ConsumeToEndOfLine();
    return true;
  }

  SkipHorizontalWhitespace();
  if (!MatchChar('(')) {
    diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L005", kMalformedPragmaMessage));
    ConsumeToEndOfLine();
    return true;
  }

  SkipHorizontalWhitespace();
  const unsigned version_line = line_;
  const unsigned version_column = column_;
  if (index_ >= source_.size() || std::isdigit(static_cast<unsigned char>(source_[index_])) == 0) {
    diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L005", kMalformedPragmaMessage));
    ConsumeToEndOfLine();
    return true;
  }

  std::string version;
  while (index_ < source_.size() && std::isdigit(static_cast<unsigned char>(source_[index_])) != 0) {
    version.push_back(source_[index_]);
    Advance();
  }

  SkipHorizontalWhitespace();
  if (!MatchChar(')')) {
    diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L005", kMalformedPragmaMessage));
    ConsumeToEndOfLine();
    return true;
  }

  SkipHorizontalWhitespace();
  if (index_ < source_.size() && source_[index_] != '\n') {
    diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L005", kMalformedPragmaMessage));
    ConsumeToEndOfLine();
    return true;
  }

  if (version != std::to_string(options_.language_version)) {
    diagnostics.push_back(MakeDiag(version_line, version_column, "O3L006",
                                   "unsupported objc language version '" + version + "'; expected " +
                                       std::to_string(options_.language_version)));
  }

  RecordLanguageVersionPragmaObservation(directive_line, directive_column, placement);
  if (placement == LanguageVersionPragmaPlacement::kNonLeading) {
    diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L008", kNonLeadingPragmaMessage));
  }
  if (language_version_pragma_contract_.directive_count > 1) {
    diagnostics.push_back(MakeDiag(directive_line, directive_column, "O3L007", kDuplicatePragmaMessage));
  }

  if (index_ < source_.size() && source_[index_] == '\n') {
    Advance();
  }
  return true;
}

bool Objc3Lexer::MatchLiteralAt(std::size_t cursor, const char *literal) const {
  for (const char *it = literal; *it != '\0'; ++it) {
    if (cursor >= source_.size() || source_[cursor] != *it) {
      return false;
    }
    ++cursor;
  }
  return true;
}

void Objc3Lexer::RecordLanguageVersionPragmaObservation(unsigned line, unsigned column,
                                                        LanguageVersionPragmaPlacement placement) {
  if (!language_version_pragma_contract_.seen) {
    language_version_pragma_contract_.seen = true;
    language_version_pragma_contract_.first_line = line;
    language_version_pragma_contract_.first_column = column;
  }
  ++language_version_pragma_contract_.directive_count;
  language_version_pragma_contract_.last_line = line;
  language_version_pragma_contract_.last_column = column;
  if (language_version_pragma_contract_.directive_count > 1) {
    language_version_pragma_contract_.duplicate = true;
  }
  if (placement == LanguageVersionPragmaPlacement::kNonLeading) {
    language_version_pragma_contract_.non_leading = true;
  }
}

void Objc3Lexer::SkipHorizontalWhitespace() {
  while (index_ < source_.size() && IsObjc3HorizontalWhitespace(source_[index_])) {
    Advance();
  }
}

bool Objc3Lexer::MatchLiteral(const char *literal) {
  std::size_t cursor = index_;
  for (const char *it = literal; *it != '\0'; ++it) {
    if (cursor >= source_.size() || source_[cursor] != *it) {
      return false;
    }
    ++cursor;
  }

  while (index_ < cursor) {
    Advance();
  }
  return true;
}

void Objc3Lexer::ConsumeToEndOfLine() {
  while (index_ < source_.size() && source_[index_] != '\n') {
    Advance();
  }
  if (index_ < source_.size() && source_[index_] == '\n') {
    Advance();
  }
}
