#include "config/objc3_removed_command_option_table.h"

#include "config/objc3_removed_command_option_data.h"

namespace objc3c::config {

std::span<const CommandOptionState> RemovedCommandOptionEntries() {
  return RemovedCommandOptionData();
}

}  // namespace objc3c::config
