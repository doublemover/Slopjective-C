#include "io/objc3_line_join.h"

#include <sstream>

namespace objc3::io {

std::string JoinLinesWithTrailingNewline(const std::vector<std::string> &lines) {
  std::ostringstream out;
  for (const auto &line : lines) {
    out << line << "\n";
  }
  return out.str();
}

}  // namespace objc3::io
