#include "io/json/json_object_writer_value_emission.h"

#include <ostream>

#include "io/json/json_writer.h"

namespace objc3::io::json {

void WriteJsonObjectStringArrayMemberValue(
    std::ostream &out,
    const std::vector<std::string> &values) {
  WriteJsonStringArray(out, values);
}

void WriteJsonObjectValueMemberValue(std::ostream &out,
                                     const JsonValue &value) {
  WriteJson(out, value);
}

}  // namespace objc3::io::json
