#pragma once

#include <vector>

#include "parse/objc3_parser_contract.h"
#include "token/objc3_token_contract.h"

struct Objc3AstBuilderResult {
  Objc3ParsedProgram program;
  std::vector<std::string> diagnostics;
};

Objc3AstBuilderResult BuildObjc3AstFromTokens(const Objc3LexTokenStream &tokens);
