#include "io/json/json_schema_errors.h"

#include <cstddef>
#include <sstream>

namespace objc3::io::json {

std::string JsonSchemaKeywordPath(const std::string &base,
                                  std::string_view keyword) {
  std::string path = base;
  path.push_back('.');
  path.append(keyword);
  return path;
}

std::string JsonSchemaArrayElementPath(const std::string &base,
                                       std::string_view keyword,
                                       std::size_t index) {
  std::ostringstream out;
  out << JsonSchemaKeywordPath(base, keyword) << '[' << index << ']';
  return out.str();
}

std::string JsonSchemaPropertySchemaPath(const std::string &base,
                                         std::string_view property) {
  std::string path = JsonSchemaKeywordPath(base, "properties");
  path.push_back('.');
  path.append(property);
  return path;
}

}  // namespace objc3::io::json
