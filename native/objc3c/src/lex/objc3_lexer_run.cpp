#include "lex/objc3_lexer.h"

#include <cctype>

#include "diag/objc3_diag_utils.h"
#include "lex/objc3_lexer_char_class.h"
#include "token/objc3_token_kind.h"
#include "token/objc3_token_punctuation.h"

namespace {

using Token = Objc3LexToken;
using TokenKind = Objc3LexTokenKind;

}  // namespace

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
              std::string("legacy literal alias '") +
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
