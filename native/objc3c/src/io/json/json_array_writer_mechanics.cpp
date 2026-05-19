#include "io/json/json_writer.h"

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

void JsonArrayWriter::End() {
  if (ended_) {
    return;
  }
  out_ << ']';
  ended_ = true;
}

}  // namespace objc3::io::json
