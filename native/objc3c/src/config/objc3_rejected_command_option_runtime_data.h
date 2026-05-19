#pragma once

#include <cstddef>
#include <span>

#include "config/objc3_removed_command_options.h"

namespace objc3c::config {

inline constexpr std::size_t kRejectedRuntimeCommandOptionDataCount = 5;

std::span<const CommandOptionState> RejectedRuntimeCommandOptionData();

}  // namespace objc3c::config
