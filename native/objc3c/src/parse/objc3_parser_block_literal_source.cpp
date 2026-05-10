#include "parse/objc3_parser_block_literal_source.h"

#include <algorithm>
#include <sstream>
#include <unordered_set>

namespace objc3c::parse {
namespace {

std::vector<std::string> BuildSortedUniqueStrings(std::vector<std::string> values) {
  std::sort(values.begin(), values.end());
  values.erase(std::unique(values.begin(), values.end()), values.end());
  return values;
}

#include "parse/objc3_parser_block_literal_capture_identifiers.inc"

#include "parse/objc3_parser_block_literal_mutated_identifiers.inc"

}  // namespace

#include "parse/objc3_parser_block_literal_source_capture_sets.inc"

#include "parse/objc3_parser_block_literal_source_profiles.inc"

}  // namespace objc3c::parse
