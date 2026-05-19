#include "io/json/json_writer.h"

#include <sstream>

#include "io/json/json_value_writer.h"

namespace objc3::io::json {

void WriteJson(std::ostream &out, const JsonValue &value) {
  WriteJsonValue(out, value);
}

std::string RenderJson(const JsonValue &value) {
  std::ostringstream out;
  WriteJson(out, value);
  return out.str();
}

}  // namespace objc3::io::json
