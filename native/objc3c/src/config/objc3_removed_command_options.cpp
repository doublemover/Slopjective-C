#include "config/objc3_removed_command_options.h"

#include <string>

#include "config/objc3_config_parsing.h"
#include "config/objc3_removed_command_option_table.h"

namespace objc3c::config {

std::span<const CommandOptionState> RemovedCommandOptions() {
  return RemovedCommandOptionEntries();
}

const CommandOptionState *FindRemovedCommandOption(std::string_view flag) {
  const std::string key = NormalizeCommandOptionKey(flag);
  for (const auto &option : RemovedCommandOptionEntries()) {
    if (key == option.spelling) {
      return &option;
    }
  }
  return nullptr;
}

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
