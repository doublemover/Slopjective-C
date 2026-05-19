#pragma once

#include <string>

namespace objc3c::diagnostics::modes {

struct ModeOptionDiagnostic {
  bool matched = false;
  std::string flag;
  std::string diagnostic_code;
  std::string message;
};

}  // namespace objc3c::diagnostics::modes
