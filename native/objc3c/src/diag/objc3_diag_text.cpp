#include "diag/objc3_diag_text.h"

#include <algorithm>
#include <cctype>

bool StartsWith(std::string_view value, std::string_view prefix) {
  return value.size() >= prefix.size() &&
         value.compare(0u, prefix.size(), prefix) == 0;
}

std::string ToLower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char c) {
                   return static_cast<char>(std::tolower(c));
                 });
  return value;
}
