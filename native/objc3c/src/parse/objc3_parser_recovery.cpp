#include "parse/objc3_parser_recovery.h"

#include "parse/objc3_parser_cursor.h"
#include "parse/objc3_parser_statement_surface.h"

namespace objc3c::parse {
namespace {

using TokenKind = Objc3LexTokenKind;

bool At(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index,
    TokenKind kind) {
  return AtTokenKind(tokens, index, kind);
}

bool Match(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    TokenKind kind) {
  return MatchTokenKind(tokens, index, kind);
}

void Advance(const std::vector<Objc3LexToken> &tokens, std::size_t &index) {
  (void)AdvanceToken(tokens, index);
}

}  // namespace

void SynchronizeObjc3ParserTopLevel(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index) {
  while (!At(tokens, index, TokenKind::Eof)) {
    if (Match(tokens, index, TokenKind::Semicolon)) {
      return;
    }
    if (At(tokens, index, TokenKind::KwModule) ||
        At(tokens, index, TokenKind::KwLet) ||
        At(tokens, index, TokenKind::KwFn) ||
        At(tokens, index, TokenKind::KwPure) ||
        At(tokens, index, TokenKind::KwExtern) ||
        At(tokens, index, TokenKind::KwAsync) ||
        At(tokens, index, TokenKind::KwAtInterface) ||
        At(tokens, index, TokenKind::KwAtImplementation) ||
        At(tokens, index, TokenKind::KwAtProtocol) ||
        At(tokens, index, TokenKind::KwAtProperty)) {
      return;
    }
    Advance(tokens, index);
  }
}

void SynchronizeObjc3ParserFunctionTail(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index) {
  if (At(tokens, index, TokenKind::LBrace)) {
    int depth = 0;
    while (!At(tokens, index, TokenKind::Eof)) {
      if (Match(tokens, index, TokenKind::LBrace)) {
        ++depth;
        continue;
      }
      if (Match(tokens, index, TokenKind::RBrace)) {
        --depth;
        if (depth <= 0) {
          return;
        }
        continue;
      }
      Advance(tokens, index);
    }
    return;
  }
  SynchronizeObjc3ParserTopLevel(tokens, index);
}

void SynchronizeObjc3ParserStatement(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index) {
  while (!At(tokens, index, TokenKind::Eof)) {
    if (Match(tokens, index, TokenKind::Semicolon)) {
      return;
    }
    if (At(tokens, index, TokenKind::KwLet) ||
        At(tokens, index, TokenKind::KwReturn) ||
        At(tokens, index, TokenKind::KwIf) ||
        At(tokens, index, TokenKind::KwGuard) ||
        At(tokens, index, TokenKind::KwDefer) ||
        At(tokens, index, TokenKind::KwDo) ||
        At(tokens, index, TokenKind::KwMatch) ||
        At(tokens, index, TokenKind::KwFor) ||
        At(tokens, index, TokenKind::KwSwitch) ||
        At(tokens, index, TokenKind::KwWhile) ||
        At(tokens, index, TokenKind::KwBreak) ||
        At(tokens, index, TokenKind::KwContinue) ||
        At(tokens, index, TokenKind::KwAtAutoreleasePool) ||
        IsObjc3IdentifierAssignmentStatementLead(tokens, index) ||
        IsObjc3IdentifierUpdateStatementLead(tokens, index) ||
        IsObjc3PrefixUpdateStatementLead(tokens, index) ||
        At(tokens, index, TokenKind::RBrace)) {
      return;
    }
    Advance(tokens, index);
  }
}

}  // namespace objc3c::parse
