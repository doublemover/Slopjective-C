#pragma once

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>

#include "io/json/json_value.h"

namespace objc3::io::json {

void WriteJsonObjectStringMemberValue(std::ostream &out,
                                      std::string_view value);
void WriteJsonObjectBoolMemberValue(std::ostream &out, bool value);
void WriteJsonObjectIntMemberValue(std::ostream &out, std::int64_t value);
void WriteJsonObjectNumberMemberValue(std::ostream &out, double value);
void WriteJsonObjectSizeMemberValue(std::ostream &out, std::size_t value);
void WriteJsonObjectUnsignedMemberValue(std::ostream &out,
                                        std::uint64_t value);
void WriteJsonObjectStringArrayMemberValue(
    std::ostream &out,
    const std::vector<std::string> &values);
void WriteJsonObjectValueMemberValue(std::ostream &out,
                                     const JsonValue &value);

}  // namespace objc3::io::json
