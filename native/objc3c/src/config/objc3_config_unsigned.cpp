#include "config/objc3_config_parsing.h"

#include <cstdint>
#include <limits>

namespace objc3c::config {

bool ParseUnsignedConfigValue(std::string_view text, std::uint32_t &value) {
  const std::string trimmed = TrimConfigText(text);
  if (trimmed.empty()) {
    return false;
  }

  std::uint32_t parsed = 0;
  for (char c : trimmed) {
    if (c < '0' || c > '9') {
      return false;
    }
    const std::uint32_t digit = static_cast<std::uint32_t>(c - '0');
    if (parsed >
        (std::numeric_limits<std::uint32_t>::max() - digit) / 10u) {
      return false;
    }
    parsed = parsed * 10u + digit;
  }

  value = parsed;
  return true;
}

}  // namespace objc3c::config
