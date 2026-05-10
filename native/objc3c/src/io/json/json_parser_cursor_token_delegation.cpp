#include "io/json/json_parser_cursor.h"

#include <string>

#include "io/json/json_parser_number_token.h"
#include "io/json/json_parser_string_token.h"

namespace objc3::io::json {

bool JsonParserCursor::ParseString(std::string &out) {
  return ParseJsonStringToken(text_, cursor_, error_, out);
}

bool JsonParserCursor::ParseNumber(JsonValue &out) {
  return ParseJsonNumberToken(text_, cursor_, error_, out);
}

}  // namespace objc3::io::json
