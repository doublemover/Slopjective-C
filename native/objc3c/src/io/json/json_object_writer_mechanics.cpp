#include "io/json/json_writer.h"

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

void JsonObjectWriter::End() {
  if (ended_) {
    return;
  }
  out_ << '}';
  ended_ = true;
}

}  // namespace objc3::io::json
