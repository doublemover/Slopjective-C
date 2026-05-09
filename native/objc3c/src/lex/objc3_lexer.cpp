#include "lex/objc3_lexer.h"

#include <cctype>

#include "diag/objc3_diag_utils.h"
#include "lex/objc3_lexer_char_class.h"
#include "support/objc3_ascii_predicates.h"
#include "token/objc3_token_kind.h"
#include "token/objc3_token_punctuation.h"

namespace {

using objc3c::support::IsBinaryDigit;
using objc3c::support::IsDigitSeparator;
using objc3c::support::IsHexDigit;
using objc3c::support::IsOctalDigit;
using Token = Objc3LexToken;
using TokenKind = Objc3LexTokenKind;

}  // namespace

Objc3Lexer::Objc3Lexer(const std::string &source, const Objc3LexerOptions &options)
    : source_(source), options_(options) {}

const Objc3LexerCanonicalLiteralRejectionCounts
    &Objc3Lexer::CanonicalLiteralRejectionCounts() const {
  return canonical_literal_rejection_counts_;
}

const Objc3LexerLanguageVersionPragmaContract &Objc3Lexer::LanguageVersionPragmaContract() const {
  return language_version_pragma_contract_;
}

const Objc3LexerBootstrapRegistrationSourceContract &
Objc3Lexer::BootstrapRegistrationSourceContract() const {
  return bootstrap_registration_source_contract_;
}

std::vector<Objc3LexToken> Objc3Lexer::Run(std::vector<std::string> &diagnostics) {
  // mode-truth source anchor: the lexer owns the effective language
  // version and canonical rejection prelude inputs consumed by the
  // runnable feature-claim inventory emitted by the frontend manifest.
  // truth-surface wiring anchor: strictness / strict concurrency do
  // not have accepted hidden lexer/prelude selection forms yet, so the emitted
  // truth packet must keep those surfaces fail-closed and unadvertised.
  // source-closure note: task/executor/cancellation work does not
  // add dedicated lexer keywords yet; do not add dedicated lexer keywords yet.
  // The admitted source surface remains the existing async/await tokens plus
  // parser-owned identifier profiles and canonical `objc_executor(...)`
  // attribute payloads, including the later task-group/task-creation
  // callable source completion.
  // source-closure note: actor/isolation/sendable work also remains
  // identifier/attribute-profile driven here. Do not add dedicated
  // actor/sendable/nonisolated lexer keywords yet; the truthful source
  // boundary is still parser-owned profiling on top of the existing token
  // stream.
  // source-surface note: actor-member completion keeps `actor`
  // contextual rather than promoting it to a reserved token; `actor class`
  // and `objc_nonisolated` are parser-owned surfaces built on this same token
  // stream.
  // source-surface note: `borrowed`, `weak`, `unowned`, and `move`
  // remain contextual rather than promoting them to reserved tokens; Part 8
  // resource attributes, borrowed-pointer qualifiers, and explicit block
  // capture lists are parser-owned surfaces built on this same token stream.
  // source-surface note: `@cleanup` and `@resource` are admitted as
  // local storage-annotation directives without claiming cleanup lowering or
  // runtime resource behavior yet.
  // source-surface note: retainable C-family callable annotations
  // remain parser-owned `__attribute__` spellings rather than dedicated lexer
  // keywords; the lexer only needs to preserve their identifier tokens.
  // source-closure note: base interop closure keeps
  // `objc_foreign`/`objc_import_module` parser-owned and identifier-based
  // here; do not promote them into dedicated lexer keywords.
  // source-completion note: C++/Swift-facing interop annotations are
  // also parser-owned callable attributes. Keep `objc_swift_name`,
  // `objc_swift_private`, `objc_cxx_name`, and `objc_header_name` as
  // identifier tokens here.
  // source note: Part 12 stays token-stable. The advanced
  // diagnostics/fix-it inventory and the canonical rejection
  // completion packet are both derived from the existing Part 6 through Part
  // 11 parser-owned surfaces plus the rejection counters emitted here for
  // rejected `YES` / `NO` / `NULL` spellings.
  // source-surface note: direct/final/sealed/dynamic dispatch-intent
  // controls also remain parser-owned `__attribute__` spellings rather than
  // new reserved lexer keywords; the truthful Part 9 source boundary is
  // frontend-owned attribute profiling on top of this same token stream.
  // source-completion note: prefixed container attribute lists ahead
  // of `@interface` and `actor class` still reuse the same parser-owned
  // identifier token stream rather than widening the lexer boundary.
  // source-closure note: derive, macro, and property-behavior work
  // likewise stays identifier/attribute-profile driven here. Do not add
  // dedicated derive/macro/property-behavior lexer keywords yet; the truthful
  // Part 10 source boundary is still parser-owned profiling on top of this
  // same token stream.
  // source-completion note: macro package/provenance admission also
  // remains parser-owned callable attribute profiling on top of this same token
  // stream; do not widen the lexer with dedicated package/provenance keywords.
  // source-completion note: property-behavior synthesized
  // declaration visibility likewise stays a parser/frontend packet concern on
  // top of the existing `behavior=...` property attribute spelling.
  ConsumePreludePragmas(diagnostics);
  std::vector<Token> tokens;
  while (true) {
    SkipTrivia(diagnostics);
    if (index_ >= source_.size()) {
      tokens.push_back(Token{TokenKind::Eof, "", line_, column_});
      break;
    }

    const unsigned token_line = line_;
    const unsigned token_column = column_;
    const char c = source_[index_];
    if (c == '#') {
      if (ConsumeLanguageVersionPragmaDirective(
              diagnostics, LanguageVersionPragmaPlacement::kNonLeading, false) ||
          ConsumeBootstrapRegistrationPragmaDirective(
              diagnostics, NamedIdentifierPragmaPlacement::kNonLeading)) {
        continue;
      }
    }
    if (c == '@') {
      Advance();
      if (index_ < source_.size() && IsObjc3IdentifierStart(source_[index_])) {
        const std::string directive = ConsumeIdentifier();
        const Objc3TokenKindClassification at_directive =
            ClassifyObjc3AtDirectiveToken(directive);
        if (at_directive.recognized) {
          tokens.push_back(Token{at_directive.kind, "@" + directive, token_line, token_column});
          continue;
        }
        diagnostics.push_back(MakeDiag(token_line, token_column, "O3L001",
                                       "unsupported '@' directive '@" + directive + "'"));
        continue;
      }
      diagnostics.push_back(
          MakeDiag(token_line, token_column, "O3L001", "unexpected character '@'"));
      continue;
    }
    if (c == '"') {
      Advance();
      std::string value;
      bool terminated = false;
      while (index_ < source_.size()) {
        const char current = source_[index_];
        if (current == '"') {
          Advance();
          terminated = true;
          break;
        }
        if (current == '\\' && index_ + 1 < source_.size()) {
          Advance();
          value.push_back(source_[index_]);
          Advance();
          continue;
        }
        if (current == '\n') {
          break;
        }
        value.push_back(current);
        Advance();
      }
      if (!terminated) {
        diagnostics.push_back(
            MakeDiag(token_line, token_column, "O3L005",
                     "unterminated string literal"));
        continue;
      }
      tokens.push_back(Token{TokenKind::String, EscapeObjc3StringTokenText(value),
                             token_line, token_column});
      continue;
    }
    if (IsObjc3IdentifierStart(c)) {
      std::string ident = ConsumeIdentifier();
      TokenKind kind = TokenKind::Identifier;
      const Objc3TokenKindClassification keyword = ClassifyObjc3IdentifierToken(ident);
      if (keyword.recognized) {
        kind = keyword.kind;
      } else {
        const Objc3RejectedCanonicalLiteralKind rejected_literal =
            ClassifyObjc3RejectedCanonicalLiteral(ident);
        switch (rejected_literal) {
        case Objc3RejectedCanonicalLiteralKind::Yes:
          ++canonical_literal_rejection_counts_.yes_literal_sites;
          break;
        case Objc3RejectedCanonicalLiteralKind::No:
          ++canonical_literal_rejection_counts_.no_literal_sites;
          break;
        case Objc3RejectedCanonicalLiteralKind::Null:
          ++canonical_literal_rejection_counts_.null_literal_sites;
          break;
        case Objc3RejectedCanonicalLiteralKind::None:
          break;
        }
        if (rejected_literal != Objc3RejectedCanonicalLiteralKind::None) {
          diagnostics.push_back(MakeDiag(
              token_line,
              token_column,
              "O3C002",
              std::string("rejected canonical literal spelling '") +
                  Objc3RejectedCanonicalLiteralDiagnosticSpelling(rejected_literal) +
                  "' is rejected; use canonical '" +
                  Objc3RejectedCanonicalLiteralReplacementSpelling(rejected_literal) + "'"));
        }
      }
      tokens.push_back(Token{kind, ident, token_line, token_column});
      continue;
    }

    if (std::isdigit(static_cast<unsigned char>(c)) != 0) {
      tokens.push_back(Token{TokenKind::Number, ConsumeNumber(), token_line, token_column});
      continue;
    }

    const Objc3PunctuationTokenClassification punctuation =
        ClassifyObjc3PunctuationToken(source_, index_);
    if (punctuation.recognized) {
      for (std::size_t offset = 0; offset < punctuation.width; ++offset) {
        Advance();
      }
      if (punctuation.stray_block_comment_terminator) {
        diagnostics.push_back(MakeDiag(
            token_line,
            token_column,
            "O3L004",
            "stray block comment terminator"));
        continue;
      }
      tokens.push_back(Token{
          punctuation.kind,
          punctuation.text,
          token_line,
          token_column});
      continue;
    }

    Advance();
    diagnostics.push_back(
        MakeDiag(token_line, token_column, "O3L001", std::string("unexpected character '") + c + "'"));
  }
  return tokens;
}

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

void Objc3Lexer::SkipTrivia(std::vector<std::string> &diagnostics) {
  while (index_ < source_.size()) {
    const char c = source_[index_];
    if (std::isspace(static_cast<unsigned char>(c)) != 0) {
      Advance();
      continue;
    }
    if (c == '/' && index_ + 1 < source_.size() && source_[index_ + 1] == '/') {
      while (index_ < source_.size() && source_[index_] != '\n') {
        Advance();
      }
      continue;
    }
    if (c == '/' && index_ + 1 < source_.size() && source_[index_ + 1] == '*') {
      const unsigned comment_line = line_;
      const unsigned comment_column = column_;
      Advance();
      Advance();
      bool terminated = false;
      while (index_ < source_.size()) {
        if (source_[index_] == '/' && index_ + 1 < source_.size() && source_[index_ + 1] == '*') {
          diagnostics.push_back(MakeDiag(line_, column_, "O3L003", "nested block comments are unsupported"));
          index_ = source_.size();
          return;
        }
        if (source_[index_] == '*' && index_ + 1 < source_.size() && source_[index_ + 1] == '/') {
          Advance();
          Advance();
          terminated = true;
          break;
        }
        Advance();
      }
      if (!terminated) {
        diagnostics.push_back(MakeDiag(comment_line, comment_column, "O3L002", "unterminated block comment"));
        index_ = source_.size();
        return;
      }
      continue;
    }
    break;
  }
}

std::string Objc3Lexer::ConsumeIdentifier() {
  const std::size_t begin = index_;
  Advance();
  while (index_ < source_.size() && IsObjc3IdentifierBody(source_[index_])) {
    Advance();
  }
  return source_.substr(begin, index_ - begin);
}

std::string Objc3Lexer::ConsumeNumber() {
  const std::size_t begin = index_;
  if (index_ < source_.size() && source_[index_] == '0' && index_ + 1 < source_.size() &&
      (source_[index_ + 1] == 'b' || source_[index_ + 1] == 'B')) {
    Advance();
    Advance();
    while (index_ < source_.size() && (IsBinaryDigit(source_[index_]) || IsDigitSeparator(source_[index_]))) {
      Advance();
    }
    return source_.substr(begin, index_ - begin);
  }
  if (index_ < source_.size() && source_[index_] == '0' && index_ + 1 < source_.size() &&
      (source_[index_ + 1] == 'o' || source_[index_ + 1] == 'O')) {
    Advance();
    Advance();
    while (index_ < source_.size() && (IsOctalDigit(source_[index_]) || IsDigitSeparator(source_[index_]))) {
      Advance();
    }
    return source_.substr(begin, index_ - begin);
  }
  if (index_ < source_.size() && source_[index_] == '0' && index_ + 1 < source_.size() &&
      (source_[index_ + 1] == 'x' || source_[index_ + 1] == 'X')) {
    Advance();
    Advance();
    while (index_ < source_.size() && (IsHexDigit(source_[index_]) || IsDigitSeparator(source_[index_]))) {
      Advance();
    }
    return source_.substr(begin, index_ - begin);
  }
  while (index_ < source_.size() &&
         (std::isdigit(static_cast<unsigned char>(source_[index_])) != 0 || IsDigitSeparator(source_[index_]))) {
    Advance();
  }
  return source_.substr(begin, index_ - begin);
}

void Objc3Lexer::Advance() {
  if (index_ >= source_.size()) {
    return;
  }
  if (source_[index_] == '\n') {
    ++line_;
    column_ = 1;
  } else {
    ++column_;
  }
  ++index_;
}

bool Objc3Lexer::MatchChar(char expected) {
  if (index_ >= source_.size() || source_[index_] != expected) {
    return false;
  }
  Advance();
  return true;
}
