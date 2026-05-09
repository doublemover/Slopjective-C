#include "config/objc3_config_parsing.h"

#include <cctype>
#include <cstddef>

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

}  // namespace objc3c::config
