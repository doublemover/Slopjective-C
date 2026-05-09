#include "io/json/json_writer.h"

#include <iomanip>
#include <string_view>

#include "io/objc3_json.h"

namespace objc3::io::json {

JsonObjectWriter::JsonObjectWriter(std::ostream &out) : out_(out) {
  out_ << '{';
}

void JsonObjectWriter::BeginField(std::string_view name) {
  if (!first_) {
    out_ << ',';
  }
  first_ = false;
  objc3::io::WriteJsonString(out_, name);
  out_ << ':';
}

void JsonObjectWriter::StringField(std::string_view name,
                                   std::string_view value) {
  BeginField(name);
  objc3::io::WriteJsonString(out_, value);
}

void JsonObjectWriter::BoolField(std::string_view name, bool value) {
  BeginField(name);
  out_ << (value ? "true" : "false");
}

void JsonObjectWriter::IntField(std::string_view name, std::int64_t value) {
  BeginField(name);
  out_ << value;
}

void JsonObjectWriter::NumberField(std::string_view name, double value) {
  BeginField(name);
  out_ << std::setprecision(17) << value;
}

void JsonObjectWriter::SizeField(std::string_view name, std::size_t value) {
  BeginField(name);
  out_ << value;
}

void JsonObjectWriter::UnsignedField(std::string_view name,
                                     std::uint64_t value) {
  BeginField(name);
  out_ << value;
}

void JsonObjectWriter::StringArrayField(
    std::string_view name,
    const std::vector<std::string> &values) {
  BeginField(name);
  WriteJsonStringArray(out_, values);
}

void JsonObjectWriter::ValueField(std::string_view name,
                                  const JsonValue &value) {
  BeginField(name);
  WriteJson(out_, value);
}

void JsonObjectWriter::RawJsonField(std::string_view name,
                                    std::string_view value) {
  BeginField(name);
  out_ << value;
}

void JsonObjectWriter::End() {
  if (ended_) {
    return;
  }
  out_ << '}';
  ended_ = true;
}

}  // namespace objc3::io::json

