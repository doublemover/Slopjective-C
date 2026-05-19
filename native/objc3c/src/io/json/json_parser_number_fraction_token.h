#pragma once

#include <cstddef>
#include <optional>
#include <string_view>

#include "io/json/json_error.h"

namespace objc3::io::json {

bool ParseJsonNumberFractionToken(std::string_view text,
                                  std::size_t &cursor,
                                  std::optional<JsonError> &error);

}  // namespace objc3::io::json
