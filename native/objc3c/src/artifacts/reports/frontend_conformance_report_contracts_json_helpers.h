#pragma once

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "io/objc3_json.h"

namespace objc3::artifacts::reports::detail {

inline std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  std::ostringstream out;
  out << "[";
  for (std::size_t index = 0; index < values.size(); ++index) {
    if (index > 0u) {
      out << ",";
    }
    out << "\"" << objc3::io::EscapeJsonString(values[index]) << "\"";
  }
  out << "]";
  return out.str();
}

}  // namespace objc3::artifacts::reports::detail
