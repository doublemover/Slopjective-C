#include "io/json/json_parser_value_dispatch.h"

#include "io/json/json_parser_array_container.h"
#include "io/json/json_parser_cursor.h"
#include "io/json/json_parser_object_container.h"
#include "io/json/json_parser_scalar_value.h"
#include "io/json/json_parser_value_delegate.h"

namespace objc3::io::json {

bool DispatchJsonParserValue(JsonParserCursor &cursor,
                             JsonParserValueDelegate &delegate,
                             JsonValue &out) {
  cursor.SkipWhitespace();
  if (cursor.AtEnd()) {
    return cursor.Fail("unexpected end of JSON input");
  }
  const char ch = cursor.Peek();
  if (ch == '{') {
    return ParseJsonObjectContainer(cursor, delegate, out);
  }
  if (ch == '[') {
    return ParseJsonArrayContainer(cursor, delegate, out);
  }
  return ParseJsonScalarValue(cursor, out);
}

}  // namespace objc3::io::json
