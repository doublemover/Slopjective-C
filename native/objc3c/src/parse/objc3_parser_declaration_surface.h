#pragma once

#include <cstddef>
#include <vector>

#include "token/objc3_token_contract.h"

namespace objc3c::parse {

bool IsObjc3ActorClassDeclarationLead(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index);
bool IsObjc3RemovedOptionalTemplateAliasLead(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index);
bool IsObjc3TopLevelFunctionQualifierLead(Objc3LexTokenKind kind);

}  // namespace objc3c::parse
