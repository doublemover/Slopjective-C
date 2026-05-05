#include "diagnostics/modes/objc3_removed_mode_options.h"

#include "config/objc3_language_profile.h"

namespace objc3c::diagnostics::modes {

bool BuildRemovedModeOptionDiagnostic(const std::string &flag, std::string &diagnostic) {
  if (const auto *option = objc3c::config::FindRemovedCommandOption(flag)) {
    diagnostic =
        "unsupported hard-cutover option: " + flag + " was removed; " +
        option->summary;
    return true;
  }
  return false;
}

}  // namespace objc3c::diagnostics::modes
