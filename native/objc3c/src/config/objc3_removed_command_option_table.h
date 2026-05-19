#pragma once

#include <span>

#include "config/objc3_command_options.h"

namespace objc3c::config {

std::span<const CommandOptionState> RemovedCommandOptionTable();

}  // namespace objc3c::config
