#pragma once

#include "io/json/json_value.h"

namespace objc3::io::json {

class JsonParserValueDelegate {
 public:
  virtual ~JsonParserValueDelegate() = default;

  virtual bool ParseValue(JsonValue &out) = 0;
};

}  // namespace objc3::io::json
