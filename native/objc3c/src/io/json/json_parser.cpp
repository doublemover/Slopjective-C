#include "io/json/json_parser.h"

#include <utility>

#include "io/json/json_parser_array_container.h"
#include "io/json/json_parser_completion.h"
#include "io/json/json_parser_cursor.h"
#include "io/json/json_parser_object_container.h"
#include "io/json/json_parser_scalar_value.h"
#include "io/json/json_parser_value_delegate.h"

namespace objc3::io::json {
namespace {

class Parser : public JsonParserValueDelegate {
 public:
  explicit Parser(std::string_view text) : cursor_(text) {}

  JsonParseResult Parse() {
    JsonValue value;
    if (!ParseValue(value)) {
      return {JsonValue::Null(), cursor_.error()};
    }
    return CompleteJsonParse(cursor_, std::move(value));
  }

 private:
  bool ParseValue(JsonValue &out) override {
    cursor_.SkipWhitespace();
    if (cursor_.AtEnd()) {
      return cursor_.Fail("unexpected end of JSON input");
    }
    const char ch = cursor_.Peek();
    if (ch == '{') {
      return ParseJsonObjectContainer(cursor_, *this, out);
    }
    if (ch == '[') {
      return ParseJsonArrayContainer(cursor_, *this, out);
    }
    return ParseJsonScalarValue(cursor_, out);
  }

  JsonParserCursor cursor_;
};

}  // namespace

JsonParseResult ParseJson(std::string_view text) {
  return Parser(text).Parse();
}

}  // namespace objc3::io::json
