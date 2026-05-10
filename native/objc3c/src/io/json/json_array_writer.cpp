#include "io/json/json_writer.h"

#include <cmath>
#include <iomanip>
#include <stdexcept>
#include <string_view>

#include "io/json/json_parser.h"
#include "io/objc3_json.h"

namespace objc3::io::json {

void JsonArrayWriter::StringValue(std::string_view value) {
  BeginElement();
  objc3::io::WriteJsonString(out_, value);
}

void JsonArrayWriter::BoolValue(bool value) {
  BeginElement();
  out_ << (value ? "true" : "false");
}

void JsonArrayWriter::IntValue(std::int64_t value) {
  BeginElement();
  out_ << value;
}

void JsonArrayWriter::NumberValue(double value) {
  if (!std::isfinite(value)) {
    throw std::invalid_argument("NumberValue received a non-finite number");
  }
  BeginElement();
  out_ << std::setprecision(17) << value;
}

void JsonArrayWriter::SizeValue(std::size_t value) {
  BeginElement();
  out_ << value;
}

void JsonArrayWriter::UnsignedValue(std::uint64_t value) {
  BeginElement();
  out_ << value;
}

void JsonArrayWriter::Value(const JsonValue &value) {
  BeginElement();
  WriteJson(out_, value);
}

void JsonArrayWriter::RawJsonValue(std::string_view value) {
  JsonParseResult parsed = ParseJson(value);
  if (!parsed.ok()) {
    throw std::invalid_argument("RawJsonValue received invalid JSON: " +
                                parsed.error->Format());
  }
  Value(parsed.value);
}

}  // namespace objc3::io::json
