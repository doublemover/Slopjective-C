#include "io/json/json_parser_cursor.h"

#include <string_view>

namespace objc3::io::json {

bool JsonParserCursor::ConsumeLiteral(std::string_view literal) {
  if (text_.substr(cursor_, literal.size()) != literal) {
    return false;
  }
  cursor_ += literal.size();
  return true;
}

}  // namespace objc3::io::json
