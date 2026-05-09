#include "io/json/json_pointer_tokens.h"

#include <cstddef>

namespace objc3::io::json {

std::string DecodeJsonPointerToken(std::string_view token) {
  std::string decoded;
  decoded.reserve(token.size());
  for (std::size_t i = 0; i < token.size(); ++i) {
    if (token[i] == '~' && i + 1 < token.size()) {
      const char escaped = token[++i];
      if (escaped == '0') {
        decoded.push_back('~');
        continue;
      }
      if (escaped == '1') {
        decoded.push_back('/');
        continue;
      }
      decoded.push_back('~');
      decoded.push_back(escaped);
      continue;
    }
    decoded.push_back(token[i]);
  }
  return decoded;
}

}  // namespace objc3::io::json
