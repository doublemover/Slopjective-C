#include "support/objc3_ascii_case_transform.h"

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

}  // namespace objc3c::support
