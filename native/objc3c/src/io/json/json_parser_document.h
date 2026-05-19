#pragma once

#include <string_view>

#include "io/json/json_parser.h"

namespace objc3::io::json {

JsonParseResult ParseJsonDocument(std::string_view text);

}  // namespace objc3::io::json
