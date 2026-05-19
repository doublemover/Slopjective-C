#include "config/objc3_config_parsing.h"

#include <cstddef>

namespace objc3c::config {

std::string NormalizeCommandOptionKey(std::string_view flag) {
  const std::string trimmed = TrimConfigText(flag);
  const std::size_t equals = trimmed.find('=');
  if (equals == std::string::npos) {
    return trimmed;
  }
  return trimmed.substr(0, equals);
}

}  // namespace objc3c::config
