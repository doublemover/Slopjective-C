#pragma once

#include <string>

#include "libobjc3c_frontend/c_api.h"

struct FrontendCApiRunnerStringSnapshot {
  bool present = false;
  std::string text;
};

FrontendCApiRunnerStringSnapshot SnapshotOptionalFrontendCApiString(
    const objc3c_frontend_c_string_t *value);
std::string OptionalFrontendCApiString(
    const objc3c_frontend_c_string_t *value);
