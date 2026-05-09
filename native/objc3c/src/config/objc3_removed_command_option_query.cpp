#include "config/objc3_removed_command_options.h"

#include <string>

#include "config/objc3_config_parsing.h"
#include "config/objc3_removed_command_option_table.h"

namespace objc3c::config {

const CommandOptionState *FindRemovedCommandOption(std::string_view flag) {
  const std::string key = NormalizeCommandOptionKey(flag);
  for (const auto &option : RemovedCommandOptionEntries()) {
    if (key == option.spelling) {
      return &option;
    }
  }
  return nullptr;
}

}  // namespace objc3c::config
