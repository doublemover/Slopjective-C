#include "ir/objc3_ir_c_string.h"

#include <iomanip>
#include <sstream>

std::string EscapeCStringLiteral(const std::string &text) {
  std::ostringstream out;
  for (unsigned char c : text) {
    std::ostringstream byte;
    byte << std::hex << std::uppercase << static_cast<int>(c);
    std::string value = byte.str();
    if (value.size() < 2) {
      value = "0" + value;
    }
    if (c == '\\' || c == '"' || c < 32 || c > 126) {
      out << "\\" << value;
      continue;
    }
    out << static_cast<char>(c);
  }
  return out.str();
}
