#pragma once

#include <cstddef>
#include <vector>

#include "token/objc3_token_contract.h"

namespace objc3c::parse {

void SynchronizeObjc3ParserTopLevel(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index);

void SynchronizeObjc3ParserFunctionTail(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index);

void SynchronizeObjc3ParserStatement(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index);

}  // namespace objc3c::parse
