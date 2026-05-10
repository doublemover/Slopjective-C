#include "io/json/json_parser_cursor_error.h"

#include <utility>

namespace objc3::io::json {

bool ReportJsonParserCursorError(std::optional<JsonError> &error,
                                 std::size_t cursor,
                                 std::string message) {
  error = JsonError{std::move(message), cursor};
  return false;
}

}  // namespace objc3::io::json
