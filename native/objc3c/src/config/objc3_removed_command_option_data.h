#pragma once

#include <span>

#include "config/objc3_removed_command_options.h"

namespace objc3c::config {

std::span<const CommandOptionState> RemovedCommandOptionData();

}  // namespace objc3c::config
