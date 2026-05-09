#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "ast/objc3_ast_core.h"
#include "token/objc3_token_contract.h"

namespace objc3c::parse {

struct Objc3CStyleTypeTokenClassification {
  bool recognized = false;
  ValueType type = ValueType::Unknown;
  bool id_spelling = false;
  bool class_spelling = false;
  bool sel_spelling = false;
  bool instancetype_spelling = false;
};

bool ClassifyObjc3CStyleBuiltinTypeSpelling(
    const std::string &spelling,
    Objc3CStyleTypeTokenClassification &classification);
bool ClassifyObjc3CStyleBuiltinTypeToken(
    const Objc3LexToken &token,
    Objc3CStyleTypeTokenClassification &classification);
bool IsObjc3CStyleTypeLeadToken(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index);
bool IsObjc3CStyleFunctionDeclarationStart(
    const std::vector<Objc3LexToken> &tokens,
    std::size_t index);

}  // namespace objc3c::parse
