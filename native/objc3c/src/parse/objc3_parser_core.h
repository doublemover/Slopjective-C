#pragma once

#include <string>
#include <vector>

#include "parse/objc3_parser_contract_types.h"
#include "token/objc3_token_contract.h"

namespace objc3c::parse {

struct Objc3ParserCoreResult {
  Objc3ParsedProgram program;
  std::vector<std::string> diagnostics;
};

Objc3ParserCoreResult ParseObjc3ProgramCore(
    const Objc3LexTokenStream &tokens);

}  // namespace objc3c::parse
