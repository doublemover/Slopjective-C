#include "config/objc3_removed_command_options.h"

#include "config/objc3_removed_command_option_table.h"

namespace objc3c::config {

std::span<const CommandOptionState> RemovedCommandOptions() {
  return RemovedCommandOptionEntries();
}

}  // namespace objc3c::config
