#pragma once

#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>

namespace objc3c::support {

inline std::string LowercaseAscii(std::string_view value) {
  std::string lowered(value.begin(), value.end());
  std::transform(lowered.begin(), lowered.end(), lowered.begin(),
                 [](unsigned char c) {
                   return static_cast<char>(std::tolower(c));
                 });
  return lowered;
}

inline bool StartsWith(std::string_view value, std::string_view prefix) {
  return value.size() >= prefix.size() &&
         value.compare(0, prefix.size(), prefix) == 0;
}

inline bool EndsWith(std::string_view value, std::string_view suffix) {
  return value.size() >= suffix.size() &&
         value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

}  // namespace objc3c::support
