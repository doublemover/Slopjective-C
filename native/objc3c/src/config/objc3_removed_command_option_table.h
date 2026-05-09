#pragma once

#include <span>

#include "config/objc3_removed_command_options.h"

namespace objc3c::config {

std::span<const CommandOptionState> RemovedCommandOptionEntries();

}  // namespace objc3c::config
