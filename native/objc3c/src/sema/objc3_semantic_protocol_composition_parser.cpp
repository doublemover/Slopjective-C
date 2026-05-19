#include "sema/objc3_semantic_protocol_composition_parser.h"

#include "support/objc3_identifier_spelling.h"
#include "support/objc3_string_predicates.h"

#include <algorithm>
#include <unordered_set>

bool ProtocolCompositionParseResult::IsValid() const {
  return !malformed_composition && !empty_composition &&
         invalid_identifiers.empty() && duplicate_identifiers.empty();
}

ProtocolCompositionParseResult ParseProtocolCompositionSuffixText(
    const std::string &suffix_text) {
  ProtocolCompositionParseResult result;
  if (suffix_text.empty()) {
    return result;
  }

  result.has_protocol_composition = true;
  if (suffix_text.size() < 2 || suffix_text.front() != '<' ||
      suffix_text.back() != '>') {
    result.malformed_composition = true;
    return result;
  }

  const std::string inner = suffix_text.substr(1, suffix_text.size() - 2);
  if (inner.find('<') != std::string::npos ||
      inner.find('>') != std::string::npos) {
    result.malformed_composition = true;
  }

  std::unordered_set<std::string> seen_names;
  std::size_t start = 0;
  while (start <= inner.size()) {
    const std::size_t comma = inner.find(',', start);
    const std::size_t token_end =
        (comma == std::string::npos) ? inner.size() : comma;
    const std::string token =
        objc3c::support::TrimAsciiWhitespace(
            inner.substr(start, token_end - start));
    if (token.empty()) {
      result.empty_composition = true;
    } else if (!objc3c::support::IsObjcIdentifierSpelling(token)) {
      result.invalid_identifiers.push_back(token);
    } else if (!seen_names.insert(token).second) {
      result.duplicate_identifiers.push_back(token);
    } else {
      result.names_lexicographic.push_back(token);
    }

    if (comma == std::string::npos) {
      break;
    }
    start = comma + 1;
  }

  if (result.names_lexicographic.empty()) {
    result.empty_composition = true;
  }
  std::sort(result.names_lexicographic.begin(),
            result.names_lexicographic.end());
  return result;
}
