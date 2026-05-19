#pragma once

#include "parse/objc3_parse_phase_io.h"
#include "token/objc3_token_contract.h"

Objc3ParseResult ParseObjc3Program(const Objc3LexTokenStream &tokens);
