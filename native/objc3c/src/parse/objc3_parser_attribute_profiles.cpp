#include "parse/objc3_parser_attribute_profiles.h"

#include <sstream>

namespace objc3c::parse {

std::string BuildCleanupAttributeProfile(
    bool declared,
    const std::string &cleanup_symbol) {
  std::ostringstream out;
  out << "cleanup:declared=" << (declared ? "true" : "false")
      << ";symbol=" << cleanup_symbol;
  return out.str();
}

std::string BuildResourceAttributeProfile(
    bool declared,
    const std::string &close_symbol,
    const std::string &invalid_expression) {
  std::ostringstream out;
  out << "resource:declared=" << (declared ? "true" : "false")
      << ";close=" << close_symbol
      << ";invalid=" << invalid_expression;
  return out.str();
}

std::string BuildRetainableCFamilyCallableProfile(
    const std::vector<std::string> &attribute_names,
    const std::vector<std::string> &family_names) {
  std::ostringstream out;
  out << "retainable-c-family:attrs=";
  for (std::size_t index = 0; index < attribute_names.size(); ++index) {
    if (index != 0u) {
      out << ",";
    }
    out << attribute_names[index];
  }
  out << ";families=";
  for (std::size_t index = 0; index < family_names.size(); ++index) {
    if (index != 0u) {
      out << ",";
    }
    out << family_names[index];
  }
  return out.str();
}

}  // namespace objc3c::parse
