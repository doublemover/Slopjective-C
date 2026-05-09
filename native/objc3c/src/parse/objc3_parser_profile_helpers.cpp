#include "parse/objc3_parser_profile_helpers.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace objc3c::parse {

bool TryParseVectorTypeSpelling(const Objc3LexToken &type_token,
                                ValueType &vector_type,
                                std::string &vector_base_spelling,
                                unsigned &vector_lane_count) {
  const std::string &text = type_token.text;
  const bool is_i32_vector = text.rfind("i32x", 0) == 0;
  const bool is_bool_vector = text.rfind("boolx", 0) == 0;
  if (!is_i32_vector && !is_bool_vector) {
    return false;
  }

  const std::size_t prefix_length = is_i32_vector ? 4u : 5u;
  if (text.size() <= prefix_length) {
    return false;
  }

  unsigned lane_count = 0;
  for (std::size_t i = prefix_length; i < text.size(); ++i) {
    const char c = text[i];
    if (!std::isdigit(static_cast<unsigned char>(c))) {
      return false;
    }
    lane_count = (lane_count * 10u) + static_cast<unsigned>(c - '0');
    if (lane_count > 1024u) {
      return false;
    }
  }

  if (lane_count != 2u && lane_count != 4u && lane_count != 8u &&
      lane_count != 16u) {
    return false;
  }

  vector_type = is_i32_vector ? ValueType::I32 : ValueType::Bool;
  vector_base_spelling = is_i32_vector ? "i32" : "bool";
  vector_lane_count = lane_count;
  return true;
}

std::string BuildNormalizedObjcSelector(
    const std::vector<Objc3MethodDecl::SelectorPiece> &pieces) {
  std::string normalized;
  for (const auto &piece : pieces) {
    normalized += piece.keyword;
    if (piece.has_parameter) {
      normalized += ":";
    }
  }
  return normalized;
}

std::string BuildTypedKeyPathLiteralProfile(
    const std::string &root_name,
    bool root_is_self,
    const std::vector<std::string> &components) {
  std::ostringstream out;
  out << "typed-keypath:root=" << (root_is_self ? "self" : root_name)
      << ";components=";
  for (std::size_t i = 0; i < components.size(); ++i) {
    if (i != 0u) {
      out << ".";
    }
    out << components[i];
  }
  return out.str();
}

std::string BuildAutoreleasePoolScopeSymbol(unsigned serial, unsigned depth) {
  std::ostringstream out;
  out << "autoreleasepool-scope:" << serial << ";depth=" << depth;
  return out.str();
}

std::string DescribeParserDiagnosticToken(const Objc3LexToken &token) {
  if (token.kind == Objc3LexTokenKind::Eof) {
    return "end of file";
  }
  if (token.kind == Objc3LexTokenKind::Identifier) {
    return "identifier '" + token.text + "'";
  }
  if (token.kind == Objc3LexTokenKind::Number) {
    return "number '" + token.text + "'";
  }
  if (token.text.empty()) {
    return "token";
  }
  return "token '" + token.text + "'";
}

std::size_t CountMarkerOccurrences(const std::string &text,
                                   const std::string &marker) {
  if (marker.empty() || text.empty()) {
    return 0;
  }
  std::size_t count = 0;
  std::size_t offset = 0;
  while (true) {
    const std::size_t found = text.find(marker, offset);
    if (found == std::string::npos) {
      break;
    }
    ++count;
    offset = found + marker.size();
  }
  return count;
}

std::size_t CountTopLevelGenericArgumentSlots(
    const std::string &generic_suffix_text) {
  if (generic_suffix_text.size() < 2) {
    return 0;
  }
  std::size_t begin = 0;
  std::size_t end = generic_suffix_text.size();
  if (generic_suffix_text.front() == '<' &&
      generic_suffix_text.back() == '>') {
    begin = 1;
    end -= 1;
  }
  if (begin >= end) {
    return 0;
  }

  std::size_t slots = 1;
  std::size_t depth = 0;
  bool saw_non_whitespace = false;
  for (std::size_t i = begin; i < end; ++i) {
    const char c = generic_suffix_text[i];
    if (!std::isspace(static_cast<unsigned char>(c))) {
      saw_non_whitespace = true;
    }
    if (c == '<') {
      ++depth;
      continue;
    }
    if (c == '>') {
      if (depth > 0) {
        --depth;
      }
      continue;
    }
    if (c == ',' && depth == 0) {
      ++slots;
    }
  }

  return saw_non_whitespace ? slots : 0;
}

std::size_t CountNamespaceSegments(const std::string &name) {
  if (name.empty()) {
    return 0;
  }
  std::size_t segments = 1;
  for (char c : name) {
    if (c == '.') {
      ++segments;
    }
  }
  return segments;
}

std::string BuildLowercaseProfileToken(std::string token) {
  std::transform(token.begin(), token.end(), token.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });
  return token;
}

}  // namespace objc3c::parse
