#pragma once

#include "ast/objc3_ast.h"
#include "token/objc3_token_contract.h"

#include <cstddef>
#include <string>
#include <vector>

namespace objc3c::parse {

bool TryParseVectorTypeSpelling(const Objc3LexToken &type_token,
                                ValueType &vector_type,
                                std::string &vector_base_spelling,
                                unsigned &vector_lane_count);

std::string BuildTypedKeyPathLiteralProfile(
    const std::string &root_name,
    bool root_is_self,
    const std::vector<std::string> &components);

std::string BuildAutoreleasePoolScopeSymbol(unsigned serial, unsigned depth);

std::string DescribeParserDiagnosticToken(const Objc3LexToken &token);

std::size_t CountMarkerOccurrences(const std::string &text,
                                   const std::string &marker);
std::size_t CountTopLevelGenericArgumentSlots(
    const std::string &generic_suffix_text);
std::size_t CountNamespaceSegments(const std::string &name);

std::string BuildLowercaseProfileToken(std::string token);

}  // namespace objc3c::parse
