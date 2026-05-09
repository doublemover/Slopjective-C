#include "io/json/json_pointer.h"

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

const JsonValue *ResolveLocalJsonPointerRef(const JsonValue &schema_root,
                                            std::string_view ref) {
  if (ref.empty() || ref[0] != '#') {
    return nullptr;
  }
  const JsonValue *cursor = &schema_root;
  std::size_t offset = 1;
  while (offset < ref.size()) {
    if (ref[offset] != '/') {
      return nullptr;
    }
    const std::size_t next = ref.find('/', offset + 1);
    const std::size_t token_end =
        next == std::string_view::npos ? ref.size() : next;
    const std::string token =
        DecodeJsonPointerToken(ref.substr(offset + 1,
                                          token_end - offset - 1));
    cursor = cursor->Find(token);
    if (cursor == nullptr) {
      return nullptr;
    }
    offset = token_end;
  }
  return cursor;
}

}  // namespace objc3::io::json
