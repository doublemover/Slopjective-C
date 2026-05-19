#include "config/objc3_command_options.h"

#include <string>

namespace objc3c::config {

ConfigValidationResult ValidateRemovedCommandOption(std::string_view flag) {
  const CommandOptionState *option = FindRemovedCommandOption(flag);
  if (option == nullptr) {
    return MakeConfigValidationAccepted();
  }
  return MakeConfigValidationRejected(
      option->diagnostic_code,
      "unsupported hard-cutover option: " + std::string(flag) +
          " was removed; " + option->summary);
}

}  // namespace objc3c::config
