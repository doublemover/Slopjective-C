#pragma once

#include <cstddef>
#include <span>

#include "config/objc3_command_options.h"

namespace objc3c::config {

inline constexpr std::size_t kRejectedLanguageModeCommandOptionDataCount = 7;

std::span<const CommandOptionState> RejectedLanguageModeCommandOptionData();

}  // namespace objc3c::config
