#include "io/json/json_error.h"

#include <sstream>

namespace objc3::io::json {

std::string JsonError::Format() const {
  std::ostringstream out;
  out << message << " at byte " << offset;
  return out.str();
}

}  // namespace objc3::io::json
