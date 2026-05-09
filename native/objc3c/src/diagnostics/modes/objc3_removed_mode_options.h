#pragma once

#include <string>
#include <string_view>

namespace objc3c::diagnostics::modes {

struct RemovedModeOptionDiagnostic {
  bool matched = false;
  std::string flag;
  std::string diagnostic_code;
  std::string message;
};

RemovedModeOptionDiagnostic ClassifyRemovedModeOption(std::string_view flag);
bool BuildRemovedModeOptionDiagnostic(const std::string &flag, std::string &diagnostic);

}  // namespace objc3c::diagnostics::modes
