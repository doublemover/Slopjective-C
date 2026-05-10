#include "tools/objc3c_frontend_c_api_runner_c_string.h"

#include <cstddef>

std::string ReadFrontendCApiLastError(
    const objc3c_frontend_c_context_t *context) {
  const std::size_t required =
      objc3c_frontend_c_copy_last_error(context, nullptr, 0);
  if (required == 0) {
    return "";
  }
  std::string message(required, '\0');
  const std::size_t written = objc3c_frontend_c_copy_last_error(
      context, message.data(), message.size());
  if (written == 0) {
    return "";
  }
  if (!message.empty() && message.back() == '\0') {
    message.pop_back();
  }
  return message;
}
