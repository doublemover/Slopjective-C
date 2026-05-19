#include "diagnostics/modes/objc3_removed_mode_options.h"

namespace objc3c::diagnostics::modes {

bool BuildRemovedModeOptionDiagnostic(const std::string &flag,
                                      std::string &diagnostic) {
  const RemovedModeOptionDiagnostic removed =
      ClassifyRemovedModeOption(flag);
  if (!removed.matched) {
    return false;
  }
  diagnostic = removed.diagnostic_code + ": " + removed.message;
  return true;
}

}  // namespace objc3c::diagnostics::modes
