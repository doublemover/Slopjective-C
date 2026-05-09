#pragma once

#include <string>

#include "driver/objc3_driver_status_codes.h"

struct Objc3DriverCommandResult {
  Objc3DriverStatusCode status_code = Objc3DriverStatusCode::kSuccess;
  std::string diagnostic;
};

Objc3DriverCommandResult Objc3DriverCommandSucceeded();
Objc3DriverCommandResult Objc3DriverCommandFailed(
    Objc3DriverStatusCode status_code,
    std::string diagnostic);
int EmitObjc3DriverCommandResult(const Objc3DriverCommandResult &result);
