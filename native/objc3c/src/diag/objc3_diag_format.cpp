#include "diag/objc3_diag_format.h"

#include <sstream>

std::string MakeDiag(unsigned line,
                     unsigned column,
                     const std::string &code,
                     const std::string &message) {
  std::ostringstream out;
  out << "error:" << line << ":" << column << ": " << message << " ["
      << code << "]";
  return out.str();
}
