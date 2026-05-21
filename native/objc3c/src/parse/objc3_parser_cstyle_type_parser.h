#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ast/objc3_ast_core.h"
#include "token/objc3_token_contract.h"

namespace objc3c::parse {

struct Objc3CStyleFunctionTypeParseResult {
  bool ok = false;
  ValueType type = ValueType::Unknown;
  bool id_spelling = false;
  bool class_spelling = false;
  bool sel_spelling = false;
  bool instancetype_spelling = false;
  bool object_pointer_spelling = false;
  std::string object_pointer_name;
  bool has_generic_suffix = false;
  bool generic_suffix_terminated = true;
  std::string generic_suffix_text;
  unsigned generic_line = 1;
  unsigned generic_column = 1;
  unsigned pointer_depth = 0;
};

Objc3CStyleFunctionTypeParseResult ParseObjc3CStyleFunctionType(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t &index,
    bool allow_void,
    unsigned diag_line,
    unsigned diag_column,
    const char *diagnostic_context,
    std::vector<std::string> &diagnostics);

}  // namespace objc3c::parse
