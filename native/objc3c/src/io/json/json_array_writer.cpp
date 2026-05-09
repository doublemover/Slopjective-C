#include "io/json/json_writer.h"

#include <iomanip>
#include <string_view>

#include "io/objc3_json.h"

namespace objc3::io::json {

JsonArrayWriter::JsonArrayWriter(std::ostream &out) : out_(out) {
  out_ << '[';
}

void JsonArrayWriter::BeginElement() {
  if (!first_) {
    out_ << ',';
  }
  first_ = false;
}

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
  BeginElement();
  out_ << value;
}

void JsonArrayWriter::End() {
  if (ended_) {
    return;
  }
  out_ << ']';
  ended_ = true;
}

}  // namespace objc3::io::json

