#include "driver/objc3_driver_diagnostic_output.h"

#include <iostream>

void EmitObjc3DriverError(const std::string &message) {
  std::cerr << message << '\n';
}

void EmitObjc3DriverError(const char *message) {
  EmitObjc3DriverError(std::string(message));
}
