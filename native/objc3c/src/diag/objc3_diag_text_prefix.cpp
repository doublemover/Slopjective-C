#include "diag/objc3_diag_text.h"

bool StartsWith(std::string_view value, std::string_view prefix) {
  return value.size() >= prefix.size() &&
         value.compare(0u, prefix.size(), prefix) == 0;
}
