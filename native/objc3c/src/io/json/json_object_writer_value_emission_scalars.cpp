#include "io/json/json_object_writer_value_emission.h"

#include <iomanip>
#include <ostream>

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

}  // namespace objc3::io::json
