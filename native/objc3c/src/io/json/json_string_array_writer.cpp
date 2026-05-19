#include "io/json/json_writer.h"

#include <sstream>

namespace objc3::io::json {

void WriteJsonStringArray(std::ostream &out,
                          const std::vector<std::string> &values) {
  JsonArrayWriter array(out);
  for (const std::string &value : values) {
    array.StringValue(value);
  }
  array.End();
}

std::string RenderJsonStringArray(const std::vector<std::string> &values) {
  std::ostringstream out;
  WriteJsonStringArray(out, values);
  return out.str();
}

}  // namespace objc3::io::json
