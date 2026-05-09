#include "support/objc3_string_predicates.h"

#include <algorithm>
#include <cctype>

namespace objc3c::support {

std::string LowercaseAscii(std::string_view value) {
  std::string lowered(value.begin(), value.end());
  std::transform(lowered.begin(), lowered.end(), lowered.begin(),
                 [](unsigned char c) {
                   return static_cast<char>(std::tolower(c));
                 });
  return lowered;
}

bool StartsWith(std::string_view value, std::string_view prefix) {
  return value.size() >= prefix.size() &&
         value.compare(0, prefix.size(), prefix) == 0;
}

bool EndsWith(std::string_view value, std::string_view suffix) {
  return value.size() >= suffix.size() &&
         value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0;
}

}  // namespace objc3c::support
