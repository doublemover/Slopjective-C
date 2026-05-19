#include "io/json/json_writer.h"

#include <string_view>

#include "io/json/json_array_writer_value_emission.h"
#include "io/json/json_array_writer_value_validation.h"

namespace objc3::io::json {

void JsonArrayWriter::Value(const JsonValue &value) {
  BeginElement();
  WriteJsonArrayJsonValue(out_, value);
}

void JsonArrayWriter::RawJsonValue(std::string_view value) {
  JsonValue parsed = ParseJsonArrayRawValue(value);
  BeginElement();
  WriteJsonArrayJsonValue(out_, parsed);
}

}  // namespace objc3::io::json
