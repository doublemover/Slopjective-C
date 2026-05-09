#pragma once

#include <string>
#include <vector>

#include "io/json/json_value.h"

namespace objc3::io::json {

struct JsonSchemaIssue {
  std::string domain;
  std::string code;
  std::string instance_path;
  std::string schema_path;
  std::string message;

  [[nodiscard]] std::string Format() const;
};

struct JsonSchemaResult {
  bool ok = true;
  std::vector<JsonSchemaIssue> errors;
};

JsonSchemaResult ValidateJsonSchema(const JsonValue &schema, const JsonValue &payload);

}  // namespace objc3::io::json
