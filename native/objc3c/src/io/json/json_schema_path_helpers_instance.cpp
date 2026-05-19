#include "io/json/json_schema_errors.h"

#include <cstddef>
#include <sstream>

namespace objc3::io::json {

std::string JsonInstancePropertyPath(const std::string &base,
                                     std::string_view property) {
  std::string path = base;
  path.push_back('.');
  path.append(property);
  return path;
}

std::string JsonInstanceArrayElementPath(const std::string &base,
                                         std::size_t index) {
  std::ostringstream out;
  out << base << '[' << index << ']';
  return out.str();
}

}  // namespace objc3::io::json
