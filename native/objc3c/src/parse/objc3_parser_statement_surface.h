#pragma once

#include <cstddef>
#include <vector>

#include "token/objc3_token_contract.h"

namespace objc3c::parse {

bool IsObjc3IdentifierColonStatementLead(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index);
bool IsObjc3IdentifierAssignmentStatementLead(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index);
bool IsObjc3IdentifierUpdateStatementLead(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index);
bool IsObjc3PrefixUpdateStatementLead(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index);

}  // namespace objc3c::parse
