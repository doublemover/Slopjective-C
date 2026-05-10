#include "tools/objc3c_frontend_c_api_runner_option_values.h"

#include "support/objc3_runtime_dispatch_symbol.h"

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
