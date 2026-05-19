#pragma once

#include <cstddef>
#include <optional>
#include <string>

#include "io/json/json_error.h"

namespace objc3::io::json {

bool ReportJsonParserCursorError(std::optional<JsonError> &error,
                                 std::size_t cursor,
                                 std::string message);

}  // namespace objc3::io::json
