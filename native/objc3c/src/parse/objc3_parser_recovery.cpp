#include "parse/objc3_parser_recovery.h"

#include "parse/objc3_parser_cursor.h"
#include "parse/objc3_parser_recovery_boundaries.h"
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
  if (!Objc3ParserRecoveryBoundaryIsHardCutoverOwned() ||
      Objc3ParserRecoveryBoundaryCountsAsSuccess()) {
    return;
  }
  while (!At(tokens, index, TokenKind::Eof)) {
    if (Match(tokens, index, TokenKind::Semicolon)) {
      return;
    }
    if (IsObjc3TopLevelRecoveryBoundaryToken(PeekToken(tokens, index).kind)) {
      return;
    }
    Advance(tokens, index);
  }
}

void SynchronizeObjc3ParserFunctionTail(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index) {
  if (!Objc3ParserRecoveryBoundaryIsHardCutoverOwned() ||
      Objc3ParserRecoveryBoundaryCountsAsSuccess()) {
    return;
  }
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
  if (!Objc3ParserRecoveryBoundaryIsHardCutoverOwned() ||
      Objc3ParserRecoveryBoundaryCountsAsSuccess()) {
    return;
  }
  while (!At(tokens, index, TokenKind::Eof)) {
    if (Match(tokens, index, TokenKind::Semicolon)) {
      return;
    }
    if (IsObjc3StatementRecoveryBoundaryToken(PeekToken(tokens, index).kind) ||
        IsObjc3IdentifierAssignmentStatementLead(tokens, index) ||
        IsObjc3IdentifierUpdateStatementLead(tokens, index) ||
        IsObjc3PrefixUpdateStatementLead(tokens, index)) {
      return;
    }
    Advance(tokens, index);
  }
}

}  // namespace objc3c::parse
