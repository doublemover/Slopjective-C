#include "io/json/json_writer.h"

#include <string_view>

#include "io/json/json_object_writer_value_emission.h"
#include "io/json/json_object_writer_value_validation.h"

namespace objc3::io::json {

void JsonObjectWriter::StringField(std::string_view name,
                                   std::string_view value) {
  BeginField(name);
  WriteJsonObjectStringMemberValue(out_, value);
}

void JsonObjectWriter::BoolField(std::string_view name, bool value) {
  BeginField(name);
  WriteJsonObjectBoolMemberValue(out_, value);
}

void JsonObjectWriter::IntField(std::string_view name, std::int64_t value) {
  BeginField(name);
  WriteJsonObjectIntMemberValue(out_, value);
}

void JsonObjectWriter::NumberField(std::string_view name, double value) {
  ValidateJsonObjectNumberMemberValue(value);
  BeginField(name);
  WriteJsonObjectNumberMemberValue(out_, value);
}

void JsonObjectWriter::SizeField(std::string_view name, std::size_t value) {
  BeginField(name);
  WriteJsonObjectSizeMemberValue(out_, value);
}

void JsonObjectWriter::UnsignedField(std::string_view name,
                                     std::uint64_t value) {
  BeginField(name);
  WriteJsonObjectUnsignedMemberValue(out_, value);
}

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
