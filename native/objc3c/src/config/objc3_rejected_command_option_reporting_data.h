#pragma once

#include <cstddef>
#include <span>

#include "config/objc3_command_options.h"

namespace objc3c::config {

inline constexpr std::size_t kRejectedReportingCommandOptionDataCount = 2;

std::span<const CommandOptionState> RejectedReportingCommandOptionData();

}  // namespace objc3c::config
