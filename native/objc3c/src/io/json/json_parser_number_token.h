#pragma once

#include <cstddef>
#include <optional>
#include <string_view>

#include "io/json/json_error.h"
#include "io/json/json_value.h"

namespace objc3::io::json {

bool ParseJsonNumberToken(std::string_view text,
                          std::size_t &cursor,
                          std::optional<JsonError> &error,
                          JsonValue &out);

}  // namespace objc3::io::json
