#include "tools/objc3c_frontend_c_api_runner_option_values.h"

#include <cerrno>
#include <cstdlib>

#include "support/objc3_ir_object_backend_token.h"
#include "support/objc3_runtime_dispatch_symbol.h"

bool ParseFrontendCApiRunnerIrObjectBackend(
    const std::string &value,
    objc3c_frontend_c_ir_object_backend_t &backend) {
  objc3c::support::IrObjectBackendToken token;
  if (!objc3c::support::ParseIrObjectBackendToken(value, token)) {
    return false;
  }
  if (token == objc3c::support::IrObjectBackendToken::Clang) {
    backend = OBJC3C_FRONTEND_IR_OBJECT_BACKEND_CLANG;
    return true;
  }
  backend = OBJC3C_FRONTEND_IR_OBJECT_BACKEND_LLVM_DIRECT;
  return true;
}

bool ParseFrontendCApiRunnerMaxMessageSendArgs(const std::string &value,
                                               std::uint32_t &parsed_value,
                                               std::string &error) {
  errno = 0;
  char *end = nullptr;
  const unsigned long parsed = std::strtoul(value.c_str(), &end, 10);
  if (value.empty() || end == value.c_str() || *end != '\0' ||
      errno == ERANGE || parsed > kFrontendCApiRunnerMaxMessageSendArgs) {
    error = "invalid --objc3-max-message-args (expected integer 0-" +
            std::to_string(kFrontendCApiRunnerMaxMessageSendArgs) + "): " +
            value;
    return false;
  }
  parsed_value = static_cast<std::uint32_t>(parsed);
  return true;
}

bool ParseFrontendCApiRunnerRegistrationOrderOrdinal(
    const std::string &value,
    std::uint64_t &parsed_value,
    std::string &error) {
  errno = 0;
  char *end = nullptr;
  const unsigned long long parsed = std::strtoull(value.c_str(), &end, 10);
  if (value.empty() || end == value.c_str() || *end != '\0' ||
      errno == ERANGE || parsed == 0) {
    error =
        "invalid --objc3-bootstrap-registration-order-ordinal (expected "
        "positive integer): " +
        value;
    return false;
  }
  parsed_value = static_cast<std::uint64_t>(parsed);
  return true;
}

bool ParseFrontendCApiRunnerRuntimeDispatchSymbol(
    const std::string &value,
    std::string &parsed_value,
    std::string &error) {
  if (!objc3c::support::IsValidRuntimeDispatchSymbol(value)) {
    error =
        "invalid --objc3-runtime-dispatch-symbol (expected [A-Za-z_.$][A-Za-z0-9_.$]*): " +
        value;
    return false;
  }
  parsed_value = value;
  return true;
}
