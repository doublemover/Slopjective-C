#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "parse/objc3_parser_contract_types.h"

struct Objc3ParseResult {
  Objc3ParsedProgram program;
  std::vector<std::string> diagnostics;
  Objc3ParserContractSnapshot contract_snapshot;
};

Objc3ParseResult BuildObjc3ParseResult(
    Objc3ParsedProgram program,
    std::vector<std::string> diagnostics,
    std::size_t token_count);
