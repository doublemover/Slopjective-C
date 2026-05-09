#pragma once

#include <string>

#include "token/objc3_token_contract.h"

namespace objc3c::parse {

std::string BuildObjc3ParserDiagnostic(
    const Objc3LexToken &token,
    const char *code,
    const std::string &message);
std::string BuildObjc3RemovedOptionalTemplateAliasDiagnostic(
    const Objc3LexToken &token);
std::string BuildObjc3UnsupportedTopLevelDiagnostic(
    const Objc3LexToken &token);

}  // namespace objc3c::parse
