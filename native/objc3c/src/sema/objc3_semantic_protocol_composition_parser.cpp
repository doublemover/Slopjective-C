#include "sema/objc3_semantic_protocol_composition_parser.h"

#include <algorithm>
#include <cctype>
#include <unordered_set>

bool ProtocolCompositionParseResult::IsValid() const {
  return !malformed_composition && !empty_composition &&
         invalid_identifiers.empty() && duplicate_identifiers.empty();
}

std::string TrimAsciiWhitespace(const std::string &text) {
  std::size_t start = 0;
  while (start < text.size() &&
         std::isspace(static_cast<unsigned char>(text[start])) != 0) {
    ++start;
  }
  if (start == text.size()) {
    return "";
  }

  std::size_t end = text.size();
  while (end > start &&
         std::isspace(static_cast<unsigned char>(text[end - 1])) != 0) {
    --end;
  }
  return text.substr(start, end - start);
}

bool IsValidProtocolIdentifier(const std::string &identifier) {
  if (identifier.empty()) {
    return false;
  }
  const unsigned char first = static_cast<unsigned char>(identifier.front());
  if (!(std::isalpha(first) != 0 || first == '_')) {
    return false;
  }
  for (std::size_t i = 1; i < identifier.size(); ++i) {
    const unsigned char c = static_cast<unsigned char>(identifier[i]);
    if (!(std::isalnum(c) != 0 || c == '_')) {
      return false;
    }
  }
  return true;
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
        TrimAsciiWhitespace(inner.substr(start, token_end - start));
    if (token.empty()) {
      result.empty_composition = true;
    } else if (!IsValidProtocolIdentifier(token)) {
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
