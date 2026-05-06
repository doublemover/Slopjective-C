#pragma once

#include <optional>
#include <string_view>

#include "io/json/json_error.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

struct JsonParseResult {
  JsonValue value;
  std::optional<JsonError> error;

  [[nodiscard]] bool ok() const { return !error.has_value(); }
};

JsonParseResult ParseJson(std::string_view text);

}  // namespace objc3::io::json
