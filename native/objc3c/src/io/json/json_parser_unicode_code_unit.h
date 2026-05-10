#pragma once

#include <cstddef>
#include <optional>
#include <string_view>

#include "io/json/json_error.h"

namespace objc3::io::json {

bool ParseJsonUnicodeCodeUnit(std::string_view text,
                              std::size_t &cursor,
                              std::optional<JsonError> &error,
                              unsigned &codepoint);

}  // namespace objc3::io::json
