#include "io/json/json_parser.h"

#include "io/json/json_parser_document.h"

namespace objc3::io::json {

JsonParseResult ParseJson(std::string_view text) {
  return ParseJsonDocument(text);
}

}  // namespace objc3::io::json
