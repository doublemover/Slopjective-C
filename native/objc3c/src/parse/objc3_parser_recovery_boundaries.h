#pragma once

#include "token/objc3_token_contract.h"

namespace objc3c::parse {

bool IsObjc3TopLevelRecoveryBoundaryToken(Objc3LexTokenKind kind);
bool IsObjc3StatementRecoveryBoundaryToken(Objc3LexTokenKind kind);

}  // namespace objc3c::parse
