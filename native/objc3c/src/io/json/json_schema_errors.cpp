#include "io/json/json_schema_errors.h"

#include <cstddef>
#include <sstream>
#include <utility>

namespace objc3::io::json {

std::string JsonSchemaIssue::Format() const {
  std::ostringstream out;
  out << domain << "." << code << " instance=" << instance_path
      << " schema=" << schema_path << ": " << message;
  return out.str();
}

void AppendJsonSchemaIssue(JsonSchemaResult &result, JsonSchemaIssue issue) {
  result.errors.push_back(std::move(issue));
  result.ok = false;
}

void AddJsonSchemaPayloadError(JsonSchemaResult &result,
                               std::string code,
                               std::string instance_path,
                               std::string schema_path,
                               std::string message) {
  AppendJsonSchemaIssue(
      result, JsonSchemaIssue{"payload", std::move(code),
                              std::move(instance_path),
                              std::move(schema_path), std::move(message)});
}

void AddJsonSchemaContractError(JsonSchemaResult &result,
                                std::string code,
                                std::string schema_path,
                                std::string message) {
  AppendJsonSchemaIssue(
      result, JsonSchemaIssue{"schema", std::move(code), "$",
                              std::move(schema_path), std::move(message)});
}

bool HasJsonSchemaContractIssue(const JsonSchemaResult &result) {
  for (const JsonSchemaIssue &issue : result.errors) {
    if (issue.domain == "schema") {
      return true;
    }
  }
  return false;
}

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
