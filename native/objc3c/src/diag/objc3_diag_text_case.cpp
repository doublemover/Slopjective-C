#include "diag/objc3_diag_text.h"

#include <algorithm>
#include <cctype>

std::string ToLower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char c) {
                   return static_cast<char>(std::tolower(c));
                 });
  return value;
}
