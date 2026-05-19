#include "io/json/json_writer.h"

#include <string>
#include <string_view>
#include <vector>

#include "io/json/json_object_writer_value_emission.h"
#include "io/json/json_object_writer_value_validation.h"

namespace objc3::io::json {

void JsonObjectWriter::StringArrayField(
    std::string_view name,
    const std::vector<std::string> &values) {
  BeginField(name);
  WriteJsonObjectStringArrayMemberValue(out_, values);
}

void JsonObjectWriter::ValueField(std::string_view name,
                                  const JsonValue &value) {
  BeginField(name);
  WriteJsonObjectValueMemberValue(out_, value);
}

void JsonObjectWriter::RawJsonField(std::string_view name,
                                    std::string_view value) {
  JsonValue parsed = ParseJsonObjectRawMemberValue(value);
  BeginField(name);
  WriteJsonObjectValueMemberValue(out_, parsed);
}

}  // namespace objc3::io::json
