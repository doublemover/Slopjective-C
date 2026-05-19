#include "config/objc3_command_options.h"

#include <string>

#include "config/objc3_config_parsing.h"
#include "config/objc3_removed_command_option_table.h"

namespace objc3c::config {

const char *RemovedCommandOptionOwnerName(RemovedCommandOptionOwner owner) {
  switch (owner) {
    case RemovedCommandOptionOwner::kLanguageMode:
      return "language-mode";
    case RemovedCommandOptionOwner::kReporting:
      return "reporting";
    case RemovedCommandOptionOwner::kRuntime:
      return "runtime";
  }
  return "unknown";
}

std::span<const CommandOptionState> RemovedCommandOptions() {
  return RemovedCommandOptionTable();
}

const CommandOptionState *FindRemovedCommandOption(std::string_view flag) {
  const std::string key = NormalizeCommandOptionKey(flag);
  for (const auto &option : RemovedCommandOptionTable()) {
    if (key == option.spelling) {
      return &option;
    }
  }
  return nullptr;
}

}  // namespace objc3c::config
