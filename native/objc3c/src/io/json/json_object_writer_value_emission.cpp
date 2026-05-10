#include "io/json/json_object_writer_value_emission.h"

#include <cmath>
#include <iomanip>
#include <ostream>
#include <stdexcept>

#include "io/json/json_parser.h"
#include "io/json/json_writer.h"
#include "io/objc3_json.h"

namespace objc3::io::json {

void WriteJsonObjectStringMemberValue(std::ostream &out,
                                      std::string_view value) {
  objc3::io::WriteJsonString(out, value);
}

void WriteJsonObjectBoolMemberValue(std::ostream &out, bool value) {
  out << (value ? "true" : "false");
}

void WriteJsonObjectIntMemberValue(std::ostream &out, std::int64_t value) {
  out << value;
}

void ValidateJsonObjectNumberMemberValue(double value) {
  if (!std::isfinite(value)) {
    throw std::invalid_argument("NumberField received a non-finite number");
  }
}

void WriteJsonObjectNumberMemberValue(std::ostream &out, double value) {
  out << std::setprecision(17) << value;
}

void WriteJsonObjectSizeMemberValue(std::ostream &out, std::size_t value) {
  out << value;
}

void WriteJsonObjectUnsignedMemberValue(std::ostream &out,
                                        std::uint64_t value) {
  out << value;
}

void WriteJsonObjectStringArrayMemberValue(
    std::ostream &out,
    const std::vector<std::string> &values) {
  WriteJsonStringArray(out, values);
}

void WriteJsonObjectValueMemberValue(std::ostream &out,
                                     const JsonValue &value) {
  WriteJson(out, value);
}

JsonValue ParseJsonObjectRawMemberValue(std::string_view value) {
  JsonParseResult parsed = ParseJson(value);
  if (!parsed.ok()) {
    throw std::invalid_argument("RawJsonField received invalid JSON: " +
                                parsed.error->Format());
  }
  return parsed.value;
}

}  // namespace objc3::io::json
