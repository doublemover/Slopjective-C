#include "parse/objc3_parser_cursor.h"

namespace objc3c::parse {

bool AtTokenKind(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index,
    Objc3LexTokenKind kind) {
  return index < tokens.size() && tokens[index].kind == kind;
}

const Objc3LexToken &PeekToken(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index) {
  return tokens[index];
}

const Objc3LexToken &PreviousToken(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index) {
  return tokens[index - 1u];
}

const Objc3LexToken &AdvanceToken(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index) {
  if (!AtTokenKind(tokens, index, Objc3LexTokenKind::Eof)) {
    ++index;
  }
  return PreviousToken(tokens, index);
}

bool MatchTokenKind(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    Objc3LexTokenKind kind) {
  if (!AtTokenKind(tokens, index, kind)) {
    return false;
  }
  AdvanceToken(tokens, index);
  return true;
}

bool AtIdentifierText(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index,
    const std::string &text) {
  return index < tokens.size() &&
         tokens[index].kind == Objc3LexTokenKind::Identifier &&
         tokens[index].text == text;
}

bool AtIdentifierTextOffset(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index,
    std::size_t offset,
    const std::string &text) {
  return index + offset < tokens.size() &&
         tokens[index + offset].kind == Objc3LexTokenKind::Identifier &&
         tokens[index + offset].text == text;
}

}  // namespace objc3c::parse
