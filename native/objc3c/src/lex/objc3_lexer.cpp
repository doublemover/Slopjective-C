#include "lex/objc3_lexer.h"

#include <cctype>
#include <sstream>
#include <string>

namespace {

using Token = Objc3LexToken;
using TokenKind = Objc3LexTokenKind;

bool IsIdentStart(char c) {
  return std::isalpha(static_cast<unsigned char>(c)) != 0 || c == '_';
}

bool IsIdentBody(char c) {
  return std::isalnum(static_cast<unsigned char>(c)) != 0 || c == '_';
}

bool IsHexDigit(char c) {
  return std::isxdigit(static_cast<unsigned char>(c)) != 0;
}

bool IsBinaryDigit(char c) {
  return c == '0' || c == '1';
}

bool IsOctalDigit(char c) {
  return c >= '0' && c <= '7';
}

bool IsDigitSeparator(char c) {
  return c == '_';
}

bool IsHorizontalWhitespace(char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\v' || c == '\f';
}

std::string EscapeStringTokenText(const std::string &value) {
  std::string escaped;
  escaped.reserve(value.size() + 2u);
  escaped.push_back('"');
  for (char c : value) {
    if (c == '"' || c == '\\') {
      escaped.push_back('\\');
    }
    escaped.push_back(c);
  }
  escaped.push_back('"');
  return escaped;
}

std::string MakeDiag(unsigned line, unsigned column, const std::string &code, const std::string &message) {
  std::ostringstream out;
  out << "error:" << line << ":" << column << ": " << message << " [" << code << "]";
  return out.str();
}

}  // namespace

Objc3Lexer::Objc3Lexer(const std::string &source, const Objc3LexerOptions &options)
    : source_(source), options_(options) {}

const Objc3LexerMigrationHints &Objc3Lexer::MigrationHints() const {
  return migration_hints_;
}

const Objc3LexerLanguageVersionPragmaContract &Objc3Lexer::LanguageVersionPragmaContract() const {
  return language_version_pragma_contract_;
}

const Objc3LexerBootstrapRegistrationSourceContract &
Objc3Lexer::BootstrapRegistrationSourceContract() const {
  return bootstrap_registration_source_contract_;
}

std::vector<Objc3LexToken> Objc3Lexer::Run(std::vector<std::string> &diagnostics) {
  // M264-A001 mode-truth source anchor: the lexer owns the effective language
  // version / compatibility / migration prelude inputs consumed by the
  // runnable feature-claim inventory emitted by the frontend manifest.
  // M264-A002 truth-surface wiring anchor: strictness / strict concurrency do
  // not have accepted hidden lexer/prelude selection forms yet, so the emitted
  // truth packet must keep those surfaces fail-closed and unadvertised.
  // M269-A001 source-closure note: task/executor/cancellation work does not
  // add dedicated lexer keywords yet; do not add dedicated lexer keywords yet.
  // The admitted source surface remains the existing async/await tokens plus
  // parser-owned identifier profiles and canonical `objc_executor(...)`
  // attribute payloads, including the later M269-A002 task-group/task-creation
  // callable source completion.
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
      if (index_ < source_.size() && IsIdentStart(source_[index_])) {
        const std::string directive = ConsumeIdentifier();
        if (directive == "interface") {
          tokens.push_back(Token{TokenKind::KwAtInterface, "@interface", token_line, token_column});
          continue;
        }
        if (directive == "implementation") {
          tokens.push_back(Token{TokenKind::KwAtImplementation, "@implementation", token_line, token_column});
          continue;
        }
        if (directive == "protocol") {
          tokens.push_back(Token{TokenKind::KwAtProtocol, "@protocol", token_line, token_column});
          continue;
        }
        if (directive == "required") {
          tokens.push_back(Token{TokenKind::KwAtRequired, "@required", token_line, token_column});
          continue;
        }
        if (directive == "optional") {
          tokens.push_back(Token{TokenKind::KwAtOptional, "@optional", token_line, token_column});
          continue;
        }
        if (directive == "property") {
          tokens.push_back(Token{TokenKind::KwAtProperty, "@property", token_line, token_column});
          continue;
        }
        if (directive == "keypath") {
          tokens.push_back(Token{TokenKind::KwAtKeypath, "@keypath", token_line, token_column});
          continue;
        }
        if (directive == "end") {
          tokens.push_back(Token{TokenKind::KwAtEnd, "@end", token_line, token_column});
          continue;
        }
        if (directive == "autoreleasepool") {
          tokens.push_back(Token{TokenKind::KwAtAutoreleasePool, "@autoreleasepool", token_line, token_column});
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
      tokens.push_back(Token{TokenKind::String, EscapeStringTokenText(value),
                             token_line, token_column});
      continue;
    }
    if (IsIdentStart(c)) {
      std::string ident = ConsumeIdentifier();
      TokenKind kind = TokenKind::Identifier;
      if (ident == "module") {
        kind = TokenKind::KwModule;
      } else if (ident == "let") {
        kind = TokenKind::KwLet;
      } else if (ident == "var") {
        kind = TokenKind::KwVar;
      } else if (ident == "fn") {
        kind = TokenKind::KwFn;
      } else if (ident == "async") {
        kind = TokenKind::KwAsync;
      } else if (ident == "pure") {
        kind = TokenKind::KwPure;
      } else if (ident == "extern") {
        kind = TokenKind::KwExtern;
      } else if (ident == "return") {
        kind = TokenKind::KwReturn;
      } else if (ident == "if") {
        kind = TokenKind::KwIf;
      } else if (ident == "else") {
        kind = TokenKind::KwElse;
      } else if (ident == "guard") {
        kind = TokenKind::KwGuard;
      // M266-A001 source-closure anchor: reserve defer/match as explicit
      // frontend-owned keywords so later Part 5 work can fail closed
      // deterministically instead of drifting as plain identifiers.
      } else if (ident == "defer") {
        kind = TokenKind::KwDefer;
      } else if (ident == "do") {
        kind = TokenKind::KwDo;
      } else if (ident == "await") {
        kind = TokenKind::KwAwait;
      } else if (ident == "try") {
        kind = TokenKind::KwTry;
      } else if (ident == "throw") {
        kind = TokenKind::KwThrow;
      } else if (ident == "catch") {
        kind = TokenKind::KwCatch;
      } else if (ident == "for") {
        kind = TokenKind::KwFor;
      } else if (ident == "switch") {
        kind = TokenKind::KwSwitch;
      } else if (ident == "match") {
        kind = TokenKind::KwMatch;
      } else if (ident == "case") {
        kind = TokenKind::KwCase;
      } else if (ident == "default") {
        kind = TokenKind::KwDefault;
      } else if (ident == "while") {
        kind = TokenKind::KwWhile;
      } else if (ident == "break") {
        kind = TokenKind::KwBreak;
      } else if (ident == "continue") {
        kind = TokenKind::KwContinue;
      } else if (ident == "i32") {
        kind = TokenKind::KwI32;
      } else if (ident == "bool") {
        kind = TokenKind::KwBool;
      } else if (ident == "BOOL") {
        kind = TokenKind::KwBOOL;
      } else if (ident == "NSInteger") {
        kind = TokenKind::KwNSInteger;
      } else if (ident == "NSUInteger") {
        kind = TokenKind::KwNSUInteger;
      } else if (ident == "void") {
        kind = TokenKind::KwVoid;
      } else if (ident == "id") {
        kind = TokenKind::KwId;
      } else if (ident == "Class") {
        kind = TokenKind::KwClass;
      } else if (ident == "SEL") {
        kind = TokenKind::KwSEL;
      } else if (ident == "Protocol") {
        kind = TokenKind::KwProtocol;
      } else if (ident == "instancetype") {
        kind = TokenKind::KwInstancetype;
      } else if (ident == "true") {
        kind = TokenKind::KwTrue;
      } else if (ident == "false") {
        kind = TokenKind::KwFalse;
      } else if (ident == "nil") {
        kind = TokenKind::KwNil;
      } else if (ident == "YES") {
        kind = TokenKind::KwTrue;
        if (options_.migration_assist) {
          ++migration_hints_.legacy_yes_count;
        }
      } else if (ident == "NO") {
        kind = TokenKind::KwFalse;
        if (options_.migration_assist) {
          ++migration_hints_.legacy_no_count;
        }
      } else if (ident == "NULL") {
        kind = TokenKind::KwNil;
        if (options_.migration_assist) {
          ++migration_hints_.legacy_null_count;
        }
      }
      tokens.push_back(Token{kind, ident, token_line, token_column});
      continue;
    }

    if (std::isdigit(static_cast<unsigned char>(c)) != 0) {
      tokens.push_back(Token{TokenKind::Number, ConsumeNumber(), token_line, token_column});
      continue;
    }

    Advance();
    switch (c) {
      case '(':
        tokens.push_back(Token{TokenKind::LParen, "(", token_line, token_column});
        break;
      case ')':
        tokens.push_back(Token{TokenKind::RParen, ")", token_line, token_column});
        break;
      case '[':
        tokens.push_back(Token{TokenKind::LBracket, "[", token_line, token_column});
        break;
      case ']':
        tokens.push_back(Token{TokenKind::RBracket, "]", token_line, token_column});
        break;
      case '{':
        tokens.push_back(Token{TokenKind::LBrace, "{", token_line, token_column});
        break;
      case '}':
        tokens.push_back(Token{TokenKind::RBrace, "}", token_line, token_column});
        break;
      case ',':
        tokens.push_back(Token{TokenKind::Comma, ",", token_line, token_column});
        break;
      case ':':
        tokens.push_back(Token{TokenKind::Colon, ":", token_line, token_column});
        break;
      case '.':
        tokens.push_back(Token{TokenKind::Dot, ".", token_line, token_column});
        break;
      case ';':
        tokens.push_back(Token{TokenKind::Semicolon, ";", token_line, token_column});
        break;
      case '=':
        if (MatchChar('=')) {
          tokens.push_back(Token{TokenKind::EqualEqual, "==", token_line, token_column});
        } else {
          tokens.push_back(Token{TokenKind::Equal, "=", token_line, token_column});
        }
        break;
      case '!':
        if (MatchChar('=')) {
          tokens.push_back(Token{TokenKind::BangEqual, "!=", token_line, token_column});
        } else {
          tokens.push_back(Token{TokenKind::Bang, "!", token_line, token_column});
        }
        break;
      case '<':
        if (MatchChar('<')) {
          if (MatchChar('=')) {
            tokens.push_back(Token{TokenKind::LessLessEqual, "<<=", token_line, token_column});
          } else {
            tokens.push_back(Token{TokenKind::LessLess, "<<", token_line, token_column});
          }
        } else if (MatchChar('=')) {
          tokens.push_back(Token{TokenKind::LessEqual, "<=", token_line, token_column});
        } else {
          tokens.push_back(Token{TokenKind::Less, "<", token_line, token_column});
        }
        break;
      case '>':
        if (MatchChar('>')) {
          if (MatchChar('=')) {
            tokens.push_back(Token{TokenKind::GreaterGreaterEqual, ">>=", token_line, token_column});
          } else {
            tokens.push_back(Token{TokenKind::GreaterGreater, ">>", token_line, token_column});
          }
        } else if (MatchChar('=')) {
          tokens.push_back(Token{TokenKind::GreaterEqual, ">=", token_line, token_column});
        } else {
          tokens.push_back(Token{TokenKind::Greater, ">", token_line, token_column});
        }
        break;
      case '&':
        if (MatchChar('&')) {
          tokens.push_back(Token{TokenKind::AndAnd, "&&", token_line, token_column});
        } else if (MatchChar('=')) {
          tokens.push_back(Token{TokenKind::AmpersandEqual, "&=", token_line, token_column});
        } else {
          tokens.push_back(Token{TokenKind::Ampersand, "&", token_line, token_column});
        }
        break;
      case '|':
        if (MatchChar('|')) {
          tokens.push_back(Token{TokenKind::OrOr, "||", token_line, token_column});
        } else if (MatchChar('=')) {
          tokens.push_back(Token{TokenKind::PipeEqual, "|=", token_line, token_column});
        } else {
          tokens.push_back(Token{TokenKind::Pipe, "|", token_line, token_column});
        }
        break;
      case '^':
        if (MatchChar('=')) {
          tokens.push_back(Token{TokenKind::CaretEqual, "^=", token_line, token_column});
        } else {
          tokens.push_back(Token{TokenKind::Caret, "^", token_line, token_column});
        }
        break;
      case '?':
        if (MatchChar('?')) {
          tokens.push_back(Token{TokenKind::QuestionQuestion, "??", token_line, token_column});
          break;
        }
        if (MatchChar('.')) {
          tokens.push_back(Token{TokenKind::QuestionDot, "?.", token_line, token_column});
          break;
        }
        tokens.push_back(Token{TokenKind::Question, "?", token_line, token_column});
        break;
      case '~':
        tokens.push_back(Token{TokenKind::Tilde, "~", token_line, token_column});
        break;
      case '+':
        if (MatchChar('+')) {
          tokens.push_back(Token{TokenKind::PlusPlus, "++", token_line, token_column});
        } else if (MatchChar('=')) {
          tokens.push_back(Token{TokenKind::PlusEqual, "+=", token_line, token_column});
        } else {
          tokens.push_back(Token{TokenKind::Plus, "+", token_line, token_column});
        }
        break;
      case '-':
        if (MatchChar('-')) {
          tokens.push_back(Token{TokenKind::MinusMinus, "--", token_line, token_column});
        } else if (MatchChar('=')) {
          tokens.push_back(Token{TokenKind::MinusEqual, "-=", token_line, token_column});
        } else {
          tokens.push_back(Token{TokenKind::Minus, "-", token_line, token_column});
        }
        break;
      case '*':
        if (MatchChar('/')) {
          diagnostics.push_back(MakeDiag(token_line, token_column, "O3L004", "stray block comment terminator"));
        } else if (MatchChar('=')) {
          tokens.push_back(Token{TokenKind::StarEqual, "*=", token_line, token_column});
        } else {
          tokens.push_back(Token{TokenKind::Star, "*", token_line, token_column});
        }
        break;
      case '/':
        if (MatchChar('=')) {
          tokens.push_back(Token{TokenKind::SlashEqual, "/=", token_line, token_column});
        } else {
          tokens.push_back(Token{TokenKind::Slash, "/", token_line, token_column});
        }
        break;
      case '%':
        if (MatchChar('=')) {
          tokens.push_back(Token{TokenKind::PercentEqual, "%=", token_line, token_column});
        } else {
          tokens.push_back(Token{TokenKind::Percent, "%", token_line, token_column});
        }
        break;
      default:
        diagnostics.push_back(
            MakeDiag(token_line, token_column, "O3L001", std::string("unexpected character '") + c + "'"));
        break;
    }
  }
  return tokens;
}

void Objc3Lexer::ConsumePreludePragmas(std::vector<std::string> &diagnostics) {
  // M263-A002 frontend-closure anchor: bootstrap registration pragmas must be
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
  while (cursor < source_.size() && IsHorizontalWhitespace(source_[cursor])) {
    ++cursor;
  }
  if (!MatchLiteralAt(cursor, "pragma")) {
    return false;
  }
  cursor += 6;
  while (cursor < source_.size() && IsHorizontalWhitespace(source_[cursor])) {
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
  if (index_ >= source_.size() || !IsIdentStart(source_[index_])) {
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
  while (cursor < source_.size() && IsHorizontalWhitespace(source_[cursor])) {
    ++cursor;
  }
  if (!MatchLiteralAt(cursor, "pragma")) {
    return false;
  }
  cursor += 6;
  while (cursor < source_.size() && IsHorizontalWhitespace(source_[cursor])) {
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
  while (index_ < source_.size() && IsHorizontalWhitespace(source_[index_])) {
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
  while (index_ < source_.size() && IsIdentBody(source_[index_])) {
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
