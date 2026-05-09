#include "diag/objc3_diag_parse.h"

#include <limits>

bool TryParseUnsignedSegment(std::string_view text,
                             std::size_t begin,
                             std::size_t delimiter_offset,
                             unsigned &value) {
  if (delimiter_offset <= begin || delimiter_offset > text.size()) {
    return false;
  }
  unsigned parsed = 0;
  for (std::size_t i = begin; i < delimiter_offset; ++i) {
    const char c = text[i];
    if (c < '0' || c > '9') {
      return false;
    }
    const unsigned digit = static_cast<unsigned>(c - '0');
    if (parsed > (std::numeric_limits<unsigned>::max() - digit) / 10u) {
      return false;
    }
    parsed = parsed * 10u + digit;
  }
  value = parsed;
  return true;
}
