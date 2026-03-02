#pragma once

#include <string>
#include <vector>

#include "parse/objc3_parser_contract.h"
#include "token/objc3_token_contract.h"

struct Objc3ParseResult {
  Objc3ParsedProgram program;
  std::vector<std::string> diagnostics;
  Objc3ParserContractSnapshot contract_snapshot;
};

Objc3ParseResult ParseObjc3Program(const Objc3LexTokenStream &tokens);
