#pragma once

#include <string>
#include <string_view>

#include "io/json/json_value.h"

namespace objc3::io::json {

std::string JsonSchemaValueKindName(JsonValue::Kind kind);
bool JsonSchemaTypeNameMatchesPayload(std::string_view type,
                                      const JsonValue &payload);

}  // namespace objc3::io::json
