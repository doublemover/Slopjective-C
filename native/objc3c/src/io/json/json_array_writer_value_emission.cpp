#include "io/json/json_array_writer_value_emission.h"

#include <iomanip>
#include <ostream>

#include "io/json/json_writer.h"
#include "io/objc3_json.h"

namespace objc3::io::json {

void WriteJsonArrayStringValue(std::ostream &out, std::string_view value) {
  objc3::io::WriteJsonString(out, value);
}

void WriteJsonArrayBoolValue(std::ostream &out, bool value) {
  out << (value ? "true" : "false");
}

void WriteJsonArrayIntValue(std::ostream &out, std::int64_t value) {
  out << value;
}

void WriteJsonArrayNumberValue(std::ostream &out, double value) {
  out << std::setprecision(17) << value;
}

void WriteJsonArraySizeValue(std::ostream &out, std::size_t value) {
  out << value;
}

void WriteJsonArrayUnsignedValue(std::ostream &out, std::uint64_t value) {
  out << value;
}

void WriteJsonArrayJsonValue(std::ostream &out, const JsonValue &value) {
  WriteJson(out, value);
}

}  // namespace objc3::io::json
