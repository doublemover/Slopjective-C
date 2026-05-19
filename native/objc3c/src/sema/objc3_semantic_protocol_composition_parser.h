#pragma once

#include <string>
#include <vector>

struct ProtocolCompositionParseResult {
  bool has_protocol_composition = false;
  bool malformed_composition = false;
  bool empty_composition = false;
  std::vector<std::string> names_lexicographic;
  std::vector<std::string> invalid_identifiers;
  std::vector<std::string> duplicate_identifiers;

  bool IsValid() const;
};

ProtocolCompositionParseResult ParseProtocolCompositionSuffixText(
    const std::string &suffix_text);
