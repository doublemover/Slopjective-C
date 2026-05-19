#pragma once

#include "token/objc3_token_contract.h"

namespace objc3c::parse {

Objc3SemaTokenMetadata MakeObjc3ParserSemaTokenMetadata(
    Objc3SemaTokenKind kind,
    const Objc3LexToken &token);

}  // namespace objc3c::parse
