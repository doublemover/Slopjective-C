#include "io/json/json_parser_document.h"

#include <utility>

#include "io/json/json_parser_completion.h"
#include "io/json/json_parser_cursor.h"
#include "io/json/json_parser_value_dispatch.h"
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
    return DispatchJsonParserValue(cursor_, *this, out);
  }

  JsonParserCursor cursor_;
};

}  // namespace

JsonParseResult ParseJsonDocument(std::string_view text) {
  return Parser(text).Parse();
}

}  // namespace objc3::io::json
