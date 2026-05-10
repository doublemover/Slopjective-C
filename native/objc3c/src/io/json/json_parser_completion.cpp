#include "io/json/json_parser_completion.h"

#include <optional>
#include <utility>

namespace objc3::io::json {

JsonParseResult CompleteJsonParse(JsonParserCursor &cursor, JsonValue value) {
  cursor.SkipWhitespace();
  if (!cursor.AtEnd()) {
    cursor.Fail("unexpected trailing JSON content");
    return {JsonValue::Null(), cursor.error()};
  }
  return {std::move(value), std::nullopt};
}

}  // namespace objc3::io::json
