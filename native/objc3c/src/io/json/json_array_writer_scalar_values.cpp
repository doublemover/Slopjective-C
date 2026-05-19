#include "io/json/json_writer.h"

#include <string_view>

#include "io/json/json_array_writer_value_emission.h"
#include "io/json/json_array_writer_value_validation.h"

namespace objc3::io::json {

void JsonArrayWriter::StringValue(std::string_view value) {
  BeginElement();
  WriteJsonArrayStringValue(out_, value);
}

void JsonArrayWriter::BoolValue(bool value) {
  BeginElement();
  WriteJsonArrayBoolValue(out_, value);
}

void JsonArrayWriter::IntValue(std::int64_t value) {
  BeginElement();
  WriteJsonArrayIntValue(out_, value);
}

void JsonArrayWriter::NumberValue(double value) {
  ValidateJsonArrayNumberValue(value);
  BeginElement();
  WriteJsonArrayNumberValue(out_, value);
}

void JsonArrayWriter::SizeValue(std::size_t value) {
  BeginElement();
  WriteJsonArraySizeValue(out_, value);
}

void JsonArrayWriter::UnsignedValue(std::uint64_t value) {
  BeginElement();
  WriteJsonArrayUnsignedValue(out_, value);
}

}  // namespace objc3::io::json
