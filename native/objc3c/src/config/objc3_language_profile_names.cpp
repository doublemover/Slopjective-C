#include "config/objc3_config_parsing.h"

#include <algorithm>
#include <cctype>

namespace objc3c::config {

std::string NormalizeLanguageProfileName(std::string_view name) {
  std::string normalized = TrimConfigText(name);
  std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                 [](unsigned char c) {
                   return static_cast<char>(std::tolower(c));
                 });
  return normalized;
}

}  // namespace objc3c::config
