#include "io/json/json_writer.h"

#include <iomanip>
#include <sstream>
#include <string_view>

#include "io/objc3_json.h"

namespace objc3::io::json {
namespace {

void WriteValue(std::ostream &out, const JsonValue &value) {
  switch (value.kind()) {
    case JsonValue::Kind::kNull:
      out << "null";
      return;
    case JsonValue::Kind::kBool:
      out << (value.AsBool() ? "true" : "false");
      return;
    case JsonValue::Kind::kNumber:
      out << std::setprecision(17) << value.AsNumber();
      return;
    case JsonValue::Kind::kString:
      objc3::io::WriteJsonString(out, value.AsString());
      return;
    case JsonValue::Kind::kArray: {
      out << '[';
      const auto &array = value.AsArray();
      for (std::size_t i = 0; i < array.size(); ++i) {
        if (i > 0) {
          out << ',';
        }
        WriteValue(out, array[i]);
      }
      out << ']';
      return;
    }
    case JsonValue::Kind::kObject: {
      out << '{';
      bool first = true;
      for (const auto &[key, item] : value.AsObject()) {
        if (!first) {
          out << ',';
        }
        first = false;
        objc3::io::WriteJsonString(out, key);
        out << ':';
        WriteValue(out, item);
      }
      out << '}';
      return;
    }
  }
}

}  // namespace

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

void JsonObjectWriter::StringField(std::string_view name, std::string_view value) {
  BeginField(name);
  objc3::io::WriteJsonString(out_, value);
}

void JsonObjectWriter::BoolField(std::string_view name, bool value) {
  BeginField(name);
  out_ << (value ? "true" : "false");
}

void JsonObjectWriter::NumberField(std::string_view name, double value) {
  BeginField(name);
  out_ << std::setprecision(17) << value;
}

void JsonObjectWriter::UnsignedField(std::string_view name, std::uint64_t value) {
  BeginField(name);
  out_ << value;
}

void JsonObjectWriter::RawJsonField(std::string_view name, std::string_view value) {
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

void WriteJson(std::ostream &out, const JsonValue &value) {
  WriteValue(out, value);
}

std::string RenderJson(const JsonValue &value) {
  std::ostringstream out;
  WriteJson(out, value);
  return out.str();
}

}  // namespace objc3::io::json
