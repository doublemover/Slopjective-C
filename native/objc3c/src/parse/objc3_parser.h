#pragma once

#include <string>
#include <vector>

#include "ast/objc3_ast.h"
#include "token/objc3_token_contract.h"

struct Objc3ParseResult {
  Objc3Program program;
  std::vector<std::string> diagnostics;
};

Objc3ParseResult ParseObjc3Program(const std::vector<Objc3LexToken> &tokens);
