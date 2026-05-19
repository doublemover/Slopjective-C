#include "io/json/json_string_escape_classification.h"

namespace objc3::io::json {

JsonStringEscapeClassification ClassifyJsonStringEscape(unsigned char ch) {
  switch (ch) {
    case '"':
      return JsonStringEscapeClassification{"\\\"", false};
    case '\\':
      return JsonStringEscapeClassification{"\\\\", false};
    case '\b':
      return JsonStringEscapeClassification{"\\b", false};
    case '\f':
      return JsonStringEscapeClassification{"\\f", false};
    case '\n':
      return JsonStringEscapeClassification{"\\n", false};
    case '\r':
      return JsonStringEscapeClassification{"\\r", false};
    case '\t':
      return JsonStringEscapeClassification{"\\t", false};
    default:
      return JsonStringEscapeClassification{nullptr, ch < 0x20u};
  }
}

}  // namespace objc3::io::json
