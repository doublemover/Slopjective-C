#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "token/objc3_token_contract.h"

namespace objc3c::parse {

bool AtTokenKind(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index,
    Objc3LexTokenKind kind);
const Objc3LexToken &PeekToken(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index);
const Objc3LexToken &PreviousToken(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index);
const Objc3LexToken &AdvanceToken(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index);
bool MatchTokenKind(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    Objc3LexTokenKind kind);
bool AtIdentifierText(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index,
    const std::string &text);
bool AtIdentifierTextOffset(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index,
    std::size_t offset,
    const std::string &text);

}  // namespace objc3c::parse
