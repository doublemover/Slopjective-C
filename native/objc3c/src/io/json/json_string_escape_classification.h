#pragma once

namespace objc3::io::json {

struct JsonStringEscapeClassification {
  const char *short_escape = nullptr;
  bool unicode_control_escape = false;
};

JsonStringEscapeClassification ClassifyJsonStringEscape(unsigned char ch);

}  // namespace objc3::io::json
