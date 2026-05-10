#pragma once

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string_view>

#include "io/json/json_value.h"

namespace objc3::io::json {

void WriteJsonArrayStringValue(std::ostream &out, std::string_view value);
void WriteJsonArrayBoolValue(std::ostream &out, bool value);
void WriteJsonArrayIntValue(std::ostream &out, std::int64_t value);
void WriteJsonArrayNumberValue(std::ostream &out, double value);
void WriteJsonArraySizeValue(std::ostream &out, std::size_t value);
void WriteJsonArrayUnsignedValue(std::ostream &out, std::uint64_t value);
void WriteJsonArrayJsonValue(std::ostream &out, const JsonValue &value);

}  // namespace objc3::io::json
