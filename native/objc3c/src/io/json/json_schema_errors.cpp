#include "io/json/json_schema_errors.h"

#include <utility>

namespace objc3::io::json {

void AddJsonSchemaError(JsonSchemaResult &result, std::string message) {
  result.errors.push_back(std::move(message));
  result.ok = false;
}

}  // namespace objc3::io::json

