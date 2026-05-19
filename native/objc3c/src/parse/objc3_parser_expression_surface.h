#pragma once

#include "token/objc3_token_contract.h"

namespace objc3c::parse {

bool IsObjc3AssignmentOperatorToken(Objc3LexTokenKind kind);
bool IsObjc3UpdateOperatorToken(Objc3LexTokenKind kind);
const char *Objc3AssignmentOperatorSpelling(Objc3LexTokenKind kind);
const char *Objc3UpdateOperatorSpelling(Objc3LexTokenKind kind);

}  // namespace objc3c::parse
