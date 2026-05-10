#pragma once

#include <cstddef>
#include <optional>

#include "io/json/json_error.h"

namespace objc3::io::json {

bool FailUnterminatedJsonStringEscape(std::optional<JsonError> &error,
                                      std::size_t cursor);
bool FailInvalidJsonStringEscape(std::optional<JsonError> &error,
                                 std::size_t cursor);

}  // namespace objc3::io::json
