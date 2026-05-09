#pragma once

#include <string_view>

#include "contracts/objc3_diagnostic_owner_contract.h"
#include "token/objc3_token_contract.h"

namespace objc3c::parse {

inline constexpr std::string_view kObjc3ParserRecoveryBoundaryOwner =
    ::kObjc3ParserDiagnosticRecoveryOwner;

bool IsObjc3TopLevelRecoveryBoundaryToken(Objc3LexTokenKind kind);
bool IsObjc3StatementRecoveryBoundaryToken(Objc3LexTokenKind kind);
bool Objc3ParserRecoveryBoundaryIsHardCutoverOwned();
bool Objc3ParserRecoveryBoundaryCountsAsSuccess();

}  // namespace objc3c::parse
