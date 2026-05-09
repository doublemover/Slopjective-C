#include "config/objc3_config_parsing.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <limits>

namespace objc3c::config {

std::string TrimConfigText(std::string_view text) {
  std::size_t begin = 0;
  while (begin < text.size() &&
         std::isspace(static_cast<unsigned char>(text[begin])) != 0) {
    ++begin;
  }

  std::size_t end = text.size();
  while (end > begin &&
         std::isspace(static_cast<unsigned char>(text[end - 1])) != 0) {
    --end;
  }
  return std::string(text.substr(begin, end - begin));
}

std::string NormalizeLanguageProfileName(std::string_view name) {
  std::string normalized = TrimConfigText(name);
  std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                 [](unsigned char c) {
                   return static_cast<char>(std::tolower(c));
                 });
  return normalized;
}

std::string NormalizeCommandOptionKey(std::string_view flag) {
  const std::string trimmed = TrimConfigText(flag);
  const std::size_t equals = trimmed.find('=');
  if (equals == std::string::npos) {
    return trimmed;
  }
  return trimmed.substr(0, equals);
}

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
