#include "diagnostics/modes/objc3_removed_mode_options.h"

namespace objc3c::diagnostics::modes {

bool BuildRemovedModeOptionDiagnostic(const std::string &flag, std::string &diagnostic) {
  if (flag == "--objc3-compat-mode") {
    diagnostic =
        "unsupported hard-cutover option: --objc3-compat-mode was removed; Objective-C 3.0 is canonical-only";
    return true;
  }
  if (flag == "--objc3-canonical-rejection-diagnostics") {
    diagnostic =
        "unsupported hard-cutover option: --objc3-canonical-rejection-diagnostics was removed; legacy literal diagnostics is not an active compiler mode";
    return true;
  }
  return false;
}

}  // namespace objc3c::diagnostics::modes
