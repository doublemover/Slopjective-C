#include "parse/objc3_parser_interop_profiles.h"

#include <algorithm>
#include <sstream>

namespace objc3c::parse {

std::string BuildProtocolQualifiedObjectTypeProfile(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated,
    bool has_pointer_declarator,
    const std::string &generic_suffix_text) {
  const bool protocol_composition_valid =
      !has_generic_suffix || (generic_suffix_terminated && object_pointer_type_spelling);
  std::ostringstream out;
  out << "protocol-qualified-object-type:object-pointer="
      << (object_pointer_type_spelling ? "true" : "false")
      << ";has-protocol-composition=" << (has_generic_suffix ? "true" : "false")
      << ";terminated=" << (generic_suffix_terminated ? "true" : "false")
      << ";pointer-declarator=" << (has_pointer_declarator ? "true" : "false")
      << ";composition-bytes=" << generic_suffix_text.size()
      << ";composition-valid=" << (protocol_composition_valid ? "true" : "false");
  return out.str();
}

bool IsProtocolQualifiedObjectTypeProfileNormalized(
    bool object_pointer_type_spelling,
    bool has_generic_suffix,
    bool generic_suffix_terminated) {
  if (!has_generic_suffix) {
    return true;
  }
  return generic_suffix_terminated && object_pointer_type_spelling;
}

#include "parse/objc3_parser_interop_ownership_profiles.inc"

std::vector<std::string> BuildSortedUniqueStrings(
    std::vector<std::string> values) {
  std::sort(values.begin(), values.end());
  values.erase(std::unique(values.begin(), values.end()), values.end());
  return values;
}

bool IsSortedUniqueStrings(const std::vector<std::string> &values) {
  return std::adjacent_find(values.begin(), values.end()) == values.end() &&
         std::is_sorted(values.begin(), values.end());
}

}  // namespace objc3c::parse
