#include "libobjc3c_frontend/objc3c_frontend_result_ownership.h"

#include <cstdlib>
#include <cstring>

namespace objc3c::frontend {

objc3c_frontend_string_t *CloneOwnedFrontendString(const std::string &text) {
  if (text.empty()) {
    return nullptr;
  }

  auto *string = static_cast<objc3c_frontend_string_t *>(
      std::malloc(sizeof(objc3c_frontend_string_t)));
  if (string == nullptr) {
    return nullptr;
  }

  auto *data = static_cast<char *>(std::malloc(text.size() + 1u));
  if (data == nullptr) {
    std::free(string);
    return nullptr;
  }

  std::memcpy(data, text.data(), text.size());
  data[text.size()] = '\0';
  string->data = data;
  string->size = text.size();
  return string;
}

void ReleaseOwnedFrontendString(objc3c_frontend_string_t *string) {
  if (string == nullptr) {
    return;
  }
  std::free(const_cast<char *>(string->data));
  std::free(string);
}

}  // namespace objc3c::frontend
