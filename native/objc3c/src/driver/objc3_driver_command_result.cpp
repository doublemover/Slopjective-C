#include "driver/objc3_driver_command_result.h"

#include <utility>

#include "driver/objc3_driver_diagnostic_output.h"

Objc3DriverCommandResult Objc3DriverCommandSucceeded() {
  return {};
}

Objc3DriverCommandResult Objc3DriverCommandFailed(
    Objc3DriverStatusCode status_code,
    std::string diagnostic) {
  return {.status_code = status_code, .diagnostic = std::move(diagnostic)};
}

int EmitObjc3DriverCommandResult(const Objc3DriverCommandResult &result) {
  if (!result.diagnostic.empty()) {
    EmitObjc3DriverError(result.diagnostic);
  }
  return Objc3DriverStatusValue(result.status_code);
}
