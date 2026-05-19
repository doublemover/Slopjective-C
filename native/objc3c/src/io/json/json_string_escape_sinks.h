#pragma once

#include <iosfwd>
#include <string>

#include "io/json/json_string_escape_emission.h"

namespace objc3::io::json {

JsonStringEscapeEmitter MakeJsonStringAppendEscapeEmitter(std::string &out);
JsonStringEscapeEmitter MakeJsonStringStreamEscapeEmitter(std::ostream &out);

}  // namespace objc3::io::json
