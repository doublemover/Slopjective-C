#pragma once

#include <cstddef>
#include <string>
#include <string_view>

#include "io/json/json_schema.h"

namespace objc3::io::json {

void AddJsonSchemaPayloadError(JsonSchemaResult &result,
                               std::string code,
                               std::string instance_path,
                               std::string schema_path,
                               std::string message);
void AddJsonSchemaContractError(JsonSchemaResult &result,
                                std::string code,
                                std::string schema_path,
                                std::string message);
void AppendJsonSchemaIssue(JsonSchemaResult &result, JsonSchemaIssue issue);

[[nodiscard]] bool HasJsonSchemaContractIssue(
    const JsonSchemaResult &result);
[[nodiscard]] std::string JsonSchemaKeywordPath(
    const std::string &base,
    std::string_view keyword);
[[nodiscard]] std::string JsonSchemaArrayElementPath(
    const std::string &base,
    std::string_view keyword,
    std::size_t index);
[[nodiscard]] std::string JsonSchemaPropertySchemaPath(
    const std::string &base,
    std::string_view property);
[[nodiscard]] std::string JsonInstancePropertyPath(
    const std::string &base,
    std::string_view property);
[[nodiscard]] std::string JsonInstanceArrayElementPath(
    const std::string &base,
    std::size_t index);

}  // namespace objc3::io::json
