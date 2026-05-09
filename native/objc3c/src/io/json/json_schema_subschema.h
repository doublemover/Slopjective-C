#pragma once

#include <string>

#include "io/json/json_value.h"

namespace objc3::io::json {

bool JsonSubschemaPasses(const JsonValue &schema_root,
                         const JsonValue &schema,
                         const JsonValue &payload,
                         const std::string &instance_path,
                         const std::string &schema_path);

}  // namespace objc3::io::json
