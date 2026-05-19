#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace objc3c::config {

std::string TrimConfigText(std::string_view text);
std::string NormalizeLanguageProfileName(std::string_view name);
std::string NormalizeCommandOptionKey(std::string_view flag);
bool ParseUnsignedConfigValue(std::string_view text, std::uint32_t &value);

}  // namespace objc3c::config
