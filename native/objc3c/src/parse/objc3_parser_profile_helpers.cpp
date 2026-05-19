#include "parse/objc3_parser_profile_helpers.h"

#include "support/objc3_ascii_case_transform.h"

#include <cctype>
#include <sstream>

namespace objc3c::parse {

#include "parse/objc3_parser_profile_helpers_vector_types.inc"
#include "parse/objc3_parser_profile_helpers_diagnostics.inc"
#include "parse/objc3_parser_profile_helpers_counts.inc"

std::string BuildTypedKeyPathLiteralProfile(
    const std::string &root_name,
    bool root_is_self,
    const std::vector<std::string> &components) {
  std::ostringstream out;
  out << "typed-keypath:root=" << (root_is_self ? "self" : root_name)
      << ";components=";
  for (std::size_t index = 0; index < components.size(); ++index) {
    if (index != 0u) {
      out << ".";
    }
    out << components[index];
  }
  return out.str();
}

std::string BuildAutoreleasePoolScopeSymbol(unsigned serial, unsigned depth) {
  std::ostringstream out;
  out << "autoreleasepool-scope:" << serial << ";depth=" << depth;
  return out.str();
}

#include "parse/objc3_parser_profile_helpers_normalization.inc"

}  // namespace objc3c::parse
